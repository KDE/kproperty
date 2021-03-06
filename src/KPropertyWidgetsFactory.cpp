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


#include "KPropertyFactory.h"

#include "KDefaultPropertyFactory.h"
#include "KPropertyEditorView.h"
#include "KPropertyStringEditor.h"
#include "KPropertyUtils.h"
#include "KPropertyUtils_p.h"

class Q_DECL_HIDDEN KPropertyLabel::Private
{
public:
    Private()
    {
    }
    ~Private()
    {
    }

    const KProperty *property;
    const KPropertyValueDisplayInterface *iface;
    QVariant value;
};

KPropertyLabel::KPropertyLabel(QWidget *parent, const KProperty *property,
                               const KPropertyValueDisplayInterface *iface)
    : QLabel(parent), d(new Private)
{
    d->property = property;
    d->iface = iface;
    setAutoFillBackground(true);

    KPropertyEditorView* view = nullptr;
    if (parent) {
        view = qobject_cast<KPropertyEditorView *>(parent->parentWidget());
    }
    const QColor gridLineColor(view ? view->gridLineColor()
                                    : KPropertyEditorView::defaultGridLineColor());
    const int top = 1 + (gridLineColor.isValid() ? 1 : 0);

    setContentsMargins(0, top, 0, 0);
    setIndent(1);
}

KPropertyLabel::~KPropertyLabel()
{
    delete d;
}

QVariant KPropertyLabel::value() const
{
    return d->value;
}

void KPropertyLabel::setValue(const QVariant& value)
{
    d->value = value;
    setText(d->iface->propertyValueToString(d->property, QLocale()));
}

void KPropertyLabel::paintEvent( QPaintEvent * event )
{
    QLabel::paintEvent(event);
    KPropertyWidgetsFactory::paintTopGridLine(this);
}

//---------------

//! @internal
class Q_DECL_HIDDEN KPropertyWidgetsFactory::Private
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
    KPropertyEditorView* view = nullptr;
    if (widget->parentWidget()) {
        view = qobject_cast<KPropertyEditorView*>(widget->parentWidget()->parentWidget());
    }
    const QColor gridLineColor(view ? view->gridLineColor() : KPropertyEditorView::defaultGridLineColor());
    if (gridLineColor.isValid()) {
        const QAbstractScrollArea *area = qobject_cast<const QAbstractScrollArea*>(widget);
        QWidget *widgetToPaint;
        if (area && area->viewport()) {
            widgetToPaint = area->viewport();
        } else {
            widgetToPaint = widget;
        }
        QPainter p(widgetToPaint);
        p.setPen(QPen( QBrush(gridLineColor), 1));
        p.drawLine(0, 0, widget->width()-1, 0);
    }
}

//static
void KPropertyWidgetsFactory::setTopAndBottomBordersUsingStyleSheet(QWidget *widget,
                                                                    const QString& extraStyleSheet)
{
    KPropertyEditorView* view = nullptr;
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

class Q_DECL_HIDDEN KPropertyEditorCreatorOptions::Private
{
public:
    Private() : bordersVisible(false)
    {
    }
    Private(const Private &other) {
        copy(other);
    }
#define KPropertyEditorCreatorOptionsPrivateArgs(o) std::tie(o.bordersVisible)
    void copy(const Private &other) {
        KPropertyEditorCreatorOptionsPrivateArgs((*this)) = KPropertyEditorCreatorOptionsPrivateArgs(other);
    }
    bool operator==(const Private &other) const {
        return KPropertyEditorCreatorOptionsPrivateArgs((*this)) == KPropertyEditorCreatorOptionsPrivateArgs(other);
    }
    bool bordersVisible;
};

KPropertyEditorCreatorOptions::KPropertyEditorCreatorOptions()
    : d(new Private)
{
}

KPropertyEditorCreatorOptions::KPropertyEditorCreatorOptions(const KPropertyEditorCreatorOptions &other)
    : d(new Private(*other.d))
{
}

KPropertyEditorCreatorOptions::~KPropertyEditorCreatorOptions()
{
    delete d;
}

bool KPropertyEditorCreatorOptions::bordersVisible() const
{
    return d->bordersVisible;
}

void KPropertyEditorCreatorOptions::setBordersVisible(bool visible)
{
    d->bordersVisible = visible;
}

KPropertyEditorCreatorOptions& KPropertyEditorCreatorOptions::operator=(const KPropertyEditorCreatorOptions &other)
{
    if (this != &other) {
        d->copy(*other.d);
    }
    return *this;
}

bool KPropertyEditorCreatorOptions::operator==(const KPropertyEditorCreatorOptions &other) const
{
    return *d == *other.d;
}

//------------

class Q_DECL_HIDDEN KPropertyEditorCreatorInterface::Private
{
public:
    Private()
    {
    }

    //! Options for altering the editor widget creation process
    KPropertyEditorCreatorOptions options;
};

KPropertyEditorCreatorInterface::KPropertyEditorCreatorInterface()
    : d(new Private)
{
}

KPropertyEditorCreatorInterface::~KPropertyEditorCreatorInterface()
{
    delete d;
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

const KPropertyEditorCreatorOptions* KPropertyEditorCreatorInterface::options() const
{
    return &d->options;
}

KPropertyEditorCreatorOptions* KPropertyEditorCreatorInterface::options()
{
    return &d->options;
}

//------------

KPropertyValuePainterInterface::KPropertyValuePainterInterface()
{
}

KPropertyValuePainterInterface::~KPropertyValuePainterInterface()
{
}

//static
void KPropertyValuePainterInterface::paint(const KPropertyValueDisplayInterface *iface,
                                           QPainter *painter, const QStyleOptionViewItem &option,
                                           const QModelIndex &index)
{
    const KPropertyUtilsPrivate::PainterSaver saver(painter);
    QRect r(option.rect);
    r.setLeft(r.left() + 1);
    KProperty *prop = KPropertyUtils::propertyForIndex(index);
    painter->drawText(r, Qt::AlignLeft | Qt::AlignVCenter,
        iface->propertyValueToString(prop, QLocale()));
}
