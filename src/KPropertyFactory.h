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

//! An interface for for composed property handlers
/*! KComposedPropertyInterface should be subclassed to override the behaviour of a property type.
  In the constructor child property objects should be created if there are any.
  Then functions handling values should be implemented.

  Example implementation of composed properties can be found in editors/ directory.
*/
class KPROPERTYCORE_EXPORT KComposedPropertyInterface
{
public:
    explicit KComposedPropertyInterface(KProperty *parent);
    virtual ~KComposedPropertyInterface();

    /*! This function modifies the child properties for parent value @a value.
     It is called by @ref KProperty::setValue() when
     the property is composed.
    It is not necessary to modify the property value, it is done by KProperty.
    @note When this method is called valueOptions should not have the KProperty::ValueOption::UseComposedProperty
          flag set or else there will be infinite recursion. KProperty class makes sure this is the case
          but custom property developers should take care about this too.
    */
    virtual void setValue(KProperty *property, const QVariant &value, KProperty::ValueOptions valueOptions) = 0;

    void childValueChangedInternal(KProperty *child, const QVariant &value, KProperty::ValueOptions valueOptions);

    bool childValueChangedEnabled() const;

    void setChildValueChangedEnabled(bool set);

    /*! @return true if values @a first and @a second are equal. Used in KProperty::setValue()
     to check if value has been changed before setting value.
     Default implementation uses operator==. */
    inline virtual bool valuesEqual(const QVariant &first, const QVariant &second) { return first == second; }

protected:
    virtual void childValueChanged(KProperty *child, const QVariant &value, KProperty::ValueOptions valueOptions) = 0;

    /*! This method emits the \a KPropertySet::propertyChanged() signal for all
    sets our parent-property is registered in. */
    void emitPropertyChanged();

private:
    Q_DISABLE_COPY(KComposedPropertyInterface)
    class Private;
    Private * const d;
};

class KPROPERTYCORE_EXPORT KComposedPropertyCreatorInterface
{
public:
    KComposedPropertyCreatorInterface();

    virtual ~KComposedPropertyCreatorInterface();

    virtual KComposedPropertyInterface* createComposedProperty(KProperty *parent) const = 0;

private:
    Q_DISABLE_COPY(KComposedPropertyCreatorInterface)
    class Private;
    Private * const d;
};

//! Creator returning composed property object
template<class ComposedProperty>
class KComposedPropertyCreator : public KComposedPropertyCreatorInterface
{
public:
    KComposedPropertyCreator() : KComposedPropertyCreatorInterface() {}

    ~KComposedPropertyCreator() override {}

    ComposedProperty* createComposedProperty(KProperty *parent) const override {
        return new ComposedProperty(parent);
    }

    Q_DISABLE_COPY(KComposedPropertyCreator)
};

//! Provides a specialized conversion of value to string depending on type
class KPROPERTYCORE_EXPORT KPropertyValueDisplayInterface
{
public:
    KPropertyValueDisplayInterface();

    virtual ~KPropertyValueDisplayInterface();

    virtual QString propertyValueToString(const KProperty* property, const QLocale &locale) const
        { return valueToString(property->value(), locale); }

    virtual QString valueToString(const QVariant& value, const QLocale &locale) const = 0;

    //! Maximum length of strings to display in valueToString(), propertyValueToString()
    //! and KPropertyValuePainterInterface::paint().
    //! Used to avoid inefficiences. Equal to 250.
    //! @todo Make configurable?
    static int maxStringValueLength();

    //! @return @a value converted to string using QVariant::toString(), truncated if it's longer than @ref maxStringValueLength()
    //! @see maxStringValueLength();
    static QString valueToLocalizedString(const QVariant& value);

private:
    Q_DISABLE_COPY(KPropertyValueDisplayInterface)
    class Private;
    Private * const d;
};

class KPROPERTYCORE_EXPORT KPropertyFactory
{
public:
    KPropertyFactory();

    virtual ~KPropertyFactory();

    QHash<int, KComposedPropertyCreatorInterface*> composedPropertyCreators() const;
    QHash<int, KPropertyValueDisplayInterface*> valueDisplays() const;

    void addComposedPropertyCreator(int type, KComposedPropertyCreatorInterface* creator);

    void addDisplay(int type, KPropertyValueDisplayInterface *display);

protected:
    void addComposedPropertyCreatorInternal(int type,
        KComposedPropertyCreatorInterface* creator, bool own = true);

    //! Adds value-to-text converted @a painter for type @a type.
    //! The converter becomes owned by the factory.
    void addDisplayInternal(int type, KPropertyValueDisplayInterface *display, bool own = true);

    Q_DISABLE_COPY(KPropertyFactory)
    class Private;
    Private * const d;
};

class KPROPERTYCORE_EXPORT KPropertyFactoryManager : public QObject
{
    Q_OBJECT
public:
    KComposedPropertyInterface* createComposedProperty(KProperty *parent);

    //! Registers factory @a factory. It becomes owned by the manager.
    void registerFactory(KPropertyFactory *factory);

    /*! \return a pointer to a factory manager instance.*/
    static KPropertyFactoryManager* self();

    bool canConvertValueToText(int type) const;

    bool canConvertValueToText(const KProperty* property) const;

    QString propertyValueToString(const KProperty* property) const;

    QString valueToString(int type, const QVariant &value) const;

    QString propertyValueToLocalizedString(const KProperty* property) const;

    QString valueToLocalizedString(int type, const QVariant &value) const;

    KPropertyFactoryManager();
    ~KPropertyFactoryManager() override;

    //! Adds function @a initFunction that will be called after the manager is created.
    //! Useful for creation custom factories.
    static void addInitFunction(void (*initFunction)());

private:
    Q_DISABLE_COPY(KPropertyFactoryManager)
    class Private;
    Private * const d;
};

#endif
