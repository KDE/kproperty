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

#include "KProperty.h"
#include "KProperty_p.h"
#include "KPropertySet.h"
#include "KPropertyFactory.h"
#include "kproperty_debug.h"

#include <QPointer>
#include <QSizePolicy>

//! @internal
class KProperty::Private
{
public:
    Private()
            : type(0), caption(0), listData(0), changed(false), storable(true),
            readOnly(false), visible(true),
            autosync(-1), composed(0), useComposedProperty(true),
            sets(0), parent(0), children(0), relatedProperties(0)
    {
    }

    inline void setCaptionForDisplaying(const QString& captionForDisplaying) {
        delete caption;
        if (captionForDisplaying.simplified() != captionForDisplaying) {
            if (captionForDisplaying.isEmpty()) {
                caption = 0;
            }
            else {
               caption = new QString(captionForDisplaying.simplified());
            }
        }
        else {
            caption = 0;
        }
        this->captionForDisplaying = captionForDisplaying;
    }

    ~Private() {
        delete caption;
        caption = 0;
        delete listData;
        if (children) {
            qDeleteAll(*children);
            delete children;
        }
        delete relatedProperties;
        delete composed;
        delete sets;
    }

    int type;
    QByteArray name;
    QString captionForDisplaying;
    QString* caption;
    QString description;
    QVariant value;
    QVariant oldValue;
    /*! The string-to-value correspondence list of the property.*/
    KPropertyListData* listData;
    QString icon;

    bool changed;
    bool storable;
    bool readOnly;
    bool visible;
    int autosync;
    QMap<QByteArray, QVariant> options;

    KComposedPropertyInterface *composed;
    //! Flag used to allow composed property to use setValue() without causing recursion
    bool useComposedProperty;

    //! Used when a single set is assigned for the property
    QPointer<KPropertySet> set;
    //! Used when multiple sets are assigned for the property
    QList< QPointer<KPropertySet> > *sets;

    KProperty  *parent;
    QList<KProperty*>  *children;
    //! list of properties with the same name (when intersecting buffers)
    QList<KProperty*>  *relatedProperties;
};

/////////////////////////////////////////////////////////////////

KPropertyListData::KPropertyListData(const QStringList& keys_, const QStringList& names_)
        : names(names_)
{
    setKeysAsStringList(keys_);
}

KPropertyListData::KPropertyListData(const QList<QVariant> keys_, const QStringList& names_)
        : keys(keys_), names(names_)
{
}

KPropertyListData::KPropertyListData()
{
}

KPropertyListData::~KPropertyListData()
{
}

void KPropertyListData::setKeysAsStringList(const QStringList& list)
{
    keys.clear();
    for (QStringList::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it) {
        keys.append(*it);
    }
}

QStringList KPropertyListData::keysAsStringList() const
{
    QStringList result;
    for (QList<QVariant>::ConstIterator it = keys.constBegin(); it != keys.constEnd(); ++it) {
        result.append((*it).toString());
    }
    return result;
}

/////////////////////////////////////////////////////////////////

KProperty::KProperty(const QByteArray &name, const QVariant &value,
                   const QString &caption, const QString &description,
                   int type, KProperty* parent)
        : d(new KProperty::Private())
{
    d->name = name;
    d->setCaptionForDisplaying(caption);
    d->description = description;

    if (type == (int)Auto)
        d->type = value.type();
    else
        d->type = type;

    d->composed = KPropertyFactoryManager::self()->createComposedProperty(this);

    if (parent)
        parent->addChild(this);
    setValue(value, false);
}

KProperty::KProperty(const QByteArray &name, const QStringList &keys, const QStringList &strings,
                   const QVariant &value, const QString &caption, const QString &description,
                   int type, KProperty* parent)
        : d(new KProperty::Private())
{
    d->name = name;
    d->setCaptionForDisplaying(caption);
    d->description = description;
    d->type = type;
    setListData(keys, strings);

    d->composed = KPropertyFactoryManager::self()->createComposedProperty(this);

    if (parent)
        parent->addChild(this);
    setValue(value, false);
}

