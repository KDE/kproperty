/* This file is part of the KDE project
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

#include "KPropertyCoreUtils_p.h"

#include <QMetaEnum>

//! @internal Needed by keyForEnumValue() to get access to staticQtMetaObject
struct StaticQtMetaObject : public QObject
{
    static inline const QMetaObject& get() {return staticQtMetaObject;}
};

QString KPropertyUtils::keyForEnumValue(const char *enumName, int enumIndex)
{
    const QMetaObject& mo = StaticQtMetaObject::get();
    const int index = mo.indexOfEnumerator(enumName);
    if (index < 0) {
        return QString();
    }
    QMetaEnum me = mo.enumerator(index);
    return QString::fromLatin1(me.valueToKey(enumIndex));
}
