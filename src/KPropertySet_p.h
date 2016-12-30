/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2004-2009 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_SET_P_H
#define KPROPERTY_SET_P_H

#include "KPropertySet.h"
#include "kproperty_debug.h"

class KPROPERTYCORE_EXPORT KPropertySetPrivate
{
public:
    explicit KPropertySetPrivate(KPropertySet *set);

    ~KPropertySetPrivate();

    //! Asccessor within the KProperty*
    inline static KPropertySetPrivate* d(KPropertySet *set) { return set->d; }
    inline static const KPropertySetPrivate* d(const KPropertySet *set) { return set->d; }

    inline uint visiblePropertiesCount() const { return m_visiblePropertiesCount; }

    inline KProperty* property(const QByteArray &name) const {
        return hash.value(name.toLower());
    }

    inline KProperty& propertyOrNull(const QByteArray &name) const
    {
        KProperty *p = property(name);
        if (p)
            return *p;
        nonConstNull.setName(0); //to ensure returned property is null
        kprWarning() << "PROPERTY" << name << "NOT FOUND";
        return nonConstNull;
    }

    void addProperty(KProperty *property, const QByteArray &group/*, bool updateSortingKey*/);

    void removeProperty(KProperty *property);

    void clear();

    inline int count() const { return list.count(); }

    inline bool isEmpty() const { return list.isEmpty(); }

    inline QByteArray groupForProperty(const KProperty *property) const {
        return groupForProperties.value(const_cast<KProperty*>(property));
    }

    inline void addPropertyToGroup(KProperty *property, const QByteArray &groupLower) {
        groupForProperties.insert(property, groupLower);
    }

    inline void removePropertyFromGroup(KProperty *property) {
        groupForProperties.remove(property);
    }

    //! Copy all attributes except complex ones
    void copyAttributesFrom(const KPropertySetPrivate &other);

    //! Copy all properties from the other set
    void copyPropertiesFrom(
        const QList<KProperty*>::ConstIterator& constBegin,
        const QList<KProperty*>::ConstIterator& constEnd, const KPropertySet & set);

    /*! Add property to a group.*/
    void addToGroup(const QByteArray &group, KProperty *property);

    /*! Remove property from a group.*/
    void removeFromGroup(KProperty *property);

    /*! Used to declare that \a property wants to be informed
     that the set has been cleared (all properties are deleted) */
    void informAboutClearing(bool* cleared);

    /*! Helper for Private class. */
    void addRelatedProperty(KProperty *p1, KProperty *p2) const;

    inline QList<KProperty*>::ConstIterator listConstIterator() const {
        return list.constBegin();
    }

    inline QList<KProperty*>::ConstIterator listConstEnd() const {
        return list.constEnd();
    }

    /*! @return index of property @a property within its parent or group. */
    int indexOfProperty(const KProperty *property) const;

    /*! @return index of property @a property within its group. */
    int indexOfPropertyInGroup(const KProperty *property) const;

    KPropertySet *q;

    //groups of properties:
    // list of group name: (list of property names)
    QMap<QByteArray, QList<QByteArray>* > propertiesOfGroup;
    QList<QByteArray>  groupNames;
    QHash<QByteArray, QString>  groupCaptions;
    QHash<QByteArray, QString>  groupIconNames;
    // map of property: group

    bool ownProperty;
    bool readOnly = false;
    QByteArray prevSelection;

    mutable KProperty nonConstNull;

private:
    //! A list of properties, preserving their order, owner of KProperty objects
    QList<KProperty*> list;
    //! A hash of properties in form name -> property
    QHash<QByteArray, KProperty*> hash;
    QHash<KProperty*, QByteArray> groupForProperties;
    uint m_visiblePropertiesCount = 0; //!< Cache for optimization,
                                       //!< used by @ref bool KPropertySet::hasVisibleProperties()
    //! Used in KPropertySetPrivate::informAboutClearing(bool&) to declare that the property wants
    //! to be informed that the set has been cleared (all properties are deleted)
    bool* m_informAboutClearing = nullptr;
};

#endif