KProperty::KProperty(const QByteArray &name, KPropertyListData* listData,
                   const QVariant &value, const QString &caption, const QString &description,
                   int type, KProperty* parent)
        : d(new KProperty::Private())
{
    d->name = name;
    d->setCaptionForDisplaying(caption);
    d->description = description;
    d->type = type;
    d->listData = listData;

    d->composed = KPropertyFactoryManager::self()->createComposedProperty(this);

    if (parent)
        parent->addChild(this);
    setValue(value, false);
}

KProperty::KProperty()
        : d(new KProperty::Private())
{
}

KProperty::KProperty(const KProperty &prop)
        : d(new KProperty::Private())
{
    *this = prop;
}

KProperty::~KProperty()
{
    delete d;
}

QByteArray
KProperty::name() const
{
    return d->name;
}

void
KProperty::setName(const QByteArray &name)
{
    d->name = name;
}

QString
KProperty::caption() const
{
    return d->caption ? *d->caption : d->captionForDisplaying;
}

QString
KProperty::captionForDisplaying() const
{
    return d->captionForDisplaying;
}

void
KProperty::setCaption(const QString &caption)
{
    d->setCaptionForDisplaying(caption);
}

QString
KProperty::description() const
{
    return d->description;
}

void
KProperty::setDescription(const QString &desc)
{
    d->description = desc;
}

int
KProperty::type() const
{
    return d->type;
}

void
KProperty::setType(int type)
{
    d->type = type;
}

QString
KProperty::icon() const
{
    return d->icon;
}

void
KProperty::setIcon(const QString &icon)
{
    d->icon = icon;
}

QVariant
KProperty::value() const
{
    return d->value;
}

QVariant
KProperty::oldValue() const
{
    return d->oldValue;
}

void
KProperty::childValueChanged(KProperty *child, const QVariant &value, bool rememberOldValue)
{
    if (!d->composed)
        return;
    d->composed->childValueChangedInternal(child, value, rememberOldValue);
}

//! @return true if @a currentValue and @a value are compatible
static bool compatibleTypes(const QVariant& currentValue, const QVariant &value)
{
    if (currentValue.isNull() || value.isNull())
        return true;
    const QVariant::Type t = currentValue.type();
    const QVariant::Type newt = value.type();
    if (t == newt)
        return true;
    if (   (t == QVariant::Int && newt == QVariant::UInt)
        || (t == QVariant::UInt && newt == QVariant::Int)
        || (t == QVariant::ByteArray && newt == QVariant::String)
        || (t == QVariant::String && newt == QVariant::ByteArray)
        || (t == QVariant::ULongLong && newt == QVariant::LongLong)
        || (t == QVariant::LongLong && newt == QVariant::ULongLong))
    {
        return true;
    }
    return false;
}

