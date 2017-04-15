/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
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

#ifndef KPROPERTY_SIZEFEDIT_H
#define KPROPERTY_SIZEFEDIT_H

#include "KPropertyWidgetsFactory.h"

class KPROPERTYWIDGETS_EXPORT KSizeFComposedProperty : public KComposedPropertyInterface
{
public:
    explicit KSizeFComposedProperty(KProperty *parent);

    virtual void setValue(KProperty *property,
        const QVariant &value, KProperty::ValueOptions valueOptions);

    virtual void childValueChanged(KProperty *child,
        const QVariant &value, KProperty::ValueOptions valueOptions);
};

class KPROPERTYWIDGETS_EXPORT KPropertySizeFDelegate : public KPropertyLabelCreator,
                                                       public KComposedPropertyCreator<KSizeFComposedProperty>
{
public:
    KPropertySizeFDelegate();

    QString propertyValueToString(const KProperty *property,
                                  const QLocale &locale) const override;

    virtual QString valueToString(const QVariant& value, const QLocale &locale) const;
};

#endif
