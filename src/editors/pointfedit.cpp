/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2008-2017 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2010 Adam Pigg <adam@piggz.co.uk>

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

#include "pointfedit.h"
#include "KPropertyUtils_p.h"

#include <QPointF>

KPropertyPointFDelegate::KPropertyPointFDelegate()
{
}

QString KPropertyPointFDelegate::propertyValueToString(const KProperty *property,
                                                       const QLocale &locale) const
{
    const KPropertyUtilsPrivate::ValueOptionsHandler options(*property);
    return options.valueWithPrefixAndSuffix(valueToString(property->value(), locale), locale);
}

QString KPropertyPointFDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    const QPointF p(value.toPointF());
    if (p.isNull()) {
        if (locale.language() == QLocale::C) {
            return QString();
        }
        return QObject::tr("None", "Null value");
    }
    if (locale.language() == QLocale::C) {
        return QString::fromLatin1("%1, %2").arg(p.x()).arg(p.y());
    }
    return QObject::tr("%1, %2", "Point")
        .arg(locale.toString(p.x()))
        .arg(locale.toString(p.y()));
}

//------------

KPointFComposedProperty::KPointFComposedProperty(KProperty *property)
        : KComposedPropertyInterface(property)
{
    (void)new KProperty("x",
        QVariant(), QObject::tr("X", "Property: X coordinate"),
        QObject::tr("X Coordinate", "Property: X coordinate"), KProperty::Double, property);
    (void)new KProperty("y",
        QVariant(), QObject::tr("Y", "Property: Y coordinate"),
        QObject::tr("Y Coordinate", "Property: Y coordinate"), KProperty::Double, property);
}

void KPointFComposedProperty::setValue(KProperty *property,
    const QVariant &value, KProperty::ValueOptions valueOptions)
{
    const QPointF p( value.toPointF() );
    property->child("x")->setValue(p.x(), valueOptions);
    property->child("y")->setValue(p.y(), valueOptions);
}

void KPointFComposedProperty::childValueChanged(KProperty *child,
    const QVariant &value, KProperty::ValueOptions valueOptions)
{
    QPointF p( child->parent()->value().toPointF() );

    if (child->name() == "x")
        p.setX(value.toDouble());
    else if (child->name() == "y")
        p.setY(value.toDouble());

    child->parent()->setValue(p, valueOptions);
}