void KProperty::setValue(const QVariant &value, bool rememberOldValue, bool useComposedProperty)
{
    if (d->name.isEmpty()) {
        kprWarning() << "COULD NOT SET value to a null property";
        return;
    }
    QVariant currentValue = this->value();
    if (!compatibleTypes(currentValue, value)) {
        kprWarning() << "INCOMPATIBLE TYPES! old=" << currentValue << "new=" << value;
    }

    //1. Check if the value should be changed
    bool ch;
    const QVariant::Type t = currentValue.type();
    const QVariant::Type newt = value.type();
    if (   t == QVariant::DateTime
        || t == QVariant::Time)
    {
        //for date and datetime types: compare with strings, because there
        //can be miliseconds difference
        ch = (currentValue.toString() != value.toString());
    }
    else if (t == QVariant::String || t == QVariant::ByteArray) {
        //property is changed for string type,
        //if one of value is empty and other isn't..
        ch = ((currentValue.toString().isEmpty() != value.toString().isEmpty())
              //..or both are not empty and values differ
              || (!currentValue.toString().isEmpty() && !value.toString().isEmpty() && currentValue != value));
    }
    else if (t == QVariant::Double) {
        const double factor = 1.0 / option("step", KPROPERTY_DEFAULT_DOUBLE_VALUE_STEP).toDouble();
        //kprDebug()
        //    << "double compared:" << currentValue.toDouble() << value.toDouble()
        //    << ":" << static_cast<qlonglong>(currentValue.toDouble() * factor) << static_cast<qlonglong>(value.toDouble() * factor);
        ch = static_cast<qlonglong>(currentValue.toDouble() * factor) != static_cast<qlonglong>(value.toDouble() * factor);
    } else if (t == QVariant::Invalid && newt == QVariant::Invalid) {
        ch = false;
    } else if (t == QVariant::SizePolicy) {
        ch = (currentValue.value<QSizePolicy>() != value.value<QSizePolicy>());
    }
    else {
        ch = (currentValue != value);
    }

    if (!ch)
        return;

    //2. Then change it, and store old value if necessary
    if (rememberOldValue) {
        if (!d->changed)
            d->oldValue = currentValue;
        d->changed = true;
    }
    else {
        d->oldValue = QVariant(); // clear old value
        d->changed = false;
    }
    if (d->parent) {
        d->parent->childValueChanged(this, value, rememberOldValue);
    }

    QVariant prevValue;
    if (d->composed && useComposedProperty) {
        prevValue = currentValue; //???
        d->composed->setChildValueChangedEnabled(false);
        d->composed->setValue(this, value, rememberOldValue);
        d->composed->setChildValueChangedEnabled(true);
    }
    else {
        prevValue = currentValue;
    }

    d->value = value;

    if (!d->parent) { // emit only if parent has not done it
        emitPropertyChanged(); // called as last step in this method!
    }
}

void
KProperty::resetValue()
{
    d->changed = false;
    bool cleared = false;
    if (d->set)
        d->set->informAboutClearing(cleared); //inform me about possibly clearing the property sets
    setValue(oldValue(), false);
    if (cleared)
        return; //property set has been cleared: no further actions make sense as 'this' is dead

    // maybe parent  prop is also unchanged now
    if (d->parent && d->parent->value() == d->parent->oldValue())
        d->parent->d->changed = false;

    if (d->sets) {
        foreach (QPointer<KPropertySet> set, *d->sets) {
            if (!set.isNull()) //may be destroyed in the meantime
                emit set->propertyReset(*set, *this);
        }
    } else if (d->set) {
        emit d->set->propertyReset(*d->set, *this);
    }
}

KPropertyListData* KProperty::listData() const
{
    return d->listData;
}

void
KProperty::setListData(KPropertyListData* list)
{
    if (list == d->listData)
        return;
    delete d->listData;
    d->listData = list;
}

void
KProperty::setListData(const QStringList &keys, const QStringList &names)
{
    KPropertyListData* list = new KPropertyListData(keys, names);
    setListData(list);
}

////////////////////////////////////////////////////////////////

bool
KProperty::isNull() const
{
    return d->name.isEmpty();
}

bool
KProperty::isModified() const
{
    return d->changed;
}

void
KProperty::clearModifiedFlag()
{
    d->changed = false;
}

bool
KProperty::isReadOnly() const
{
    return d->readOnly;
}

void
KProperty::setReadOnly(bool readOnly)
{
    d->readOnly = readOnly;
}

bool
KProperty::isVisible() const
{
    return d->visible;
}

void
KProperty::setVisible(bool visible)
{
    d->visible = visible;
}

int
KProperty::autoSync() const
{
    return d->autosync;
}

void
KProperty::setAutoSync(int sync)
{
    d->autosync = sync;
}

