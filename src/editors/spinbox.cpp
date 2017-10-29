/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
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

#include "spinbox.h"
#include "KProperty.h"
#include "KProperty_p.h"
#include "KPropertyEditorView.h"
#include "KPropertyUtils.h"
#include "KPropertyUtils_p.h"
#include "KPropertyWidgetsFactory.h"
#include "kproperty_debug.h"

#include <QVariant>
#include <QLineEdit>
#include <QLocale>

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

namespace {

void intRangeValue(const KProperty &property, QVariant *min, QVariant *max)
{
    Q_ASSERT(min);
    Q_ASSERT(max);
    *min = property.option("min");
    *max = property.option("max");
    if (!min->canConvert(QMetaType::Int) || min->toInt() < -INT_MAX) {
        min->clear();
    }
    if (!max->canConvert(QMetaType::Int) || max->toInt() > INT_MAX) {
        max->clear();
    }
    if (min->canConvert(QMetaType::Int) && max->canConvert(QMetaType::Int)
        && min->toInt() > max->toInt())
    {
        min->clear();
        max->clear();
    }
    if (min->isNull()) {
        switch (property.type()) {
        case KProperty::UInt:
            *min = 0;
            break;
        default:
            *min = -INT_MAX;
        }
    }
    if (max->isNull()) {
        *max = INT_MAX;
    }
}

//! Fixes @a value to fit in range @a min to @a max.
//! Displays qWarning if @a warn is @c true.
int fixIntValue(const QVariant &value, int min, int max, bool warn)
{
    if (value.toInt() < min) {
        if (warn) {
            kprWarning() << "Could not assign value" << value.toInt() << "smaller than minimum" << min
                         << "-- setting to" << min;
        }
        return min;
    }
    if (value.toInt() > max) {
        if (warn) {
            kprWarning() << "Could not assign value" << value.toInt() << "larger than maximum" << max
                         << "-- setting to" << max;
        }
        return max;
    }
    return value.toInt();
}

} // namespace

class Q_DECL_HIDDEN KPropertyIntSpinBox::Private
{
public:
    explicit Private(const KProperty& prop) : property(&prop)
    {
    }

    const KProperty * const property;
};

KPropertyIntSpinBox::KPropertyIntSpinBox(const KProperty& prop, QWidget *parent, int itemHeight)
        : QSpinBox(parent)
        , d(new Private(prop))
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

    QVariant minVal;
    QVariant maxVal;
    intRangeValue(prop, &minVal, &maxVal);
    setRange(minVal.toInt(), maxVal.toInt());
    const KPropertyUtilsPrivate::ValueOptionsHandler options(prop);
    if (!options.minValueText.isNull()) {
        setSpecialValueText(options.minValueText.toString());
    }
    if (!options.prefix.isEmpty()) {
        setPrefix(options.prefix + QLatin1Char(' '));
    }
    if (!options.suffix.isEmpty()) {
        setSuffix(QLatin1Char(' ') + options.suffix);
    }
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged(int)));
}

KPropertyIntSpinBox::~KPropertyIntSpinBox()
{
    delete d;
}

QVariant KPropertyIntSpinBox::value() const
{
    if (d->property->type() == KProperty::UInt) {
        return uint(QSpinBox::value());
    }
    return QSpinBox::value();
}

void KPropertyIntSpinBox::setValue(const QVariant& value)
{
    QVariant minVal;
    QVariant maxVal;
    intRangeValue(*d->property, &minVal, &maxVal);
    QSpinBox::setValue(fixIntValue(value, minVal.toInt(), maxVal.toInt(), true));
}

void KPropertyIntSpinBox::slotValueChanged(int value)
{
    Q_UNUSED(value);
    emit commitData(this);
}

//-----------------------

class Q_DECL_HIDDEN KPropertyDoubleSpinBox::Private
{
public:
    explicit Private(const KProperty& prop) : property(&prop)
    {
    }

    const KProperty * const property;
};

namespace {

void doubleRangeValue(const KProperty &property, QVariant *min, QVariant *max)
{
    Q_ASSERT(min);
    Q_ASSERT(max);
    *min = property.option("min");
    *max = property.option("max");
    if (!min->canConvert(QMetaType::Double) || min->toDouble() < KPROPERTY_MIN_PRECISE_DOUBLE) {
        min->clear();
    }
    if (!max->canConvert(QMetaType::Double) || max->toDouble() > KPROPERTY_MAX_PRECISE_DOUBLE) {
        max->clear();
    }
    if (min->canConvert(QMetaType::Double) && max->canConvert(QMetaType::Double)
        && min->toDouble() > max->toDouble())
    {
        min->clear();
        max->clear();
    }
    if (min->isNull()) {
        *min = 0.0;
    }
    if (max->isNull()) {
        *max = KPROPERTY_MAX_PRECISE_DOUBLE;
    }
}

//! Fixes @a value to fit in range @a min to @a max.
//! Displays qWarning if @a warn is @c true.
double fixDoubleValue(const QVariant &value, double min, double max, bool warn)
{
    if (value.toDouble() < min) {
        if (warn) {
            kprWarning() << "Could not assign value" << value.toDouble() << "smaller than minimum" << min
                         << "-- setting to" << min;
        }
        return min;
    }
    if (value.toDouble() > max) {
        if (warn) {
            kprWarning() << "Could not assign value" << value.toDouble() << "larger than maximum" << max
                         << "-- setting to" << max;
        }
        return max;
    }
    return value.toDouble();
}

QVariant precisionValue(const KProperty &property)
{
    QVariant result = property.option("precision", KPROPERTY_DEFAULT_DOUBLE_VALUE_PRECISION);
    if (result.canConvert(QMetaType::Int) && result.toInt() >= 0) {
        return result;
    }
    return QVariant();
}

} // namespace

