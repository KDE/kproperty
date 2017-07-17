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

#ifndef KPROPERTYSETBUFFER_H
#define KPROPERTYSETBUFFER_H

#include "KPropertySet.h"

/**
 * @brief A property set's buffer
 */
class KPROPERTYCORE_EXPORT KPropertySetBuffer : public KPropertySet
{
    Q_OBJECT

public:
    KPropertySetBuffer();

    explicit KPropertySetBuffer(const KPropertySet &set);

    ~KPropertySetBuffer() override;

    /*! Intersects with other KPropertySet.*/
    virtual void intersect(const KPropertySet& set);

protected Q_SLOTS:
    void intersectedChanged(KPropertySet& set, KProperty& prop);
    void intersectedReset(KPropertySet& set, KProperty& prop);

private:
    void init(const KPropertySet& set);

    Q_DISABLE_COPY(KPropertySetBuffer)
    class Private;
    Private * const d;
};

#endif
