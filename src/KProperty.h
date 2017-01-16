/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2004-2016 Jarosław Staniek <staniek@kde.org>

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
#include <QDebug>

#include "kpropertycore_export.h"

/*! \brief Namespace for a set of classes implementing generic properties framework.

 Main classes of this framework are:
  - KProperty, representing a single property with its own type and value
  - KPropertySet, a set of properties
  - KPropertyEditorView, a widget for displaying and editing properties provided by a KPropertySet object.
    Every property has its own row displayed within the editor view.
    The editor view enables editing of property values.

 The KProperty framework also supports adding composed and property types and custom property editor types.

 Take a look at the example application, available in the examples/ directory.

 @author Cedric Pasteur <cedric.pasteur@free.fr>
 @author Alexander Dymo <cloudtemple@mskat.net>
 @author Jarosław Staniek <staniek@kde.org>
*/

class KComposedPropertyInterface;
class KPropertySet;
class KPropertySetPrivate;

/*! Data container for properties of list type. */
class KPROPERTYCORE_EXPORT KPropertyListData
{
public:
    /*! Data container for list-value property.
     We will be able to choose an item from this list. */
    KPropertyListData(const QStringList& keys_, const QStringList& names_);
    KPropertyListData(const QList<QVariant> keys_, const QStringList& names_);
    KPropertyListData();
    ~KPropertyListData();

    void setKeysAsStringList(const QStringList& list);
    QStringList keysAsStringList() const;

    /*! The string list containing all possible keys for this property
     or NULL if this is not a property of type 'list'. The values in this list are ordered,
     so the first key element is associated with first element from
     the 'names' list, and so on. */
    QList<QVariant> keys;
//   QStringList keys;

//! @todo what about using QValueList<QVariant> here too?
    /*! The list of translated names that will be visible on the screen.
     First value is referenced by first key, and so on. */
    QStringList names;

//unused for now   /*! True (the default), if the list has fixed number of possible
//unused for now    items (keys). If this is false, user can add or enter own values. */
//unused for now   bool fixed;
};

/*! \brief The base class representing a single property

  KProperty object can hold a property of given type supported by QVariant. Properties of custom types
  can be also created, see using KPropertyFactory. Composed or custome properties
  are not created using subclassing of KProperty but using @ref KComposedPropertyInterface.

  Each property stores old value to allows undoing that reverts the value to the old one.
  Property has a non-empty name (a QByteArray), a caption that is user-visible translated string
  displayed in property editor. Description is a translatable string that can be specified too
  in order to further explain meaning of the property.

  Propery also supports setting arbitrary number of options using KProperty::setOption() that allow
  to customize look or behavior of the property in the editor.

  @code
  // Creating a simple property:
  KProperty *property = new KProperty(name, value, caption, description);
  // name is a QByteArray, value is whatever type QVariant supports

  // Creating a valueFromList property matching keys with names:
  QStringList keys, names;
  keys << "one" << "two" << "three"; // possible values of the property
  // Names (possibly translated) shown in the editor instead of the keys
  names << tr("One") << tr("Two") << tr("Three");
  property = new KProperty(name, keys, names, "two", caption);

  // Creating a valueFromList property matching QVariant keys with names:
  QValueList<QVariant> variantKeys;
  variantKeys << 1 << 2 << 3;
  KPropertyListData *listData = new KPropertyListData(variantKeys, names);
  propertySet->addProperty(new KProperty("List", listData, "otheritem", "List"));
  @endcode

  @note Sometimes it makes sense to split property captions that have with more words to multiple lines
  using a newline character, e.g. "Allow Zero Size" to "Allow Zero\nSize".
  This is suitable especially for the needs of property editor which can offer only limited area.
  The text of property caption containing newline characters is available in its original form using
  KProperty::captionForDisplaying(). KProperty::caption() returns modified caption text in which
  the newline characters are substituted with spaces and any trailing and leading whitespace is removed.

  \author Cedric Pasteur <cedric.pasteur@free.fr>
  \author Alexander Dymo <cloudtemple@mskat.net>
  \author Jarosław Staniek <staniek@kde.org>
*/
class KPROPERTYCORE_EXPORT KProperty
{
public:
    /*! Defines types of properties.
     Properties defined by plugins should have a type number >= UserDefined .*/
    enum Type {
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

        UserDefined = 4000            /**<plugin defined properties should start here*/
    };

    /*! Constructs a null property. */
    KProperty();

    /*! Constructs property of simple type.
     If \a caption contains newline characters, caption() will return \a caption with substituted
     these with spaces. captionForDisplaying() is used to get original caption text usable
     (with newline, if any) for displaying within a property editor. */
    explicit KProperty(const QByteArray &name, const QVariant &value = QVariant(),
             const QString &caption = QString(), const QString &description = QString(),
             int type = Auto, KProperty* parent = 0);

    /*! Constructs property of \ref ValueFromList type. */
    KProperty(const QByteArray &name, const QStringList &keys, const QStringList &strings,
             const QVariant &value = QVariant(),
             const QString &caption = QString(), const QString &description = QString(),
             int type = ValueFromList, KProperty* parent = 0);