bool
KProperty::isStorable() const
{
    return d->storable;
}

void
KProperty::setStorable(bool storable)
{
    d->storable = storable;
}

void
KProperty::setOption(const char* name, const QVariant& val)
{
    d->options[name] = val;
}

QVariant
KProperty::option(const char* name, const QVariant& defaultValue) const
{
    if (d->options.contains(name))
        return d->options[name];
    return defaultValue;
}

bool
KProperty::hasOptions() const
{
    return !d->options.isEmpty();
}

/////////////////////////////////////////////////////////////////

const KProperty&
KProperty::operator= (const QVariant & val)
{
    setValue(val);
    return *this;
}

const KProperty&
KProperty::operator= (const KProperty & property)
{
    if (&property == this)
        return *this;

    delete d->listData;
    d->listData = 0;
    delete d->children;
    d->children = 0;
    delete d->relatedProperties;
    d->relatedProperties = 0;
    delete d->composed;
    d->composed = 0;

    d->name = property.d->name;
    d->setCaptionForDisplaying(property.captionForDisplaying());
    d->description = property.d->description;
    d->type = property.d->type;

    d->icon = property.d->icon;
    d->autosync = property.d->autosync;
    d->visible = property.d->visible;
    d->storable = property.d->storable;
    d->readOnly = property.d->readOnly;
    d->options = property.d->options;

    if (property.d->listData) {
        d->listData = new KPropertyListData(*property.d->listData);
    }
    if (property.d->composed) {
        delete d->composed;
        d->composed = KPropertyFactoryManager::self()->createComposedProperty(this);
        // updates all children value, using KComposedPropertyInterface
        setValue(property.value());
    } else {
        d->value = property.d->value;
        if (property.d->children) {
            // no KComposedPropertyInterface (should never happen), simply copy all children
            d->children = new QList<KProperty*>();
            QList<KProperty*>::ConstIterator endIt = property.d->children->constEnd();
            for (QList<KProperty*>::ConstIterator it = property.d->children->constBegin(); it != endIt; ++it) {
                KProperty *child = new KProperty(*(*it));
                addChild(child);
            }
        }
    }

    if (property.d->relatedProperties) {
        d->relatedProperties = new QList<KProperty*>(*(property.d->relatedProperties));
    }

    // update these later because they may have been changed when creating children
    d->oldValue = property.d->oldValue;
    d->changed = property.d->changed;
    return *this;
}

bool
KProperty::operator ==(const KProperty &prop) const
{
    return ((d->name == prop.d->name) && (value() == prop.value()));
}

/////////////////////////////////////////////////////////////////

const QList<KProperty*>*
KProperty::children() const
{
    return d->children;
}

KProperty*
KProperty::child(const QByteArray &name)
{
    QList<KProperty*>::ConstIterator endIt = d->children->constEnd();
    for (QList<KProperty*>::ConstIterator it = d->children->constBegin(); it != endIt; ++it) {
        if ((*it)->name() == name)
            return *it;
    }
    return 0;
}

KProperty*
KProperty::parent() const
{
    return d->parent;
}

void
KProperty::addChild(KProperty *prop)
{
    if (!prop)
        return;

    if (!d->children || qFind(d->children->begin(), d->children->end(), prop) == d->children->end()) { // not in our list
        if (!d->children)
            d->children = new QList<KProperty*>();
        d->children->append(prop);
        prop->d->parent = this;
    } else {
        kprWarning() << "property" << name()
                   << ": child property" << prop->name() << "already added";
        return;
    }
}

void
KProperty::addSet(KPropertySet *set)
{
    if (!set)
        return;

    if (!d->set) {//simple case
        d->set = set;
        return;
    }
    if (d->set == set || d->sets->contains(set))
        return;
    if (!d->sets) {
        d->sets = new QList< QPointer<KPropertySet> >;
    }

    d->sets->append(QPointer<KPropertySet>(set));
}

