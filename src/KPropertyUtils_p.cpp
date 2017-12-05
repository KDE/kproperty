/* This file is part of the KDE project
   Copyright (C) 2010-2017 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPropertyUtils_p.h"
#include "KPropertyEditorView.h"
#include "KProperty.h"

#include <config-kproperty.h>
#if defined KPropertyCore_EXPORTS || defined KPropertyWidgets_EXPORTS
#include "kproperty_debug.h"
#else
# define kprDebug qDebug
# define kprWarning qWarning
# define kprCritical qCritical
#endif

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QPainter>
#include <QRegularExpression>
#include <QResource>
#include <QStandardPaths>

#ifdef KPROPERTY_KF
#include <KConfigGroup>
#include <KSharedConfig>
#include <KMessageBox>
#else
#include <QMessageBox>
#endif

#ifdef QT_GUI_LIB
#include <QIcon>
#endif

#include <algorithm>

class QColor;
class QWidget;

namespace KPropertyUtilsPrivate
{

void showMessageBox(QWidget *parent, const QString &errorMessage, const QString &detailedErrorMessage)
{
    if (detailedErrorMessage.isEmpty()) {
#ifdef KPROPERTY_KF
        KMessageBox::error(parent, errorMessage);
#else
        QMessageBox::warning(parent, QString(), errorMessage);
#endif
    } else {
#ifdef KPROPERTY_KF
        KMessageBox::detailedError(parent, errorMessage, detailedErrorMessage);
#else
        QMessageBox::warning(parent, QString(), errorMessage + QLatin1Char('\n') + detailedErrorMessage);
#endif
    }
}

// -- icon support

QString supportedIconTheme()
{
    return QLatin1String("breeze");
}

//! @brief @return true if @a path is readable
bool fileReadable(const QString &path)
{
    return !path.isEmpty() && QFileInfo(path).isReadable();
}

//! @brief Used for a workaround: locations for QStandardPaths::AppDataLocation end with app name.
//! If this is not an expected app but for example a test app, replace
//! the subdir name with app name so we can find resource file(s).
QStringList correctStandardLocations(const QString &privateName,
                                     QStandardPaths::StandardLocation location,
                                     const QString &extraLocation)
{
    QSet<QString> result;
    if (!privateName.isEmpty()) {
        QRegularExpression re(QLatin1Char('/') + QCoreApplication::applicationName() + QLatin1Char('$'));
        QStringList standardLocations(QStandardPaths::standardLocations(location));
        if (!extraLocation.isEmpty()) {
            standardLocations.append(extraLocation);
        }
        for(const QString &dir : standardLocations) {
            if (dir.indexOf(re) != -1) {
                QString realDir(dir);
                realDir.replace(re, QLatin1Char('/') + privateName);
                result.insert(realDir);
            }
        }
    }
    return result.toList();
}

#ifdef Q_OS_WIN
#define KPATH_SEPARATOR ';'
#else
#define KPATH_SEPARATOR ':'
#endif

/*! @brief Locates a file path for specified parameters
 * @param privateName Name to be used instead of application name for resource lookup
 * @param path Relative path to the resource file
 * @param location Standard file location to use for file lookup
 * @param extraLocation Extra directory path for file lookup
 * @return Empty string on failure
 */
QString locateFile(const QString &privateName,
                   const QString& path, QStandardPaths::StandardLocation location,
                   const QString &extraLocation)
{
    // let QStandardPaths handle this, it will look for app local stuff
    QString fullPath = QFileInfo(
        QStandardPaths::locate(location, path)).canonicalFilePath();
    if (fileReadable(fullPath)) {
        return fullPath;
    }

    // Try extra location
    fullPath = QFileInfo(extraLocation + QLatin1Char('/') + path).canonicalFilePath();
    if (fileReadable(fullPath)) {
        return fullPath;
    }
    // Try in PATH subdirs, useful for running apps from the build dir, without installing
    for(const QByteArray &pathDir : qgetenv("PATH").split(KPATH_SEPARATOR)) {
        const QString dataDirFromPath = QFileInfo(QFile::decodeName(pathDir) + QStringLiteral("/data/")
                                                  + path).canonicalFilePath();
        if (fileReadable(dataDirFromPath)) {
            return dataDirFromPath;
        }
    }

    const QStringList correctedStandardLocations(correctStandardLocations(privateName, location, extraLocation));
    for(const QString &dir : correctedStandardLocations) {
        fullPath = QFileInfo(dir + QLatin1Char('/') + path).canonicalFilePath();
        if (fileReadable(fullPath)) {
            return fullPath;
        }
    }
    return QString();
}

/*! @brief Registers icons resource file
 * @param privateName Name to be used instead of application name for resource lookup
 * @param path Relative path to the resource file
 * @param location Standard file location to use for file lookup
 * @param resourceRoot A resource root for QResource::registerResource()
 * @param errorMessage On failure it is set to a brief error message.
 * @param errorDescription On failure it is set to a detailed error message.
 */
