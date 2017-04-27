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
class KPROPERTYWIDGETS_EXPORT KPropertyEditorView : public QTreeView
{
    Q_OBJECT
public:
    /*! Creates an empty property editor with @a parent as parent widget. */
    explicit KPropertyEditorView(QWidget *parent = nullptr);

    ~KPropertyEditorView() override;

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
    QSize sizeHint() const override;

    //! @return the property set object that is assigned to this view or nullptr is no set
    //! is currently assigned.
    KPropertySet* propertySet() const;

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
    void changeSetInternal(KPropertySet *set, SetOptions options,
                           const QByteArray &propertyToSelect);
    bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event) override;
    void drawBranches(QPainter *painter, const QRect &rect,
                      const QModelIndex &index) const override;
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
