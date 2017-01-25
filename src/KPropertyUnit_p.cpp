/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>
   Copyright (C) 2004, Nicolas GOUTTE <goutte@kde.org>
   Copyright 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "KPropertyUnit_p.h"
#include "kproperty_debug.h"

#include <QLocale>

// ensure the same order as in KPropertyUnit::Unit
static const char* const unitNameList[int(KPropertyUnit::Type::TypeCount)] =
{
    "mm",
    "pt",
    "in",
    "cm",
    "dm",
    "pi",
    "cc",
    "px"
};

QString KPropertyUnit::unitDescription(KPropertyUnit::Type type)
{
    switch (type) {
    case KPropertyUnit::Type::Millimeter:
        return QObject::tr("Millimeters (mm)");
    case KPropertyUnit::Type::Centimeter:
        return QObject::tr("Centimeters (cm)");
    case KPropertyUnit::Type::Decimeter:
        return QObject::tr("Decimeters (dm)");
    case KPropertyUnit::Type::Inch:
        return QObject::tr("Inches (in)");
    case KPropertyUnit::Type::Pica:
        return QObject::tr("Pica (pi)");
    case KPropertyUnit::Type::Cicero:
        return QObject::tr("Cicero (cc)");
    case KPropertyUnit::Type::Point:
        return QObject::tr("Points (pt)");
    case KPropertyUnit::Type::Pixel:
        return QObject::tr("Pixels (px)");
    default:
        return QObject::tr("Unsupported unit");
    }
}

// grouped by units which are similar
static const KPropertyUnit::Type typesInUi[int(KPropertyUnit::Type::TypeCount)] =
{
    KPropertyUnit::Type::Millimeter,
    KPropertyUnit::Type::Centimeter,
    KPropertyUnit::Type::Decimeter,
    KPropertyUnit::Type::Inch,
    KPropertyUnit::Type::Pica,
    KPropertyUnit::Type::Cicero,
    KPropertyUnit::Type::Point,
    KPropertyUnit::Type::Pixel,
};

QStringList KPropertyUnit::listOfUnitNameForUi(ListOptions listOptions)
{
    QStringList lst;
    for (int i = 0; i < int(KPropertyUnit::Type::TypeCount); ++i) {
        const Type type = typesInUi[i];
        if (type != Type::Pixel || !(listOptions & ListOption::HidePixel)) {
            lst.append(unitDescription(type));
        }
    }
    return lst;
}

KPropertyUnit KPropertyUnit::fromListForUi(int index, ListOptions listOptions, qreal factor)
{
    KPropertyUnit::Type type = KPropertyUnit::Type::Point;

    if ((0 <= index) && (index < int(KPropertyUnit::Type::TypeCount))) {
        // iterate through all enums and skip the Pixel enum if needed
        for (int i = 0; i < int(KPropertyUnit::Type::TypeCount); ++i) {
            if ((listOptions & ListOption::HidePixel) && (typesInUi[i] == Type::Pixel)) {
                ++index;
                continue;
            }
            if (i == index) {
                type = typesInUi[i];
                break;
            }
        }
    }

    return KPropertyUnit(type, factor);
}

int KPropertyUnit::indexInListForUi(ListOptions listOptions) const
{
    if ((listOptions & ListOption::HidePixel) && (m_type == Type::Pixel)) {
        return -1;
    }

    int result = -1;

    int skipped = 0;
    for (int i = 0; i < int(KPropertyUnit::Type::TypeCount); ++i) {
        if ((listOptions & ListOption::HidePixel) && (typesInUi[i] == Type::Pixel)) {
            ++skipped;
            continue;
        }
        if (typesInUi[i] == m_type) {
            result = i - skipped;
            break;
        }
    }

    return result;
}

qreal KPropertyUnit::toUserValue(qreal ptValue) const
{
    switch (m_type) {
    case Type::Millimeter:
        return toMillimeter(ptValue);
    case Type::Centimeter:
        return toCentimeter(ptValue);
    case Type::Decimeter:
        return toDecimeter(ptValue);
    case Type::Inch:
        return toInch(ptValue);
    case Type::Pica:
        return toPica(ptValue);
    case Type::Cicero:
        return toCicero(ptValue);
    case Type::Pixel:
        return ptValue * m_pixelConversion;
    case Type::Point:
    default:
        return toPoint(ptValue);
    }
}

qreal KPropertyUnit::ptToUnit(qreal ptValue, const KPropertyUnit &unit)
{
    switch (unit.m_type) {
    case Type::Millimeter:
        return POINT_TO_MM(ptValue);
    case Type::Centimeter:
        return POINT_TO_CM(ptValue);
    case Type::Decimeter:
        return POINT_TO_DM(ptValue);
    case Type::Inch:
        return POINT_TO_INCH(ptValue);
    case Type::Pica:
        return POINT_TO_PI(ptValue);
    case Type::Cicero:
        return POINT_TO_CC(ptValue);
    case Type::Pixel:
        return ptValue * unit.m_pixelConversion;
    case Type::Point:
    default:
        return ptValue;
    }
}

QString KPropertyUnit::toUserStringValue(qreal ptValue) const
{
    return QLocale::system().toString(toUserValue(ptValue));
}

