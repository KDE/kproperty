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
static const char* const unitNameList[KPropertyUnit::TypeCount] =
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
    case KPropertyUnit::Millimeter:
        return QObject::tr("Millimeters (mm)");
    case KPropertyUnit::Centimeter:
        return QObject::tr("Centimeters (cm)");
    case KPropertyUnit::Decimeter:
        return QObject::tr("Decimeters (dm)");
    case KPropertyUnit::Inch:
        return QObject::tr("Inches (in)");
    case KPropertyUnit::Pica:
        return QObject::tr("Pica (pi)");
    case KPropertyUnit::Cicero:
        return QObject::tr("Cicero (cc)");
    case KPropertyUnit::Point:
        return QObject::tr("Points (pt)");
    case KPropertyUnit::Pixel:
        return QObject::tr("Pixels (px)");
    default:
        return QObject::tr("Unsupported unit");
    }
}

// grouped by units which are similar
static const KPropertyUnit::Type typesInUi[KPropertyUnit::TypeCount] =
{
    KPropertyUnit::Millimeter,
    KPropertyUnit::Centimeter,
    KPropertyUnit::Decimeter,
    KPropertyUnit::Inch,
    KPropertyUnit::Pica,
    KPropertyUnit::Cicero,
    KPropertyUnit::Point,
    KPropertyUnit::Pixel,
};

QStringList KPropertyUnit::listOfUnitNameForUi(ListOptions listOptions)
{
    QStringList lst;
    for (int i = 0; i < KPropertyUnit::TypeCount; ++i) {
        const Type type = typesInUi[i];
        if ((type != Pixel) || ((listOptions & HideMask) == ListAll))
            lst.append(unitDescription(type));
    }
    return lst;
}

KPropertyUnit KPropertyUnit::fromListForUi(int index, ListOptions listOptions, qreal factor)
{
    KPropertyUnit::Type type = KPropertyUnit::Point;

    if ((0 <= index) && (index < KPropertyUnit::TypeCount)) {
        // iterate through all enums and skip the Pixel enum if needed
        for (int i = 0; i < KPropertyUnit::TypeCount; ++i) {
            if ((listOptions&HidePixel) && (typesInUi[i] == Pixel)) {
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
    if ((listOptions&HidePixel) && (m_type == Pixel)) {
        return -1;
    }

    int result = -1;

    int skipped = 0;
    for (int i = 0; i < KPropertyUnit::TypeCount; ++i) {
        if ((listOptions&HidePixel) && (typesInUi[i] == Pixel)) {
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
    case Millimeter:
        return toMillimeter(ptValue);
    case Centimeter:
        return toCentimeter(ptValue);
    case Decimeter:
        return toDecimeter(ptValue);
    case Inch:
        return toInch(ptValue);
    case Pica:
        return toPica(ptValue);
    case Cicero:
        return toCicero(ptValue);
    case Pixel:
        return ptValue * m_pixelConversion;
    case Point:
    default:
        return toPoint(ptValue);
    }
}

qreal KPropertyUnit::ptToUnit(qreal ptValue, const KPropertyUnit &unit)
{
    switch (unit.m_type) {
    case Millimeter:
        return POINT_TO_MM(ptValue);
    case Centimeter:
        return POINT_TO_CM(ptValue);
    case Decimeter:
        return POINT_TO_DM(ptValue);
    case Inch:
        return POINT_TO_INCH(ptValue);
    case Pica:
        return POINT_TO_PI(ptValue);
    case Cicero:
        return POINT_TO_CC(ptValue);
    case Pixel:
        return ptValue * unit.m_pixelConversion;
    case Point:
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
    case Millimeter:
        return MM_TO_POINT(value);
    case Centimeter:
        return CM_TO_POINT(value);
    case Decimeter:
        return DM_TO_POINT(value);
    case Inch:
        return INCH_TO_POINT(value);
    case Pica:
        return PI_TO_POINT(value);
    case Cicero:
        return CC_TO_POINT(value);
    case Pixel:
        return value / m_pixelConversion;
    case Point:
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
    Type result = Point;

    if (symbol == QLatin1String("inch") /*compat*/) {
        result = Inch;
        if (ok)
            *ok = true;
    } else {
        if (ok)
            *ok = false;

        for (int i = 0; i < TypeCount; ++i) {
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
    case Millimeter:
        pt = MM_TO_POINT(value);
        break;
    case Centimeter:
        pt = CM_TO_POINT(value);
        break;
    case Decimeter:
        pt = DM_TO_POINT(value);
        break;
    case Inch:
        pt = INCH_TO_POINT(value);
        break;
    case Pica:
        pt = PI_TO_POINT(value);
        break;
    case Cicero:
        pt = CC_TO_POINT(value);
        break;
    case Pixel:
        pt = value / factor;
        break;
    case Point:
    default:
        pt = value;
    }

    switch (toUnit.type()) {
    case Millimeter:
        return POINT_TO_MM(pt);
    case Centimeter:
        return POINT_TO_CM(pt);
    case Decimeter:
        return POINT_TO_DM(pt);
    case Inch:
        return POINT_TO_INCH(pt);
    case Pica:
        return POINT_TO_PI(pt);
    case Cicero:
        return POINT_TO_CC(pt);
    case Pixel:
        return pt * factor;
    case Point:
    default:
        return pt;
    }

}

QString KPropertyUnit::symbol() const
{
    return QLatin1String(unitNameList[m_type]);
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
