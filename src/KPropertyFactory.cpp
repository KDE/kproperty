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


#include "KPropertyFactory.h"

//---------------

//! @internal
class KPropertyFactoryManager::Private
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
};

 Q_GLOBAL_STATIC(KPropertyFactoryManager, _self)

//! @internal
class KPropertyFactory::Private
{
public:
    Private()
    {
    }
    ~Private()
    {
    }

    QHash<int, KComposedPropertyCreatorInterface*> composedPropertyCreators;
    QSet<KComposedPropertyCreatorInterface*> composedPropertyCreatorsSet;
};

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

#ifdef KPROPERTY_WIDGETS
QHash<int, KPropertyEditorCreatorInterface*> KPropertyFactory::editorCreators() const
{
    return d->editorCreators;
}

QHash<int, KPropertyValuePainterInterface*> KPropertyFactory::valuePainters() const
{
    return d->valuePainters;
}

QHash<int, KPropertyValueDisplayInterface*> KPropertyFactory::valueDisplays() const
{
    return d->valueDisplays;
}

void KPropertyFactory::addEditor(int type, KPropertyEditorCreatorInterface *creator)
{
    addEditorInternal( type, creator, true );
    if (dynamic_cast<KComposedPropertyCreatorInterface*>(creator)) {
        addComposedPropertyCreatorInternal( type,
            dynamic_cast<KComposedPropertyCreatorInterface*>(creator), false/* !own*/ );
    }
    if (dynamic_cast<KPropertyValuePainterInterface*>(creator)) {
        addPainterInternal( type, dynamic_cast<KPropertyValuePainterInterface*>(creator), false/* !own*/ );
    }
    if (dynamic_cast<KPropertyValueDisplayInterface*>(creator)) {
        addDisplayInternal( type, dynamic_cast<KPropertyValueDisplayInterface*>(creator), false/* !own*/ );
    }
}
#endif

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

//------------

KPropertyFactoryManager::KPropertyFactoryManager()
        : QObject(0)
        , d(new Private)
{
    setObjectName(QLatin1String("KPropertyFactoryManager"));
    //TODO ??? registerFactory(new KDefaultPropertyFactory);

}

KPropertyFactoryManager::~KPropertyFactoryManager()
{
    delete d;
}

KPropertyFactoryManager* KPropertyFactoryManager::self()
{
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
}

KComposedPropertyInterface* KPropertyFactoryManager::createComposedProperty(KProperty *parent)
{
    const KComposedPropertyCreatorInterface *creator = d->composedPropertyCreators.value( parent->type() );
    return creator ? creator->createComposedProperty(parent) : 0;
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

KComposedPropertyInterface::KComposedPropertyInterface(KProperty *parent)
 : m_childValueChangedEnabled(true)
{
    Q_UNUSED(parent)
}

KComposedPropertyInterface::~KComposedPropertyInterface()
{
}

KComposedPropertyCreatorInterface::KComposedPropertyCreatorInterface()
{
}

KComposedPropertyCreatorInterface::~KComposedPropertyCreatorInterface()
{
}