bool registerIconsResource(const QString &privateName, const QString& path,
                             QStandardPaths::StandardLocation location,
                             const QString &resourceRoot, const QString &extraLocation,
                             QString *errorMessage, QString *detailedErrorMessage)
{
    const QString fullPath = locateFile(privateName, path, location, extraLocation);
    if (fullPath.isEmpty() || !QFileInfo(fullPath).isReadable()
        || !QResource::registerResource(fullPath, resourceRoot))
    {
        QStringList triedLocations(QStandardPaths::standardLocations(location));
        if (!extraLocation.isEmpty()) {
            triedLocations.append(extraLocation);
        }
        triedLocations.append(correctStandardLocations(privateName, location, extraLocation));
        std::transform(triedLocations.begin(), triedLocations.end(),
                       triedLocations.begin(), [](const QString &path) {
                         return QDir::toNativeSeparators(path);
                       });
        const QString triedLocationsString = QLocale().createSeparatedList(triedLocations);
#ifdef QT_ONLY
        *errorMessage = QString("Could not open icon resource file %1.").arg(path);
        *detailedErrorMessage = QString("Tried to find in %1.").arg(triedLocationsString);
#else
        *errorMessage = QObject::tr(
            "Could not open icon resource file \"%1\". "
            "Application will not start. "
            "Please check if it is properly installed.")
            .arg(QDir::toNativeSeparators(path));
        *detailedErrorMessage = QObject::tr("Tried to find in %1.").arg(triedLocationsString);
#endif
        return false;
    }
    *errorMessage = QString();
    *detailedErrorMessage = QString();
    return true;
}

/*! @brief Registers a global icon resource file
 * @param themeName A name of icon theme to use.
 * @param errorMessage On failure it is set to a brief error message.
 * @param errorDescription On failure it is set to a detailed error message.
 */
bool registerGlobalIconsResource(const QString &themeName,
                                 QString *errorMessage,
                                 QString *detailedErrorMessage)
{
    QString extraLocation;
#ifdef CMAKE_INSTALL_FULL_ICONDIR
    extraLocation = QDir::fromNativeSeparators(QFile::decodeName(CMAKE_INSTALL_FULL_ICONDIR));
    if (extraLocation.endsWith("/icons")) {
        extraLocation.chop(QLatin1String("/icons").size());
    }
#elif defined(Q_OS_WIN)
    extraLocation = QCoreApplication::applicationDirPath() + QStringLiteral("/data");
#endif
    return registerIconsResource(QString(), QString::fromLatin1("icons/%1/%1-icons.rcc").arg(themeName),
                            QStandardPaths::GenericDataLocation,
                            QStringLiteral("/icons/") + themeName,
                            extraLocation, errorMessage,
                            detailedErrorMessage);
}

/*! @brief Registers a global icon resource file
 * @param themeName A name of icon theme to use.
 */
bool registerGlobalIconsResource(const QString &themeName)
{
    QString errorMessage;
    QString detailedErrorMessage;
    if (!registerGlobalIconsResource(themeName, &errorMessage, &detailedErrorMessage)) {
        showMessageBox(nullptr, errorMessage, detailedErrorMessage);
        kprWarning() << qPrintable(errorMessage);
        return false;
    }
    return true;
}

/*! @brief Registers a global icon resource file for default theme name.
 */
bool registerGlobalIconsResource()
{
    return registerGlobalIconsResource(supportedIconTheme());
}

/*! @brief Sets up a private icon resource file
 * @return @c false on failure and sets error message. Does not warn or exit on failure.
 * @param privateName Name to be used instead of application name for resource lookup
 * @param path Relative path to the resource file
 * @param themeName Icon theme to use. It affects filename.
 * @param errorMessage On failure it is set to a brief error message
 * @param errorDescription On failure it is set to a detailed error message
 * @param prefix Resource path prefix. The default is useful for library-global resource,
 * other values is useful for plugins.
 */
bool setupPrivateIconsResource(const QString &privateName, const QString& path,
                               const QString &themeName,
                               QString *errorMessage, QString *detailedErrorMessage,
                               const QString &prefix = QLatin1String(":/icons"))
{
    // Register application's resource first to have priority over the theme.
    // Some icons may exists in both resources.
    QString extraLocation = QCoreApplication::applicationDirPath() + QStringLiteral("/data");
    if (!registerIconsResource(privateName, path,
                          QStandardPaths::AppDataLocation,
                          QString(), QString(), errorMessage, detailedErrorMessage))
    {
        return false;
    }
    bool changeTheme = false;
#ifdef QT_GUI_LIB
    QIcon::setThemeSearchPaths(QStringList() << prefix << QIcon::themeSearchPaths());
    changeTheme = 0 != QIcon::themeName().compare(themeName, Qt::CaseInsensitive);
    if (changeTheme) {
        QIcon::setThemeName(themeName);
    }
#endif

#ifdef KPROPERTY_KF
    KConfigGroup cg(KSharedConfig::openConfig(), "Icons");
    changeTheme = changeTheme || 0 != cg.readEntry("Theme", QString()).compare(themeName, Qt::CaseInsensitive);
    // tell KIconLoader an co. about the theme
    if (changeTheme) {
        cg.writeEntry("Theme", themeName);
        cg.sync();
    }
#endif
    return true;
}

