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

#include "KPropertySet.h"
#include "KPropertySet_p.h"
#include "KProperty_p.h"

#include <QByteArray>

KPropertySetPrivate::KPropertySetPrivate(KPropertySet *set)
    : q(set)
{
    groupCaptions.insert("common", QObject::tr("General", "General properties"));
}

KPropertySetPrivate::~KPropertySetPrivate()
{
}

void KPropertySetPrivate::addProperty(KProperty *property, const QByteArray &group/*, bool updateSortingKey*/)
{
    if (!property) {
        kprWarning() << "property == 0";
        return;
    }
    if (property->isNull()) {
        kprWarning() << "COULD NOT ADD NULL PROPERTY";
        return;
    }
    const QByteArray realGroup(group.isEmpty() ? "common" : group);
    KProperty *p = this->property(property->name());
    if (p) {
        addRelatedProperty(p, property);
    }
    else {
        list.append(property);
        hash.insert(property->name().toLower(), property);
        if (property->isVisible()) {
            m_visiblePropertiesCount++;
        }
        addToGroup(realGroup, property);
    }

    property->d->addSet(q);
#if 0
    if (updateSortingKey)
        property->setSortingKey(count());
#endif
}

void KPropertySetPrivate::removeProperty(KProperty *property)
{
    if (!property)
        return;

    if (!list.removeOne(property)) {
        kprDebug() << "The property set does not contain property" << property;
        return;
    }
    KProperty *p = hash.take(property->name().toLower());
    if (p) {
        removeFromGroup(p);
        if (p->isVisible()) {
            m_visiblePropertiesCount--;
        }
        if (ownProperty) {
            emit q->aboutToDeleteProperty(*q, *p);
            delete p;
        }
    }
}

void KPropertySetPrivate::clear()
{
    if (m_informAboutClearing) {
        *m_informAboutClearing = true;
    }
    m_informAboutClearing = nullptr;
    emit q->aboutToBeCleared();
    m_visiblePropertiesCount = 0;
    qDeleteAll(propertiesOfGroup);
    propertiesOfGroup.clear();
    groupNames.clear();
    groupForProperties.clear();
    groupCaptions.clear();
    groupIconNames.clear();
    qDeleteAll(list);
    list.clear();
    hash.clear();
}

void KPropertySetPrivate::copyAttributesFrom(const KPropertySetPrivate &other)
{
    KPropertySet *origSet = q;
    *this = other;
    q = origSet;
    // do not copy too deeply
    list.clear();
    hash.clear();
    propertiesOfGroup.clear();
    groupForProperties.clear();
    m_visiblePropertiesCount = 0;
    m_informAboutClearing = nullptr;
}

void KPropertySetPrivate::copyPropertiesFrom(
    const QList<KProperty*>::ConstIterator& constBegin,
    const QList<KProperty*>::ConstIterator& constEnd, const KPropertySet & set)
{
    for (QList<KProperty*>::ConstIterator it(constBegin); it!=constEnd; ++it) {
        KProperty *prop = new KProperty(*(*it));
        addProperty(prop, set.d->groupForProperty( *it )
#if 0
                    ,
                    false /* don't updateSortingKey, because the key is already
                             set in KProperty copy ctor.*/
#endif
                   );
    }
}

void KPropertySetPrivate::addToGroup(const QByteArray &group, KProperty *property)
{
    if (!property || group.isEmpty()) {
        return;
    }
    //do not add the same property to the group twice
    const QByteArray groupLower(group.toLower());
    if (groupForProperty(property) == groupLower) {
        kprWarning() << "Group" << group << "already contains property" << property->name();
        return;
    }
    QList<QByteArray>* propertiesOfGroup = this->propertiesOfGroup.value(groupLower);
    if (!propertiesOfGroup) {
        propertiesOfGroup = new QList<QByteArray>();
        this->propertiesOfGroup.insert(groupLower, propertiesOfGroup);
        groupNames.append(groupLower);
    }
    propertiesOfGroup->append(property->name());
    addPropertyToGroup(property, groupLower);
}

