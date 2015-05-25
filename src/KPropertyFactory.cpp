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

#include <QGlobalStatic>

#include "KPropertyFactory.h"
#include "KDefaultPropertyFactory.h"
#include "KPropertyEditorView.h"
#include "KPropertyEditorDataModel.h"

KPropertyLabel::KPropertyLabel(QWidget *parent, const KPropertyValueDisplayInterface *iface)
    : QLabel(parent)
    , m_iface(iface)
{
  setAutoFillBackground(true);
  setContentsMargins(0,1,0,0);
  setIndent(1);
}

QVariant KPropertyLabel::value() const
{
    return m_value;
}

void KPropertyLabel::setValue(const QVariant& value)
{
    setText( m_iface->displayText(value) );
    m_value = value;
}

void KPropertyLabel::paintEvent( QPaintEvent * event )
{
    QLabel::paintEvent(event);
    KPropertyFactory::paintTopGridLine(this);
}

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
    QHash<int, KPropertyEditorCreatorInterface*> editorCreators;
    QHash<int, KPropertyValuePainterInterface*> valuePainters;
    QHash<int, KPropertyValueDisplayInterface*> valueDisplays;
};

 Q_GLOBAL_STATIC(KPropertyFactoryManager, _self);

//! @internal
class KPropertyFactory::Private
{
public:
    Private()
    {
    }
    ~Private()
    {
        qDeleteAll(editorCreatorsSet);
        qDeleteAll(valuePaintersSet);
        qDeleteAll(valueDisplaysSet);
    }

