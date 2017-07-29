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

#ifndef KPROPERTY_SET_P_H
#define KPROPERTY_SET_P_H

#include "KPropertySet.h"
#include "kproperty_debug.h"

class KPROPERTYCORE_EXPORT KPropertySetPrivate
{
public:
    explicit KPropertySetPrivate(KPropertySet *set, bool isOwnProperty);

    ~KPropertySetPrivate();

    //! Asccessor within the KProperty*
    inline static KPropertySetPrivate* d(KPropertySet *set) { return set->d; }
    inline static const KPropertySetPrivate* d(const KPropertySet *set) { return set->d; }

    inline int visiblePropertiesCount() const { return m_visiblePropertiesCount; }

    inline KProperty* property(const QByteArray &name) const {
        return m_hash.value(name.toLower());
    }

    inline KProperty& propertyOrNull(const QByteArray &name) const
    {
        KProperty *p = property(name);
        if (p)
            return *p;
        m_nonConstNull.setName(nullptr); //to ensure returned property is null
        kprWarning() << "PROPERTY" << name << "NOT FOUND";
        return m_nonConstNull;
    }

    void addProperty(KProperty *property, const QByteArray &group/*, bool updateSortingKey*/);

    void removeProperty(KProperty *property);

    void clear();

    inline int count() const { return m_list.count(); }

    inline bool isEmpty() const { return m_list.isEmpty(); }

    /*! @return @c true if there are groups explicitly defined.
     In this case groups are displayed by the property editor.
     If there is only one "common" group, it means that all properties belong to this group,
     and no groups are displayed.
     @since 3.1 */
    bool hasGroups() const;

    inline QByteArray groupForProperty(const KProperty *property) const {
        return m_groupForProperties.value(const_cast<KProperty*>(property));
    }

    inline void setGroupCaption(const QByteArray &group, const QString &caption)
    {
        m_groupCaptions.insert(group.toLower(), caption);
    }

    inline void addPropertyToGroup(KProperty *property, const QByteArray &groupLower) {
        m_groupForProperties.insert(property, groupLower);
    }

    inline void removePropertyFromGroup(KProperty *property) {
        m_groupForProperties.remove(property);
    }

    //! Copy all attributes except complex ones
    void copyAttributesFrom(const KPropertySetPrivate &other);

    //! Copy all properties from the other set
    void copyPropertiesFrom(
        const QList<KProperty*>::ConstIterator& constBegin,
        const QList<KProperty*>::ConstIterator& constEnd, const KPropertySet & set);

    QList<QByteArray> groupNames() const
    {
        return m_groupNames;
    }

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
        return m_list.constBegin();
    }

    inline QList<KProperty*>::ConstIterator listConstEnd() const {
        return m_list.constEnd();
    }

    /*! @return index of property @a property within its parent or group. */
    int indexOfProperty(const KProperty *property) const;

    /*! @return index of property @a property within its group. */
    int indexOfPropertyInGroup(const KProperty *property) const;

    QString groupCaption(const QByteArray &group) const;

    inline void setGroupIconName(const QByteArray &group, const QString& iconName)
    {
        m_groupIconNames.insert(group.toLower(), iconName);
    }

    inline QString groupIconName(const QByteArray &group) const
    {
        return m_groupIconNames.value(group);
    }

    inline QByteArray previousSelection() const
    {
        return m_prevSelection;
    }

    inline void setPreviousSelection(const QByteArray &prevSelection)
    {
        m_prevSelection = prevSelection;
    }

    inline QList<QByteArray> *propertyNamesForGroup(const QByteArray &group)
    {
        return m_propertiesOfGroup.value(group);
    }

    inline QByteArray groupName(int index) const
    {
        return m_groupNames.value(index);
    }

    inline int indexOfGroup(const QByteArray &group) const
    {
        return m_groupNames.indexOf(group);
    }

    bool readOnly = false;

private:
    KPropertySet *q;

    //groups of properties:
    // list of group name: (list of property names)
    QMap<QByteArray, QList<QByteArray>* > m_propertiesOfGroup;
    QList<QByteArray> m_groupNames;
    QHash<QByteArray, QString> m_groupCaptions;
    QHash<QByteArray, QString> m_groupIconNames;
    // map of property: group

    bool m_ownProperty;
    QByteArray m_prevSelection;

    mutable KProperty m_nonConstNull;

    //! A list of properties, preserving their order, owner of KProperty objects
    QList<KProperty*> m_list;
    //! A hash of properties in form name -> property
    QHash<QByteArray, KProperty*> m_hash;
    QHash<KProperty*, QByteArray> m_groupForProperties;
    int m_visiblePropertiesCount = 0; //!< Cache for optimization,
                                       //!< used by @ref bool KPropertySet::hasVisibleProperties()
    //! Used in KPropertySetPrivate::informAboutClearing(bool&) to declare that the property wants
    //! to be informed that the set has been cleared (all properties are deleted)
    bool* m_informAboutClearing = nullptr;
};

#endif