void KPropertySetPrivate::removeFromGroup(KProperty *property)
{
    if (!property) {
        return;
    }
    const QByteArray group(groupForProperty(property));
    if (group.isEmpty()) {
        return;
    }
    QList<QByteArray>* propertiesOfGroup = this->propertiesOfGroup.value(group);
    if (propertiesOfGroup) {
        propertiesOfGroup->removeAt(propertiesOfGroup->indexOf(property->name()));
        if (propertiesOfGroup->isEmpty()) {
            //remove group as well
            this->propertiesOfGroup.take(group);
            delete propertiesOfGroup;
            const int i = groupNames.indexOf(group);
            if (i != -1) {
                groupNames.removeAt(i);
            }
        }
    }
    removePropertyFromGroup(property);
}

bool KPropertySetPrivate::hasGroups() const
{
    return groupNames.count() > 1 || (groupNames.count() == 1 && groupNames.first() != "common");
}

void KPropertySetPrivate::informAboutClearing(bool* cleared)
{
    Q_ASSERT(cleared);
    *cleared = false;
    m_informAboutClearing = cleared;
}

void KPropertySetPrivate::addRelatedProperty(KProperty *p1, KProperty *p2) const
{
    p1->d->addRelatedProperty(p2);
}

int KPropertySetPrivate::indexOfProperty(const KProperty *property) const
{
    KProperty *parentProperty = property->parent();
    if (parentProperty) {
        const QList<KProperty*>* children = parentProperty->children();
        Q_ASSERT(children);
        const int index = children->indexOf(parentProperty);
        Q_ASSERT(index != -1);
        return index;
    }
    return indexOfPropertyInGroup(property);
}

int KPropertySetPrivate::indexOfPropertyInGroup(const KProperty *property) const
{
    const QByteArray group(groupForProperties.value(const_cast<KProperty *>(property)));
    QList<QByteArray>* propertiesOfGroup = this->propertiesOfGroup.value(group);
    if (!propertiesOfGroup) {
        return -1;
    }
    return propertiesOfGroup->indexOf(property->name());
}

//////////////////////////////////////////////

KPropertySelector::KPropertySelector()
{
}

KPropertySelector::~KPropertySelector()
{
}

//////////////////////////////////////////////

typedef QPair<KProperty*, QString> Iterator_PropertyAndString;

static inline bool Iterator_propertyAndStringLessThan(
    const Iterator_PropertyAndString &n1, const Iterator_PropertyAndString &n2)
{
    return QString::compare(n1.second, n2.second, Qt::CaseInsensitive) < 0;
}

//////////////////////////////////////////////

class Q_DECL_HIDDEN KPropertySetIterator::Private
{
public:
    explicit Private(KPropertySetIterator *iter) : q(iter)
    {
    }
    Private(KPropertySetIterator *iter, const Private &other)
    : q(iter)
    {
        copy(other);
    }
    ~Private()
    {
        delete selector;
    }

#define KPropertySetIteratorPrivateArgs(o) std::tie(o.set, o.iterator, o.end, o.selector, o.order, o.sorted)
    void copy(const Private &other) {
        KPropertySetIteratorPrivateArgs((*this)) = KPropertySetIteratorPrivateArgs(other);
    }
    bool operator==(const Private &other) const {
        return KPropertySetIteratorPrivateArgs((*this)) == KPropertySetIteratorPrivateArgs(other);
    }

    void skipNotAcceptable()
    {
        if (!selector)
            return;
        //kprDebug() << "FROM:" << *current();
        if (q->current() && !(*selector)( *q->current() )) {
            // skip first items that not are acceptable by the selector
            ++(*q);
        }
        //kprDebug() << "TO:" << *current();
    }

    const KPropertySet *set;
    QList<KProperty*>::ConstIterator iterator;
    QList<KProperty*>::ConstIterator end;
    KPropertySelector *selector;
    KPropertySetIterator::Order order;
    QList<KProperty*> sorted; //!< for sorted order

private:
    KPropertySetIterator * const q;
};

KPropertySetIterator::KPropertySetIterator(const KPropertySet &set)
    : d(new Private(this))
{
    d->set = &set;
    d->iterator = KPropertySetPrivate::d(&set)->listConstIterator();
    d->end = KPropertySetPrivate::d(&set)->listConstEnd();
    d->selector = nullptr;
    d->order = KPropertySetIterator::Order::Insertion;
}

KPropertySetIterator::KPropertySetIterator(const KPropertySet &set,
                                           const KPropertySelector &selector)
    : d(new Private(this))
{
    d->set = &set;
    d->iterator = KPropertySetPrivate::d(&set)->listConstIterator();
    d->end = KPropertySetPrivate::d(&set)->listConstEnd();
    d->selector = selector.clone();
    d->order = KPropertySetIterator::Order::Insertion;
    d->skipNotAcceptable();
}

