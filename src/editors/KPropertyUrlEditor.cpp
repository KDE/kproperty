/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2016-2017 Jarosław Staniek <staniek@kde.org>

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
#include "KPropertyUtils.h"

#include <QLineEdit>
#include <QFileDialog>

class Q_DECL_HIDDEN KPropertyUrlEditor::Private
{
public:
    Private(const KProperty &property)
        : fileMode(property.option("fileMode").toByteArray().toLower())
        , confirmOverwrites(property.option("confirmOverwrites", false).toBool())
    {
    }

    bool isValid(const QUrl &url) const
    {
        if (url.isEmpty()) {
            return true;
        }
        if (!url.isValid()) {
            return false;
        }
        if (url.isLocalFile()) {
            const QString path = url.toLocalFile();
            QFileInfo info(path);
            if (!info.isNativePath()) {
                return false;
            }
            // @todo check for illegal characters -- https://stackoverflow.com/a/45282192
            if (fileMode == "existingfile") {
                //! @todo display error for false
                return info.isFile() && info.exists();
            } else if (fileMode == "dirsonly") {
                //! @todo display error for false
                return info.isDir() && info.exists();
            }
            // fileMode is "":
            //! @todo support confirmOverwrites, display error
            //if (info.exists()) {
            //}
        }
        return true;
    }

    QUrl value;
    QLineEdit *lineEdit;
    KPropertyUrlDelegate delegate;
    QByteArray fileMode;
    bool confirmOverwrites;
    bool slotTextChangedEnabled = true;
};

KPropertyUrlEditor::KPropertyUrlEditor(const KProperty &property, QWidget *parent)
        : KPropertyGenericSelectionEditor(parent), d(new Private(property))
{
    d->lineEdit = new QLineEdit;
    connect(d->lineEdit, &QLineEdit::textChanged, this, &KPropertyUrlEditor::slotTextChanged);
    d->lineEdit->setClearButtonEnabled(true);
    setMainWidget(d->lineEdit);
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
    d->slotTextChangedEnabled = false;
    d->lineEdit->setText(d->delegate.valueToString(d->value, locale()));
    d->slotTextChangedEnabled = true;
}

void KPropertyUrlEditor::slotTextChanged(const QString &text)
{
    if (!d->slotTextChangedEnabled) {
        return;
    }
    const QUrl newUrl = QUrl::fromUserInput(text);
    if (d->isValid(newUrl)) {
        d->value = newUrl;
        emit commitData(this);
    }
}

void KPropertyUrlEditor::selectButtonClicked()
{
    QUrl url;
    QFileDialog::Options options;
    if (d->fileMode == "existingfile") {
        url = QFileDialog::getOpenFileUrl(this, tr("Select Existing File"), d->value, QString(), nullptr, options);
    } else if (d->fileMode == "dirsonly") {
        options |= QFileDialog::ShowDirsOnly;
        url = QFileDialog::getExistingDirectoryUrl(this, tr("Select Existing Directory"), d->value, options);
    } else {
        if (!d->confirmOverwrites) {
            options |= QFileDialog::DontConfirmOverwrite;
        }
        url = QFileDialog::getSaveFileUrl(this, tr("Select File"), d->value, QString(), nullptr, options);
    }
    //! @todo filters, more options, supportedSchemes, localFilesOnly?
    if (!url.isEmpty() && d->isValid(url)) {
        setValue(url);
        emit commitData(this);
    }
}

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