const QList<KProperty*>*
KProperty::related() const
{
    return d->relatedProperties;
}

void
KProperty::addRelatedProperty(KProperty *property)
{
    if (!d->relatedProperties)
        d->relatedProperties = new QList<KProperty*>();

    QList<KProperty*>::iterator it = qFind(d->relatedProperties->begin(), d->relatedProperties->end(), property);
    if (it == d->relatedProperties->end()) // not in our list
        d->relatedProperties->append(property);
}

KComposedPropertyInterface* KProperty::composedProperty() const
{
    return d->composed;
}

void
KProperty::setComposedProperty(KComposedPropertyInterface *prop)
{
    if (d->composed == prop)
        return;
    delete d->composed;
    d->composed = prop;
}

#if 0
int Property::sortingKey() const
{
    return d->sortingKey;
}

void Property::setSortingKey(int key)
{
    d->sortingKey = key;
}
#endif

void KProperty::emitPropertyChanged()
{
    QList< QPointer<KPropertySet> > *sets = 0;
    if (d->sets) {
        sets = d->sets;
    }
    else if (d->parent) {
        sets = d->parent->d->sets;
    }
    if (sets) {
        foreach (QPointer<KPropertySet> s, *sets) {
            if (!s.isNull()) { //may be destroyed in the meantime
                emit s->propertyChangedInternal(*s, *this);
                emit s->propertyChanged(*s, *this);
            }
        }
    }
    else {
        QPointer<KPropertySet> set;
        set = d->set;
        if (d->set) {
            set = d->set;
        }
        else if (d->parent) {
            set = d->parent->d->set;
        }
        if (!set.isNull()) {
            //if the slot connect with that signal may call set->clear() - that's
            //the case e.g. at kexi/plugins/{macros|scripting}/* -  this Property
            //may got destroyed ( see Set::removeProperty(Property*) ) while we are
            //still on it. So, if we try to access ourself/this once the signal
            //got emitted we may end in a very hard to reproduce crash. So, the
            //emit should happen as last step in this method!
            emit set->propertyChangedInternal(*set, *this);
            emit set->propertyChanged(*set, *this);
        }
    }
}

QMap<QByteArray, QVariant> KProperty::options() const
{
    return d->options;
}

/////////////////////////////////////////////////////////////////

void KProperty::debug() const
{
    kprDebug() << *this;
}

KPROPERTY_EXPORT QDebug operator<<(QDebug dbg, const KProperty &p)
{
    dbg.nospace() << "KProperty("
        << "NAME=" << p.name();
    if (!p.caption().isEmpty()) {
        dbg.nospace() << " CAPTION=" << p.caption();
    }
    if (!p.description().isEmpty()) {
        dbg.nospace() << " DESC=" << p.description();
    }
    dbg.nospace() << " TYPE=" << p.type();
    if (p.value().isValid()) {
        dbg.nospace() << " VALUE=" << p.value();
    }
    else {
        dbg.nospace() << " VALUE=<INVALID>";
    }
    if (p.oldValue().isValid()) {
        dbg.nospace() << " OLDVALUE=" << p.oldValue();
    }
    if (p.isModified()) {
        dbg.nospace() << " MODIFIED";
    }
    if (!p.isVisible()) {
        dbg.nospace() << " HIDDEN";
    }

//! @todo children...

    if (p.hasOptions()) {
        dbg.nospace() << " OPTIONS(" << p.options().count() << "): [";
        QList<QByteArray> optionKeys( p.options().keys() );
        qSort(optionKeys);
        bool first = true;
        foreach (const QByteArray& key, optionKeys) {
            if (first) {
                first = false;
            }
            else {
                dbg.space() << ",";
            }
            dbg.nospace() << key << ":" << p.option(key.constData());
        }
        dbg.nospace() << "]";
    }

    dbg.nospace() << ")";
    return dbg.space();
}
