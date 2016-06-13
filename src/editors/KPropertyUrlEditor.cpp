/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2016 Jarosław Staniek <staniek@kde.org>

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

class KPropertyUrlEditor::Private
{
public:
    Private(const KProperty &property)
        : fileMode(property.option("fileMode").toByteArray().toLower())
        , confirmOverwrites(property.option("confirmOverwrites", false).toBool())
    {
    }
    QUrl value;
    QLineEdit *lineEdit;
    KPropertyUrlDelegate delegate;
    QByteArray fileMode;
    bool confirmOverwrites;
};

KPropertyUrlEditor::KPropertyUrlEditor(const KProperty &property, QWidget *parent)
        : KPropertyGenericSelectionEditor(parent), d(new Private(property))
{
    d->lineEdit = new QLineEdit;
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
    d->lineEdit->setText(d->delegate.valueToString(d->value, locale()));
}

void KPropertyUrlEditor::selectButtonClicked()
{
    QUrl url;
    QFileDialog::Options options;
    if (!d->confirmOverwrites) {
        options |= QFileDialog::DontConfirmOverwrite;
    }
    if (d->fileMode == "existingfile") {
        url = QFileDialog::getSaveFileUrl(this, tr("Select File"), d->value, QString(), 0, options);
    } else if (d->fileMode == "dirsonly") {
        options |= QFileDialog::ShowDirsOnly;
        url = QFileDialog::getExistingDirectoryUrl(this, tr("Select Directory"), d->value, options);
    } else {
        url = QFileDialog::getOpenFileUrl(this, tr("Select File"), d->value, QString(), 0, options);
    }
    //! @todo filters, more options, supportedSchemes, localFilesOnly?
    if (!url.isEmpty()) {
        setValue(url);
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

QString KPropertyUrlDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    const QUrl url(value.toUrl());
    QString s(url.isLocalFile() ? url.toLocalFile() : value.toString());
    if (locale.language() == QLocale::C) {
        return s;
    }
    return valueToLocalizedString(s);
}
