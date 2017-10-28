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

#include "rectedit.h"

#include <QRect>

KPropertyRectDelegate::KPropertyRectDelegate()
{
}

QString KPropertyRectDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    const QRect r(value.toRect());
    if (locale.language() == QLocale::C) {
        return QString::fromLatin1("%1, %2, %3x%4").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height());
    }
    return QObject::tr("%1, %2, %3x%4", "Rectangle")
        .arg(locale.toString(r.x()))
        .arg(locale.toString(r.y()))
        .arg(locale.toString(r.width()))
        .arg(locale.toString(r.height()));
}

//------------

KRectComposedProperty::KRectComposedProperty(KProperty *property)
        : KComposedPropertyInterface(property)
{
    (void)new KProperty("x",
        QVariant(), QObject::tr("X", "Property: X coordinate"),
        QObject::tr("X Coordinate", "Property: X coordinate"), KProperty::Int, property);
    (void)new KProperty("y",
        QVariant(), QObject::tr("Y", "Property: Y coordinate"),
        QObject::tr("Y Coordinate", "Property: Y coordinate"), KProperty::Int, property);
    (void)new KProperty("width",
        QVariant(), QObject::tr("Width", "Property: width of rectangle"),
        QObject::tr("Width", "Property: width of rectangle"), KProperty::UInt, property);
    (void)new KProperty("height",
        QVariant(), QObject::tr("Height", "Property: height of rectangle"),
        QObject::tr("Height", "Property: height of rectangle"), KProperty::UInt, property);
}

void KRectComposedProperty::setValue(KProperty *property,
    const QVariant &value, KProperty::ValueOptions valueOptions)
{
    const QRect r( value.toRect() );
    property->child("x")->setValue(r.x(), valueOptions);
    property->child("y")->setValue(r.y(), valueOptions);
    property->child("width")->setValue(r.width(), valueOptions);
    property->child("height")->setValue(r.height(), valueOptions);
}

void KRectComposedProperty::childValueChanged(KProperty *child,
    const QVariant &value, KProperty::ValueOptions valueOptions)
{
    QRect r( child->parent()->value().toRect() );

    if (child->name() == "x")
        r.moveLeft(value.toInt());
    else if (child->name() == "y")
        r.moveTop(value.toInt());
    else if (child->name() == "width")
        r.setWidth(value.toInt());
    else if (child->name() == "height")
        r.setHeight(value.toInt());

    child->parent()->setValue(r, valueOptions);
}
