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

#ifndef KPROPERTY_PROPERTY_H
#define KPROPERTY_PROPERTY_H

#include <QVariant>
#include <QStringList>
#include <QByteArray>
#include "koproperty_global.h"

/*! \brief Namespace for a set of classes implementing generic properties framework.

 Main classes of this framework are:
  - Property, representing a single property with its own type and value
  - Set, a set of properties
  - Editor, a widget for displaying and editing properties provided by a Set object.
    Every property has its own row displayed using EditorItem object, within Editor widget.
    Widget class provides editing feature for EditorItem objects if a user selects a given item.

 KoProperty framework also supports adding composed and property types
 and custom property editor types.

 Take a look at the test application, available in koproperty/test/ directory 
 to see example uses of the framework.

 @author Cedric Pasteur <cedric.pasteur@free.fr>
 @author Alexander Dymo <cloudtemple@mskat.net>
 @author Jarosław Staniek <staniek@kde.org>
*/
namespace KoProperty
{

class PropertyPrivate;
class ComposedPropertyInterface;
class Set;

/*! PropertyType.
Integers that represent the type of the property. Plugin defined properties
should have a type number >= UserDefined .*/
enum PropertyType {
    //standard supported QVariant types
    Auto = 0x00ffffff,
    Invalid = QVariant::Invalid,
    BitArray = QVariant::BitArray,
    Bitmap = QVariant::Bitmap,
    Bool = QVariant::Bool,
    Brush = QVariant::Brush,
    ByteArray = QVariant::ByteArray,
    Char = QVariant::Char,
    Color = QVariant::Color,
    Cursor = QVariant::Cursor,
    Date = QVariant::Date,
    DateTime = QVariant::DateTime,
    Double = QVariant::Double,
    Font = QVariant::Font,
    Icon = QVariant::Icon,
    Image = QVariant::Image,
    Int = QVariant::Int,
    KeySequence = QVariant::KeySequence,
    Line = QVariant::Line,
    LineF = QVariant::LineF,
    List = QVariant::List,
    Locale = QVariant::Locale,
    LongLong = QVariant::LongLong,
    Map = QVariant::Map,
    Matrix = QVariant::Matrix,
    Transform = QVariant::Transform,
    Palette = QVariant::Palette,
    Pen = QVariant::Pen,
    Pixmap = QVariant::Pixmap,
    Point = QVariant::Point,
    PointF = QVariant::PointF,
    Polygon = QVariant::Polygon,
    Rect = QVariant::Rect,
    RectF = QVariant::RectF,
    RegExp = QVariant::RegExp,
    Region = QVariant::Region,
    Size = QVariant::Size,
    SizeF = QVariant::SizeF,
    SizePolicy = QVariant::SizePolicy,
    String = QVariant::String,
    StringList = QVariant::StringList,
    TextFormat = QVariant::TextFormat,
    TextLength = QVariant::TextLength,
    Time = QVariant::Time,
    UInt = QVariant::UInt,
    ULongLong = QVariant::ULongLong,
    Url = QVariant::Url,

    //predefined custom types
    ValueFromList = 1000          /**<string value from a list*/,
    Symbol                        /**<unicode symbol code*/,
    FontName                      /**<font name, e.g. "times new roman"*/,
    FileURL                       /**<url of a file*/,
    PictureFileURL                /**<url of a pixmap*/,
    DirectoryURL                  /**<url of a directory*/,
    LineStyle                     /**<line style*/,

    // Child property types
/*    Size_Height = 3001,
    Size_Width,
    Point_X,
    Point_Y,
    Rect_X,
    Rect_Y,
    Rect_Width,
    Rect_Height,
    SizePolicy_HorizontalPolicy,
    SizePolicy_VerticalPolicy,
    SizePolicy_HorizontalStretch,
    SizePolicy_VerticalStretch,*/

