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
#ifndef KPROPERTYLINESTYLEITEMDELEGATE_H
#define KPROPERTYLINESTYLEITEMDELEGATE_H

#include <QAbstractItemDelegate>

//! Line style item delegate for rendering the styles
class KPropertyLineStyleItemDelegate : public QAbstractItemDelegate
{
public:
    explicit KPropertyLineStyleItemDelegate(QObject *parent = nullptr);
    ~KPropertyLineStyleItemDelegate() override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    static void paintItem(QPainter *painter, const QPen &pen_, const QRect &rect, const QStyleOption &option);

    static QString styleName(Qt::PenStyle style, const QLocale &locale);
};

#endif