KPropertySetIterator::KPropertySetIterator(const KPropertySetIterator &set)
    : d(new Private(this, *set.d))
{
}

KPropertySetIterator& KPropertySetIterator::operator=(const KPropertySetIterator &other)
{
    if (this != &other) {
        d->copy(*other.d);
    }
    return *this;
}

bool KPropertySetIterator::operator==(const KPropertySetIterator &other) const
{
    return *d == *other.d;
}

KPropertySetIterator::~KPropertySetIterator()
{
    delete d;
}

void KPropertySetIterator::setOrder(KPropertySetIterator::Order order)
{
    if (d->order == order)
        return;
    d->order = order;
    switch (d->order) {
    case KPropertySetIterator::Order::Alphabetical:
    case KPropertySetIterator::Order::AlphabeticalByName:
    {
        QList<Iterator_PropertyAndString> propertiesAndStrings;
        d->iterator = KPropertySetPrivate::d(d->set)->listConstIterator();
        d->end = KPropertySetPrivate::d(d->set)->listConstEnd();
        for (; d->iterator!=d->end; ++d->iterator) {
            KProperty *prop = *d->iterator;
            QString captionOrName;
            if (d->order == KPropertySetIterator::Order::Alphabetical) {
                captionOrName = prop->caption();
            }
            if (captionOrName.isEmpty()) {
                captionOrName = QLatin1String(prop->name());
            }
            propertiesAndStrings.append( qMakePair(prop, captionOrName) );
        }
        qSort(propertiesAndStrings.begin(), propertiesAndStrings.end(),
            Iterator_propertyAndStringLessThan);
        d->sorted.clear();
        foreach (const Iterator_PropertyAndString& propertyAndString, propertiesAndStrings) {
            d->sorted.append(propertyAndString.first);
        }
        // restart the iterator
        d->iterator = d->sorted.constBegin();
        d->end = d->sorted.constEnd();
        break;
    }
    default:
        d->sorted.clear();
        // restart the iterator
        d->iterator = KPropertySetPrivate::d(d->set)->listConstIterator();
        d->end = KPropertySetPrivate::d(d->set)->listConstEnd();
    }
    d->skipNotAcceptable();
}

KPropertySetIterator::Order KPropertySetIterator::order() const
{
    return d->order;
}

KProperty* KPropertySetIterator::current() const
{
    return d->iterator == d->end ? nullptr : *d->iterator;
}

void KPropertySetIterator::operator ++()
{
    while (true) {
        ++d->iterator;
        if (!d->selector)
            return;
        // selector exists
        if (!current()) // end encountered
            return;
        if ((*d->selector)( *current() ))
            return;
    }
}

//////////////////////////////////////////////

KPropertySet::KPropertySet(QObject *parent)
        : QObject(parent)
        , d(new KPropertySetPrivate(this))
{
    d->ownProperty = true;
}


KPropertySet::KPropertySet(const KPropertySet &set)
        : QObject(nullptr /* implicit sharing the parent is dangerous */)
        , d(new KPropertySetPrivate(this))
{
    setObjectName(set.objectName());
    *this = set;
}

KPropertySet::KPropertySet(bool propertyOwner)
        : QObject(nullptr)
        , d(new KPropertySetPrivate(this))
{
    d->ownProperty = propertyOwner;
}

KPropertySet::~KPropertySet()
{
    emit aboutToBeCleared();
    emit aboutToBeDeleted();
    clear();
    delete d;
}

/////////////////////////////////////////////////////

void
KPropertySet::addProperty(KProperty *property, const QByteArray &group)
{
    d->addProperty(property, group);
}

void
KPropertySet::removeProperty(KProperty *property)
{
    d->removeProperty(property);
}

void
KPropertySet::removeProperty(const QByteArray &name)
{
    KProperty *p = d->property(name);
    removeProperty(p);
}

void
KPropertySet::clear()
{
    d->clear();
}

/////////////////////////////////////////////////////

QByteArray KPropertySet::groupNameForProperty(const QByteArray &propertyName) const
{
    const KProperty *property = d->property(propertyName);
    return property ? groupNameForProperty(*property) : QByteArray();
}

QByteArray KPropertySet::groupNameForProperty(const KProperty &property) const
{
    return d->groupForProperty(&property);
}

