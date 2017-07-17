/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2004-2017 Jarosław Staniek <staniek@kde.org>

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

#include "KPropertyListData.h"

class Q_DECL_HIDDEN KPropertyListData::Private
{
public:
    Private() {
    }
    Private(const Private &other) {
        copy(other);
    }
#define KPropertyListDataPrivateArgs(o) std::tie(o.keys, o.names)
    void copy(const Private &other) {
        KPropertyListDataPrivateArgs((*this)) = KPropertyListDataPrivateArgs(other);
    }
    bool operator==(const Private &other) const {
        return KPropertyListDataPrivateArgs((*this)) == KPropertyListDataPrivateArgs(other);
    }
    QVariantList keys;
    QVariantList names;
};

KPropertyListData::KPropertyListData()
    : d(new Private)
{
}

KPropertyListData::KPropertyListData(const KPropertyListData &other)
    : d(new Private(*other.d))
{
}

KPropertyListData::KPropertyListData(const QStringList &keys, const QStringList &names)
        : d(new Private)
{
    setKeysAsStringList(keys);
    setNamesAsStringList(names);
}

KPropertyListData::KPropertyListData(const QVariantList &keys, const QVariantList &names)
    : d(new Private)
{
    setKeys(keys);
    setNames(names);
}

KPropertyListData::KPropertyListData(const QVariantList &keys, const QStringList &names)
    : d(new Private)
{
    setKeys(keys);
    setNamesAsStringList(names);
}

KPropertyListData::~KPropertyListData()
{
    delete d;
}

KPropertyListData& KPropertyListData::operator=(const KPropertyListData &other)
{
    if (this != &other) {
        d->copy(*other.d);
    }
    return *this;
}

bool KPropertyListData::operator==(const KPropertyListData &other) const
{
    return *d == *other.d;
}

void KPropertyListData::setKeysAsStringList(const QStringList &keys)
{
    d->keys.clear();
    for (const QString &key : keys) {
        d->keys.append(key);
    }
}

void KPropertyListData::setKeys(const QVariantList &keys)
{
    d->keys = keys;
}

QVariantList KPropertyListData::keys() const
{
    return d->keys;
}

QStringList KPropertyListData::keysAsStringList() const
{
    QStringList result;
    for (const QVariant &key : d->keys) {
        result.append(key.toString());
    }
    return result;
}

QVariantList KPropertyListData::names() const
{
    return d->names;
}

QStringList KPropertyListData::namesAsStringList() const
{
    QStringList result;
    for (const QVariant &name : d->names) {
        result.append(name.toString());
    }
    return result;
}

void KPropertyListData::setNamesAsStringList(const QStringList &names)
{
    d->names.clear();
    for (const QString &name : names) {
        d->names.append(name);
    }
}

void KPropertyListData::setNames(const QVariantList &names)
{
    d->names = names;
}
