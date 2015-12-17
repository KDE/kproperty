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

#include "KDefaultPropertyFactory.h"
#include "KPropertyEditorView.h"
#include "KPropertyEditorDataModel.h"
#include "KPropertyStringEditor.h"

KPropertyLabel::KPropertyLabel(QWidget *parent, const KPropertyValueDisplayInterface *iface)
    : QLabel(parent)
    , m_iface(iface)
{
  setAutoFillBackground(true);

  KPropertyEditorView* view = 0;
  if (parent) {
      view = qobject_cast<KPropertyEditorView*>(parent->parentWidget());
  }
  const QColor gridLineColor(view ? view->gridLineColor() : KPropertyEditorView::defaultGridLineColor());
  const int top = 1 + (gridLineColor.isValid() ? 1 : 0);

  setContentsMargins(0, top, 0, 0);
  setIndent(1);
}

QVariant KPropertyLabel::value() const
{
    return m_value;
}

void KPropertyLabel::setValue(const QVariant& value)
{
    setText( m_iface->valueToString(value, QLocale()) );
    m_value = value;
}

void KPropertyLabel::paintEvent( QPaintEvent * event )
{
    QLabel::paintEvent(event);
    KPropertyWidgetsFactory::paintTopGridLine(this);
}

//---------------

void paintInternal(const KPropertyValueDisplayInterface *iface,
                   QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    painter->save();
    QRect r(option.rect);
    r.setLeft(r.left()+1);
    painter->drawText( r, Qt::AlignLeft | Qt::AlignVCenter,
        iface->valueToString(index.data(Qt::EditRole), QLocale()));
    painter->restore();
}

//---------------

//! @internal
class KPropertyWidgetsFactoryManager::Private
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

Q_GLOBAL_STATIC(KPropertyWidgetsFactoryManager, _self)

//! @internal Make sure sure the KPropertyWidgetsFactoryManager is created after
//! KPropertyFactoryManager (delayed). Unless KPropertyFactoryManager is created,
//! KPropertyWidgetsFactoryManager isn't created.
//! @todo is this worth putting in a reusable macro?
struct KPropertyWidgetsFactoryManagerInitializer {
    KPropertyWidgetsFactoryManagerInitializer() {
        KPropertyFactoryManager::addInitFunction(&initMe);
    }
    static void initMe() { KPropertyWidgetsFactoryManager::self(); }
};
KPropertyWidgetsFactoryManagerInitializer init;

//! @internal
class KPropertyWidgetsFactory::Private
{
public:
    Private()
    {
    }
    ~Private()
    {
        qDeleteAll(editorCreatorsSet);
        qDeleteAll(valuePaintersSet);
    }

    QHash<int, KPropertyEditorCreatorInterface*> editorCreators;
    QHash<int, KPropertyValuePainterInterface*> valuePainters;

    QSet<KPropertyEditorCreatorInterface*> editorCreatorsSet;
    QSet<KPropertyValuePainterInterface*> valuePaintersSet;
};

KPropertyWidgetsFactory::KPropertyWidgetsFactory()
    : d( new Private )
{
}

KPropertyWidgetsFactory::~KPropertyWidgetsFactory()
{
    delete d;
}

QHash<int, KPropertyEditorCreatorInterface*> KPropertyWidgetsFactory::editorCreators() const
{
    return d->editorCreators;
}

QHash<int, KPropertyValuePainterInterface*> KPropertyWidgetsFactory::valuePainters() const
{
    return d->valuePainters;
}

void KPropertyWidgetsFactory::addEditor(int type, KPropertyEditorCreatorInterface *creator)
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

void KPropertyWidgetsFactory::addPainter(int type, KPropertyValuePainterInterface *painter)
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

void KPropertyWidgetsFactory::addEditorInternal(int type, KPropertyEditorCreatorInterface *editor, bool own)
{
    if (own)
        d->editorCreatorsSet.insert(editor);
    d->editorCreators.insert(type, editor);
}

void KPropertyWidgetsFactory::addPainterInternal(int type, KPropertyValuePainterInterface *painter, bool own)
{
    if (own)
        d->valuePaintersSet.insert(painter);
    d->valuePainters.insert(type, painter);
}

//static
void KPropertyWidgetsFactory::paintTopGridLine(QWidget *widget)
{
    // paint top grid line
    KPropertyEditorView* view = 0;
    if (widget->parentWidget()) {
        view = qobject_cast<KPropertyEditorView*>(widget->parentWidget()->parentWidget());
    }
    const QColor gridLineColor(view ? view->gridLineColor() : KPropertyEditorView::defaultGridLineColor());
    if (gridLineColor.isValid()) {
        QPainter p(widget);
        p.setPen(QPen( QBrush(gridLineColor), 1));
        p.drawLine(0, 0, widget->width()-1, 0);
    }
}

//static
void KPropertyWidgetsFactory::setTopAndBottomBordersUsingStyleSheet(QWidget *widget,
                                                                    const QString& extraStyleSheet)
{
    KPropertyEditorView* view = 0;
    if (widget->parentWidget()) {
        view = qobject_cast<KPropertyEditorView*>(widget->parentWidget()->parentWidget());
    }
    const QColor gridLineColor(view ? view->gridLineColor() : KPropertyEditorView::defaultGridLineColor());
    widget->setStyleSheet(
        QString::fromLatin1("%1 { border-top: 1px solid %2;border-bottom: 1px solid %2; } %3")
        .arg(QLatin1String(widget->metaObject()->className()))
        .arg(gridLineColor.name()).arg(extraStyleSheet));
}

//------------

KPropertyWidgetsFactoryManager::KPropertyWidgetsFactoryManager()
        : d(new Private)
{
    registerFactory(new KDefaultPropertyFactory);
}

KPropertyWidgetsFactoryManager::~KPropertyWidgetsFactoryManager()
{
    delete d;
}

KPropertyWidgetsFactoryManager* KPropertyWidgetsFactoryManager::self()
{
    KPropertyFactoryManager::self(); // make sure KPropertyFactoryManager instance exists
                                     // before KPropertyWidgetsFactoryManager ctor is called
    return _self;
}

void KPropertyWidgetsFactoryManager::registerFactory(KPropertyWidgetsFactory *factory)
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

bool KPropertyWidgetsFactoryManager::isEditorForTypeAvailable( int type ) const
{
    return d->editorCreators.value(type);
}

QWidget * KPropertyWidgetsFactoryManager::createEditor(
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

bool KPropertyWidgetsFactoryManager::paint( int type, QPainter * painter,
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

KPropertyEditorCreatorInterface::KPropertyEditorCreatorInterface()
{
}

KPropertyEditorCreatorInterface::~KPropertyEditorCreatorInterface()
{
}

QWidget* KPropertyEditorCreatorInterface::createEditor(int type, QWidget *parent,
                                                       const QStyleOptionViewItem & option,
                                                       const QModelIndex & index) const
{
    Q_UNUSED(type);
    Q_UNUSED(option);
    Q_UNUSED(index);
    return new KPropertyStringEditor(parent);
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
