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

#include <cmath>

/**
 * Maximum double value working precisely. The spin box has localized contents and its code
 * supports just this maximum. For a 64-bit machine it's 2**53.
 * See also https://phabricator.kde.org/D5419#inline-22329
 */
#define MAX_PRECISE_DOUBLE (pow(2, std::numeric_limits<double>::digits))

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

bool intRangeValue(const KProperty &property, QVariant *min, QVariant *max)
{
    Q_ASSERT(min);
    Q_ASSERT(max);
    *min = property.option("min", (property.type() == KProperty::UInt) ? 0 : -INT_MAX);
    *max = property.option("max", INT_MAX);
    if (!min->canConvert(QMetaType::Int)) {
        min->clear();
    }
    if (!max->canConvert(QMetaType::Int)) {
        max->clear();
    }
    return min->isValid() && max->isValid() && min->toInt() <= max->toInt();
}

} // namespace

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

    QVariant minVal;
    QVariant maxVal;
    if (intRangeValue(*prop, &minVal, &maxVal)) {
        setRange(minVal.toInt(), maxVal.toInt());
    }
    const KPropertyUtilsPrivate::ValueOptionsHandler options(*prop);
    if (!options.minValueText.isEmpty()) {
        setSpecialValueText(options.minValueText);
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

class Q_DECL_HIDDEN KPropertyDoubleSpinBox::Private
{
public:
    bool dummy = false;
};

namespace {

bool doubleRangeValue(const KProperty &property, QVariant *min, QVariant *max)
{
    Q_ASSERT(min);
    Q_ASSERT(max);
    *min = property.option("min", 0.0);
    *max = property.option("max", MAX_PRECISE_DOUBLE);
    if (!min->canConvert(QMetaType::Double)) {
        min->clear();
    }
    if (!max->canConvert(QMetaType::Double)) {
        max->clear();
    }
    return min->isValid() && max->isValid() && min->toDouble() <= max->toDouble();
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

    QVariant minVal;
    QVariant maxVal;
    if (doubleRangeValue(*prop, &minVal, &maxVal)) {
        setRange(minVal.toDouble(), maxVal.toDouble());
    }
    QVariant step = prop->option("step", KPROPERTY_DEFAULT_DOUBLE_VALUE_STEP);
    if (step.canConvert(QMetaType::Double) && step.toDouble() > 0.0) {
        setSingleStep(step.toDouble());
    }
    const QVariant precision = precisionValue(*prop);
    if (precision.isValid()) {
        setDecimals(precision.toInt());
    }
    //! @todo implement slider
    // bool slider = prop->option("slider", false).toBool();
    const KPropertyUtilsPrivate::ValueOptionsHandler options(*prop);
    if (!options.minValueText.isEmpty()) {
        setSpecialValueText(options.minValueText);
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
    QVariant minVal;
    QVariant maxVal;
    const KPropertyUtilsPrivate::ValueOptionsHandler options(*prop);
    (void)intRangeValue(*prop, &minVal, &maxVal);
    if (minVal.isValid() && minVal.toInt() == prop->value().toInt()) {
        return options.minValueText;
    }
    return options.valueWithPrefixAndSuffix(valueToString(prop->value(), locale), locale);
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
    return new KPropertyIntSpinBox(prop, parent, option.rect.height() - 2);
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
    (void)doubleRangeValue(*prop, &minVal, &maxVal);
    if (minVal.isValid() && minVal.toDouble() == prop->value().toDouble()) {
        return options.minValueText;
    }
    QString valueString;
    const QVariant precision = precisionValue(*prop);
    if (precision.isValid()) {
        valueString = locale.toString(prop->value().toReal(), 'f', precision.toInt());
    } else {
        valueString = valueToString(prop->value().toReal(), locale);
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
    return new KPropertyDoubleSpinBox(prop, parent, option.rect.height() - 2 - 1);
}
