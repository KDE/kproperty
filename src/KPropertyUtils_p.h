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

#ifndef KPROPERTY_UTILS_P_H
#define KPROPERTY_UTILS_P_H

#include <QPainter>
#include <QString>
#include <QVariant>

class KProperty;

namespace KPropertyUtilsPrivate
{

//! @return contrast color for @a c color.
QColor contrastColor(const QColor& c);

//! @return grid line color defined by a KPropertyEditorView widget contains @a widget
//! Invalid  color is returned if no grid is defined or KPropertyEditorView was not found.
QColor gridLineColor(const QWidget *widget);

//! @return supported icon theme
//! @todo Support other themes
QString supportedIconTheme();

/*! @brief Sets up a private icon resource file
 * Warns on failure and returns @c false.
 * @param privateName Name to be used instead of application name for resource lookup
 * @param path Relative path to the resource file
 * @param messageType Type of message to use on error, QtFatalMsg for fatal exit and any
 * other for warning
 * @param prefix Resource path prefix. The default is useful for library-global resource,
 * other values is useful for plugins.
 */
bool setupPrivateIconsResourceWithMessage(const QString &privateName, const QString& path,
                                          QtMsgType messageType,
                                          const QString &prefix = QLatin1String(":/icons"));

//! Sets up a global icon theme if it is different from supported.
//! Warns on failure and returns @c false.
bool setupGlobalIconTheme();

//! Helper for handling minValueText, prefix and suffix property options
class ValueOptionsHandler
{
public:
    explicit ValueOptionsHandler(const KProperty &property);

    //! @return @a valueString value with prefix and suffix, if present
    QString valueWithPrefixAndSuffix(const QString &valueString, const QLocale &locale) const;

    QVariant minValueText;
    QString prefix;
    QString suffix;
};

//! @short Manages the QPainter::save()/QPainter::restore() block using RAII
/*! The PainterSaver class makes sure that restore() is called when exiting from the block of code.

   Instead of:
   @code
   painter.save();
   // (code)
   painter.restore();
   @endcode

   Use this:
   @code
   const PainterSaver saver(&painter);
   // (code)
   @endcode
*/
class PainterSaver
{
public:
    explicit PainterSaver(QPainter *p);

    ~PainterSaver();

private:
    QPainter* const m_painter;
};

/**
 * @brief Returns @c true if native dialog should be used
 *
 * If @c false Qt's standard dialog should be used instead of the operating system native dialog.
 * Can be used with QColorDialog, QFileDialog and QFontDialog.
 * Depends on the curent desktop in use:
 * - on Unix (other than macOS) returns @c true if the XDG_CURRENT_DESKTOP environment variable is
 *   empty or equal to "KDE", @c false for other values of XDG_CURRENT_DESKTOP (i.e. @c false for
 *   XFCE, GNOME and other desktops)
 * - @c true for all other operating systems, i.e. for MS Windows, macOS, etc.
 *
 * @todo Share this code with KReport and Kexi
 */
bool shouldUseNativeDialogs();

} // namespace KPropertyUtilsPrivate

#endif