    QHash<int, KComposedPropertyCreatorInterface*> composedPropertyCreators;
    QHash<int, KPropertyEditorCreatorInterface*> editorCreators;
    QHash<int, KPropertyValuePainterInterface*> valuePainters;
    QHash<int, KPropertyValueDisplayInterface*> valueDisplays;
    QSet<KComposedPropertyCreatorInterface*> composedPropertyCreatorsSet;
    QSet<KPropertyEditorCreatorInterface*> editorCreatorsSet;
    QSet<KPropertyValuePainterInterface*> valuePaintersSet;
    QSet<KPropertyValueDisplayInterface*> valueDisplaysSet;
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

void KPropertyFactory::addComposedPropertyCreator( int type, KComposedPropertyCreatorInterface* creator )
{
    addComposedPropertyCreatorInternal( type, creator, true );
    if (dynamic_cast<KPropertyEditorCreatorInterface*>(creator)) {
        addEditorInternal( type, dynamic_cast<KPropertyEditorCreatorInterface*>(creator), false/* !own*/ );
    }
    if (dynamic_cast<KPropertyValuePainterInterface*>(creator)) {
        addPainterInternal( type, dynamic_cast<KPropertyValuePainterInterface*>(creator), false/* !own*/ );
    }
    if (dynamic_cast<KPropertyValueDisplayInterface*>(creator)) {
        addDisplayInternal( type, dynamic_cast<KPropertyValueDisplayInterface*>(creator), false/* !own*/ );
    }
}

void KPropertyFactory::addPainter(int type, KPropertyValuePainterInterface *painter)
{
    addPainterInternal(type, painter, true);
    if (dynamic_cast<KComposedPropertyCreatorInterface*>(painter)) {
        addComposedPropertyCreatorInternal( type,
        dynamic_cast<KComposedPropertyCreatorInterface*>(painter), false/* !own*/ );
    }
    if (dynamic_cast<KPropertyEditorCreatorInterface*>(painter)) {
        addEditorInternal( type, dynamic_cast<KPropertyEditorCreatorInterface*>(painter), false/* !own*/ );
    }
    if (dynamic_cast<KPropertyValueDisplayInterface*>(painter)) {
        addDisplayInternal( type, dynamic_cast<KPropertyValueDisplayInterface*>(painter), false/* !own*/ );
    }
}

void KPropertyFactory::addDisplay(int type, KPropertyValueDisplayInterface *display)
{
    addDisplayInternal(type, display, true);
    if (dynamic_cast<KComposedPropertyCreatorInterface*>(display)) {
        addComposedPropertyCreatorInternal( type,
        dynamic_cast<KComposedPropertyCreatorInterface*>(display), false/* !own*/ );
    }
    if (dynamic_cast<KPropertyEditorCreatorInterface*>(display)) {
        addEditorInternal( type, dynamic_cast<KPropertyEditorCreatorInterface*>(display), false/* !own*/ );
    }
    if (dynamic_cast<KPropertyValueDisplayInterface*>(display)) {
        addDisplayInternal( type, dynamic_cast<KPropertyValueDisplayInterface*>(display), false/* !own*/ );
    }
}

void KPropertyFactory::addEditorInternal(int type, KPropertyEditorCreatorInterface *editor, bool own)
{
    if (own)
        d->editorCreatorsSet.insert(editor);
    d->editorCreators.insert(type, editor);
}

void KPropertyFactory::addComposedPropertyCreatorInternal(int type, KComposedPropertyCreatorInterface* creator, bool own)
{
    if (own)
        d->composedPropertyCreatorsSet.insert(creator);
    d->composedPropertyCreators.insert(type, creator);
}

void KPropertyFactory::addPainterInternal(int type, KPropertyValuePainterInterface *painter, bool own)
{
    if (own)
        d->valuePaintersSet.insert(painter);
    d->valuePainters.insert(type, painter);
}

void KPropertyFactory::addDisplayInternal(int type, KPropertyValueDisplayInterface *display, bool own)
{
    if (own)
        d->valueDisplaysSet.insert(display);
    d->valueDisplays.insert(type, display);
}

//static
void KPropertyFactory::paintTopGridLine(QWidget *widget)
{
    // paint top grid line
    QPainter p(widget);
    QColor gridLineColor( dynamic_cast<KPropertyEditorView*>(widget->parentWidget()) ?
        dynamic_cast<KPropertyEditorView*>(widget->parentWidget())->gridLineColor()
        : KPropertyEditorView::defaultGridLineColor() );
    p.setPen(QPen( QBrush(gridLineColor), 1));
    p.drawLine(0, 0, widget->width()-1, 0);
}

//static
void KPropertyFactory::setTopAndBottomBordersUsingStyleSheet(QWidget *widget, QWidget* parent, const QString& extraStyleSheet)
{
    QColor gridLineColor( dynamic_cast<KPropertyEditorView*>(parent) ?
        dynamic_cast<KPropertyEditorView*>(parent)->gridLineColor()
        : KPropertyEditorView::defaultGridLineColor() );
    widget->setStyleSheet(
        QString::fromLatin1("%1 { border-top: 1px solid %2;border-bottom: 1px solid %2; } %3")
        .arg(QLatin1String(widget->metaObject()->className()))
        .arg(gridLineColor.name()).arg(extraStyleSheet));
}

//------------

KPropertyFactoryManager::KPropertyFactoryManager()
        : QObject(0)
        , d(new Private)
{
    setObjectName(QLatin1String("KPropertyFactoryManager"));
    registerFactory(new KDefaultPropertyFactory);
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
    QHash<int, KPropertyValueDisplayInterface*>::ConstIterator valueDisplaysItEnd
        = factory->valueDisplays().constEnd();
    for (QHash<int, KPropertyValueDisplayInterface*>::ConstIterator it( factory->valueDisplays().constBegin() );
        it != valueDisplaysItEnd; ++it)
    {
        d->valueDisplays.insert(it.key(), it.value());
    }
}

bool KPropertyFactoryManager::isEditorForTypeAvailable( int type ) const
{
    return d->editorCreators.value(type);
}

QWidget * KPropertyFactoryManager::createEditor(
    int type, QWidget *parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const KPropertyEditorCreatorInterface *creator = d->editorCreators.value(type);
    if (!creator)
        return 0;
    QWidget *w = creator->createEditor(type, parent, option, index);
    if (w) {
       const KPropertyEditorDataModel *editorModel
           = dynamic_cast<const KPropertyEditorDataModel*>(index.model());
       KProperty *property = editorModel->propertyForItem(index);
       w->setObjectName(QLatin1String(property->name()));
       if (creator->options.removeBorders) {
//! @todo get real border color from the palette
            QColor gridLineColor( dynamic_cast<KPropertyEditorView*>(parent) ?
                dynamic_cast<KPropertyEditorView*>(parent)->gridLineColor()
                : KPropertyEditorView::defaultGridLineColor() );
            QString cssClassName = QLatin1String(w->metaObject()->className());
            cssClassName.replace(QLatin1String("KProperty"), QString()); //!< @todo
            QString css =
                QString::fromLatin1("%1 { border-top: 1px solid %2; } ")
                .arg(cssClassName).arg(gridLineColor.name());
//            kDebug() << css;
            w->setStyleSheet(css);
        }
    }
    return w;
}

bool KPropertyFactoryManager::paint( int type, QPainter * painter,
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

bool KPropertyFactoryManager::canConvertValueToText( int type ) const
{
    return d->valueDisplays.value(type) != 0;
}

bool KPropertyFactoryManager::canConvertValueToText( const KProperty* property ) const
{
    return d->valueDisplays.value( property->type() ) != 0;
}

QString KPropertyFactoryManager::convertValueToText( const KProperty* property ) const
{
    const KPropertyValueDisplayInterface *display = d->valueDisplays.value( property->type() );
    return display ? display->displayTextForProperty( property ) : property->value().toString();
}

KComposedPropertyInterface* KPropertyFactoryManager::createComposedProperty(KProperty *parent)
{
    const KComposedPropertyCreatorInterface *creator = d->composedPropertyCreators.value( parent->type() );
    return creator ? creator->createComposedProperty(parent) : 0;
}

#if 0
    const int type = parent->type();
/* TODO
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

KPropertyEditorCreatorInterface::KPropertyEditorCreatorInterface()
{
}

KPropertyEditorCreatorInterface::~KPropertyEditorCreatorInterface()
{
}

KPropertyEditorCreatorInterface::Options::Options()
 : removeBorders(true)
{
}

KPropertyValuePainterInterface::KPropertyValuePainterInterface()
{
}

KPropertyValuePainterInterface::~KPropertyValuePainterInterface()
{
}

KPropertyValueDisplayInterface::KPropertyValueDisplayInterface()
{
}

KPropertyValueDisplayInterface::~KPropertyValueDisplayInterface()
{
}
