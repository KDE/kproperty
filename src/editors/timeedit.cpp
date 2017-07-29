/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2012 Friedrich W. H. Kossebau <kossebau@kde.org>
   Copyright (C) 2015 Jarosław Staniek <staniek@kde.org>

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
#include "KPropertyUtils.h"

#include <QLocale>

class Q_DECL_HIDDEN KPropertyTimeEditor::Private
{
public:
    Private() {
    }
};

KPropertyTimeEditor::KPropertyTimeEditor(const KProperty* prop, QWidget* parent)
  : QTimeEdit(parent), d(new Private)
{
    setFrame(false);
    setContentsMargins(0,1,0,0);

    if (prop->hasOptions()) {
        const QTime minTime = prop->option("min", minimumTime()).toTime();
        const QTime maxTime = prop->option("max", maximumTime()).toTime();
        if (minTime.isValid() && maxTime.isValid() && minTime <= maxTime) {
            setTimeRange(minTime, maxTime);
        }
        const QString minValueText(prop->option("minValueText").toString());
        if (!minValueText.isEmpty()) {
            setSpecialValueText(minValueText);
        }
    }

    connect(this, SIGNAL(timeChanged(QTime)), this, SLOT(onTimeChanged()));
}

KPropertyTimeEditor::~KPropertyTimeEditor()
{
    delete d;
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

QString KPropertyTimeDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    if (locale.language() == QLocale::C) {
        if (value.isNull()) {
            return QString();
        }
        return value.toTime().toString(Qt::ISODate);
    }
    const QString defaultTimeFormat = locale.timeFormat(QLocale::ShortFormat);
    return value.toTime().toString(defaultTimeFormat);
}

QWidget* KPropertyTimeDelegate::createEditor(int type, QWidget* parent,
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(type);
    Q_UNUSED(option);

    KProperty* prop = KPropertyUtils::propertyForIndex(index);
    if (!prop) {
        return nullptr;
    }
    return new KPropertyTimeEditor(prop, parent);
}
