/* This file is part of the KDE project
   Copyright (C) 2008 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_EDITORDATAMODEL_H
#define KPROPERTY_EDITORDATAMODEL_H

#include "kpropertywidgets_export.h"
#include "KPropertySet.h"

#include <QAbstractItemModel>
#include <QModelIndex>

class KProperty;

/*! @short A data model that integrates a KPropertySet object with the Qt's model/view API.
 @see KPropertyEditorView
 @internal
*/
class KPropertyEditorDataModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    //! Creates a new model. @a propertySet is required.
    explicit KPropertyEditorDataModel(KPropertySet *propertySet, QObject *parent = nullptr,
                                      KPropertySetIterator::Order order = KPropertySetIterator::InsertionOrder);
    ~KPropertyEditorDataModel() override;

    enum Role {
        PropertyModifiedRole = Qt::UserRole + 0
    };
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole) override;

    QModelIndex buddy(const QModelIndex &index) const override;

    //! @return property set object for this model. It is never @c nullptr.
    KPropertySet* propertySet() const;

    //! @return property object for model index @a index
    //! or @c nullptr for invalid index or index without a property assigned.
    KProperty *propertyForIndex(const QModelIndex& index) const;

    //! @return model index for property named @a propertyName
    //! or invalid index if such property could not be found.
    QModelIndex indexForPropertyName(const QByteArray& propertyName) const;

    //! @return a sibling for model index @a index and columnd @a column
    QModelIndex indexForColumn(const QModelIndex& index, int column) const;

    //! Sets order for properties. Restarts the iterator.
    void setOrder(KPropertySetIterator::Order order);

    //! @return order for properties.
    KPropertySetIterator::Order order() const;

    //! Reimplemented for optimization.
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

private:
    void collectIndices() const;

    class Private;
    Private * const d;

    friend class KPropertyEditorView;
};

#endif
