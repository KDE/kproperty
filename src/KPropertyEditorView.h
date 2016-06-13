/* This file is part of the KDE project
   Copyright (C) 2008-2016 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_EDITORVIEW_H
#define KPROPERTY_EDITORVIEW_H

#include "kpropertywidgets_export.h"

#include <QTreeView>

class KProperty;
class KPropertySet;

//! @brief A widget for editing properties
//! @todo review this .............
/*! Editor widgets use property options using Property::option(const char *)
    to override default behaviour of editor items.
    Currently supported options are:
    <ul><li> min: integer setting for minimum value for integer and double types.
             The default is 0.
             Set it to -1, if you want this special value to be allowed.</li>
    <li> minValueText: user-visible translated string used for integer type to set "specialValueText"
         widget's property</li>
    <li> max: integer setting for minimum value the property of integer type. Default is 0xffff.</li>
    <li> precision:  The number of decimals after the decimal point (for double types).</li>
    <li> step: the size of the step that is taken when the user hits the up
    or down buttons (for double types).</li>
    <li> 3State: boolean value used for boolean types; if true, the editor becomes a combobox
         (instead of checkable button) and accepta the third "null" state.</li>
    <li> yesName: user-visible translated string used for boolean types (both 2- and 3-state)
         to display the "true" value. If not present, tr("Yes") is used.</li>
    <li> noName: user-visible translated string used for boolean types (both 2- and 3-state)
         to display the "false" value. If not present, tr("No") is used.</li>
    <li> 3rdStateName: user-visible translated string used for boolean types (both 2- and 3-state)
         to display the 3rd state "null" value. If not present, tr("None") is used.</li>
    <li> nullName: user-visible translated string used for boolean types to display the "null" value if
         and only if the property accepts two states (i.e. when "3State" option is not true).
         If this option is not present, null values are displayed as false.</li>
    <li> extraValueAllowed: Allow the user to manually enter a value into a combobox
         that is not in the list. The entered text will be returned as opposed to a matching key.</li>
    <li> fileMode: indicates what the user may select in the file dialog for the url type's editor:
         <ul>
         <li>"dirsOnly": only support and display directories
             See QFileDialog::ShowDirsOnly and QFileDialog::Directory.</li>
         <li>"existingFile": only allow to select one existing file. See QFileDialog::ExistingFile.</li>
         <li>For any other value, supports any file, whether it exists or not. See QFileDialog::AnyFile.</li>
         </ul>
         @note The line edit does not validate the content.
         </li>
    <li>confirmOverwrites: if true, user will be asked for file overwriting by url editor; false by default.
         @note The line edit does not validate the content.
    </li>
    </ul>
 */
class KPROPERTYWIDGETS_EXPORT KPropertyEditorView : public QTreeView
{
    Q_OBJECT
public:
    /*! Creates an empty property editor with @a parent as parent widget. */
    explicit KPropertyEditorView(QWidget *parent = 0);

    ~KPropertyEditorView();

    //! Options for changeSet().
    enum SetOption {
        NoOptions = 0,
        PreservePreviousSelection = 1, //!< If used, previously selected editor item
                                       //!< will be kept selected.
        AlphabeticalOrder = 2,         //!< Alphabetical order of properties (the default is insert-order)
        ExpandChildItems = 4           //!< Child property items are expanded (the default is "collapsed")
    };
    Q_DECLARE_FLAGS(SetOptions, SetOption)

    //! @return grid line color, defaultGridLineColor() by default
    QColor gridLineColor() const;

    //! @return default grid line color - Qt::gray
    static QColor defaultGridLineColor() { return Qt::gray; }

    //! Reimplemented to suggest widget size that is based on number of property items.
    QSize sizeHint() const Q_DECL_OVERRIDE;

public Q_SLOTS:
    /*! Populates the editor view with items for each property from the @ set set.
     Child items for composed properties are also created.
     See SetOption documentation for description of @a options options.
     If @a preservePreviousSelection is true, previously selected editor
     item will be kept selected, if present. */
    void changeSet(KPropertySet *set, SetOptions options = NoOptions);

    /*! Populates the editor view with items for each property from the @ set set.
     Child items for composed properties are also created.
     If @a propertyToSelect is provided, item for this property name
     will be selected, if present. */
    void changeSet(KPropertySet *set, const QByteArray& propertyToSelect, SetOptions options = NoOptions);

    /*! If @a enable is true (the default), property values are automatically synced as
    soon as editor contents change (e.g. every time the user types a character)
    and the values are written back to the assigned property set.
    If @a enable is false, property set is updated only when selection within
    the property editor or user presses Enter/Return key.
    Each property can overwrite this setting by changing its own autoSync flag.
    */
    void setAutoSync(bool enable);

    /*! @return value of autoSync flag. */
    bool isAutoSync() const;

    /*! Accepts the changes made to the current editor item (if any)
     (as if the user had pressed Enter key). */
    void acceptInput();

    //! Sets color of grid lines. Use invalid color QColor() to hide grid lines.
    void setGridLineColor(const QColor& color);

Q_SIGNALS:
    /*! Emitted when current property set has been changed. May be 0. */
    void propertySetChanged(KPropertySet *set);

protected:
    virtual bool viewportEvent( QEvent * event );

protected Q_SLOTS:
    virtual void currentChanged( const QModelIndex & current, const QModelIndex & previous );
    virtual void commitData( QWidget * editor );

    /*! Called when current propertis of this set are about to be cleared. */
    void slotSetWillBeCleared();

    /*! Called when current property set is about to be destroyed. */
    void slotSetWillBeDeleted();

    /*! Updates editor widget in the editor.*/
    void slotPropertyChanged(KPropertySet& set, KProperty& property);

    void slotPropertyReset(KPropertySet& set, KProperty& property);

private:
    /*! Used by changeSet(). */
    void changeSetInternal(KPropertySet *set, SetOptions options, const QByteArray& propertyToSelect);
    virtual bool edit( const QModelIndex & index, EditTrigger trigger, QEvent * event );
    virtual void drawBranches( QPainter * painter, const QRect & rect, const QModelIndex & index ) const;
    virtual void mousePressEvent( QMouseEvent * event );

    //! @return true if @a x is within the area of the revert button for @a index index.
    bool withinRevertButtonArea( int x, const QModelIndex& index ) const;

    //! @return area of revert button, if it is displayed for @a index index.
    //! Otherwise invalid QRect is returned.
    QRect revertButtonArea( const QModelIndex& index ) const;

    //! Updates item for @a index and all its children.
    void updateSubtree(const QModelIndex &index);

    /*! Undoes the last change in the property editor.*/
    void undo();

    class Private;
    Private * const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KPropertyEditorView::SetOptions)

#endif