    UserDefined = 4000            /**<plugin defined properties should start here*/
};

/*! \brief The base class representing a single property

  It can hold a property of any type supported by QVariant. You can also create you own property
  types (see Using Custom Properties in Factory doc). As a consequence, do not subclass Property,
  use \ref ComposedPropertyInterface instead. \n
  Each property stores old value to allow undo. It has a name (a QByteArray), a caption (i18n'ed name
  shown in Editor) and a description (also i18n'ed). \n
  It also supports setting arbitrary number of options (of type option=value).
  See Editor for a list of options, and their meaning.

  \code
  // To create a property
  property = Property(name, value, caption, description); // name is a QByteArray,
  // value is whatever type QVariant supports

  // To create a valueFromList property (matching strings with strings)
  QStringList keys, strings;
  keys << "one" << "two" << "three"; // possible values of the property
  // Strings (possibly i18n-ed) shown in the editor instead of the values
  strings << i18n("One") << i18n("Two") << i18n("Three");
  property = Property(name, keys, strings, "two", caption);

  // To create a valueFromList property (matching strings with QVariant)
  QValueList<QVariant> keys2;
  keys2.append(1);
  keys2.append(2);
  keys2.append(3);
  Property::ListData listData(keys2, strings);
  m_set->addProperty(new Property("List2", listData, "otheritem", "List2"), group);
  \endcode

  Note that you need to use QVariant(bool, int) to create a boolean property value.
  See QVariant docs for more details.

  Sometimes, for longer property captions or these with more words, e.g. "Allow Zero Size",
  its usable to provide newline characters, e.g. "Allow Zero\nSize".
  If caption argument of the constructors contains newline characters,
  caption() will return this text with substituted these characters with spaces.
  In such cases, captionForDisplaying() is used to get the original caption text usable
  (with newline, if any) for displaying within a property editor.

  \author Cedric Pasteur <cedric.pasteur@free.fr>
  \author Alexander Dymo <cloudtemple@mskat.net>
  \author Jarosław Staniek <staniek@kde.org>
*/
class KOPROPERTY_EXPORT Property
{
public:
    /*! Data container for properties of list type. */
    class KOPROPERTY_EXPORT ListData
    {
    public:
        /*! Data container for list-value property.
         We will be able to choose an item from this list. */
        ListData(const QStringList& keys_, const QStringList& names_);
        ListData(const QList<QVariant> keys_, const QStringList& names_);
        ListData();
        ~ListData();

        void setKeysAsStringList(const QStringList& list);
        QStringList keysAsStringList() const;

        /*! The string list containing all possible keys for this property
         or NULL if this is not a property of type 'list'. The values in this list are ordered,
         so the first key element is associated with first element from
         the 'names' list, and so on. */
        QList<QVariant> keys;
//   QStringList keys;

//! @todo what about using QValueList<QVariant> here too?
        /*! The list of i18n'ed names that will be visible on the screen.
         First value is referenced by first key, and so on. */
        QStringList names;

//unused for now   /*! True (the default), if the list has fixed number of possible
//unused for now    items (keys). If this is false, user can add or enter own values. */
//unused for now   bool fixed;
    };

    /*! Constructs a null property. */
    Property();

    /*! Constructs property of simple type.
     If \a caption contains newline characters, caption() will return \a caption with substituted
     these with spaces. captionForDisplaying() is used to get original caption text usable
     (with newline, if any) for displaying within a property editor. */
    explicit
    Property(const QByteArray &name, const QVariant &value = QVariant(),
             const QString &caption = QString(), const QString &description = QString(),
             int type = Auto, Property* parent = 0);

    /*! Constructs property of \ref ValueFromList type. */
    Property(const QByteArray &name, const QStringList &keys, const QStringList &strings,
             const QVariant &value = QVariant(),
             const QString &caption = QString(), const QString &description = QString(),
             int type = ValueFromList, Property* parent = 0);

    /*! Constructs property of \ref ValueFromList type.
     This is overload of the above ctor added for convenience. */
    Property(const QByteArray &name, ListData* listData,
             const QVariant &value = QVariant(),
             const QString &caption = QString(), const QString &description = QString(),
             int type = ValueFromList, Property* parent = 0);

    /*! Constructs a deep copy of \a prop property. */
    Property(const Property &prop);

    ~Property();

    /*! \return the internal name of the property (that's used in List).*/
    QByteArray name() const;

    /*! Sets the internal name of the property.*/
    void setName(const QByteArray &name);

    /*! \return the caption of the property.*/
    QString caption() const;

    /*! \return the caption text of the property for displaying.
     It is similar to caption() but if the property caption contains newline characters,
     these are not substituted with spaces. */
    QString captionForDisplaying() const;

    /*! Sets the name of the property. If the caption contains newline characters,
     these are replaced by spaces. You can use captionForDisplaying()
     to access the original caption text you passed here.*/
    void setCaption(const QString &caption);

    /*! \return the description of the property.*/
    QString description() const;

    /*! Sets the description of the property.*/
    void setDescription(const QString &description);

    /*! \return the type of the property.*/
    int type() const;

    /*! Sets the type of the property.*/
    void setType(int type);

    /*! \return the value of the property.*/
    QVariant value() const;

    /*! Gets the previous property value.*/
    QVariant oldValue() const;

    void childValueChanged(Property *child, const QVariant &value, bool rememberOldValue);

    /*! Sets the value of the property.*/
    void setValue(const QVariant &value, bool rememberOldValue = true, bool useComposedProperty = true);

    /*! Resets the value of the property to the old value.
     @see oldValue() */
    void resetValue();

    /*! \return the qstring-to-value correspondence list of the property.
     used to create comboboxes-like property editors.*/
    ListData* listData() const;

    /*! Sets the qstring-to-value correspondence list of the property.
    This is used to create comboboxes-like property editors.*/
    void setListData(ListData* list);

    /*! Sets the string-to-value correspondence list of the property.
     This is used to create comboboxes-like property editors.
     This is overload of the above ctor added for convenience. */
    void setListData(const QStringList &keys, const QStringList &names);

