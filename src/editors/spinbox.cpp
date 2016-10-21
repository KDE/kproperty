/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
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

#include "spinbox.h"
#include "KProperty.h"
#include "KProperty_p.h"
#include "KPropertyEditorView.h"
#include "KPropertyUnit_p.h"
#include "KPropertyUtils.h"
#include "KPropertyWidgetsFactory.h"
#include "kproperty_debug.h"

#include <QVariant>
#include <QLineEdit>
#include <QLocale>

#include <climits>
#include <cfloat>

//! @return font size expressed in points (pt)
//! or if points are not available - in pixels (px) for @a font
static QString fontSizeForCSS(const QFont& font)
{
    return font.pointSize() > 0
        ? QString::fromLatin1("%1pt").arg(font.pointSize())
        : QString::fromLatin1("%1px").arg(font.pixelSize());
}

static QString cssForSpinBox(const char *_class, const QFont& font, int itemHeight)
{
        return QString::fromLatin1(
            "%5 { border-left: 0; border-right: 0; font-size: %3; } "
            "%5::down-button { height: %1px; %4 } "
            "%5::up-button { height: %2px; } "
            "QLineEdit { border-width:0px;  } "
        )
        .arg(itemHeight/2 - 1).arg(itemHeight - itemHeight/2 - 1)
        .arg(fontSizeForCSS(font))
        .arg(QLatin1String((itemHeight/2 <= 9) ? "bottom: 2px;" : "bottom: 0px;"))
        .arg(QLatin1String(_class));
}

KPropertyIntSpinBox::KPropertyIntSpinBox(const KProperty* prop, QWidget *parent, int itemHeight)
        : QSpinBox(parent)
        , m_unsigned(prop->type() == KProperty::UInt)
{
    QLineEdit* le = findChild<QLineEdit*>();
    setContentsMargins(0,0,0,0);
    if (le) {
        le->setAlignment(Qt::AlignLeft);
        le->setContentsMargins(0,0,0,0);
    }
    setFrame(true);
    QString css = cssForSpinBox("QSpinBox", font(), itemHeight);
    KPropertyWidgetsFactory::setTopAndBottomBordersUsingStyleSheet(this, css);
    setStyleSheet(css);

    QVariant minVal(prop->option("min", m_unsigned ? 0 : -INT_MAX));
    QVariant maxVal(prop->option("max", INT_MAX));
    setMinimum(minVal.toInt());
    setMaximum(maxVal.toInt());
    QString minValueText(prop->option("minValueText").toString());
    if (!minValueText.isEmpty())
        setSpecialValueText(minValueText);
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged(int)));
}

KPropertyIntSpinBox::~KPropertyIntSpinBox()
{
}

QVariant KPropertyIntSpinBox::value() const
{
    if (m_unsigned)
        return uint(QSpinBox::value());
    return QSpinBox::value();
}

void KPropertyIntSpinBox::setValue(const QVariant& value)
{
    int v( value.toInt() );
    if (m_unsigned && v<0) {
        kprWarning() << "could not assign negative value" << v << "- assigning 0";
        v = 0;
    }
    QSpinBox::setValue(v);
}

void KPropertyIntSpinBox::slotValueChanged(int value)
{
    Q_UNUSED(value);
    emit commitData(this);
}

//-----------------------

class KPropertyDoubleSpinBox::Private
{
public:
    KPropertyUnit unit;
    bool hasUnit;
};

static void decodeUnit(const KProperty &property, KPropertyUnit *unit, bool *hasUnit)
{
    const QString unitString = property.option("unit").toString();
    if (unitString.isEmpty()) {
        *hasUnit = false;
    }
    else {
        *unit = KPropertyUnit::fromSymbol(unitString, hasUnit);
    }
}

KPropertyDoubleSpinBox::KPropertyDoubleSpinBox(const KProperty* prop, QWidget *parent, int itemHeight)
        : QDoubleSpinBox(parent)
        , d(new Private)
{
    setFrame(false);
    QLineEdit* le = findChild<QLineEdit*>();
    if (le) {
        le->setAlignment(Qt::AlignLeft);
        le->setContentsMargins(0,0,0,0);
        le->setFrame(false);
    }
/*    KPropertyFactory::setTopAndBottomBordersUsingStyleSheet(sb,
        QString::fromLatin1(
            "QDoubleSpinBox { border-left: 0; border-right: 0; } "
            "QDoubleSpinBox::down-button { height: %1px; } "
            "QDoubleSpinBox::up-button { height: %2px; }"
        ).arg(itemHeight/2).arg(itemHeight - itemHeight/2)
    );*/
    QString css = cssForSpinBox("QDoubleSpinBox", font(), itemHeight);
    KPropertyWidgetsFactory::setTopAndBottomBordersUsingStyleSheet(this, css);
    setStyleSheet(css);

    QVariant minVal(prop->option("min"));
    if (minVal.canConvert(QMetaType::Double)) {
        setMinimum(minVal.toDouble());
    }
    QVariant maxVal(prop->option("max", DBL_MAX / 100.0));
    if (maxVal.canConvert(QMetaType::Double)) {
        setMaximum(maxVal.toDouble());
    }
    QVariant step(prop->option("step", KPROPERTY_DEFAULT_DOUBLE_VALUE_STEP));
    if (step.canConvert(QMetaType::Double)) {
        setSingleStep(step.toDouble());
    }
    //! @todo implement slider
    // bool slider = prop->option("slider", false).toBool();
    QVariant precision(prop->option("precision"));
    if (precision.canConvert(QMetaType::Int)) {
        setDecimals(precision.toInt());
    }
    QString minValueText(prop->option("minValueText").toString());
    if (!minValueText.isEmpty())
        setSpecialValueText(minValueText);
    decodeUnit(*prop, &d->unit, &d->hasUnit);
    if (d->hasUnit) {
        setSuffix(
            QObject::tr("%1 %2", "<value> <unit>") // this adds necessary space
                    .arg(QString()).arg(d->unit.toString()));
    }
    connect(this, SIGNAL(valueChanged(double)), this, SLOT(slotValueChanged(double)));
}

