/* This file is part of the KDE project
   Copyright (C) 2008-2017 Jarosław Staniek <staniek@kde.org>

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
class KPROPERTYWIDGETS_EXPORT KPropertyEditorView : public QTreeView
{
    Q_OBJECT
public:
    /*! Creates an empty property editor with @a parent as parent widget. */
    explicit KPropertyEditorView(QWidget *parent = nullptr);

    ~KPropertyEditorView() override;

    //! Options for changeSet()
    enum class SetOption {
        None = 0,
        PreservePreviousSelection = 1, //!< If used, previously selected editor item
                                       //!< will be kept selected.
        AlphabeticalOrder = 2          //!< Alphabetical order of properties. The default is order of insertion.
    };
    Q_DECLARE_FLAGS(SetOptions, SetOption)

    //! @return grid line color, defaultGridLineColor() by default
    QColor gridLineColor() const;

    //! @return default grid line color - Qt::gray
    static QColor defaultGridLineColor() { return Qt::gray; }

    //! Reimplemented to suggest widget size that is based on number of property items.
    QSize sizeHint() const override;

    //! @return the property set object that is assigned to this view or nullptr is no set
    //! is currently assigned.
    KPropertySet* propertySet() const;

    /*! @return @c true if items for parent composed properties are expanded so items for child
     properties are displayed.
     @since 3.1 */
    bool childPropertyItemsExpanded() const;

    /*! @return value of the valueSyncEnabled flag.
     @since 3.1 */
    bool isValueSyncEnabled() const;

    /*! @return @c true if the property groups should be visible.
     By default groups are visible.
     A group is visualized as a subtree displaying group caption and group icon at its root node
     (see KProperty::groupCaption and KProperty::groupIconName) and properties as children of this node.
     A property is assigned to a group while KPropertySet::addProperty() is called.

     @note Regardless of this flag, no groups are displayed if there is only the default group
     "common".

     When the group visibility flag is off or only the "common" group is present, all properties
     are displayed on the same (top) level.
     @since 3.1 */
    bool groupsVisible() const;

    /*! @return @c true if group items for newly added groups are exapanded so properties for these
     groups are displayed.
     @see setGroupItemsExpanded()
     @since 3.1 */
    bool groupItemsExpanded() const;

public Q_SLOTS:
    /*! Populates the editor view with items for each property from the @a set set.
     Child items for composed properties are also created.
     See SetOption documentation for description of @a options options.
     If @a preservePreviousSelection is true, previously selected editor
     item will be kept selected, if present. */
    void changeSet(KPropertySet *set, SetOptions options = SetOption::None);

    /*! Populates the editor view with items for each property from the @a set set.
     Child items for composed properties are also created.
     If @a propertyToSelect is provided, item for this property name
     will be selected, if present. */
    void changeSet(KPropertySet *set, const QByteArray& propertyToSelect, SetOptions options = SetOption::None);

    /*! If @a set is @c true (the default), items for parent composed properties are expanded
     so items for child properties are displayed.
     If @a set is @c false, the items are collapsed.
     @note Appearance of the existing child items is not altered. This method can be typically called
           before a changeSet() call or before adding properties.
     @note Expansion of group items is not affected by this method. Use setGroupItemsExpanded()
           to control expansion of group items.
     @note To expand all items use expandAll(). To collapse all items use collapseAll().
     @since 3.1 */
    void setChildPropertyItemsExpanded(bool set);

    /*! If @a set is @c true (the default), property values are automatically synchronized as
    soon as editor contents change (e.g. every time the user types a character)
    and the values are saved back to the assigned property set.
    If @a enable is false, property set is updated only when selection within the property editor
    or user presses Enter/Return key.
    Each property can override this policy by changing its own valueSyncPolicy flag.
    @see KProperty::setValueSyncPolicy()
     @since 3.1 */
    void setValueSyncEnabled(bool set);

    /*! Accepts the changes made to the current editor item (if any)
     (as if the user had pressed Enter key). */
    void acceptInput();

    //! Sets color of grid lines. Use invalid color QColor() to hide grid lines.
    void setGridLineColor(const QColor& color);

    /*! Shows the property groups if @a set is @c true.
     @see groupsVisible()
     @since 3.1 */
    void setGroupsVisible(bool set);

    /*! If @a set is @c true (the default), group items for newly added groups are exapanded
     so properties for these groups are displayed.
     If @a set is @c false, the items are collapsed.
     @note Appearance of the existing group items is not altered. This method can be typically called
           before a changeSet() call or before adding properties.
     @note Expansion of child items for composed properties is not affected by this method.
           Use setChildPropertyItemsExpanded() to control expansion child items for composed properties.
     @note To expand all items use expandAll(). To collapse all items use collapseAll().
     @since 3.1 */
    void setGroupItemsExpanded(bool set);

Q_SIGNALS:
    /*! Emitted when current property set has been changed. May be 0. */
    void propertySetChanged(KPropertySet *set);

protected:
    bool viewportEvent(QEvent * event) override;

protected Q_SLOTS:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
    void commitData(QWidget * editor) override;

    /*! Called when current propertis of this set are about to be cleared. */
    void slotSetWillBeCleared();

    /*! Called when current property set is about to be destroyed. */
    void slotSetWillBeDeleted();

    /*! Called when property set's read-only flag has changed.
     Refreshes selection so editor is displayed again if needed. */
    void slotReadOnlyFlagChanged();

    /*! Updates editor widget in the editor.*/
    void slotPropertyChanged(KPropertySet& set, KProperty& property);

    void slotPropertyReset(KPropertySet& set, KProperty& property);

private:
    /*! Used by changeSet(). */
    void changeSetInternal(KPropertySet *set, SetOptions options, const QByteArray &propertyToSelect);
    bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event) override;
    void drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const override;

    //! Reimplemented to draw group header text by hand.
    void drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void mousePressEvent(QMouseEvent *event) override;

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
