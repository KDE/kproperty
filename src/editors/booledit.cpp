/* This file is part of the KDE project
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2006-2018 Jarosław Staniek <staniek@kde.org>

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

#include "booledit.h"
#include "KPropertyListData.h"
#include "KPropertyUtils.h"
#include "KPropertyUtils_p.h"
#include "kproperty_debug.h"

#include <QIcon>
#include <QVariant>

namespace {


/**
 * 3-state Index
 */
enum ThreeStateIndex {
    TrueIndex,
    FalseIndex,
    NoneIndex
};

//! @return name for state with index @a index
QString stateName(ThreeStateIndex index, const QLocale &locale, const KProperty* prop = nullptr)
{
    QString stateNameString;
    switch (index) {
    case TrueIndex:
        stateNameString = prop ? prop->option("yesName", QString()).toString() : QString();
        if (stateNameString.isEmpty()) {
            stateNameString = locale.language() == QLocale::C
                ? QString::fromLatin1("true")
                : QObject::tr("Yes", "Property value: Boolean state Yes");
        }
        break;
    case FalseIndex:
        stateNameString = prop ? prop->option("noName", QString()).toString() : QString();
        if (stateNameString.isEmpty()) {
            stateNameString = locale.language() == QLocale::C
                ? QString::fromLatin1("false")
                : QObject::tr("No", "Property value: Boolean state No");
        }
        break;
    case NoneIndex:
        stateNameString = prop ? prop->option("3rdStateName", QString()).toString() : QString();
        if (stateNameString.isEmpty()) {
            stateNameString = locale.language() == QLocale::C
                ? QString::fromLatin1("null")
                : QObject::tr("None", "Property value: Boolean (3rd) undefined state None");
        }
        break;
    }
    return stateNameString;
}

//! Sets up @a data list data with keys and names for true, false, none values, respectively
void setupThreeStateListData(KPropertyListData *data, const KProperty* prop)
{
    data->setKeys({ true, false, QVariant() });
    data->setNamesAsStringList({ stateName(TrueIndex, QLocale(), prop), stateName(FalseIndex, QLocale(), prop),
                                 stateName(NoneIndex, QLocale(), prop) });
}

/**
 * Returns index for given value
 *
 * Assumes that support for 3-state is enabled.
 */
ThreeStateIndex valueToIndex(const QVariant& value)
{
    if (value.isNull() || !value.isValid())
        return NoneIndex;
    else
        return value.toBool() ? TrueIndex : FalseIndex;
}
} // namespace

//-------------------------

class BoolEditGlobal
{
public:
    BoolEditGlobal()
        : yesIcon(QIcon::fromTheme(QLatin1String("dialog-ok")))
        , noIcon(QIcon::fromTheme(QLatin1String("kproperty-value-false")))
    {
        QPixmap none(16, 16);
        none.fill(Qt::transparent);
        noneIcon.addPixmap(none);
        none = QPixmap(22, 22);
        none.fill(Qt::transparent);
        noneIcon.addPixmap(none);
    }
    QIcon yesIcon;
    QIcon noIcon;
    QIcon noneIcon;
};

Q_GLOBAL_STATIC(BoolEditGlobal, g_boolEdit)

class Q_DECL_HIDDEN KPropertyBoolEditor::Private
{
public:
    Private(const KProperty *prop)
     : yesText( stateName(TrueIndex, QLocale(), prop) )
     , noText( stateName(FalseIndex, QLocale(), prop) )
    {
    }
    QVariant value;
    QString yesText;
    QString noText;
};

KPropertyBoolEditor::KPropertyBoolEditor(const KProperty *prop, QWidget *parent)
    : QToolButton(parent), d(new Private(prop))
{
    setFocusPolicy(Qt::WheelFocus);
    setCheckable(true);
    setAutoFillBackground(true);
    connect(this, SIGNAL(toggled(bool)), this, SLOT(slotValueChanged(bool)));
}

KPropertyBoolEditor::~KPropertyBoolEditor()
{
    delete d;
}

QVariant KPropertyBoolEditor::value() const
{
    return d->value;
}

void KPropertyBoolEditor::setValue(const QVariant &value)
{
    d->value = value;
    if (value.type() == QVariant::Bool) {
        setChecked(value.toBool());
    }
}

void
KPropertyBoolEditor::slotValueChanged(bool state)
{
    d->value = state;
    emit commitData(this);
}

void KPropertyBoolEditor::draw(QPainter *p, const QRect &r, const QVariant &value,
                    const QString& text, bool threeState)
{
    QIcon icon;
    QSize actualIconSize;
    QPoint textOffset;
    if (threeState && valueToIndex(value) == NoneIndex) {
        // draw icon for the 3rd state for Three-State editor
        icon = g_boolEdit->noneIcon;
        actualIconSize = g_boolEdit->yesIcon.actualSize(r.size());
        textOffset = QPoint(actualIconSize.width() + 6, 0);
    } else {
        // draw true or false icon regardless of the 2 or 3 state version
        icon = value.toBool() ? g_boolEdit->yesIcon : g_boolEdit->noIcon;
        actualIconSize = icon.actualSize(r.size());
        textOffset = QPoint(actualIconSize.width() + 6, 0);
    }
    QRect r2(r);
    r2.moveTop(r2.top() + 2);
    r2.setLeft(r2.left() + 3);
    //r2.setTop(r2.top() + (r.height() - actualIconSize.height()) / 2);

    icon.paint(p, r2, Qt::AlignVCenter | Qt::AlignLeft);
    p->drawText(r2.translated(textOffset), Qt::AlignVCenter | Qt::AlignLeft, text);
}

