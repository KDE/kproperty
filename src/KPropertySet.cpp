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

KPropertySetIterator::KPropertySetIterator(const KPropertySet &set)
    : m_set(&set)
    , m_iterator(KPropertySetPrivate::d(&set)->listConstIterator())
    , m_end(KPropertySetPrivate::d(&set)->listConstEnd() )
    , m_selector( 0 )
    , m_order(KPropertySetIterator::Order::Insertion)
{
}

KPropertySetIterator::KPropertySetIterator(const KPropertySet &set, const KPropertySelector& selector)
    : m_set(&set)
    , m_iterator(KPropertySetPrivate::d(&set)->listConstIterator())
    , m_end(KPropertySetPrivate::d(&set)->listConstEnd())
    , m_selector( selector.clone() )
    , m_order(KPropertySetIterator::Order::Insertion)
{
    skipNotAcceptable();
}

KPropertySetIterator::~KPropertySetIterator()
{
    delete m_selector;
}

void KPropertySetIterator::skipNotAcceptable()
{
    if (!m_selector)
        return;
    //kprDebug() << "FROM:" << *current();
    if (current() && !(*m_selector)( *current() )) {
        // skip first items that not are acceptable by the selector
        ++(*this);
    }
    //kprDebug() << "TO:" << *current();
}

void KPropertySetIterator::setOrder(KPropertySetIterator::Order order)
{
    if (m_order == order)
        return;
    m_order = order;
    switch (m_order) {
    case KPropertySetIterator::Order::Alphabetical:
    case KPropertySetIterator::Order::AlphabeticalByName:
    {
        QList<Iterator_PropertyAndString> propertiesAndStrings;
        m_iterator = KPropertySetPrivate::d(m_set)->listConstIterator();
        m_end = KPropertySetPrivate::d(m_set)->listConstEnd();
        for (; m_iterator!=m_end; ++m_iterator) {
            KProperty *prop = *m_iterator;
            QString captionOrName;
            if (m_order == KPropertySetIterator::Order::Alphabetical) {
                captionOrName = prop->caption();
            }
            if (captionOrName.isEmpty()) {
                captionOrName = QLatin1String(prop->name());
            }
            propertiesAndStrings.append( qMakePair(prop, captionOrName) );
        }
        qSort(propertiesAndStrings.begin(), propertiesAndStrings.end(),
            Iterator_propertyAndStringLessThan);
        m_sorted.clear();
        foreach (const Iterator_PropertyAndString& propertyAndString, propertiesAndStrings) {
            m_sorted.append(propertyAndString.first);
        }
        // restart the iterator
        m_iterator = m_sorted.constBegin();
        m_end = m_sorted.constEnd();
        break;
    }
    default:
        m_sorted.clear();
        // restart the iterator
        m_iterator = KPropertySetPrivate::d(m_set)->listConstIterator();
        m_end = KPropertySetPrivate::d(m_set)->listConstEnd();
    }
    skipNotAcceptable();
}

void
KPropertySetIterator::operator ++()
{
    while (true) {
        ++m_iterator;
        if (!m_selector)
            return;
        // selector exists
        if (!current()) // end encountered
            return;
        if ((*m_selector)( *current() ))
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
        : QObject(0 /* implicit sharing the parent is dangerous */)
        , d(new KPropertySetPrivate(this))
{
    setObjectName(set.objectName());
    *this = set;
}

KPropertySet::KPropertySet(bool propertyOwner)
        : QObject(0)
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

uint
KPropertySet::count() const
{
    return d->count();
}

uint KPropertySet::count(const KPropertySelector& selector) const
{
    uint result = 0;
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

/////////////////////////////////////////////////////

KPropertyBuffer::KPropertyBuffer()
        : KPropertySet(false)
{
    connect(this, SIGNAL(propertyChanged(KPropertySet&,KProperty&)),
            this, SLOT(intersectedChanged(KPropertySet&,KProperty&)));

    connect(this, SIGNAL(propertyReset(KPropertySet&,KProperty&)),
            this, SLOT(intersectedReset(KPropertySet&,KProperty&)));
}

KPropertyBuffer::KPropertyBuffer(const KPropertySet& set)
        : KPropertySet(false)
{
    connect(this, SIGNAL(propertyChanged(KPropertySet&,KProperty&)),
            this, SLOT(intersectedChanged(KPropertySet&,KProperty&)));

    connect(this, SIGNAL(propertyReset(KPropertySet&,KProperty&)),
            this, SLOT(intersectedReset(KPropertySet&,KProperty&)));

    init(set);
}

void KPropertyBuffer::init(const KPropertySet& set)
{
    //deep copy of set
    const QList<KProperty*>::ConstIterator itEnd(KPropertySetPrivate::d(&set)->listConstEnd());
    for (QList<KProperty*>::ConstIterator it(KPropertySetPrivate::d(&set)->listConstIterator());
        it!=itEnd; ++it)
    {
        KProperty *prop = new KProperty(*(*it));
        QByteArray group = KPropertySetPrivate::d(&set)->groupForProperty(*it);
        const QString groupCaption = set.groupCaption(group);
        setGroupCaption(group, groupCaption);
        addProperty(prop, group);
        prop->d->addRelatedProperty(*it);
    }
}

void KPropertyBuffer::intersect(const KPropertySet& set)
{
    if (isEmpty()) {
        init(set);
        return;
    }

    const QList<KProperty*>::ConstIterator itEnd(KPropertySetPrivate::d(&set)->listConstEnd());
    for (QList<KProperty*>::ConstIterator it(KPropertySetPrivate::d(&set)->listConstIterator());
        it!=itEnd; ++it)
    {
        const QByteArray key( (*it)->name() );
        KProperty *property = KPropertySetPrivate::d(&set)->property( key );
        if (property) {
            blockSignals(true);
            (*it)->resetValue();
            (*it)->d->addRelatedProperty(property);
            blockSignals(false);
        } else {
            removeProperty(key);
        }
    }
}

void KPropertyBuffer::intersectedChanged(KPropertySet& set, KProperty& prop)
{
    Q_UNUSED(set);
    if (!contains(prop.name()))
        return;

    const QList<KProperty*> *props = prop.d->relatedProperties;
    for (QList<KProperty*>::ConstIterator it = props->constBegin(); it != props->constEnd(); ++it) {
        (*it)->setValue(prop.value(), KProperty::DefaultValueOptions & ~KProperty::ValueOptions(KProperty::ValueOption::RememberOld));
    }
}

void KPropertyBuffer::intersectedReset(KPropertySet& set, KProperty& prop)
{
    Q_UNUSED(set);
    if (!contains(prop.name()))
        return;

    const QList<KProperty*> *props = prop.d->relatedProperties;
    for (QList<KProperty*>::ConstIterator it = props->constBegin(); it != props->constEnd(); ++it)  {
        (*it)->setValue(prop.value(), KProperty::DefaultValueOptions & ~KProperty::ValueOptions(KProperty::ValueOption::RememberOld));
    }
}