    /*! Constructs property of \ref ValueFromList type.
     This is overload of the above ctor added for convenience. */
    KProperty(const QByteArray &name, KPropertyListData* listData,
             const QVariant &value = QVariant(),
             const QString &caption = QString(), const QString &description = QString(),
             int type = ValueFromList, KProperty* parent = 0);

    /*! Constructs a deep copy of \a prop property. */
    KProperty(const KProperty &prop);

    ~KProperty();

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
     these are replaced by spaces. captionForDisplaying() can be used to access the original caption
     text passed here.*/
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

    void childValueChanged(KProperty *child, const QVariant &value, bool rememberOldValue);

    //! Sets value of the property to @a value
    //! @todo 4.0 BCI: replace with bool setValue(QVariant, ValueOptions)
    void setValue(const QVariant &value, bool rememberOldValue = true, bool useComposedProperty = true);

    //! Option for handling values
    //! @since 3.1
    enum ValueOption {
        RememberOldValue = 1,   //!< Remeber old value before setting a new one
        UseComposedProperty = 2 //!< Use composed property when comparing values
    };
    Q_DECLARE_FLAGS(ValueOptions, ValueOption)

    /**
     * Sets value of the property
     *
     * @overload setValue(const QVariant &, bool rememberOldValue, bool useComposedProperty)
     * @param value Value to set.
     * @param changed Pointer to value that will be set to true if the value has been assigned. Can be @a nullptr.
     * @param valueOptions Options to use when setting the value.
     * @since 3.1
     * @todo 4.0 BCI: replace with bool setValue(QVariant, ValueOptions)
     */
    void setValue(const QVariant &value, bool *changed,
                  ValueOptions valueOptions = ValueOptions(RememberOldValue | UseComposedProperty));

    /**
     * @return true if value of this property is equal to specified value
     *
     * Takes type into account.
     * @param value Value to compare.
     * @param valueOptions Options to use when comparing. The @c RememberOldValue flag is ignored.
     * @since 3.1
     */
    bool valueEqualsTo(const QVariant &value, ValueOptions valueOptions = UseComposedProperty) const;

    /*! Resets the value of the property to the old value.
     @see oldValue() */
    void resetValue();

    /*! \return the qstring-to-value correspondence list of the property.
     used to create comboboxes-like property editors.*/
    KPropertyListData* listData() const;

    /*! Sets the qstring-to-value correspondence list of the property.
    This is used to create comboboxes-like property editors.*/
    void setListData(KPropertyListData* list);

    /*! Sets the string-to-value correspondence list of the property.
     This is used to create comboboxes-like property editors.
     This is overload of the above ctor added for convenience. */
    void setListData(const QStringList &keys, const QStringList &names);

    /*! Sets icon name to \a name for this property. Icons are optional and are used e.g.
     in property editor - displayed at the left hand. */
    void setIconName(const QString &name);

    /*! \return property icon's name. Can be empty. */
    QString iconName() const;

    /*! \return a list of all children for this property, or NULL of there
     is no children for this property */
    const QList<KProperty*>*  children() const;

    /*! \return a child property for \a name, or NULL if there is no property with that name. */
    KProperty* child(const QByteArray &name);

    /*! \return parent property for this property, or NULL if there is no parent property. */
    KProperty* parent() const;

    /*! \return the composed property for this property, or NULL if there was
    no composed property defined. */
    KComposedPropertyInterface* composedProperty() const;

    /*! Sets composed property \a prop for this property. */
    void setComposedProperty(KComposedPropertyInterface *prop);

    /*! \return true if this property is null. Null properties have empty names. */
    bool isNull() const;

    //! \return true if this property value is changed.
    bool isModified() const;

    //! Clears "modified" flag, so isModified() will return false.
    void clearModifiedFlag();

    /*! \return true if the property is read-only when used in a property editor.
     @c false by default.
     The property can be read-write but still not editable for the user if the parent property set's
     read-only flag is set.
     @see KPropertySet::isReadOnly() */
    bool isReadOnly() const;

    /*! Sets this property to be read-only.
     @see isReadOnly() */
    void setReadOnly(bool readOnly);

    /*! \return true if the property is visible.
     Only visible properties are displayed by the property editor view. */
    bool isVisible() const;

    /*! Sets the visibility flag.*/
    void setVisible(bool visible);

    /*! \return true if the property can be saved to a stream, xml, etc.
    There is a possibility to use "GUI" properties that aren't
    stored but used only in a GUI.*/
    bool isStorable() const;

    /*! Sets "storable" flag for this property. @see isStorable() */
    void setStorable(bool storable);

    //! Synchronization policy for property values
    //! @since 3.1
    enum class ValueSyncPolicy {
        Editor,   //!< Allow to synchronize by the property editor using its valueSync setting (default)
        FocusOut, //!< Synchronize the value when focus is out of the editor widget for this property
                  //!< or when the user presses the Enter key
        Auto      //!< Synchronize automatically as soon as the editor widget for this property signals
                  //! (using commitData) that the value has been changed, e.g. when the user types
                  //! another letter in a text box
    };

