/* This file is part of the KDE project
   Copyright (C) 2008-2017 Jarosław Staniek <staniek@kde.org>

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

#include "sizefedit.h"
#include "KPropertyUtils_p.h"

#include <QSize>

KPropertySizeFDelegate::KPropertySizeFDelegate()
{
}

QString KPropertySizeFDelegate::propertyValueToString(const KProperty *property,
                                                      const QLocale &locale) const
{
    const KPropertyUtilsPrivate::ValueOptionsHandler options(*property);
    return options.valueWithPrefixAndSuffix(valueToString(property->value(), locale), locale);
}

QString KPropertySizeFDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    const QSizeF s(value.toSizeF());
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

KSizeFComposedProperty::KSizeFComposedProperty(KProperty *property)
        : KComposedPropertyInterface(property)
{
    (void)new KProperty("width",
        QVariant(), QObject::tr("Width"), QObject::tr("Width"), KProperty::Double, property);
    (void)new KProperty("height",
        QVariant(), QObject::tr("Height"), QObject::tr("Height"), KProperty::Double, property);
}

void KSizeFComposedProperty::setValue(KProperty *property,
    const QVariant &value, KProperty::ValueOptions valueOptions)
{
    const QSizeF s( value.toSizeF() );
    property->child("width")->setValue(s.width(), valueOptions);
    property->child("height")->setValue(s.height(), valueOptions);
}

void KSizeFComposedProperty::childValueChanged(KProperty *child,
    const QVariant &value, KProperty::ValueOptions valueOptions)
{
    QSizeF s( child->parent()->value().toSizeF() );
    if (child->name() == "width")
        s.setWidth(value.toDouble());
    else if (child->name() == "height")
        s.setHeight(value.toDouble());

    child->parent()->setValue(s, valueOptions);
}
