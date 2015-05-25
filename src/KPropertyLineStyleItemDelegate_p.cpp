/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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

#include <QPen>
#include <QPainter>

KPropertyLineStyleItemDelegate::KPropertyLineStyleItemDelegate(QObject * parent)
    : QAbstractItemDelegate(parent)
{
}

KPropertyLineStyleItemDelegate::~KPropertyLineStyleItemDelegate()
{
}

void KPropertyLineStyleItemDelegate::paint(QPainter *painter,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{
    painter->save();

    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());

    QPen pen = index.data(Qt::DecorationRole).value<QPen>();
    pen.setBrush(option.palette.text()); // use the view-specific palette; the model hardcodes this to black
    painter->setPen(pen);
    painter->drawLine(option.rect.left(), option.rect.center().y(),
                      option.rect.right(), option.rect.center().y());

    painter->restore();
}

QSize KPropertyLineStyleItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                               const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(100, 15);
}