KPropertyDoubleSpinBox::~KPropertyDoubleSpinBox()
{
    delete d;
}

void KPropertyDoubleSpinBox::resizeEvent( QResizeEvent * event )
{
    setFixedHeight(height() + 1);
    QDoubleSpinBox::resizeEvent(event);
}

void KPropertyDoubleSpinBox::setValue(double v)
{
    if (d->hasUnit) {
        QDoubleSpinBox::setValue(d->unit.toUserValue(v));
        return;
    }
    QDoubleSpinBox::setValue(v);
}

double KPropertyDoubleSpinBox::value() const
{
    if (d->hasUnit) {
        return d->unit.fromUserValue(QDoubleSpinBox::value());
    }
    return QDoubleSpinBox::value();
}

void KPropertyDoubleSpinBox::slotValueChanged(double value)
{
    Q_UNUSED(value);
    emit commitData(this);
}

//-----------------------

KPropertyIntSpinBoxDelegate::KPropertyIntSpinBoxDelegate()
{
}

QString KPropertyIntSpinBoxDelegate::propertyValueToString(const KProperty* prop,
                                                           const QLocale &locale) const
{
    KPropertyUnit unit;
    bool hasUnit;
    decodeUnit(*prop, &unit, &hasUnit);
    if (prop->hasOptions()) {
        //replace min value with minValueText if defined
        QVariant minValue(prop->option("min"));
        QString minValueText(prop->option("minValueText").toString());
        if (!minValue.isNull() && !minValueText.isEmpty()
            && minValue.toInt() == prop->value().toInt())
        {
            return minValueText;
        }
    }
    if (!hasUnit) {
        return valueToString(prop->value(), locale);
    }
    if (locale.language() == QLocale::C) {
        return QString::fromLatin1("%1 %2")
                                   .arg(valueToString(prop->value(), locale))
                                   .arg(unit.toString());
    }
    return QObject::tr("%1 %2", "<value> <unit>")
                       .arg(valueToString(unit.toUserValue(prop->value().toDouble()), locale))
                       .arg(unit.toString());
}

QString KPropertyIntSpinBoxDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    return locale.toString(value.toDouble(), 'f', 0);
}

QWidget* KPropertyIntSpinBoxDelegate::createEditor( int type, QWidget *parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Q_UNUSED(type);

    KProperty *prop = KPropertyUtils::propertyForIndex(index);
    if (!prop) {
        return 0;
    }
    return new KPropertyIntSpinBox(prop, parent, option.rect.height() - 2);
}

//-----------------------

KPropertyDoubleSpinBoxDelegate::KPropertyDoubleSpinBoxDelegate()
{
}

QString KPropertyDoubleSpinBoxDelegate::propertyValueToString(const KProperty* prop,
                                                              const QLocale &locale) const
{
    KPropertyUnit unit;
    bool hasUnit;
    decodeUnit(*prop, &unit, &hasUnit);
    int precision = -1;
    if (prop->hasOptions()) {
        //replace min value with minValueText if defined
        QVariant minValue(prop->option("min"));
        QString minValueText(prop->option("minValueText").toString());
        if (!minValue.isNull() && !minValueText.isEmpty()
            && minValue.toDouble() == prop->value().toDouble())
        {
            return minValueText;
        }
        if (prop->option("precision").canConvert(QMetaType::Int)) {
            precision = prop->option("precision").toInt();
        }
    }
    const qreal realValue = hasUnit ? unit.toUserValue(prop->value().toReal()) : prop->value().toReal();
    QString valueString;
    if (precision >= 0) {
        valueString = locale.toString(realValue, 'f', precision);
    } else {
        valueString = valueToString(realValue, locale);
    }

    if (!hasUnit) {
        return valueString;
    }
    if (locale.language() == QLocale::C) {
        return QString::fromLatin1("%1 %2").arg(valueString).arg(unit.toString());
    }
    return QObject::tr("%1 %2", "<value> <unit>").arg(valueString).arg(unit.toString());
}

QString KPropertyDoubleSpinBoxDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    return locale.toString(value.toDouble());
}

QWidget* KPropertyDoubleSpinBoxDelegate::createEditor( int type, QWidget *parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Q_UNUSED(type);

    KProperty *prop = KPropertyUtils::propertyForIndex(index);
    if (!prop) {
        return 0;
    }
    return new KPropertyDoubleSpinBox(prop, parent, option.rect.height() - 2 - 1);
}
