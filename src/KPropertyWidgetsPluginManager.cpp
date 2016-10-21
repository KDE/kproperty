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


#include "KPropertyWidgetsPluginManager.h"
#include "KDefaultPropertyFactory.h"
#include "KPropertyEditorView.h"
#include "KPropertyUtils.h"

//! Class for access to KPropertyWidgetsPluginManager constructor
class KPropertyWidgetsPluginManagerSingleton
{
public:
    KPropertyWidgetsPluginManager object;
};

//! @internal
class KPropertyWidgetsPluginManager::Private
{
public:
    Private()
    {
    }
    ~Private()
    {
    }

    QHash<int, KPropertyEditorCreatorInterface*> editorCreators;
    QHash<int, KPropertyValuePainterInterface*> valuePainters;
};

Q_GLOBAL_STATIC(KPropertyWidgetsPluginManagerSingleton, _self)

//! @internal Make sure sure the KPropertyWidgetsPluginManager is created after
//! KPropertyFactoryManager (delayed). Unless KPropertyFactoryManager is created,
//! KPropertyWidgetsPluginManager isn't created.
//! @todo is this worth putting in a reusable macro?
struct KPropertyWidgetsPluginManagerInitializer {
    KPropertyWidgetsPluginManagerInitializer() {
        KPropertyFactoryManager::addInitFunction(&initMe);
    }
    static void initMe() { KPropertyWidgetsPluginManager::self(); }
};
namespace {
KPropertyWidgetsPluginManagerInitializer init;
}

//------------

KPropertyWidgetsPluginManager::KPropertyWidgetsPluginManager()
        : d(new Private)
{
    registerFactory(new KDefaultPropertyFactory);
}

KPropertyWidgetsPluginManager::~KPropertyWidgetsPluginManager()
{
    delete d;
}

KPropertyWidgetsPluginManager* KPropertyWidgetsPluginManager::self()
{
    KPropertyFactoryManager::self(); // make sure KPropertyFactoryManager instance exists
                                     // before KPropertyWidgetsPluginManager ctor is called
    return &_self->object;
}

void KPropertyWidgetsPluginManager::registerFactory(KPropertyWidgetsFactory *factory)
{
    KPropertyFactoryManager::self()->registerFactory(factory);

    QHash<int, KPropertyEditorCreatorInterface*>::ConstIterator editorCreatorsItEnd
        = factory->editorCreators().constEnd();
    for (QHash<int, KPropertyEditorCreatorInterface*>::ConstIterator it( factory->editorCreators().constBegin() );
        it != editorCreatorsItEnd; ++it)
    {
        d->editorCreators.insert(it.key(), it.value());
    }
    QHash<int, KPropertyValuePainterInterface*>::ConstIterator valuePaintersItEnd
        = factory->valuePainters().constEnd();
    for (QHash<int, KPropertyValuePainterInterface*>::ConstIterator it( factory->valuePainters().constBegin() );
        it != valuePaintersItEnd; ++it)
    {
        d->valuePainters.insert(it.key(), it.value());
    }
}

bool KPropertyWidgetsPluginManager::isEditorForTypeAvailable( int type ) const
{
    return d->editorCreators.value(type);
}

QWidget * KPropertyWidgetsPluginManager::createEditor(
    int type, QWidget *parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const KPropertyEditorCreatorInterface *creator = d->editorCreators.value(type);
    if (!creator)
        return 0;
    QWidget *w = creator->createEditor(type, parent, option, index);
    KProperty *property = KPropertyUtils::propertyForIndex(index);
    if (w && property) {
       w->setObjectName(QLatin1String(property->name()));
       if (creator->options.removeBorders) {
//! @todo get real border color from the palette
            QColor gridLineColor(qobject_cast<KPropertyEditorView*>(parent->parentWidget()) ?
                qobject_cast<KPropertyEditorView*>(parent->parentWidget())->gridLineColor()
                : KPropertyEditorView::defaultGridLineColor());
            QString cssClassName = QLatin1String(w->metaObject()->className());
            cssClassName.replace(QLatin1String("KProperty"), QString()); //!< @todo
            QString css =
                QString::fromLatin1("%1 { border-top: 1px solid %2; } ")
                .arg(cssClassName).arg(gridLineColor.name());
//            kprDebug() << css;
            w->setStyleSheet(css);
        }
    }
    return w;
}

bool KPropertyWidgetsPluginManager::paint( int type, QPainter * painter,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const KPropertyValuePainterInterface *_painter = d->valuePainters.value(type);
    if (!_painter)
        return false;
    QStyleOptionViewItem realOption(option);
    if (option.state & QStyle::State_Selected) {
        // paint background because there may be editor widget with no autoFillBackground set
        realOption.palette.setBrush(QPalette::Text, realOption.palette.highlightedText());
        painter->fillRect(realOption.rect, realOption.palette.highlight());
    }
    painter->setPen(realOption.palette.text().color());
    _painter->paint(painter, realOption, index);
    return true;
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
