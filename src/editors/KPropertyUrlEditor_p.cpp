/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2016-2018 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2018 Dmitry Baryshev <dmitrymq@gmail.com>

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

#include "KPropertyUrlEditor_p.h"
#include "KProperty.h"
#include "KPropertyComposedUrl.h"
#include "KPropertyEditorItemEvent.h"
#include "KPropertyEditorView.h"
#include "KPropertyGenericSelectionEditor.h"
#include "KPropertySet.h"
#include "kproperty_debug.h"

#include <QEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeyEvent>
#include <QLineEdit>
#include <QLocale>

KPropertyUrlEditorPrivate::KPropertyUrlEditorPrivate(KPropertyGenericSelectionEditor *editor,
                                                     const KProperty &property)
    : QObject()
    , fileMode(property.option("fileMode").toByteArray().toLower())
    , confirmOverwrites(property.option("confirmOverwrites", false).toBool())
    , propertyName(property.name())
    , m_editor(editor)
{
    lineEdit = new QLineEdit;
    lineEdit->setClearButtonEnabled(true);
    lineEdit->installEventFilter(m_editor);

    isComposedUrl = property.type() == KProperty::ComposedUrl;
    setValue(property.value());
}

void KPropertyUrlEditorPrivate::setValue(const QVariant &newValue)
{
    if (isComposedUrl && newValue.type() == QVariant::Url) {
        KPropertyComposedUrl composedUrl = value.value<KPropertyComposedUrl>();
        const QUrl newUrlValue = newValue.toUrl();

        if (newUrlValue.isRelative()) {
            composedUrl.setRelativePath(newUrlValue.path());
        } else {
            composedUrl.setAbsoluteUrl(newUrlValue);
        }

        value = QVariant::fromValue(composedUrl);
    } else {
        value = newValue;
    }
}

void KPropertyUrlEditorPrivate::updateLineEdit(const QString &textToDisplay)
{
    lineEdit->setText(textToDisplay);
    savedText = lineEdit->text();
}

bool KPropertyUrlEditorPrivate::checkAndUpdate(QUrl *url) const
{
    Q_ASSERT(url);

    if (url->isEmpty()) {
        return true;
    }
    if (!url->isValid()) {
        return false;
    }

    QUrl finalUrlForChecking;

    // handle relative URLs. We pass absolute URLs or truly relative URLs
    // into this function. We don't pass weird things like QUrl("C:/report.txt")
    // (which is considered to be relative). That's why isRelative() is enough
    if (url->isRelative()) {
        const KPropertyComposedUrl composedUrl = value.value<KPropertyComposedUrl>();

        // regular QUrl types must be absolute
        if (!isComposedUrl) {
            kprWarning() << "Property" << propertyName << "doesn't support relative URLs:" << *url;
            return false;
            // complex url may be relative, so check its base URL
        } else if (!composedUrl.baseUrl().isValid()) {
            kprWarning() << "The base URL in property" << propertyName
                         << "is invalid:" << composedUrl;
            return false;
        } else {
            finalUrlForChecking = composedUrl.baseUrl().resolved(*url);
        }
    } else {
        finalUrlForChecking = *url;
    }

    if (!finalUrlForChecking.isLocalFile()) {
        // Non-file protocols are only allowed for "" fileMode
        return fileMode.isEmpty();
    }
    const QString path = finalUrlForChecking.toLocalFile();
    const QFileInfo info(path);
    if (!info.isNativePath()) {
        return false;
    }
    //! @todo check for illegal characters -- https://stackoverflow.com/a/45282192
    if (fileMode == "existingfile") {
        //! @todo display error for false
        return info.isFile() && info.exists();
    } else if (fileMode == "dirsonly") {
        //! @todo display error for false
        if (info.isDir() && info.exists()) {
            if (url->isRelative()) {
                url->setPath(fixUp(url->toString(), /* isRelative */ true));
            } else {
                url->setPath(fixUp(path, /* isRelative */ false));
            }
            return true;
        } else {
            return false;
        }
    }
    // fileMode is "":
    //! @todo support confirmOverwrites, display error
    // if (info.exists()) {
    //}
    return true;
}

