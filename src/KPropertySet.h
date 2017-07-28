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

#ifndef KPROPERTY_SET_H
#define KPROPERTY_SET_H

#include <QObject>
#include <QHash>
#include <QDebug>

#include "KProperty.h"

class KPropertySetPrivate;

//! An interface for functor selecting properties.
/*! Used in Iterator. */
class KPROPERTYCORE_EXPORT KPropertySelector
{
public:
    KPropertySelector();
    virtual ~KPropertySelector();

    //! An operator implementing the functor.
    virtual bool operator()(const KProperty& prop) const = 0;

    //! Creates a deep copy of the selector.
    //! Required for proper usage of the selector.
    virtual KPropertySelector* clone() const = 0;
};

//! A class to iterate over a KPropertySet.
/*! It behaves like a QList::ConstIterator.
 Usage:
 @code  for (KPropertySetIterator it(set); it.current(); ++it) { .... }
 @endcode
 Usage with selector:
 @code  for (KPropertySetIterator it(set, MySelector()); it.current(); ++it) { .... }
 @endcode */
class KPROPERTYCORE_EXPORT KPropertySetIterator
{
public:
    //! Creates iterator for @a set set of properties.
    /*!             The properties are sorted by insertion order by default.
        Use setOrder(Iterator::Alphabetical) to have alphabetical order. */
    explicit KPropertySetIterator(const KPropertySet &set);

    //! Creates iterator for @a set set of properties.
    /*! @a selector functor is used to iterate only
        over specified properties.
        The properties are sorted by insertion order by default.
        Use setOrder(Iterator::Alphabetical) to have alphabetical order. */
    KPropertySetIterator(const KPropertySet &set, const KPropertySelector& selector);

    //! Copy constructor
    //! @since 3.1
    KPropertySetIterator(const KPropertySetIterator &set);

    ~KPropertySetIterator();

    //! Assigns @a other to this KPropertySetIterator
    //! @since 3.1
    KPropertySetIterator& operator=(const KPropertySetIterator &other);

    //! @return true if this iterator equals to @a other
    //! @since 3.1
    bool operator==(const KPropertySetIterator &other) const;

    //! @return true if this iterator does not equal to @a other
    //! @since 3.1
    inline bool operator!=(const KPropertySetIterator &other) const { return !operator==(other); }

    //! Ordering options for properties
    /*! @see setOrder() */
    enum class Order {
        Insertion,    //!< insertion order
        Alphabetical, //!< alphabetical order (case-insensitively by captions)
        AlphabeticalByName //!< alphabetical order (case-insensitively by name)
    };

    //! Sets order for properties. Restarts the iterator.
    void setOrder(Order order);

    //! @return order for properties.
    Order order() const;

    void operator++();

    inline KProperty* operator*() const { return current(); }

    KProperty* current() const;

    friend class KPropertySet;

private:
    class Private;
    Private * const d;
};

/*! \brief Set of properties
 */
class KPROPERTYCORE_EXPORT KPropertySet : public QObject
{
    Q_OBJECT
public:
    //! Constructs a new property set object.
    explicit KPropertySet(QObject *parent = nullptr);

    /*! Constructs a deep copy of \a set.
     The new object will not have a QObject parent even if \a set has such parent. */
    explicit KPropertySet(const KPropertySet& set);

    ~KPropertySet() override;

    /*! @return the number of top-level properties in the set. */
    int count() const;

    /*! @return the number of top-level properties in the set
                matching criteria defined by @a selector. */
    int count(const KPropertySelector& selector) const;

    /*! @return true if the set is empty, i.e. count() is 0; otherwise returns false. */
    bool isEmpty() const;

    /*! @return true if the set is contains visible properties. */
    bool hasVisibleProperties() const;

    /*! @return true if the set is contains properties
                matching criteria defined by @a selector. */
    bool hasProperties(const KPropertySelector& selector) const;

    /*! \return true if the set is read-only when used in a property editor.
     @c false by default.
     In a read-only property set no property can be modified by the user regardless of read-only flag
     of any property (KProperty::isReadOnly()). On the other hand if KProperty::isReadOnly() is @c true
     and KPropertySet::isReadOnly() is @c false, the property is still read-only.
     Read-only property set prevents editing in the property editor but it is still possible to change
     value or other parameters of property programatically using KProperty::setValue(),
     KProperty::resetValue(), etc. */
    bool isReadOnly() const;

    /*! \return true if the set contains property names \a name. */
    bool contains(const QByteArray &name) const;

    /*! \return property named with \a name. If no such property is found,
     null property (KProperty()) is returned. */
    KProperty& property(const QByteArray &name) const;