    /*! Sets icon by \a name for this property. Icons are optional and are used e.g.
     in property editor - displayed at the left hand. */
    void setIcon(const QString &icon);

    /*! \return property icon's string. Can be empty. */
    QString icon() const;

    /*! \return a list of all children for this property, or NULL of there
     is no children for this property */
    const QList<Property*>*  children() const;

    /*! \return a child property for \a name, or NULL if there is no property with that name. */
    Property* child(const QByteArray &name);

    /*! \return parent property for this property, or NULL if there is no parent property. */
    Property* parent() const;

    /*! \return the composed property for this property, or NULL if there was
    no composed property defined. */
    ComposedPropertyInterface* composedProperty() const;

    /*! Sets composed property \a prop for this property. */
    void setComposedProperty(ComposedPropertyInterface *prop);

    /*! \return true if this property is null. Null properties have empty names. */
    bool isNull() const;

    //! \return true if this property value is changed.
    bool isModified() const;

    //! Clears "modified" flag, so isModified() will return false.
    void clearModifiedFlag();

    /*! \return true if the property is read-only.
     The property can be read-write but still not editable because the property
     set containing it may be set to read-only.
     By default the property is read-write.
     See Set::isReadOnly() for more details. */
    bool isReadOnly() const;

    /*! Sets this property to be read-only.
     @see isReadOnly() */
    void setReadOnly(bool readOnly);

    /*! \return true if the property is visible.*/
    bool isVisible() const;

    /*! Set the visibility.*/
    void setVisible(bool visible);

    /*! \return true if the property can be saved to a stream, xml, etc.
    There is a possibility to use "GUI" properties that aren't
    stored but used only in a GUI.*/
    bool isStorable() const;

    /*! Sets "storable" flag for this property. @see isStorable() */
    void setStorable(bool storable);

    /*! \return 1 if the property should be synced automatically in Property Editor
    as soon as editor contents change (e.g. when the user types text).
    If autoSync() == 0, property value will be updated when the user presses Enter
    or when another editor gets the focus.
    Property follows Property Editor's global rule if autoSync() !=0 and !=1 (the default).
    */
    int autoSync() const;

    /*! if \a sync is 1, the property will be synced automatically in the Property Editor
    as soon as editor's contents change (e.g. when the user types text).
    If \a sync is 0, property value will be updated when the user presses
    Enter or when another editor gets the focus.
    Property follows Property Editor's global rule if sync !=0 and !=1 (the default).
    */
    void setAutoSync(int sync);

    /*! Sets value \a val for option \a name.
     Options are used to describe additional details for property behaviour,
     e.g. within Editor. See Editor ctor documentation for
     the list of supported options.
    */
    void setOption(const char* name, const QVariant& val);

    /*! \return a value for option \a name or null value if there is no such option set. 
     If there is no such value, @a defaultValue is returned. */
    QVariant option(const char* name, const QVariant& defaultValue = QVariant()) const;

    /*! \return true if at least one option is defined for this property. */
    bool hasOptions() const;

    /*! Equivalent to setValue(const QVariant &) */
    const Property& operator= (const QVariant& val);

    /*! Assigns a deep copy of all attributes of \a property to this property. */
    const Property& operator= (const Property &property);

    /*! Compares two properties.*/
    bool operator ==(const Property &prop) const;

#if 0
    /*! \return a key used for sorting.
     Usually its set by Set::addProperty() and Property::addChild() to a unique value,
     so that this property can be sorted in a property editor in original order.
     \see EditorItem::compare() */
    int sortingKey() const;
#endif

protected:
    /*! Adds \a prop as a child of this property.
     The children will be owned by this property. */
    void addChild(Property *prop);

    /*! Adds \a set to this property. */
    void addSet(Set *set);

#if 0
    /*! Sets a key used for sorting. */
    void setSortingKey(int key);
#endif

    /*! \return a list of related properties for this property. */
    const QList<Property*>* related() const;

    /*! Adds related property for this property. */
    void addRelatedProperty(Property *property);

    /*! This method emits the \a Set::propertyChanged() signal for all
    sets this property is registered in. The \a value() method above
    calls this method of the value changed. */
    void emitPropertyChanged();

    /*! Outputs debug string for this property. */
    void debug() const;

    /*! For operator <<. */
    const QMap<QByteArray, QVariant>& options() const;

    //! @internal
    PropertyPrivate * const d;

    friend class Set;
    friend class SetPrivate;
    friend class Buffer;
    friend class ComposedPropertyInterface;
    friend KOPROPERTY_EXPORT QDebug operator<<(QDebug dbg, const Property &p);
};

//! kDebug() stream operator. Writes property @a p to the debug output in a nicely formatted way.
KOPROPERTY_EXPORT QDebug operator<<(QDebug dbg, const Property &p);

}

#endif