bool KPropertyUrlEditorPrivate::enterPressed(QEvent *event) const
{
    if (event->type() == QEvent::KeyPress) {
        const int key = static_cast<QKeyEvent *>(event)->key();
        switch (key) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Down:
        case Qt::Key_Up:
            return true;
        default:
            break;
        }
    }
    return false;
}

QString KPropertyUrlEditorPrivate::fixUp(const QString &path, bool isRelative) const
{
    Q_UNUSED(isRelative)
    QString result = path;
    if (path.endsWith(QLatin1Char('/'))
#ifdef Q_OS_WIN
        && (isRelative || path.length() > 3) // "C:" would not be a valid path on Windows
#endif
    ) {
        result.chop(1);
    }
    return result;
}

QUrl KPropertyUrlEditorPrivate::getUrl()
{
    QString caption;
    if (fileMode == "existingfile") {
        caption = QObject::tr("Select Existing File");
    } else if (fileMode == "dirsonly") {
        caption = QObject::tr("Select Existing Directory");
    } else {
        caption = QObject::tr("Select File");
    }

    QUrl dirUrlForFileDialog;

    if (isComposedUrl) {
        const KPropertyComposedUrl composedUrl = value.value<KPropertyComposedUrl>();
        dirUrlForFileDialog = composedUrl.value();
    } else {
        dirUrlForFileDialog = value.toUrl();
    }

    // Try to obtain URL from a custom dialog
    if (m_editor->parentWidget()) {
        KPropertyEditorView *view
            = qobject_cast<KPropertyEditorView *>(m_editor->parentWidget()->parentWidget());
        KProperty *property = &view->propertySet()->property(propertyName);
        if (property) {
            QVariantMap parameters;
            parameters[QStringLiteral("url")] = dirUrlForFileDialog;
            parameters[QStringLiteral("caption")] = caption;
            KPropertyEditorItemEvent event(*property, QStringLiteral("getOpenFileUrl"), parameters);
            emit view->handlePropertyEditorItemEvent(&event);
            if (event.hasResult()) {
                return event.result().toUrl();
            }
        }
    }

    // Use default dialogs
    //! @todo filters, more options, supportedSchemes, localFilesOnly?
    QFileDialog::Options options;
    if (fileMode == "existingfile") {
        return QFileDialog::getOpenFileUrl(m_editor, caption, dirUrlForFileDialog, QString(),
                                           nullptr, options);
    } else if (fileMode == "dirsonly") {
        options |= QFileDialog::ShowDirsOnly;
        return QFileDialog::getExistingDirectoryUrl(m_editor, caption, dirUrlForFileDialog,
                                                    options);
    } else {
        if (!confirmOverwrites) {
            options |= QFileDialog::DontConfirmOverwrite;
        }
        return QFileDialog::getSaveFileUrl(m_editor, caption, dirUrlForFileDialog, QString(),
                                           nullptr, options);
    }
    return QUrl();
}

void KPropertyUrlEditorPrivate::processEvent(QObject *o, QEvent *event)
{
    if (o == lineEdit && (event->type() == QEvent::FocusOut || enterPressed(event))) {
        const QString enteredText = lineEdit->text();

        if (savedText != enteredText) { // text changed since the recent setValue(): update
            QUrl newUrl(enteredText);

            // "a/b"               -> set this as a path
            // "kde.org"           -> "http://kde.org" (for regular non-composed URLs)
            // "kde.org"           -> "kde.org" (for composed URLs)
            // "C:\report.txt"     -> "file:///C:/report.txt"
            // "http://google.com" -> convert with more strict rules
            if (!newUrl.scheme().isEmpty() || (newUrl.scheme().isEmpty() && !isComposedUrl)
                || QDir::isAbsolutePath(enteredText)) {
                newUrl = QUrl::fromUserInput(enteredText);
            } else {
                newUrl.clear();
                newUrl.setPath(QDir::fromNativeSeparators(enteredText));
            }

            if (checkAndUpdate(&newUrl)) {
                setValue(newUrl);
                savedText = enteredText;
                emit commitData();
            } else { // invalid URL: revert text to last saved value which is valid,
                // emit no change
                kprWarning() << "URL" << newUrl << "is not valid";
                lineEdit->setText(savedText);
            }
        }
    }
}
