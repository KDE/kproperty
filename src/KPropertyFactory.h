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

#ifndef KPROPERTY_FACTORY_H
#define KPROPERTY_FACTORY_H

#include "KProperty.h"

#include <QObject>
#include <QVariant>
#include <QHash>

//! An interface for for composed property handlers
/*! You have to subclass KComposedPropertyInterface to override the behaviour of a property type.\n
  In the constructor, you should create the child properties (if needed).
  Then, you need to implement the functions concerning values.\n

  Example implementation of composed properties can be found in editors/ directory.
*/
class KPROPERTYCORE_EXPORT KComposedPropertyInterface
{
public:
    explicit KComposedPropertyInterface(KProperty *parent);
    virtual ~KComposedPropertyInterface();

    /*! This function modifies the child properties for parent value @a value.
     It is called by @ref Property::setValue() when
     the property is composed.
    You don't have to modify the property value, it is done by Property class.
    Note that when calling Property::setValue, you <b>need</b> to set
    useComposedProperty (the third parameter) to false, or there will be infinite recursion. */
    virtual void setValue(KProperty *property, const QVariant &value, bool rememberOldValue) = 0;

    void childValueChangedInternal(KProperty *child, const QVariant &value, bool rememberOldValue) {
      if (m_childValueChangedEnabled)
        childValueChanged(child, value, rememberOldValue);
    }

    void setChildValueChangedEnabled(bool set) { m_childValueChangedEnabled = set; }

    /*! @return true if values @a first and @a second are equal. Used in KProperty::setValue()
     to check if value has been changed before setting value.
     Default implementation uses operator==. */
    inline virtual bool valuesEqual(const QVariant &first, const QVariant &second) { return first == second; }

protected:
    virtual void childValueChanged(KProperty *child, const QVariant &value, bool rememberOldValue) = 0;

    /*! This method emits the \a Set::propertyChanged() signal for all
    sets our parent-property is registered in. */
    void emitPropertyChanged();
    bool m_childValueChangedEnabled : 1;
};

class KPROPERTYCORE_EXPORT KComposedPropertyCreatorInterface
{
public:
    KComposedPropertyCreatorInterface();

    virtual ~KComposedPropertyCreatorInterface();

    virtual KComposedPropertyInterface* createComposedProperty(KProperty *parent) const = 0;
};

//! Creator returning composed property object
template<class ComposedProperty>
class KComposedPropertyCreator : public KComposedPropertyCreatorInterface
{
public:
    KComposedPropertyCreator() : KComposedPropertyCreatorInterface() {}

    virtual ~KComposedPropertyCreator() {}

    virtual ComposedProperty* createComposedProperty(KProperty *parent) const {
        return new ComposedProperty(parent);
    }
};

class KPROPERTYCORE_EXPORT KPropertyFactory
{
public:
    KPropertyFactory();
    virtual ~KPropertyFactory();
    QHash<int, KComposedPropertyCreatorInterface*> composedPropertyCreators() const;
    void addComposedPropertyCreator( int type, KComposedPropertyCreatorInterface* creator );

protected:
    void addComposedPropertyCreatorInternal(int type,
        KComposedPropertyCreatorInterface* creator, bool own = true);

    class Private;
    Private * const d;
};

class KProperty;
class KCustomProperty;

class KPROPERTYCORE_EXPORT KPropertyFactoryManager : public QObject
{
    Q_OBJECT
public:
    KComposedPropertyInterface* createComposedProperty(KProperty *parent);

    //! Registers factory @a factory. It becomes owned by the manager.
    void registerFactory(KPropertyFactory *factory);

    /*! \return a pointer to a factory manager instance.*/
    static KPropertyFactoryManager* self();

    KPropertyFactoryManager();
    ~KPropertyFactoryManager();

    //! Adds function @a initFunction that will be called after the manager is created.
    //! Useful for creation custom factories.
    static void addInitFunction(void (*initFunction)());

private:

    class Private;
    Private * const d;
};

#endif
