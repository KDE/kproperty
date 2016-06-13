/* This file is part of the KDE project
   Copyright (C) 2008-2016 Jarosław Staniek <staniek@kde.org>

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

#include "KPropertyEditorView.h"
#include "KPropertyEditorDataModel.h"
#include "KProperty.h"
#include "KPropertySet.h"
#include "KPropertyWidgetsFactory.h"
#include "KPropertyWidgetsPluginManager.h"
#include "kproperty_debug.h"
#include "KPropertyUtils.h"
#include "KPropertyUtils_p.h"

#include <QIcon>
#include <QPointer>
#include <QItemDelegate>
#include <QPainter>
#include <QMouseEvent>
#include <QToolTip>
#include <QApplication>
#include <QHeaderView>
#include <QLineEdit>

#if 0 // not sure if we should use it, better to fix Oxygen?
#include <kexiutils/styleproxy.h>

//! Used to alter the widget's style at design time
class EditorViewStyle : public KexiUtils::StyleProxy
{
public:
    explicit EditorViewStyle(QStyle* parentStyle) : KexiUtils::StyleProxy(parentStyle)
    {
    }

    virtual void drawPrimitive(PrimitiveElement elem, const QStyleOption* option,
        QPainter* painter, const QWidget* widget) const
    {
/*        if (elem == PE_PanelLineEdit) {
            const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame*>(option);
            if (panel) {
                QStyleOptionFrame alteredOption(*panel);
                alteredOption.lineWidth = 0;
                KexiUtils::StyleProxy::drawPrimitive(elem, &alteredOption,
                    painter, widget);
                return;
            }
        }*/
        KexiUtils::StyleProxy::drawPrimitive(elem, option,
            painter, widget);
    }
};
#endif

static bool computeAutoSync(KProperty *property, bool defaultAutoSync)
{
    return (property->autoSync() != 0 && property->autoSync() != 1) ?
                defaultAutoSync : (property->autoSync() != 0);
}

//----------

