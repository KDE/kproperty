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

#include "KPropertySetBuffer.h"
#include "KPropertySet_p.h"
#include "KProperty_p.h"

class Q_DECL_HIDDEN KPropertySetBuffer::Private
{
public:
    Private()
    {
    }
};

KPropertySetBuffer::KPropertySetBuffer()
        : KPropertySet(false)
        , d(new Private)
{
    connect(this, SIGNAL(propertyChanged(KPropertySet&,KProperty&)),
            this, SLOT(intersectedChanged(KPropertySet&,KProperty&)));

    connect(this, SIGNAL(propertyReset(KPropertySet&,KProperty&)),
            this, SLOT(intersectedReset(KPropertySet&,KProperty&)));
}

KPropertySetBuffer::KPropertySetBuffer(const KPropertySet& set)
        : KPropertySet(false)
        , d(new Private)
{
    connect(this, SIGNAL(propertyChanged(KPropertySet&,KProperty&)),
            this, SLOT(intersectedChanged(KPropertySet&,KProperty&)));

    connect(this, SIGNAL(propertyReset(KPropertySet&,KProperty&)),
            this, SLOT(intersectedReset(KPropertySet&,KProperty&)));

    init(set);
}

KPropertySetBuffer::~KPropertySetBuffer()
{
    delete d;
}

void KPropertySetBuffer::init(const KPropertySet& set)
{
    //deep copy of set
    const QList<KProperty*>::ConstIterator itEnd(KPropertySetPrivate::d(&set)->listConstEnd());
    for (QList<KProperty*>::ConstIterator it(KPropertySetPrivate::d(&set)->listConstIterator());
        it!=itEnd; ++it)
    {
        KProperty *prop = new KProperty(*(*it));
        QByteArray group = KPropertySetPrivate::d(&set)->groupForProperty(*it);
        const QString groupCaption = set.groupCaption(group);
        setGroupCaption(group, groupCaption);
        addProperty(prop, group);
        prop->d->addRelatedProperty(*it);
    }
}

void KPropertySetBuffer::intersect(const KPropertySet& set)
{
    if (isEmpty()) {
        init(set);
        return;
    }

    const QList<KProperty*>::ConstIterator itEnd(KPropertySetPrivate::d(&set)->listConstEnd());
    for (QList<KProperty*>::ConstIterator it(KPropertySetPrivate::d(&set)->listConstIterator());
        it!=itEnd; ++it)
    {
        const QByteArray key( (*it)->name() );
        KProperty *property = KPropertySetPrivate::d(&set)->property( key );
        if (property) {
            blockSignals(true);
            (*it)->resetValue();
            (*it)->d->addRelatedProperty(property);
            blockSignals(false);
        } else {
            removeProperty(key);
        }
    }
}

void KPropertySetBuffer::intersectedChanged(KPropertySet& set, KProperty& prop)
{
    Q_UNUSED(set);
    if (!contains(prop.name()))
        return;

    const QList<KProperty*> *props = prop.d->relatedProperties;
    for (QList<KProperty*>::ConstIterator it = props->constBegin(); it != props->constEnd(); ++it) {
        (*it)->setValue(prop.value(), KProperty::DefaultValueOptions & ~KProperty::ValueOptions(KProperty::ValueOption::RememberOld));
    }
}

void KPropertySetBuffer::intersectedReset(KPropertySet& set, KProperty& prop)
{
    Q_UNUSED(set);
    if (!contains(prop.name()))
        return;

    const QList<KProperty*> *props = prop.d->relatedProperties;
    for (QList<KProperty*>::ConstIterator it = props->constBegin(); it != props->constEnd(); ++it)  {
        (*it)->setValue(prop.value(), KProperty::DefaultValueOptions & ~KProperty::ValueOptions(KProperty::ValueOption::RememberOld));
    }
}
