/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2012  Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "timeedit.h"

#include <KPropertyEditorDataModel.h>
#include <QLocale>

KPropertyTimeEditor::KPropertyTimeEditor(const KProperty* prop, QWidget* parent)
  : QTimeEdit(parent)
{
    setFrame(false);

    const QTime minTime = prop->option("min").toTime();
    if (minTime.isValid()) {
        setMinimumTime(minTime);
    }
    const QTime maxTime = prop->option("max").toTime();
    if (maxTime.isValid()) {
        setMaximumTime(maxTime);
    }

    connect(this, SIGNAL(timeChanged(QTime)), this, SLOT(onTimeChanged()));
}

KPropertyTimeEditor::~KPropertyTimeEditor()
{
}

QVariant KPropertyTimeEditor::value() const
{
    return QVariant(time());
}

void KPropertyTimeEditor::setValue(const QVariant& value)
{
    blockSignals(true);
    setTime(value.toTime());
    blockSignals(false);
}

void KPropertyTimeEditor::paintEvent(QPaintEvent* event)
{
    QTimeEdit::paintEvent(event);
    KPropertyWidgetsFactory::paintTopGridLine(this);
}


void KPropertyTimeEditor::onTimeChanged()
{
    emit commitData(this);
}


//! @todo Port to KLocale, be inspired by KexiTimeTableEdit (with Kexi*Formatter)
KPropertyTimeDelegate::KPropertyTimeDelegate()
{
}

QString KPropertyTimeDelegate::displayTextForProperty(const KProperty* prop) const
{
    const QLocale locale;
    const QString defaultTimeFormat = locale.timeFormat(QLocale::ShortFormat);
    return prop->value().toTime().toString(defaultTimeFormat);
}

QWidget* KPropertyTimeDelegate::createEditor(int type, QWidget* parent,
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(type);
    Q_UNUSED(option);

    const KPropertyEditorDataModel* editorModel
        = dynamic_cast<const KPropertyEditorDataModel*>(index.model());
    KProperty* prop = editorModel->propertyForItem(index);

    return new KPropertyTimeEditor(prop, parent);
}