QList<QByteArray> KPropertySet::groupNames() const
{
    return d->groupNames;
}

QList<QByteArray> KPropertySet::propertyNamesForGroup(const QByteArray &group) const
{
    QList<QByteArray>* propertiesOfGroup = d->propertiesOfGroup.value(group);
    return propertiesOfGroup ? *propertiesOfGroup : QList<QByteArray>();
}

void
KPropertySet::setGroupCaption(const QByteArray &group, const QString &caption)
{
    d->groupCaptions.insert(group.toLower(), caption);
}

QString
KPropertySet::groupCaption(const QByteArray &group) const
{
    const QString result(d->groupCaptions.value(group.toLower()));
    if (!result.isEmpty())
        return result;
    return QLatin1String(group);
}

void
KPropertySet::setGroupIconName(const QByteArray &group, const QString& iconName)
{
    d->groupIconNames.insert(group.toLower(), iconName);
}

QString
KPropertySet::groupIconName(const QByteArray &group) const
{
    return d->groupIconNames.value(group);
}


/////////////////////////////////////////////////////

int KPropertySet::count() const
{
    return d->count();
}

int KPropertySet::count(const KPropertySelector& selector) const
{
    int result = 0;
    for (KPropertySetIterator it(*this, selector); it.current(); ++it, result++)
        ;
    return result;
}

bool
KPropertySet::isEmpty() const
{
    return d->isEmpty();
}

bool KPropertySet::hasVisibleProperties() const
{
    return d->visiblePropertiesCount() > 0;
}

bool KPropertySet::hasProperties(const KPropertySelector& selector) const
{
    KPropertySetIterator it(*this, selector);
    return it.current();
}

bool
KPropertySet::isReadOnly() const
{
    return d->readOnly;
}

void
KPropertySet::setReadOnly(bool readOnly)
{
    if (d->readOnly != readOnly) {
        d->readOnly = readOnly;
        emit readOnlyFlagChanged();
    }
}

bool
KPropertySet::contains(const QByteArray &name) const
{
    return d->property(name);
}

KProperty&
KPropertySet::property(const QByteArray &name) const
{
    return d->propertyOrNull(name);
}

void KPropertySet::changePropertyIfExists(const QByteArray &property, const QVariant &value)
{
    if (contains(property)) {
        changeProperty(property, value);
    }
}

KProperty&
KPropertySet::operator[](const QByteArray &name) const
{
    return d->propertyOrNull(name);
}

KPropertySet&
KPropertySet::operator= (const KPropertySet & set)
{
    if (&set == this)
        return *this;

    clear();
    d->copyAttributesFrom(*set.d);
    d->copyPropertiesFrom(set.d->listConstIterator(), set.d->listConstEnd(), set);
    return *this;
}

QVariant KPropertySet::propertyValue(const QByteArray &name, const QVariant& defaultValue) const
{
    const KProperty *p = d->property(name);
    return p ? p->value() : defaultValue;
}

void
KPropertySet::changeProperty(const QByteArray &property, const QVariant &value)
{
    KProperty *p = d->property(property);
    if (p)
        p->setValue(value);
}

void KPropertySet::debug() const
{
    kprDebug() << *this;
}

KPROPERTYCORE_EXPORT QDebug operator<<(QDebug dbg, const KPropertySet &set)
{
    dbg.nospace() << "KPropertySet(";
    if (set.isEmpty()) {
        dbg.space() << "<EMPTY>)";
        return dbg.space();
    }
    dbg.nospace() << " PROPERTIES(" << set.count() << "):\n";

    KPropertySetIterator it(set);
    it.setOrder(KPropertySetIterator::Order::AlphabeticalByName);
    bool first = true;
    for ( ; it.current(); ++it) {
        if (first) {
            first = false;
        }
        else {
            dbg.nospace() << "\n";
        }
        dbg.nospace() << *it.current();
    }
    dbg.nospace() << "\n)";
    return dbg.space();
}

QByteArray
KPropertySet::previousSelection() const
{
    return d->prevSelection;
}

void
KPropertySet::setPreviousSelection(const QByteArray &prevSelection)
{
    d->prevSelection = prevSelection;
}

QMap<QByteArray, QVariant> KPropertySet::propertyValues() const
{
    QMap<QByteArray, QVariant> result;
    for (KPropertySetIterator it(*this); it.current(); ++it) {
        result.insert(it.current()->name(), it.current()->value());
    }
    return result;
}