KPropertyDoubleSpinBox::KPropertyDoubleSpinBox(const KProperty &prop, QWidget *parent, int itemHeight)
        : QDoubleSpinBox(parent)
        , d(new Private(prop))
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

    QVariant minVal;
    QVariant maxVal;
    doubleRangeValue(prop, &minVal, &maxVal);
    setRange(minVal.toDouble(), maxVal.toDouble());
    QVariant step = prop.option("step", KPROPERTY_DEFAULT_DOUBLE_VALUE_STEP);
    if (step.canConvert(QMetaType::Double) && step.toDouble() > 0.0) {
        setSingleStep(step.toDouble());
    }
    const QVariant precision = precisionValue(prop);
    if (precision.isValid()) {
        setDecimals(precision.toInt());
    }
    //! @todo implement slider
    // bool slider = prop->option("slider", false).toBool();
    const KPropertyUtilsPrivate::ValueOptionsHandler options(prop);
    if (!options.minValueText.isNull()) {
        setSpecialValueText(options.minValueText.toString());
    }
    if (!options.prefix.isEmpty()) {
        setPrefix(options.prefix + QLatin1Char(' '));
    }
    if (!options.suffix.isEmpty()) {
        setSuffix(QLatin1Char(' ') + options.suffix);
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

void KPropertyDoubleSpinBox::setValue(const QVariant& value)
{
    QVariant minVal;
    QVariant maxVal;
    doubleRangeValue(*d->property, &minVal, &maxVal);
    QDoubleSpinBox::setValue(fixDoubleValue(value, minVal.toDouble(), maxVal.toDouble(), true));
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
    //replace min value with minValueText if defined
    const KPropertyUtilsPrivate::ValueOptionsHandler options(*prop);
    QVariant minVal;
    QVariant maxVal;
    intRangeValue(*prop, &minVal, &maxVal);
    const int fixedValue = fixIntValue(prop->value(), minVal.toInt(), maxVal.toInt(), false);
    if (minVal.isValid() && minVal.toInt() == fixedValue && !options.minValueText.isNull())
    {
        return options.minValueText.toString();
    }
    return options.valueWithPrefixAndSuffix(valueToString(fixedValue, locale), locale);
}

QString KPropertyIntSpinBoxDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    return locale.toString(value.toReal(), 'f', 0);
}

QWidget* KPropertyIntSpinBoxDelegate::createEditor( int type, QWidget *parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Q_UNUSED(type);

    KProperty *prop = KPropertyUtils::propertyForIndex(index);
    if (!prop) {
        return nullptr;
    }
    return new KPropertyIntSpinBox(*prop, parent, option.rect.height() - 2);
}

//-----------------------

KPropertyDoubleSpinBoxDelegate::KPropertyDoubleSpinBoxDelegate()
{
}

QString KPropertyDoubleSpinBoxDelegate::propertyValueToString(const KProperty* prop,
                                                              const QLocale &locale) const
{
    //replace min value with minValueText if defined
    QVariant minVal;
    QVariant maxVal;
    const KPropertyUtilsPrivate::ValueOptionsHandler options(*prop);
    doubleRangeValue(*prop, &minVal, &maxVal);
    const double fixedValue = fixDoubleValue(prop->value(), minVal.toDouble(), maxVal.toDouble(), false);
    if (minVal.isValid() && minVal.toDouble() == fixedValue && !options.minValueText.isNull())
    {
        return options.minValueText.toString();
    }
    QString valueString;
    const QVariant precision = precisionValue(*prop);
    if (precision.isValid()) {
        valueString = locale.toString(fixedValue, 'f', precision.toInt());
    } else {
        valueString = valueToString(fixedValue, locale);
    }
    return options.valueWithPrefixAndSuffix(valueString, locale);
}

QString KPropertyDoubleSpinBoxDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    return locale.toString(value.toReal());
}

QWidget* KPropertyDoubleSpinBoxDelegate::createEditor( int type, QWidget *parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Q_UNUSED(type);

    KProperty *prop = KPropertyUtils::propertyForIndex(index);
    if (!prop) {
        return nullptr;
    }
    return new KPropertyDoubleSpinBox(*prop, parent, option.rect.height() - 2 - 1);
}