class ItemDelegate : public QItemDelegate
{
public:
    explicit ItemDelegate(KPropertyEditorView *parent);
    virtual ~ItemDelegate();
    virtual void paint(QPainter *painter,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option,
        const QModelIndex &index) const;
    virtual QWidget * createEditor(QWidget *parent,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    mutable QPointer<QWidget> m_currentEditor;
};

ItemDelegate::ItemDelegate(KPropertyEditorView *parent)
: QItemDelegate(parent)
{
}

ItemDelegate::~ItemDelegate()
{
}

static int getIconSize(int fontPixelSize)
{
    return fontPixelSize * 0.85;
}

static int typeForProperty( KProperty* prop )
{
    if (prop->listData())
        return KProperty::ValueFromList;
    else
        return prop->type();
}

void ItemDelegate::paint(QPainter *painter,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    QStyleOptionViewItem alteredOption(option);
    const QColor gridLineColor(qobject_cast<KPropertyEditorView*>(parent())->gridLineColor());
    if (gridLineColor.isValid()) {
        alteredOption.rect.setTop(alteredOption.rect.top() + 1);
    }
    painter->save();
    QRect r(option.rect);
    const KPropertyEditorDataModel *editorModel = dynamic_cast<const KPropertyEditorDataModel*>(index.model());
    if (!editorModel) {
        return;
    }
    bool modified = false;
    if (index.column()==0) {
        r.setWidth(r.width() - 1);
        r.setLeft(0);

        QVariant modifiedVariant( editorModel->data(index, KPropertyEditorDataModel::PropertyModifiedRole) );
        if (modifiedVariant.isValid() && modifiedVariant.toBool()) {
            modified = true;
            QFont font(alteredOption.font);
            font.setBold(true);
            alteredOption.font = font;
        }
    }
    else {
        r.setLeft(r.left()-1);
    }
    const int x2 = alteredOption.rect.right();
    const int y2 = alteredOption.rect.bottom();
    const int iconSize = getIconSize( alteredOption.font.pixelSize() );
    if (modified) {
        alteredOption.rect.setRight( alteredOption.rect.right() - iconSize * 1 );
    }

    KProperty *property = editorModel->propertyForIndex(index);
    const int t = typeForProperty( property );
    bool useQItemDelegatePaint = true; // ValueDisplayInterface is used by default
    if (index.column() == 1 && KPropertyWidgetsPluginManager::self()->paint(t, painter, alteredOption, index)) {
        useQItemDelegatePaint = false;
    }
    if (useQItemDelegatePaint) {
        QItemDelegate::paint(painter, alteredOption, index);
    }

    if (modified) {
        alteredOption.rect.setRight( alteredOption.rect.right() - iconSize * 3 / 2 );
        int y1 = alteredOption.rect.top();
        QLinearGradient grad(x2 - iconSize * 2, y1, x2 - iconSize / 2, y1);
        QColor color(
            alteredOption.palette.color(
                (alteredOption.state & QStyle::State_Selected) ? QPalette::Highlight : QPalette::Base ));
        color.setAlpha(0);
        grad.setColorAt(0.0, color);
        color.setAlpha(255);
        grad.setColorAt(0.5, color);
        QBrush gradBrush(grad);
        painter->fillRect(x2 - iconSize * 2, y1,
            iconSize * 2, y2 - y1 + 1, gradBrush);
        QPixmap revertIcon(QIcon::fromTheme(QLatin1String("edit-undo")).pixmap(iconSize, iconSize));

        //!TODO
        //revertIcon = KIconEffect().apply(revertIcon, KIconEffect::Colorize, 1.0,
        //    alteredOption.palette.color(
        //        (alteredOption.state & QStyle::State_Selected) ? QPalette::HighlightedText : QPalette::Text ), false);
        //painter->drawPixmap( x2 - iconSize - 2,
        //   y1 + 1 + (alteredOption.rect.height() - revertIcon.height()) / 2, revertIcon);
    }

    if (gridLineColor.isValid()) {
        QPen pen(gridLineColor);
        painter->setPen(pen);
        painter->drawRect(r);
    }
    else {
        QPen pen(alteredOption.palette.color(QPalette::AlternateBase));
        painter->setPen(pen);
        painter->drawLine(r.topLeft(), r.topRight());
    }
    //kprDebug()<<"rect:" << r << "viewport:" << painter->viewport() << "window:"<<painter->window();
    painter->restore();
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    return QItemDelegate::sizeHint(option, index) + QSize(0, 2);
}

QWidget * ItemDelegate::createEditor(QWidget * parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    if (!index.isValid())
        return 0;
    QStyleOptionViewItem alteredOption(option);
    KProperty *property = KPropertyUtils::propertyForIndex(index);
    const int t = property ? typeForProperty(property) : KProperty::String;
    alteredOption.rect.setHeight(alteredOption.rect.height()+3);
    QWidget *w = KPropertyWidgetsPluginManager::self()->createEditor(t, parent, alteredOption, index);
    if (!w) {
        // fall back to String type
        w = KPropertyWidgetsPluginManager::self()->createEditor(KProperty::String, parent, alteredOption, index);
    }
    if (w) {
        if (-1 != w->metaObject()->indexOfSignal(QMetaObject::normalizedSignature("commitData(QWidget*)").constData())
            && property && !property->children())
        {
        }
    }
    else {
        w = QItemDelegate::createEditor(parent, alteredOption, index);
    }
    QObject::disconnect(w, SIGNAL(commitData(QWidget*)),
        this, SIGNAL(commitData(QWidget*)));
    if (property && computeAutoSync( property, static_cast<KPropertyEditorView*>(this->parent())->isAutoSync() )) {
        QObject::connect(w, SIGNAL(commitData(QWidget*)),
            this, SIGNAL(commitData(QWidget*)));
    }
    m_currentEditor = w;
    return w;
}

//----------

class KPropertyEditorView::Private
{
public:
    Private()
     : set(0)
     , model(0)
     , gridLineColor( KPropertyEditorView::defaultGridLineColor() )
     , autoSync(true)
     , slotPropertyChangedEnabled(true)
    {
    }
    KPropertySet *set;
    KPropertyEditorDataModel *model;
    ItemDelegate *itemDelegate;
    QColor gridLineColor;
    bool autoSync;
    bool slotPropertyChangedEnabled;
};

KPropertyEditorView::KPropertyEditorView(QWidget* parent)
        : QTreeView(parent)
        , d( new Private )
{
    setObjectName(QLatin1String("KPropertyEditorView"));
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setAnimated(false);
    setAllColumnsShowFocus(true);
    header()->setSectionsMovable(false);

    setEditTriggers(
          QAbstractItemView::CurrentChanged
        | QAbstractItemView::DoubleClicked
        | QAbstractItemView::EditKeyPressed
        | QAbstractItemView::AnyKeyPressed
        | QAbstractItemView::AllEditTriggers);

    setItemDelegate(d->itemDelegate = new ItemDelegate(this));
}

KPropertyEditorView::~KPropertyEditorView()
{
    delete d;
}

void KPropertyEditorView::changeSet(KPropertySet *set, SetOptions options)
{
    changeSetInternal(set, options, QByteArray());
}

void KPropertyEditorView::changeSet(KPropertySet *set, const QByteArray& propertyToSelect, SetOptions options)
{
    changeSetInternal(set, options, propertyToSelect);
}

void KPropertyEditorView::changeSetInternal(KPropertySet *set, SetOptions options,
    const QByteArray& propertyToSelect)
{
//! @todo port??
#if 0
    if (d->insideSlotValueChanged) {
        //changeSet() called from inside of slotValueChanged()
        //this is dangerous, because there can be pending events,
        //especially for the GUI stuff, so let's do delayed work
        d->setListLater_list = set;
        d->preservePrevSelection_preservePrevSelection = preservePrevSelection;
        d->preservePrevSelection_propertyToSelect = propertyToSelect;
        qApp->processEvents(QEventLoop::AllEvents);
        if (d->set) {
            //store prev. selection for this prop set
            if (d->currentItem)
                d->set->setPrevSelection(d->currentItem->property()->name());
            kprDebug() << d->set->prevSelection();
        }
        if (!d->setListLater_set) {
            d->setListLater_set = true;
            d->changeSetLaterTimer.setSingleShot(true);
            d->changeSetLaterTimer.start(10);
        }
        return;
    }
#endif

    const bool setChanged = d->set != set;
    if (d->set) {
        acceptInput();
        //store prev. selection for this prop set
        QModelIndex index = currentIndex();
        if (index.isValid()) {
//! @todo This crashes when changing the interpreter type in the script plugin
#if 0
            Property *property = d->model->propertyForIndex(index);
            //if (property->isNull())
            //    kprDebug() << "WTF? a NULL property?";
            //else
                //d->set->setPreviousSelection(property->name());
#endif
        }
        else {
            d->set->setPreviousSelection(QByteArray());
        }
        if (setChanged) {
            d->set->disconnect(this);
        }
    }

    QByteArray selectedPropertyName1 = propertyToSelect;
    QByteArray selectedPropertyName2 = propertyToSelect;
    if (options & PreservePreviousSelection) {
        //try to find prev. selection:
        //1. in new list's prev. selection
        if (set)
            selectedPropertyName1 = set->previousSelection();
        //2. in prev. list's current selection
        if (d->set)
            selectedPropertyName2 = d->set->previousSelection();
    }

    if (setChanged) {
        d->set = set;
    }
    if (d->set && setChanged) {
        //receive property changes
        connect(d->set, SIGNAL(propertyChangedInternal(KPropertySet&,KProperty&)),
                this, SLOT(slotPropertyChanged(KPropertySet&,KProperty&)));
        connect(d->set, SIGNAL(propertyReset(KPropertySet&,KProperty&)),
                this, SLOT(slotPropertyReset(KPropertySet&,KProperty&)));
        connect(d->set, SIGNAL(aboutToBeCleared()), this, SLOT(slotSetWillBeCleared()));
        connect(d->set, SIGNAL(aboutToBeDeleted()), this, SLOT(slotSetWillBeDeleted()));
    }

    KPropertyEditorDataModel *oldModel = d->model;
    const KPropertySetIterator::Order setOrder
        = (options & AlphabeticalOrder) ? KPropertySetIterator::AlphabeticalOrder : KPropertySetIterator::InsertionOrder;
    d->model = d->set ? new KPropertyEditorDataModel(d->set, this, setOrder) : 0;
    setModel( d->model );
    delete oldModel;

    if (d->model && d->set && !d->set->isEmpty() && (options & ExpandChildItems)) {
        const int rowCount = d->model->rowCount();
        for (int row = 0; row < rowCount; row++) {
            expand( d->model->index(row, 0) );
        }
    }

    emit propertySetChanged(d->set);

    if (d->set) {
        //select prev. selected item
        QModelIndex index;
        if (!selectedPropertyName2.isEmpty()) //try other one for old prop set
            index = d->model->indexForPropertyName( selectedPropertyName2 );
        if (!index.isValid() && !selectedPropertyName1.isEmpty()) //try old one for current prop set
            index = d->model->indexForPropertyName( selectedPropertyName1 );

        if (index.isValid()) {
            setCurrentIndex(index);
            scrollTo(index);
        }
    }
}

void KPropertyEditorView::slotSetWillBeCleared()
{
    changeSet(0, QByteArray());
}

void KPropertyEditorView::slotSetWillBeDeleted()
{
    changeSet(0, QByteArray());
}

void KPropertyEditorView::setAutoSync(bool enable)
{
    d->autoSync = enable;
}

bool KPropertyEditorView::isAutoSync() const
{
    return d->autoSync;
}

void KPropertyEditorView::currentChanged( const QModelIndex & current, const QModelIndex & previous )
{
    QTreeView::currentChanged( current, previous );
}

bool KPropertyEditorView::edit( const QModelIndex & index, EditTrigger trigger, QEvent * event )
{
    bool result = QTreeView::edit( index, trigger, event );
    if (result) {
      QLineEdit *lineEditEditor = dynamic_cast<QLineEdit*>( (QObject*)d->itemDelegate->m_currentEditor );
      if (lineEditEditor) {
        lineEditEditor->deselect();
        lineEditEditor->end(false);
      }
    }
    return result;
}

void KPropertyEditorView::drawBranches( QPainter * painter, const QRect & rect, const QModelIndex & index ) const
{
    QTreeView::drawBranches( painter, rect, index );
}

QRect KPropertyEditorView::revertButtonArea( const QModelIndex& index ) const
{
    if (index.column() != 0)
        return QRect();
    QVariant modifiedVariant( d->model->data(index, KPropertyEditorDataModel::PropertyModifiedRole) );
    if (!modifiedVariant.isValid() || !modifiedVariant.toBool())
        return QRect();
    const int iconSize = getIconSize( fontInfo().pixelSize() );
    int x2 = columnWidth(0);
    int x1 = x2 - iconSize - 2;
    QRect r(visualRect(index));
//    kprDebug() << r;
    r.setLeft(x1);
    r.setRight(x2);
//    kprDebug() << r;
    return r;
}

bool KPropertyEditorView::withinRevertButtonArea( int x, const QModelIndex& index ) const
{
    QRect r(revertButtonArea( index ));
    if (!r.isValid())
        return false;
    return r.left() < x && x < r.right();
}

void KPropertyEditorView::mousePressEvent ( QMouseEvent * event )
{
    QTreeView::mousePressEvent( event );
    QModelIndex index = indexAt( event->pos() );
    setCurrentIndex(index);
    if (withinRevertButtonArea( event->x(), index )) {
        undo();
    }
}

void KPropertyEditorView::undo()
{
    if (!d->set || d->set->isReadOnly())
        return;

    KProperty *property = d->model->propertyForIndex(currentIndex());
    if (computeAutoSync( property, d->autoSync ))
        property->resetValue();
}

void KPropertyEditorView::acceptInput()
{
//! @todo
}

void KPropertyEditorView::commitData( QWidget * editor )
{
    d->slotPropertyChangedEnabled = false;
    QAbstractItemView::commitData( editor );
    d->slotPropertyChangedEnabled = true;
}

bool KPropertyEditorView::viewportEvent( QEvent * event )
{
    if (event->type() == QEvent::ToolTip) {
        QHelpEvent *hevent = static_cast<QHelpEvent*>(event);
        const QModelIndex index = indexAt(hevent->pos());
        if (index.column() == 0 && withinRevertButtonArea( hevent->x(), index )) {
            QRect r(revertButtonArea( index ));
            QToolTip::showText(hevent->globalPos(), tr("Undo changes"), this, r);
        }
        else {
            QToolTip::hideText();
        }
    }
    return QTreeView::viewportEvent(event);
}

QSize KPropertyEditorView::sizeHint() const
{
    return viewportSizeHint();
}

QColor KPropertyEditorView::gridLineColor() const
{
    return d->gridLineColor;
}

void KPropertyEditorView::setGridLineColor(const QColor& color)
{
    d->gridLineColor = color;
    viewport()->update();
}

static QModelIndex findChildItem(const KProperty& property, const QModelIndex &parent)
{
    if (parent.model() && KPropertyUtils::propertyForIndex(parent) == &property) {
        return parent;
    }
    int row = 0;
    while (true) {
        QModelIndex childItem = parent.child(row, 0);
        if (childItem.isValid()) {
            QModelIndex subchild = findChildItem(property, childItem);
            if (subchild.isValid()) {
                return subchild;
            }
        }
        else {
            return QModelIndex();
        }
        row++;
    }
}

void KPropertyEditorView::slotPropertyChanged(KPropertySet& set, KProperty& property)
{
    Q_UNUSED(set);
    if (!d->slotPropertyChangedEnabled)
        return;
    d->slotPropertyChangedEnabled = false;
    KProperty *realProperty = &property;
    while (realProperty->parent()) { // find top-level property
        realProperty = realProperty->parent();
    }
    const QModelIndex parentIndex( d->model->indexForPropertyName(realProperty->name()) );
    if (parentIndex.isValid()) {
        QModelIndex index = findChildItem(property, parentIndex);
        updateSubtree(index);
    }
    d->slotPropertyChangedEnabled = true;
}

void KPropertyEditorView::updateSubtree(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    update(index);
    QModelIndex valueIndex = d->model->indexForColumn(index, 1);
    if (valueIndex.isValid()) {
        update(valueIndex);
    }
    KProperty *property = static_cast<KProperty*>(index.internalPointer());
    if (property->children()) {
        int row = 0;
        foreach (KProperty* p, *property->children()) {
            updateSubtree(d->model->createIndex(row, 0, p));
            ++row;
        }
    }
}

void KPropertyEditorView::slotPropertyReset(KPropertySet& set, KProperty& property)
{
//! @todo OK?
    slotPropertyChanged(set, property);
}