    //! @return synchronization policy for property values of this property
    //! @since 3.1
    ValueSyncPolicy valueSyncPolicy() const;

    //! Sets synchronization policy for property values of this property
    //! See ValueSyncPolicy for details.
    //! @since 3.1
    void setValueSyncPolicy(ValueSyncPolicy policy);

    /*! Sets value \a val for option \a name.
     Options are used to override default settings of individual properties.
     This is most visible in property editor widget.

    Currently supported options are:
    <ul><li> min: integer value describing minimum value for properties of integer and
             double types. The default is 0.</li>
    <li> minValueText: user-visible translated string to be displayed in editor for integer
         type when minimum is set for the property.
         @see QAbstractSpinBox::specialValueText</li>
    <li> max: integer describing minimum value for properties of integer type. Default is 0xffff.</li>
    <li> precision: integer value describing the number of decimals after the decimal point
         for double type.</li>
    <li> step: integer describing the size of the step that is taken when the user hits
         the up or down button of editor for double type.</li>
    <li> 3State: boolean value used for boolean type; if @c true, the editor becomes a combobox
         (instead of checkable button) and accepts the third "null" state.</li>
    <li> yesName: user-visible translated string used for boolean type (both 2- and 3-state)
         to visually represent the "true" value. If not present, tr("Yes") is used.</li>
    <li> noName: user-visible translated string used for boolean type (both 2- and 3-state)
         to visually represent the "false" value. If not present, tr("No") is used.</li>
    <li> 3rdStateName: user-visible translated string used for boolean type (both 2- and 3-state)
         to visually represent the third "null" value. If not present, tr("None") is used.</li>
    <li> nullName: user-visible translated string used for boolean type to display the "null"
         value, if and only if the property accepts two states (i.e. when "3State" option
         is @c false). If the "nullName" option is not set, null values are displayed as
         @c false.</li>
    <li> extraValueAllowed: boolean value, if @c true the user is able to manually add extra
         values to a combobox.</li>
    <li> fileMode: string value that describes what objects may select in the file dialog
         of the url editor:
         <ul>
         <li>"dirsOnly": only support and display directories;
             @see QFileDialog::ShowDirsOnly QFileDialog::Directory</li>
         <li>"existingFile": only allow to select one existing file;
             @see QFileDialog::ExistingFile</li>
         <li>Any other value: any file is supported, whether it exists or not
             @see QFileDialog::AnyFile</li>
         </ul>
         </li>
    <li> confirmOverwrites: boolean value, if @c true, user will be asked for confirmation
         of file overwriting in the url editor. @c false by default.
         @note The line edit does not validate the content.</li>
    <li> multiLine: boolean value used for string type. If @c true, a multi-line
         QPlainTextEdit-based widget is used for editor; otherwise a single-line QLineEdit
         widget is used. @c false by default. Added in version 3.1.</li>
    </ul>*/
    void setOption(const char* name, const QVariant& val);

    /*! @brief Returns value of given option
     * If the option @a name is missing and parent property is present (see parent()),
     * parent property is checked. If the parent property offers the option, the value
     * is returned. If it is not present there, @a defaultValue value is returned.
     * Looking at parent property is available since 3.1.
     * @note The lookup is performed recursively, first in parent, then grand parent, etc.
     * @see setOption
     */
    QVariant option(const char* name, const QVariant& defaultValue = QVariant()) const;

    /*! @brief Returns @c true if at least one option is specified for this property
     * If there are no options defined @c true can be still returned if parent property
     * is present and it has at least one option specified.
     * Looking at parent property is available since 3.1.
     * @note The lookup is performed recursively, first in parent, then grand parent, etc.
     */
    bool hasOptions() const;

    /*! Equivalent to setValue(const QVariant &) */
    KProperty& operator= (const QVariant& val);

    /*! Assigns a deep copy of all attributes of \a property to this property. */
    KProperty& operator= (const KProperty &property);

    /*! Compares two properties.*/
    bool operator ==(const KProperty &prop) const;

#if 0
    /*! \return a key used for sorting.
     Usually its set by KPropertySet::addProperty() and KProperty::addChild() to a unique value,
     so that this property can be sorted in a property editor in original order.
     \see EditorItem::compare() */
    int sortingKey() const;
#endif

private:
    class Private;
    Private * const d;

    friend class KPropertySet;
    friend class KPropertySetPrivate;
    friend class KPropertyBuffer;
    friend KPROPERTYCORE_EXPORT QDebug operator<<(QDebug dbg, const KProperty &p);
};

//! qDebug() stream operator. Writes property @a p to the debug output in a nicely formatted way.
KPROPERTYCORE_EXPORT QDebug operator<<(QDebug dbg, const KProperty &p);

Q_DECLARE_OPERATORS_FOR_FLAGS(KProperty::ValueOptions)

#endif
