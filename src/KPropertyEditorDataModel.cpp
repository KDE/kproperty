/* This file is part of the KDE project
   Copyright (C) 2008-2009 Jarosław Staniek <staniek@kde.org>

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

#include "KPropertyEditorDataModel.h"
#include "KPropertyFactory.h"
#include "KProperty.h"

#include <QHash>
#include <QDebug>

class KPropertyEditorDataModel::Private
{
public:
    explicit Private(KPropertySet *_set, KPropertySetIterator::Order _order = KPropertySetIterator::InsertionOrder) : set(_set), order(_order)
    {
    }
    KPropertySet *set;
    KProperty rootItem;
    QHash<QByteArray, QPersistentModelIndex> indicesForNames;
    KPropertySetIterator::Order order; //!< order of properties
};

// -------------------

//! A property selector offering functor selecting only visible properties.
/*! Used e.g. in EditorDataModel::index(). */
class VisiblePropertySelector : public KPropertySelector
{
public:
    VisiblePropertySelector() {}
    virtual bool operator()(const KProperty& prop) const {
        return prop.isVisible();
    }
    KPropertySelector* clone() const { return new VisiblePropertySelector(); }
};

// -------------------

KPropertyEditorDataModel::KPropertyEditorDataModel(KPropertySet &propertySet, QObject *parent,
                                                   KPropertySetIterator::Order order)
        : QAbstractItemModel(parent)
        , d(new Private(&propertySet, order))
{
    collectIndices();
}

KPropertyEditorDataModel::~KPropertyEditorDataModel()
{
    delete d;
}

typedef QPair<QByteArray, QString> NameAndCaption;

static inline bool nameAndCaptionLessThan(const NameAndCaption &n1, const NameAndCaption &n2)
{
    return QString::compare(n1.second, n2.second, Qt::CaseInsensitive) < 0;
}

void KPropertyEditorDataModel::collectIndices() const
{
    KPropertySetIterator it(*d->set, VisiblePropertySelector());
    if (d->order == KPropertySetIterator::AlphabeticalOrder) {
        it.setOrder(KPropertySetIterator::AlphabeticalOrder);
    }
    d->indicesForNames.clear();
    for (int row = 0; it.current(); row++, ++it) {
        // qDebug() << it.current()->name() << "->" << row;
        d->indicesForNames.insert( it.current()->name(), QPersistentModelIndex( createIndex(row, 0, it.current()) ) );
    }
}

QModelIndex KPropertyEditorDataModel::indexForPropertyName(const QByteArray& propertyName) const
{
    return (const QModelIndex &)d->indicesForNames.value(propertyName);
}

QModelIndex KPropertyEditorDataModel::indexForColumn(const QModelIndex& index, int column) const
{
    if (column == 0)
        return index;
    return createIndex(index.row(), column, propertyForItem(index));
}

int KPropertyEditorDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant KPropertyEditorDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const int col = index.column();
    if (col == 0) {
        KProperty *prop = propertyForItem(index);
        if (role == Qt::DisplayRole) {
            if (!prop->caption().isEmpty())
                return prop->caption();
            return prop->name();
        }
        else if (role == PropertyModifiedRole) {
            return prop->isModified();
        }
    }
    else if (col == 1) {
        KProperty *prop = propertyForItem(index);
        if (role == Qt::EditRole) {
            return prop->value();
        }
        else if (role == Qt::DisplayRole) {
            return KPropertyFactoryManager::self()->convertValueToText(prop);
        }
    }
    return QVariant();
}

Qt::ItemFlags KPropertyEditorDataModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    const int col = index.column();
    Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    KProperty *prop = propertyForItem(index);
    if (prop) {
        if (col == 1) {
            f |= Qt::ItemIsEditable;
        }
    }
    return f;
}

KProperty *KPropertyEditorDataModel::propertyForItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        KProperty *item = static_cast<KProperty*>(index.internalPointer());
        if (item)
            return item;
    }
    return &d->rootItem;
}

QVariant KPropertyEditorDataModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return tr("Name", "Property name");
        } else {
            return tr("Value", "Property value");
        }
    }
    return QVariant();
}

QModelIndex KPropertyEditorDataModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    KProperty *parentItem = propertyForItem(parent);
    KProperty *childItem;
    if (parentItem == &d->rootItem) { // special case: top level
        int visibleRows = 0;
        KPropertySetIterator it(*d->set, VisiblePropertySelector());
        if (d->order == KPropertySetIterator::AlphabeticalOrder) {
            it.setOrder(KPropertySetIterator::AlphabeticalOrder);
        }
//! @todo use qBinaryFind()?
        for (; visibleRows < row && it.current(); visibleRows++, ++it)
            ;
        childItem = it.current();
    } else {
        const QList<KProperty*>* children = parentItem->children();
        if (!children)
            return QModelIndex();
        childItem = children->value(row);
    }
    if (!childItem)
        return QModelIndex();
    return createIndex(row, column, childItem);
}

QModelIndex KPropertyEditorDataModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    KProperty *childItem = propertyForItem(index);
    KProperty *parentItem = childItem->parent();

    if (!parentItem)
        return QModelIndex();

    const QList<KProperty*>* children = parentItem->children();
    Q_ASSERT(children);
    const int indexOfItem = children->indexOf(childItem);
    Q_ASSERT(indexOfItem != -1);

    return createIndex(indexOfItem, 0, parentItem);
}

int KPropertyEditorDataModel::rowCount(const QModelIndex &parent) const
{
    KProperty *parentItem = propertyForItem(parent);
    if (!parentItem || parentItem == &d->rootItem) { // top level
        return d->set->count(VisiblePropertySelector());
    }
    const QList<KProperty*>* children = parentItem->children();
    return children ? children->count() : 0;
}

bool KPropertyEditorDataModel::setData(const QModelIndex &index, const QVariant &value,
                              int role)
{
    if (role != Qt::EditRole)
        return false;

    KProperty *item = propertyForItem(index);
    if (item == &d->rootItem)
        return false;
    item->setValue(value);
    return true;
}

bool KPropertyEditorDataModel::setHeaderData(int section, Qt::Orientation orientation,
                                    const QVariant &value, int role)
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(value);
    Q_UNUSED(role);
    return false;
}

QModelIndex KPropertyEditorDataModel::buddy(const QModelIndex & idx) const
{
    if (idx.column() == 0)
        return index( idx.row(), 1, parent(idx));
    return idx;
}

KPropertySet& KPropertyEditorDataModel::propertySet() const
{
    return *d->set;
}

void KPropertyEditorDataModel::setOrder(KPropertySetIterator::Order order)
{
    if (d->order != order) {
        d->order = order;
        collectIndices();
    }
}

KPropertySetIterator::Order KPropertyEditorDataModel::order() const
{
    return d->order;
}

bool KPropertyEditorDataModel::hasChildren(const QModelIndex & parent) const
{
    KProperty *parentItem = propertyForItem(parent);
    if (!parentItem || parentItem == &d->rootItem) { // top level
        return d->set->hasVisibleProperties();
    }
    const QList<KProperty*>* children = parentItem->children();
    return children && !children->isEmpty();
}