qreal KPropertyUnit::fromUserValue(qreal value) const
{
    switch (m_type) {
    case Type::Millimeter:
        return MM_TO_POINT(value);
    case Type::Centimeter:
        return CM_TO_POINT(value);
    case Type::Decimeter:
        return DM_TO_POINT(value);
    case Type::Inch:
        return INCH_TO_POINT(value);
    case Type::Pica:
        return PI_TO_POINT(value);
    case Type::Cicero:
        return CC_TO_POINT(value);
    case Type::Pixel:
        return value / m_pixelConversion;
    case Type::Point:
    default:
        return value;
    }
}

qreal KPropertyUnit::fromUserValue(const QString &value, bool *ok) const
{
    return fromUserValue(QLocale::system().toDouble(value, ok));
}

qreal KPropertyUnit::parseValue(const QString& _value, qreal defaultVal)
{
    if (_value.isEmpty())
        return defaultVal;

    QString value(_value.simplified());
    value.remove(QLatin1Char(' '));

    int firstLetter = -1;
    for (int i = 0; i < value.length(); ++i) {
        if (value.at(i).isLetter()) {
            if (value.at(i) == QLatin1Char('e'))
                continue;
            firstLetter = i;
            break;
        }
    }

    bool ok;
    if (firstLetter == -1) {
        const qreal result = QVariant(value).toReal(&ok);
        return ok ? result : defaultVal;
    }

    const QByteArray symbol = value.mid(firstLetter).toLatin1();
    value.truncate(firstLetter);
    const qreal val = value.toDouble();

    if (symbol == "pt" || symbol.isEmpty()) {
        return val;
    }

    KPropertyUnit u = KPropertyUnit::fromSymbol(QLatin1String(symbol), &ok);
    if (ok)
        return u.fromUserValue(val);

    if (symbol == "m") {
        return DM_TO_POINT(val * 10.0);
    } else if (symbol == "km") {
        return DM_TO_POINT(val * 10000.0);
    }
    kprWarning() << "KPropertyUnit::parseValue: Unit " << symbol << " is not supported, please report.";

    //! @todo add support for mi/ft ?
    return defaultVal;
}

KPropertyUnit KPropertyUnit::fromSymbol(const QString &symbol, bool *ok)
{
    Type result = Type::Point;

    if (symbol == QLatin1String("inch") /*compat*/) {
        result = Type::Inch;
        if (ok)
            *ok = true;
    } else {
        if (ok)
            *ok = false;

        for (int i = 0; i < int(Type::TypeCount); ++i) {
            if (symbol == QLatin1String(unitNameList[i])) {
                result = static_cast<Type>(i);
                if (ok)
                    *ok = true;
            }
        }
    }

    return KPropertyUnit(result);
}

qreal KPropertyUnit::convertFromUnitToUnit(qreal value, const KPropertyUnit &fromUnit, const KPropertyUnit &toUnit, qreal factor)
{
    qreal pt;
    switch (fromUnit.type()) {
    case Type::Millimeter:
        pt = MM_TO_POINT(value);
        break;
    case Type::Centimeter:
        pt = CM_TO_POINT(value);
        break;
    case Type::Decimeter:
        pt = DM_TO_POINT(value);
        break;
    case Type::Inch:
        pt = INCH_TO_POINT(value);
        break;
    case Type::Pica:
        pt = PI_TO_POINT(value);
        break;
    case Type::Cicero:
        pt = CC_TO_POINT(value);
        break;
    case Type::Pixel:
        pt = value / factor;
        break;
    case Type::Point:
    default:
        pt = value;
    }

    switch (toUnit.type()) {
    case Type::Millimeter:
        return POINT_TO_MM(pt);
    case Type::Centimeter:
        return POINT_TO_CM(pt);
    case Type::Decimeter:
        return POINT_TO_DM(pt);
    case Type::Inch:
        return POINT_TO_INCH(pt);
    case Type::Pica:
        return POINT_TO_PI(pt);
    case Type::Cicero:
        return POINT_TO_CC(pt);
    case Type::Pixel:
        return pt * factor;
    case Type::Point:
    default:
        return pt;
    }

}

QString KPropertyUnit::symbol() const
{
    return QLatin1String(unitNameList[int(m_type)]);
}

qreal KPropertyUnit::parseAngle(const QString& _value, qreal defaultVal)
{
    if (_value.isEmpty())
        return defaultVal;

    QString value(_value.simplified());
    value.remove(QLatin1Char(' '));

    int firstLetter = -1;
    for (int i = 0; i < value.length(); ++i) {
        if (value.at(i).isLetter()) {
            if (value.at(i) == QLatin1Char('e'))
                continue;
            firstLetter = i;
            break;
        }
    }

    if (firstLetter == -1)
        return value.toDouble();

    const QString type = value.mid(firstLetter);
    value.truncate(firstLetter);
    const qreal val = value.toDouble();

    if (type == QLatin1String("deg"))
        return val;
    else if (type == QLatin1String("rad"))
        return val * 180 / M_PI;
    else if (type == QLatin1String("grad"))
        return val * 0.9;

    return defaultVal;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const KPropertyUnit &unit)
{
#ifndef NDEBUG
    debug.nospace() << unit.symbol();
#else
    Q_UNUSED(unit);
#endif
    return debug.space();

}
#endif
