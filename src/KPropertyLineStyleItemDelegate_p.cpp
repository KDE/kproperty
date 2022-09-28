/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2015 Jarosław Staniek <staniek@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KPropertyLineStyleItemDelegate_p.h"
#include "KPropertyCoreUtils_p.h"
#include "KPropertyUtils_p.h"

#include <QPen>

KPropertyLineStyleItemDelegate::KPropertyLineStyleItemDelegate(QObject * parent)
    : QAbstractItemDelegate(parent)
{
}

KPropertyLineStyleItemDelegate::~KPropertyLineStyleItemDelegate()
{
}

class PenStyleData : public QHash<Qt::PenStyle, QString>
{
public:
    PenStyleData() {
        insert(Qt::NoPen, QObject::tr("None", "No Line"));
        insert(Qt::SolidLine, QObject::tr("Solid Line"));
        insert(Qt::DashLine, QObject::tr("Dash Line"));
        insert(Qt::DotLine, QObject::tr("Dot Line"));
        insert(Qt::DashDotLine, QObject::tr("Dash-Dot Line"));
        insert(Qt::DashDotDotLine, QObject::tr("Dash-Dot-Dot Line"));
        insert(Qt::CustomDashLine, QObject::tr("Custom Dash Line"));
    }
};

Q_GLOBAL_STATIC(PenStyleData, g_penStyleData)

//static
QString KPropertyLineStyleItemDelegate::styleName(Qt::PenStyle style, const QLocale &locale)
{
    if (locale.language() == QLocale::C) {
        return KPropertyUtils::keyForEnumValue("PenStyle", style);
    }
    return g_penStyleData->value(style);
}

//static
void KPropertyLineStyleItemDelegate::paintItem(QPainter *painter, const QPen &pen_,
                                               const QRect &rect, const QStyleOption &option)
{
    const KPropertyUtilsPrivate::PainterSaver saver(painter);
    QPen pen(pen_);
    pen.setBrush((option.state & QStyle::State_Selected)
                 ? option.palette.highlightedText() : option.palette.text());
    if (pen.style() == Qt::NoPen) {
        pen.setWidth(0);
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);
        painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, g_penStyleData->value(Qt::NoPen));
    } else {
        pen.setWidth(3);
        painter->setPen(pen);
        painter->drawLine(rect.left(), rect.center().y(), rect.right(), rect.center().y());
    }
}

void KPropertyLineStyleItemDelegate::paint(QPainter *painter,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{
    const KPropertyUtilsPrivate::PainterSaver saver(painter);
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    QPen pen = index.data(Qt::DecorationRole).value<QPen>();
    paintItem(painter, pen, option.rect, option);
}

QSize KPropertyLineStyleItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                               const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(100, 15);
}