    /*! Accesses a property by name.
    A property reference is returned, so all property modifications are allowed.
    If there is no such property, null property (KProperty()) is returned,
    so it's good practice to use contains() if it's not known if the property exists.
    For example to set a value of a property, use:
    @code
    KPropertySet set;
    ...
    if (!set.contains("myProperty")) {
      dosomething;
    }
    set["myProperty"].setValue("My Value");
    @endcode
    @return \ref Property with given name.
    @see changeProperty(const QByteArray &, const QVariant &)
    @see changePropertyIfExists(const QByteArray &, const QVariant &)
    */
    KProperty& operator[](const QByteArray &name) const;

    /*! @return value for property named with @a name.
     If no such property is found, default value @a defaultValue is returned. */
    QVariant propertyValue(const QByteArray &name, const QVariant& defaultValue = QVariant()) const;

    /*! Creates a deep copy of \a set and assigns it to this property set. */
    KPropertySet& operator= (const KPropertySet &set);

    /*! \return the user-visible translated caption string for \a group that will
     be shown in property editor to represent \a group. If there is no special
     caption set for the group, \a group is just returned. */
    QString groupCaption(const QByteArray &group) const;

    /*! \return icon name for \a group. */
    QString groupIconName(const QByteArray &group) const;

    /**
     * @return group name for property @a propertyName
     * Empty value is returned if there is no such property.
     * @since 3.1
     */
    QByteArray groupNameForProperty(const QByteArray &propertyName) const;

    /**
     * @overload QByteArray groupNameForProperty(const QByteArray &propertyName) const
     * @since 3.1
     */
    QByteArray groupNameForProperty(const KProperty &property) const;

    /*! \return a list of all group names. The order of items is undefined. */
    QList<QByteArray> groupNames() const;

    /*! \return a list of all property names for group @a group.
     The order of items is undefined. */
    QList<QByteArray> propertyNamesForGroup(const QByteArray &group) const;

    /*! Used by property editor to preserve previous selection when this set
     is assigned again. */
    QByteArray previousSelection() const;

    /*! Prints debug output for this set. */
    void debug() const;

    //! @return property values for this set
    QMap<QByteArray, QVariant> propertyValues() const;

public Q_SLOTS:
    /*! Adds the property to the set, in the group.
     The property becomes owned by the set.
     Any name can be used for the @a group. "common" is the default for a basic top-level group. */
    void addProperty(KProperty *property, const QByteArray &group = "common");

    /*! Removes property from the set and deletes the object.
     Emits aboutToDeleteProperty before removing. */
    void removeProperty(KProperty *property);

    /*! Removes property with the given name from the set and deletes the object.
    Emits aboutToDeleteProperty() before removing.*/
    void removeProperty(const QByteArray &name);

    /*! Removes all property objects from the property set and deletes them. */
    void clear();

    /*! Change the value of property whose key is \a property to \a value.
    @see void changePropertyIfExists(const QByteArray &, const QVariant &) */
    void changeProperty(const QByteArray &property, const QVariant &value);

    /*! Change the value of property whose key is \a property to \a value
     only if it exists in the set.
     @see void changeProperty(const QByteArray &, const QVariant &) */
    void changePropertyIfExists(const QByteArray &property, const QVariant &value);

    /*! Sets @a caption as a user-visible translated string that will be shown in editor to represent
     \a group. */
    void setGroupCaption(const QByteArray &group, const QString &caption);

    /*! Sets the icon name \a icon to be displayed for \a group. */
    void setGroupIconName(const QByteArray &group, const QString& iconName);

    //! Sets previous section.
    //! @see previousSelection()
    void setPreviousSelection(const QByteArray &prevSelection);

    /*! Sets this set to be read-only.
     @see isReadOnly */
    void setReadOnly(bool readOnly);

Q_SIGNALS:
    /*! Emitted when the value of the property is changed.*/
    void propertyChanged(KPropertySet& set, KProperty& property);

    /*! @internal Exists to be sure that we emitted it before propertyChanged(),
     so editor object can handle this. */
    void propertyChangedInternal(KPropertySet& set, KProperty& property);

    /*! Emitted when the value of the property is reset.*/
    void propertyReset(KPropertySet& set, KProperty& property);

    /*! Emitted when property is about to be deleted.*/
    void aboutToDeleteProperty(KPropertySet& set, KProperty& property);

    /*! Emitted when property set object is about to be cleared (using clear()).
     This signal is also emitted from destructor before emitting aboutToBeDeleted(). */
    void aboutToBeCleared();

    /*! Emitted when property set object is about to be deleted.*/
    void aboutToBeDeleted();

    /*! Emitted when property set object's read-only flag has changed.*/
    void readOnlyFlagChanged();

protected:
    /*! Constructs a set which owns or does not own it's properties.*/
    explicit KPropertySet(bool propertyOwner);

private:
    KPropertySetPrivate * const d;
    friend class KPropertySetPrivate;
};

//! qDebug() stream operator. Writes this set to the debug output in a nicely formatted way.
KPROPERTYCORE_EXPORT QDebug operator<<(QDebug dbg, const KPropertySet &set);

#endif
