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

#ifndef KPROPERTY_PROPERTYLISTDATA_H
#define KPROPERTY_PROPERTYLISTDATA_H

#include "kpropertycore_export.h"

#include <QVariantList>

/**
 * @brief A data container for properties of list type.
 *
 * @since 3.1
 */
class KPROPERTYCORE_EXPORT KPropertyListData
{
public:
    KPropertyListData();

    KPropertyListData(const KPropertyListData &other);

    KPropertyListData(const QStringList& keys, const QStringList& names);

    KPropertyListData(const QVariantList &keys, const QVariantList &names);

    KPropertyListData(const QVariantList &keys, const QStringList &names);

    ~KPropertyListData();

    //! Assigns @a other to this KPropertyListData
    KPropertyListData& operator=(const KPropertyListData &other);

    //! @return true if this KPropertyListData equals to @a other
    bool operator==(const KPropertyListData &other) const;

    //! @return true if this KPropertyListData does not equal to @a other
    inline bool operator!=(const KPropertyListData &other) const { return !operator==(other); }

    /**
     * @brief A list containing all possible keys for a property
     *
     * Items of this list are ordered, so the first key element is associated with first element
     * from the 'names' list, and so on.
     */
    QVariantList keys() const;

    /**
     * @brief A list containing all possible keys for a property converted to strings
     */
    QStringList keysAsStringList() const;

    /**
     * Sets a list containing all possible keys for a property
     *
     * @note each key on the list should be unique
     */
    void setKeys(const QVariantList &keys);

    /**
     * Sets a list containing all possible keys for a property as strings
     *
     * @note each key on the list should be unique
     */
    void setKeysAsStringList(const QStringList &keys);

    /**
     * @brief The list of user-visible translated name elements
     *
     * First value is referenced by first key, and so on.
     */
    QVariantList names() const;

    /**
     * @brief The list of user-visible translated name elements as strings
     */
    QStringList namesAsStringList() const;

    /**
     * Sets a list containing all possible keys for a property
     *
     * @note each key on the list should be unique
     */
    void setNames(const QVariantList &names);

    /**
     * Sets a list containing all possible keys for a property as strings
     *
     * @note each key on the list should be unique
     */
    void setNamesAsStringList(const QStringList &names);

private:
    class Private;
    Private * const d;
};

#endif
