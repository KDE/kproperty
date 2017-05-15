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

#include "KPropertyEditorDataModel.h"
#include "KPropertyWidgetsFactory.h"
#include "KPropertySet_p.h"

#include <QHash>

class Q_DECL_HIDDEN KPropertyEditorDataModel::Private
{
public:
    explicit Private(KPropertySet *_set, KPropertySetIterator::Order _order = KPropertySetIterator::Order::Insertion)
        : set(_set), order(_order)
    {
        Q_ASSERT(set);
        if (!set) {
            kprCritical() << "KPropertyEditorDataModel requires a KPropertySet object";
        }
    }
    inline KPropertySetPrivate* set_d() { return KPropertySetPrivate::d(set); }
    KPropertySet *set;
    KProperty rootItem;
    KProperty groupItem; //!< Pseudo group item used for all group items
    QHash<QByteArray, QPersistentModelIndex> indicesForNames;
    KPropertySetIterator::Order order; //!< order of properties
    bool groupsVisible = true;
};

// -------------------

//! A property selector offering functor selecting only visible properties.
/*! Used e.g. in EditorDataModel::index(). */
class VisiblePropertySelector : public KPropertySelector
{
public:
    VisiblePropertySelector() {}
    bool operator()(const KProperty &prop) const override {
        return prop.isVisible();
    }
    KPropertySelector* clone() const override { return new VisiblePropertySelector(); }
};

// -------------------

KPropertyEditorDataModel::KPropertyEditorDataModel(KPropertySet *propertySet, QObject *parent,
                                                   KPropertySetIterator::Order order)
        : QAbstractItemModel(parent)
        , d(new Private(propertySet, order))
{
    collectIndices();
}

KPropertyEditorDataModel::~KPropertyEditorDataModel()
{
    delete d;
}

typedef QPair<QByteArray, QString> NameAndCaption;

#if 0
static inline bool nameAndCaptionLessThan(const NameAndCaption &n1, const NameAndCaption &n2)
{
    return QString::compare(n1.second, n2.second, Qt::CaseInsensitive) < 0;
}
#endif

void KPropertyEditorDataModel::collectIndices() const
{
    d->indicesForNames.clear();
    if (d->groupsVisible) {
        for (const QByteArray &groupName : d->set_d()->groupNames) {
            const QList<QByteArray>* propertyNames = d->set_d()->propertiesOfGroup.value(groupName);
            if (!propertyNames) {
                continue;
            }
            int row = 0; // row within the group
            //! @todo Care about sorting
            for (const QByteArray &propertyName : *propertyNames) {
                d->indicesForNames.insert(propertyName,
                                          QPersistentModelIndex(createIndex(row, 0, d->set_d()->property(propertyName))));
            }
        }
    } else {
        KPropertySetIterator it(*d->set, VisiblePropertySelector());
        if (d->order == KPropertySetIterator::Order::Alphabetical) {
            it.setOrder(KPropertySetIterator::Order::Alphabetical);
        }
        for (int row = 0; it.current(); row++, ++it) { // flat list
            d->indicesForNames.insert(it.current()->name(),
                                      QPersistentModelIndex( createIndex(row, 0, it.current())));
        }
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
    return createIndex(index.row(), column, propertyForIndex(index));
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
    const KProperty *prop = propertyForIndex(index);
    if (role == PropertyGroupRole) {
        return prop == &d->groupItem;
    }
    if (col == 0) {
        if (prop == &d->groupItem) {
            const QByteArray groupName(d->set_d()->groupNames.value(index.row()));
            Q_ASSERT(!groupName.isEmpty());
            switch(role) {
            case Qt::DisplayRole:
                return d->set->groupCaption(groupName);
            case Qt::DecorationRole:
                return QIcon::fromTheme(d->set->groupIconName(groupName));
            default:;
            }
        } else if (role == Qt::DisplayRole) {
            if (!prop->captionForDisplaying().isEmpty()) {
                return prop->captionForDisplaying();
            }
            return prop->name();
        }
        else if (role == PropertyModifiedRole) {
            return prop->isModified();
        }
    }
    else if (col == 1) {
        if (role == Qt::EditRole) {
            return prop->value();
        }
        else if (role == Qt::DisplayRole) {
            return KPropertyFactoryManager::self()->propertyValueToLocalizedString(prop);
        }
    }
    return QVariant();
}

Qt::ItemFlags KPropertyEditorDataModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    const int col = index.column();
    Qt::ItemFlags f = Qt::ItemIsEnabled;
    const KProperty *prop = propertyForIndex(index);
    if (prop != &d->groupItem) {
        f |= Qt::ItemIsSelectable;
        if (col == 1 && prop != &d->rootItem && !prop->isReadOnly() && !d->set->isReadOnly()) {
            f |= Qt::ItemIsEditable;
        }
    }
    return f;
}