/*! @brief Sets up a private icon resource file
 * @return @c false on failure and sets error message.
 * @param privateName Name to be used instead of application name for resource lookup
 * @param path Relative path to the resource file
 * @param themeName Icon theme to use. It affects filename.
 * @param errorMessage On failure it is set to a brief error message.
 * @param errorDescription On failure it is set to a detailed error message.
 * @param prefix Resource path prefix. The default is useful for library-global resource,
 * other values is useful for plugins.
 */
bool setupPrivateIconsResourceWithMessage(const QString &privateName, const QString& path,
                                          const QString &themeName,
                                          QString *errorMessage, QString *detailedErrorMessage,
                                          const QString &prefix = QLatin1String(":/icons"))
{
    if (!setupPrivateIconsResource(privateName, path, themeName,
                                   errorMessage, detailedErrorMessage, prefix))
    {
        showMessageBox(nullptr, *errorMessage, *detailedErrorMessage);
        return false;
    }
    return true;
}

/*! @overload setupPrivateIconsResourceWithMessage(QString &privateName, const QString& path,
                                          const QString &themeName,
                                          QString *errorMessage, QString *detailedErrorMessage,
                                          const QString &prefix = QLatin1String(":/icons"))
    Uses default theme name.
 */
bool setupPrivateIconsResourceWithMessage(const QString &privateName, const QString& path,
                                          QString *errorMessage, QString *detailedErrorMessage,
                                          const QString &prefix = QLatin1String(":/icons"))
{
    return setupPrivateIconsResourceWithMessage(privateName, path, supportedIconTheme(),
                                                errorMessage, detailedErrorMessage, prefix);
}

bool setupPrivateIconsResourceWithMessage(const QString &privateName, const QString& path,
                                          QtMsgType messageType,
                                          const QString &prefix)
{
    QString errorMessage;
    QString detailedErrorMessage;
    if (!setupPrivateIconsResourceWithMessage(privateName, path,
                                              &errorMessage, &detailedErrorMessage, prefix)) {
        if (messageType == QtFatalMsg) {
            kprCritical() << qPrintable(errorMessage) << qPrintable(detailedErrorMessage);
        } else {
            kprWarning() << qPrintable(errorMessage) << qPrintable(detailedErrorMessage);
        }
        return false;
    }
    return true;
}

bool setupGlobalIconTheme()
{
    if (0 != QIcon::themeName().compare(supportedIconTheme(), Qt::CaseInsensitive)) {
        const QString message = QObject::tr(
            "\"%1\"  supports only \"%2\" icon theme but current system theme is \"%3\". "
            "Application's icon theme will be changed to \"%2\". "
            "Please consider adding support for other themes to %4.")
            .arg(QLatin1String(KPROPERTYWIDGETS_BASE_NAME)).arg(supportedIconTheme()).arg(QIcon::themeName())
            .arg(QCoreApplication::applicationName());
        kprDebug() << qPrintable(message);
        if (!registerGlobalIconsResource()) {
            // don't fail, just warn
            const QString message = QObject::tr(
                "Failed to set icon theme to \"%1\". Icons in the application will be inconsistent. "
                "Please install .rcc file(s) for the system theme.")
                .arg(supportedIconTheme());
            kprDebug() << qPrintable(message);
            return false;
        }
    }
    return true;
}

// ----

ValueOptionsHandler::ValueOptionsHandler(const KProperty &property)
{
    minValueText = property.option("minValueText");
    prefix = property.option("prefix").toString().trimmed();
    suffix = property.option("suffix").toString().trimmed();
}

QString ValueOptionsHandler::valueWithPrefixAndSuffix(const QString &valueString, const QLocale &locale) const
{
    QString result = valueString;
    if (!suffix.isEmpty()) {
        if (locale.language() == QLocale::C) {
            result = QString::fromLatin1("%1 %2").arg(result).arg(suffix);
        } else {
            result = QObject::tr("%1 %2", "<value> <suffix>").arg(result).arg(suffix);
        }
    }
    if (!prefix.isEmpty()) {
        if (locale.language() == QLocale::C) {
            result = QString::fromLatin1("%1 %2").arg(prefix).arg(result);
        } else {
            result = QObject::tr("%1 %2", "<prefix> <value>").arg(prefix).arg(result);
        }
    }
    return result;
}

// ----

PainterSaver::PainterSaver(QPainter *p)
    : m_painter(p)
{
    if (m_painter) {
        m_painter->save();
    }
}

PainterSaver::~PainterSaver()
{
    if (m_painter) {
        m_painter->restore();
    }
}

} // namespace KPropertyUtilsPrivate
