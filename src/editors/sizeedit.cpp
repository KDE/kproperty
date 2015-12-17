/* This file is part of the KDE project
   Copyright (C) 2008-2015 Jarosław Staniek <staniek@kde.org>

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

#include "sizeedit.h"

#include <QSize>

KPropertySizeDelegate::KPropertySizeDelegate()
{
}

QString KPropertySizeDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    const QSize s(value.toSize());
    if (s.isNull()) {
        if (locale.language() == QLocale::C) {
            return QString();
        }
        return QObject::tr("None", "Null value");
    }
    if (locale.language() == QLocale::C) {
        return QString::fromLatin1("%1x%2").arg(s.width()).arg(s.height());
    }
    return QObject::tr("%1x%2", "Size")
        .arg(locale.toString(s.width()))
        .arg(locale.toString(s.height()));
}

//------------

KSizeComposedProperty::KSizeComposedProperty(KProperty *property)
        : KComposedPropertyInterface(property)
{
    (void)new KProperty("width",
        QVariant(), QObject::tr("Width"), QObject::tr("Width"), KProperty::UInt, property);
    (void)new KProperty("height",
        QVariant(), QObject::tr("Height"), QObject::tr("Height"), KProperty::UInt, property);
}

void KSizeComposedProperty::setValue(KProperty *property,
    const QVariant &value, bool rememberOldValue)
{
    const QSize s( value.toSize() );
    property->child("width")->setValue(s.width(), rememberOldValue, false);
    property->child("height")->setValue(s.height(), rememberOldValue, false);
}

void KSizeComposedProperty::childValueChanged(KProperty *child,
    const QVariant &value, bool rememberOldValue)
{
    Q_UNUSED(rememberOldValue);
    QSize s( child->parent()->value().toSize() );
    if (child->name() == "width")
        s.setWidth(value.toInt());
    else if (child->name() == "height")
        s.setHeight(value.toInt());

    child->parent()->setValue(s, true, false);
}
