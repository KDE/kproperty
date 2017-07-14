/* This file is part of the KDE project
   Copyright (C) 2008-2015 Jarosław Staniek <staniek@kde.org>

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


#include "KPropertyFactory.h"

//---------------

//! @internal
class Q_DECL_HIDDEN KPropertyFactoryManager::Private
{
public:
    Private()
    {
    }
    ~Private()
    {
        qDeleteAll(factories);
    }

    QSet<KPropertyFactory*> factories;
    QHash<int, KComposedPropertyCreatorInterface*> composedPropertyCreators;
    QHash<int, KPropertyValueDisplayInterface*> valueDisplays;
};

Q_GLOBAL_STATIC(KPropertyFactoryManager, _self)

//! @internal
class Q_DECL_HIDDEN KPropertyFactory::Private
{
public:
    Private()
    {
    }
    ~Private()
    {
        qDeleteAll(valueDisplaysSet);
    }

    QHash<int, KComposedPropertyCreatorInterface*> composedPropertyCreators;
    QSet<KComposedPropertyCreatorInterface*> composedPropertyCreatorsSet;
    QHash<int, KPropertyValueDisplayInterface*> valueDisplays;
    QSet<KPropertyValueDisplayInterface*> valueDisplaysSet;
};

typedef QList<void (*)()> InitFunctions;
//! @internal Used by KPropertyFactoryManager::addInitFunction()
Q_GLOBAL_STATIC(InitFunctions, _initFunctions)

KPropertyFactory::KPropertyFactory()
    : d( new Private )
{
}

KPropertyFactory::~KPropertyFactory()
{
    delete d;
}

QHash<int, KComposedPropertyCreatorInterface*> KPropertyFactory::composedPropertyCreators() const
{
    return d->composedPropertyCreators;
}

QHash<int, KPropertyValueDisplayInterface*> KPropertyFactory::valueDisplays() const
{
    return d->valueDisplays;
}

void KPropertyFactory::addComposedPropertyCreator( int type, KComposedPropertyCreatorInterface* creator )
{
    addComposedPropertyCreatorInternal( type, creator, true );
}

void KPropertyFactory::addComposedPropertyCreatorInternal(int type, KComposedPropertyCreatorInterface* creator, bool own)
{
    if (own)
        d->composedPropertyCreatorsSet.insert(creator);
    d->composedPropertyCreators.insert(type, creator);
}

void KPropertyFactory::addDisplay(int type, KPropertyValueDisplayInterface *display)
{
    addDisplayInternal(type, display, true);
    if (dynamic_cast<KComposedPropertyCreatorInterface*>(display)) {
        addComposedPropertyCreatorInternal( type,
        dynamic_cast<KComposedPropertyCreatorInterface*>(display), false/* !own*/ );
    }
    if (dynamic_cast<KPropertyValueDisplayInterface*>(display)) {
        addDisplayInternal( type, dynamic_cast<KPropertyValueDisplayInterface*>(display), false/* !own*/ );
    }
}

void KPropertyFactory::addDisplayInternal(int type, KPropertyValueDisplayInterface *display, bool own)
{
    if (own) {
        d->valueDisplaysSet.insert(display);
    }
    d->valueDisplays.insert(type, display);
}

//------------

class Q_DECL_HIDDEN KPropertyValueDisplayInterface::Private
{
public:
    Private() {}
};

KPropertyValueDisplayInterface::KPropertyValueDisplayInterface()
    : d(new Private)
{
}

KPropertyValueDisplayInterface::~KPropertyValueDisplayInterface()
{
    delete d;
}

//static
int KPropertyValueDisplayInterface::maxStringValueLength()
{
    return 250;
}

//static
QString KPropertyValueDisplayInterface::valueToLocalizedString(const QVariant& value)
{
    QString s(value.toString());
    if (KPropertyValueDisplayInterface::maxStringValueLength() < s.length()) {
        s.truncate(KPropertyValueDisplayInterface::maxStringValueLength());
        return QObject::tr("%1...", "Truncated string").arg(s);
    }
    return s;
}

//------------

KPropertyFactoryManager::KPropertyFactoryManager()
        : QObject(nullptr)
        , d(new Private)
{
    setObjectName(QLatin1String("KPropertyFactoryManager"));
}

KPropertyFactoryManager::~KPropertyFactoryManager()
{
    delete d;
}

KPropertyFactoryManager* KPropertyFactoryManager::self()
{
    if (_self.exists()) { // avoid recursion: initFunctions below may call self()
        return _self;
    }
    _self(); // KPropertyFactoryManager should exist as initFunctions may need it
    foreach(void (*initFunction)(), *_initFunctions) {
        initFunction();
    }
    _initFunctions->clear();
    return _self;
}