KProperty *KPropertyEditorDataModel::propertyForIndex(const QModelIndex &index) const
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
    if (row < 0 || column < 0 || /*!parent.isValid() ||*/ (parent.isValid() && parent.column() != 0)) {
        return QModelIndex();
    }

    KProperty *parentItem = propertyForIndex(parent);
    KProperty *childItem = nullptr;
    if (parentItem == &d->rootItem && d->groupsVisible && d->set_d()->hasGroups()) {
        // top level with groups: return group item
        return createIndex(row, column, &d->groupItem);
    } else if (parentItem == &d->rootItem || parentItem == &d->groupItem) {
        // top level without groups or group level: return top-level visible property item
        if (d->groupsVisible && d->set_d()->hasGroups()) {
            const QByteArray groupName(d->set_d()->groupNames.value(parent.row()));
            const QList<QByteArray>* propertyNames = d->set_d()->propertiesOfGroup.value(groupName);
            if (propertyNames) {
                int visiblePropertiesForGroup = -1;
                //! @todo sort?
                for (const QByteArray &propertyName : *propertyNames) {
                    KProperty *property = d->set_d()->property(propertyName);
                    if (property->isVisible()) {
                        ++visiblePropertiesForGroup;
                    }
                    if (visiblePropertiesForGroup == row) {
                        childItem = property;
                        break;
                    }
                }
            }
        } else { // all properties, flat
            KPropertySetIterator it(*d->set, VisiblePropertySelector());
            if (d->order == KPropertySetIterator::Order::Alphabetical) {
                it.setOrder(KPropertySetIterator::Order::Alphabetical);
            }
            //! @todo use qBinaryFind()?
            for (int visibleRows = 0; visibleRows < row && it.current(); ++it) {
                ++visibleRows;
            }
            childItem = it.current();
        }
    } else { // child properties of composed properties
        const QList<KProperty*>* children = parentItem->children();
        if (children) {
            childItem = children->value(row);
        }
    }
    if (!childItem) {
        return QModelIndex();
    }
    return createIndex(row, column, childItem);
}

QModelIndex KPropertyEditorDataModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }
    const KProperty *childItem = propertyForIndex(index);
    if (childItem == &d->rootItem || childItem == &d->groupItem) {
        // parent for the root or a group item: null
        // (parent for a group item is root since group item is top level)
        return QModelIndex();
    }
    KProperty *parentItem = childItem->parent();
    if (parentItem) { // child property: parent property is the parent
        // find index of parent within the grandparent
        const int indexOfParentItem = d->set_d()->indexOfProperty(parentItem);
        return createIndex(indexOfParentItem, 0, parentItem);
    }
    if (d->groupsVisible && d->set_d()->hasGroups()) {
        // top-level property within a group: group item is the parent
        const QByteArray group(d->set_d()->groupForProperty(childItem));
        const int indexOfGroup = d->set_d()->groupNames.indexOf(group);
        return createIndex(indexOfGroup, 0, &d->groupItem);
    }
    return QModelIndex();
}

int KPropertyEditorDataModel::rowCount(const QModelIndex &parent) const
{
    KProperty *parentItem = propertyForIndex(parent);
    if (parentItem == &d->rootItem) { // top level: return group count or top-level properties count
        if (d->groupsVisible && d->set_d()->hasGroups()) {
            return d->set_d()->groupNames.count();
        }
        return d->set->count(VisiblePropertySelector()); // number of visible properties
    } else if (parentItem == &d->groupItem) { // group level: return property count within the group
        const QByteArray groupName = d->set_d()->groupNames.value(parent.row());
        Q_ASSERT(!groupName.isEmpty());
        const QList<QByteArray>* propertyNames = d->set_d()->propertiesOfGroup.value(groupName);
        Q_ASSERT(propertyNames);
        int visiblePropertiesForGroup = 0;
        for(const QByteArray &propertyName : *propertyNames) {
            if (d->set_d()->property(propertyName)->isVisible()) {
                ++visiblePropertiesForGroup;
            }
        }
        return visiblePropertiesForGroup;
    }
    // property level: return child properties count
    const QList<KProperty*>* children = parentItem->children();
    return children ? children->count() : 0;
}

bool KPropertyEditorDataModel::setData(const QModelIndex &index, const QVariant &value,
                              int role)
{
    if (role != Qt::EditRole)
        return false;

    KProperty *item = propertyForIndex(index);
    if (item == &d->rootItem || item == &d->groupItem)
        return false;
    item->setValue(value);
    //don't do that or cursor position and editor state will be reset:
    //emit dataChanged(index, index, {Qt::EditRole});
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

KPropertySet* KPropertyEditorDataModel::propertySet() const
{
    return d->set;
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
    KProperty *parentItem = propertyForIndex(parent);
    if (parentItem == &d->rootItem) { // top level
        return d->set->hasVisibleProperties();
    } else if (parentItem == &d->groupItem) { // group level
        const QByteArray groupName(d->set_d()->groupNames.value(parent.row()));
        Q_ASSERT(!groupName.isEmpty());
        const QList<QByteArray>* propertyNames = d->set_d()->propertiesOfGroup.value(groupName);
        Q_ASSERT(propertyNames);
        for (const QByteArray &propertyName : *propertyNames) {
            if (d->set_d()->property(propertyName)->isVisible()) {
                return true; // at least one visible property in this group
            }
        }
        return false; // no visible properties in this group
    }
    // property level
    const QList<KProperty*>* children = parentItem->children();
    return children && !children->isEmpty();
}

bool KPropertyEditorDataModel::groupsVisible() const
{
    return d->groupsVisible;
}

void KPropertyEditorDataModel::setGroupsVisible(bool set)
{
    if (d->groupsVisible == set) {
        return;
    }
    beginResetModel();
    d->groupsVisible = set;
    collectIndices();
    endResetModel();
}