void KPropertyBoolEditor::paintEvent( QPaintEvent * event )
{
    QToolButton::paintEvent(event);
    QPainter p(this);
    const QVariant v( value() );
    KPropertyBoolEditor::draw(&p, rect(), v,
        v.toBool() ? d->yesText : d->noText, false /*2state*/);
}

bool KPropertyBoolEditor::eventFilter(QObject* watched, QEvent* e)
{
    if (e->type() == QEvent::KeyPress) {
        QKeyEvent* ev = static_cast<QKeyEvent*>(e);
        const int k = ev->key();
        if (k == Qt::Key_Space || k == Qt::Key_Enter || k == Qt::Key_Return) {
            toggle();
            return true;
        }
    }
    return QToolButton::eventFilter(watched, e);
}

//--------------------------------------------------

class ThreeStateBoolIconProvider : public KPropertyComboBoxEditorIconProviderInterface
{
public:
    ThreeStateBoolIconProvider() {}
    QIcon icon(int index) const override
    {
        switch (index) {
        case TrueIndex:
            return g_boolEdit->yesIcon;
        case FalseIndex:
            return g_boolEdit->noIcon;
        default:
            return g_boolEdit->noneIcon;
        }
    }
    KPropertyComboBoxEditorIconProviderInterface* clone() const override
    {
        return new ThreeStateBoolIconProvider();
    }
};

static KPropertyComboBoxEditorOptions initThreeStateBoolOptions()
{
    KPropertyComboBoxEditorOptions options;
    options.iconProvider = new ThreeStateBoolIconProvider();
    return options;
}

class Q_DECL_HIDDEN KPropertyThreeStateBoolEditor::Private
{
public:
    Private() {
    }
};

KPropertyThreeStateBoolEditor::KPropertyThreeStateBoolEditor(const KPropertyListData &listData,
                                                             QWidget *parent)
    : KPropertyComboBoxEditor(listData, initThreeStateBoolOptions(), parent), d(new Private)
{
//    QPixmap nullIcon(m_yesIcon.size());   //transparent pixmap of appropriate size
//    nullIcon.fill(Qt::transparent);
//    m_edit->addItem(nullIcon, thirdState.toString().isEmpty() ? tr("None") : thirdState.toString());
    setCurrentIndex(NoneIndex);
}

KPropertyThreeStateBoolEditor::~KPropertyThreeStateBoolEditor()
{
    delete d;
}

QVariant KPropertyThreeStateBoolEditor::value() const
{
    // list items: true, false, NULL
    const int i = currentIndex();
    const ThreeStateIndex index
        = (i >= TrueIndex && i <= NoneIndex) ? static_cast<ThreeStateIndex>(i) : NoneIndex;
    switch (index) {
    case TrueIndex:
        return true;
    case FalseIndex:
        return false;
    default:
        return QVariant();
    }
}

/*void ThreeStateBoolEdit::setProperty(Property *prop)
{
    m_setValueEnabled = false; //setValue() couldn't be called before fillBox()
    Widget::setProperty(prop);
    m_setValueEnabled = true;
    if (prop)
        setValue(prop->value(), KProperty::ValueOption::IgnoreOld); //now the value can be set
}*/

void KPropertyThreeStateBoolEditor::setValue(const QVariant &value)
{
    setCurrentIndex( valueToIndex(value) );
}

//---------------

KPropertyBoolDelegate::KPropertyBoolDelegate()
{
    options()->setBordersVisible(true);
}

QWidget * KPropertyBoolDelegate::createEditor( int type, QWidget *parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Q_UNUSED(type);
    Q_UNUSED(option);
    KProperty *prop = KPropertyUtils::propertyForIndex(index);

    // boolean editors can optionally accept 3rd state:
    if (prop && prop->option("3State", false).toBool()) {
        KPropertyListData threeStateListData;
        setupThreeStateListData(&threeStateListData, prop);
        return new KPropertyThreeStateBoolEditor(threeStateListData, parent);
    }
    else {
        return new KPropertyBoolEditor(prop, parent);
    }
}

void KPropertyBoolDelegate::paint( QPainter * painter,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const KPropertyUtilsPrivate::PainterSaver saver(painter);
    KProperty *prop = KPropertyUtils::propertyForIndex(index);
    if (!prop) {
        return;
    }
    const QVariant value( index.data(Qt::EditRole) );
    QRect rect(option.rect);
    const bool threeState = prop->option("3State", false).toBool();
    KPropertyBoolEditor::draw(painter, rect.translated(0, -2), value, propertyValueToString(prop, QLocale()), threeState);
}

QString KPropertyBoolDelegate::propertyValueToString(const KProperty* prop, const QLocale &locale) const
{
    if (prop->option("3State", false).toBool()) {
        const ThreeStateIndex listIndex = valueToIndex(prop->value());
        return stateName(listIndex, locale, prop);
    }
    if (prop->value().isNull() && !prop->option("nullName", QString()).toString().isEmpty()) {
        return prop->option("nullName", QString()).toString();
    }
    return valueToString(prop->value(), locale);
}

QString KPropertyBoolDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    // assume 2-state
    return stateName(value.toBool() ? TrueIndex : FalseIndex, locale);
}