void KPropertyFactoryManager::registerFactory(KPropertyFactory *factory)
{
    d->factories.insert(factory);
    QHash<int, KComposedPropertyCreatorInterface*>::ConstIterator composedPropertyCreatorsItEnd
        = factory->composedPropertyCreators().constEnd();
    for (QHash<int, KComposedPropertyCreatorInterface*>::ConstIterator it( factory->composedPropertyCreators().constBegin() );
        it != composedPropertyCreatorsItEnd; ++it)
    {
        d->composedPropertyCreators.insert(it.key(), it.value());
    }
    QHash<int, KPropertyValueDisplayInterface*>::ConstIterator valueDisplaysItEnd
        = factory->valueDisplays().constEnd();
    for (QHash<int, KPropertyValueDisplayInterface*>::ConstIterator it( factory->valueDisplays().constBegin() );
        it != valueDisplaysItEnd; ++it)
    {
        d->valueDisplays.insert(it.key(), it.value());
    }
}

KComposedPropertyInterface* KPropertyFactoryManager::createComposedProperty(KProperty *parent)
{
    const KComposedPropertyCreatorInterface *creator = d->composedPropertyCreators.value( parent->type() );
    return creator ? creator->createComposedProperty(parent) : nullptr;
}

//static
void KPropertyFactoryManager::addInitFunction(void (*initFunction)())
{
    _initFunctions->append(initFunction);
}

bool KPropertyFactoryManager::canConvertValueToText(int type) const
{
    return d->valueDisplays.value(type) != nullptr;
}

bool KPropertyFactoryManager::canConvertValueToText(const KProperty* property) const
{
    return canConvertValueToText(property->type());
}

QString KPropertyFactoryManager::propertyValueToString(const KProperty* property) const
{
    const KPropertyValueDisplayInterface *display = d->valueDisplays.value(property->type());
    return display ? display->propertyValueToString(property, QLocale::c()) : property->value().toString();
}

QString KPropertyFactoryManager::valueToString(int type, const QVariant &value) const
{
    const KPropertyValueDisplayInterface *display = d->valueDisplays.value(type);
    return display ? display->valueToString(value, QLocale::c()) : value.toString();
}

QString KPropertyFactoryManager::propertyValueToLocalizedString(const KProperty* property) const
{
    const KPropertyValueDisplayInterface *display = d->valueDisplays.value(property->type());
    return display ? display->propertyValueToString(property, QLocale()) : KPropertyValueDisplayInterface::valueToLocalizedString(property->value());
}

QString KPropertyFactoryManager::valueToLocalizedString(int type, const QVariant &value) const
{
    const KPropertyValueDisplayInterface *display = d->valueDisplays.value(type);
    return display ? display->valueToString(value, QLocale()) : KPropertyValueDisplayInterface::valueToLocalizedString(value.toString());
}

//! @todo
#if 0
    const int type = parent->type();
/*
    CustomPropertyFactory *factory = d->registeredWidgets[type];
    if (factory)
        return factory->createCustomProperty(parent);
*/
    switch (type) {
    case Size:
    case Size_Width:
    case Size_Height:
        return new SizeCustomProperty(parent);
    case Point:
    case Point_X:
    case Point_Y:
        return new PointCustomProperty(parent);
    case Rect:
    case Rect_X:
    case Rect_Y:
    case Rect_Width:
    case Rect_Height:
        return new RectCustomProperty(parent);
    case SizePolicy:
/*    case SizePolicy_HorizontalStretch:
    case SizePolicy_VerticalStretch:
    case SizePolicy_HorizontalPolicy:
    case SizePolicy_VerticalPolicy:*/
        return new SizePolicyCustomProperty(parent);
    default:;
    }
    return 0;
#endif

class Q_DECL_HIDDEN KComposedPropertyInterface::Private
{
public:
    Private() {}
    bool childValueChangedEnabled = true;
};

KComposedPropertyInterface::KComposedPropertyInterface(KProperty *parent)
 : d(new Private)
{
    Q_UNUSED(parent)
}

KComposedPropertyInterface::~KComposedPropertyInterface()
{
    delete d;
}

void KComposedPropertyInterface::childValueChangedInternal(KProperty *child, const QVariant &value,
                                                           KProperty::ValueOptions valueOptions)
{
    if (d->childValueChangedEnabled) {
        childValueChanged(child, value, valueOptions);
    }
}

void KComposedPropertyInterface::setChildValueChangedEnabled(bool set)
{
    d->childValueChangedEnabled = set;
}

bool KComposedPropertyInterface::childValueChangedEnabled() const
{
    return d->childValueChangedEnabled;
}

//---------------

class Q_DECL_HIDDEN KComposedPropertyCreatorInterface::Private
{
public:
    Private() {}
};

KComposedPropertyCreatorInterface::KComposedPropertyCreatorInterface()
    : d(new Private)
{
}

KComposedPropertyCreatorInterface::~KComposedPropertyCreatorInterface()
{
    delete d;
}
