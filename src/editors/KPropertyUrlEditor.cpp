/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2016-2018 Jarosław Staniek <staniek@kde.org>

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

#include "KPropertyUrlEditor.h"
#include "KPropertyEditorItemEvent.h"
#include "KPropertyEditorView.h"
#include "KPropertySet.h"
#include "KPropertyUtils.h"

#include <QFileDialog>
#include <QKeyEvent>
#include <QLineEdit>

class Q_DECL_HIDDEN KPropertyUrlEditor::Private
{
public:
    Private(KPropertyUrlEditor *editor, const KProperty &property)
        : fileMode(property.option("fileMode").toByteArray().toLower())
        , confirmOverwrites(property.option("confirmOverwrites", false).toBool())
        , propertyName(property.name())
        , m_editor(editor)
    {
    }

    QString fixUp(const QString &path) const
    {
        QString result = path;
        if (path.endsWith(QLatin1Char('/'))
#ifdef Q_OS_WIN
        && path.length() > 3 // "C:" would not be a valid path on Windows
#endif
           )
        {
            result.chop(1);
        }
        return result;
    }

    //! @return @c true is @a url is valid for the property options
    //! If path obtained from the URL needs fixing @a fixUpPath is set to fixed string, otherwise
    //! it is set to empty string.
    bool isValid(const QUrl &url, QString *fixUpPath) const
    {
        Q_ASSERT(fixUpPath);
        fixUpPath->clear();
        if (url.isEmpty()) {
            return true;
        }
        if (!url.isValid()) {
            return false;
        }
        if (!url.isLocalFile()) {
            // Non-file protocols are only allowed for "" fileMode
            return fileMode.isEmpty();
        }
        QString path = url.toLocalFile();
        QFileInfo info(path);
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
                *fixUpPath = fixUp(path);
            } else {
                return false;
            }
        }
        // fileMode is "":
        //! @todo support confirmOverwrites, display error
        //if (info.exists()) {
        //}
        return true;
    }

    //! @return URL based on text entered
    QUrl urlFromText(const QString &text) const
    {
        //! @todo offer "workingDirectory" option so relative paths are supported
        QString workingDirectory;
        return QUrl::fromUserInput(text, workingDirectory, QUrl::AssumeLocalFile);
    }

    //! @return @c true if @a event is a key press event for Enter or Return key
    bool enterPressed(QEvent *event) const
    {
        if (event->type() == QEvent::KeyPress) {
            const int key = static_cast<QKeyEvent*>(event)->key();
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

    //! Get a new URL value either from a custom or the built-in dialog
    QUrl getUrl()
    {
        QString caption;
        if (fileMode == "existingfile") {
            caption = QObject::tr("Select Existing File");
        } else if (fileMode == "dirsonly") {
            caption = QObject::tr("Select Existing Directory");
        } else {
            caption = QObject::tr("Select File");
        }
        // Try to obtain URL from a custom dialog
        if (m_editor->parentWidget()) {
            KPropertyEditorView *view
                = qobject_cast<KPropertyEditorView *>(m_editor->parentWidget()->parentWidget());
            KProperty *property = &view->propertySet()->property(propertyName);
            if (property) {
                QVariantMap parameters;
                parameters[QStringLiteral("url")] = value;
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
            return QFileDialog::getOpenFileUrl(m_editor, caption, value, QString(), nullptr, options);
        } else if (fileMode == "dirsonly") {
            options |= QFileDialog::ShowDirsOnly;
            return QFileDialog::getExistingDirectoryUrl(m_editor, caption, value, options);
        } else {
            if (!confirmOverwrites) {
                options |= QFileDialog::DontConfirmOverwrite;
            }
            return QFileDialog::getSaveFileUrl(m_editor, caption, value, QString(), nullptr, options);
        }
        return QUrl();
    }

    QUrl value;
    QString savedText;
    QLineEdit *lineEdit;
    KPropertyUrlDelegate delegate;
    QByteArray fileMode;
    bool confirmOverwrites;
    QByteArray propertyName;

private:
    KPropertyUrlEditor * const m_editor;
};

KPropertyUrlEditor::KPropertyUrlEditor(const KProperty &property, QWidget *parent)
        : KPropertyGenericSelectionEditor(parent), d(new Private(this, property))
{
    d->lineEdit = new QLineEdit;
    d->lineEdit->setClearButtonEnabled(true);
    setMainWidget(d->lineEdit);
    d->lineEdit->installEventFilter(this);
}

KPropertyUrlEditor::~KPropertyUrlEditor()
{
}

QUrl KPropertyUrlEditor::value() const
{
    return d->value;
}

void KPropertyUrlEditor::setValue(const QUrl &value)
{
    d->value = value;
    d->lineEdit->setText(d->delegate.valueToString(d->value, locale()));
    d->savedText = d->lineEdit->text();
}

void KPropertyUrlEditor::selectButtonClicked()
{
    const QUrl url = d->getUrl();
    QString fixUpPath;
    if (!url.isEmpty() && d->isValid(url, &fixUpPath)) {
        setValue(url);
        emit commitData(this);
    }
}

bool KPropertyUrlEditor::eventFilter(QObject *o, QEvent *event)
{
    if (o == d->lineEdit && (event->type() == QEvent::FocusOut || d->enterPressed(event))) {
        if (d->savedText != d->lineEdit->text()) { // text changed since the recent setValue(): update
            //! @todo offer "workingDirectory" option so relative paths are supported
            QString workingDirectory;
            QUrl newUrl = QUrl::fromUserInput(d->lineEdit->text(), workingDirectory, QUrl::AssumeLocalFile);
            QString fixUpPath;
            if (d->isValid(newUrl, &fixUpPath)) {
                if (fixUpPath.isEmpty()) { // accept value as-is
                    d->value = newUrl;
                    d->savedText = d->lineEdit->text();
                } else { // fixed up value, e.g. directory path has removed trailing slash
                    d->value = QUrl::fromUserInput(fixUpPath, workingDirectory, QUrl::AssumeLocalFile);
                    d->savedText = fixUpPath;
                }
                emit commitData(this);
            } else { // invalid URL: revert text to last saved value which is valid, emit no change
                d->lineEdit->setText(d->savedText);
            }
        }
    }
    return KPropertyGenericSelectionEditor::eventFilter(o, event);
}

// ----

KPropertyUrlDelegate::KPropertyUrlDelegate()
{
}

QWidget* KPropertyUrlDelegate::createEditor(int type, QWidget *parent,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(type)
    Q_UNUSED(option)
    const KProperty *prop = KPropertyUtils::propertyForIndex(index);
    return new KPropertyUrlEditor(prop ? *prop : KProperty(), parent);
}

QString KPropertyUrlDelegate::valueToString(const QVariant &value,
                                            const QLocale &locale) const
{
    const QUrl url(value.toUrl());
    const QString s(url.isLocalFile()
                      ? QDir::toNativeSeparators(url.toLocalFile())
                      : value.toString());
    if (locale.language() == QLocale::C) {
        return s;
    }
    return valueToLocalizedString(s);
}
