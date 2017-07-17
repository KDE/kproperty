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

#include "sizepolicyedit.h"
#include "KPropertyCoreUtils_p.h"

#include <QSizePolicy>

class SizePolicyListData : public KPropertyListData
{
public:
    SizePolicyListData() : KPropertyListData(keysInternal(), stringsInternal())
    {
    }

    QString nameForPolicy(QSizePolicy::Policy p) {
        const int index = keys().indexOf(static_cast<int>(p));
        if (index == -1)
            return names()[0].toString();
        return names()[index].toString();
    }

private:
    static QList<QVariant> keysInternal() {
        QList<QVariant> keys;
        keys
         << QSizePolicy::Fixed
         << QSizePolicy::Minimum
         << QSizePolicy::Maximum
         << QSizePolicy::Preferred
         << QSizePolicy::Expanding
         << QSizePolicy::MinimumExpanding
         << QSizePolicy::Ignored;
        return keys;
    }

    static QStringList stringsInternal() {
        QStringList strings;
        strings
         << QObject::tr("Fixed", "Size Policy")
         << QObject::tr("Minimum", "Size Policy")
         << QObject::tr("Maximum", "Size Policy")
         << QObject::tr("Preferred", "Size Policy")
         << QObject::tr("Expanding", "Size Policy")
         << QObject::tr("Minimum Expanding", "Size Policy")
         << QObject::tr("Ignored", "Size Policy");
        return strings;
    }
};

Q_GLOBAL_STATIC(SizePolicyListData, s_sizePolicyListData)

//---------

KPropertySizePolicyDelegate::KPropertySizePolicyDelegate()
{
}

QString KPropertySizePolicyDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    const QSizePolicy sp(value.value<QSizePolicy>());
    if (locale.language() == QLocale::C) {
        return QString::fromLatin1("%1, %2, %3, %4")
                .arg(KPropertyUtils::keyForEnumValue("SizePolicy", sp.horizontalPolicy()))
                .arg(KPropertyUtils::keyForEnumValue("SizePolicy", sp.verticalPolicy()))
                .arg(sp.horizontalStretch())
                .arg(sp.verticalStretch());
    }
    return QObject::tr("%1, %2, %3, %4", "Size Policy")
        .arg(s_sizePolicyListData->nameForPolicy(sp.horizontalPolicy()))
        .arg(s_sizePolicyListData->nameForPolicy(sp.verticalPolicy()))
        .arg(locale.toString(sp.horizontalStretch()))
        .arg(locale.toString(sp.verticalStretch()));
}

//static
const KPropertyListData& KPropertySizePolicyDelegate::listData()
{
    return *s_sizePolicyListData;
}

//------------

KSizePolicyComposedProperty::KSizePolicyComposedProperty(KProperty *property)
        : KComposedPropertyInterface(property)
{
    (void)new KProperty("hor_policy", new SizePolicyListData(),
        QVariant(), QObject::tr("Hor. Policy"), QObject::tr("Horizontal Policy"),
        KProperty::ValueFromList, property);
    (void)new KProperty("vert_policy", new SizePolicyListData(),
        QVariant(), QObject::tr("Vert. Policy"), QObject::tr("Vertical Policy"),
        KProperty::ValueFromList, property);
    (void)new KProperty("hor_stretch", QVariant(),
        QObject::tr("Hor. Stretch"), QObject::tr("Horizontal Stretch"),
        KProperty::UInt, property);
    (void)new KProperty("vert_stretch", QVariant(),
        QObject::tr("Vert. Stretch"), QObject::tr("Vertical Stretch"),
        KProperty::UInt, property);
}

void KSizePolicyComposedProperty::setValue(KProperty *property,
    const QVariant &value, KProperty::ValueOptions valueOptions)
{
    const QSizePolicy sp( value.value<QSizePolicy>() );
    property->child("hor_policy")->setValue(sp.horizontalPolicy(), valueOptions);
    property->child("vert_policy")->setValue(sp.verticalPolicy(), valueOptions);
    property->child("hor_stretch")->setValue(sp.horizontalStretch(), valueOptions);
    property->child("vert_stretch")->setValue(sp.verticalStretch(), valueOptions);
}

void KSizePolicyComposedProperty::childValueChanged(KProperty *child,
    const QVariant &value, KProperty::ValueOptions valueOptions)
{
    QSizePolicy sp( child->parent()->value().value<QSizePolicy>() );
    if (child->name() == "hor_policy")
        sp.setHorizontalPolicy(static_cast<QSizePolicy::Policy>(value.toInt()));
    else if (child->name() == "vert_policy")
        sp.setVerticalPolicy(static_cast<QSizePolicy::Policy>(value.toInt()));
    else if (child->name() == "hor_stretch")
        sp.setHorizontalStretch(value.toInt());
    else if (child->name() == "vert_stretch")
        sp.setVerticalStretch(value.toInt());

    child->parent()->setValue(sp, valueOptions);
}

bool KSizePolicyComposedProperty::valuesEqual(const QVariant &first, const QVariant &second)
{
    return first.value<QSizePolicy>() == second.value<QSizePolicy>();
}
