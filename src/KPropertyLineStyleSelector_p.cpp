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

#include "KPropertyLineStyleSelector_p.h"
#include "KPropertyLineStyleModel_p.h"
#include "KPropertyLineStyleItemDelegate_p.h"
#include "KPropertyWidgetsFactory.h"

#include <QPen>
#include <QPainter>

class Q_DECL_HIDDEN KPropertyLineStyleSelector::Private
{
public:
    Private(QWidget *parent)
    : model(new KPropertyLineStyleModel(parent))
    {
    }

    KPropertyLineStyleModel *model;
};

KPropertyLineStyleSelector::KPropertyLineStyleSelector(QWidget *parent)
    : QComboBox(parent), d(new Private(this))
{
    setModel(d->model);
    setItemDelegate(new KPropertyLineStyleItemDelegate(this));
    setEditable(false);
    setInsertPolicy(QComboBox::NoInsert);
    setContextMenuPolicy(Qt::NoContextMenu);
}

KPropertyLineStyleSelector::~KPropertyLineStyleSelector()
{
    delete d;
}

void KPropertyLineStyleSelector::paintEvent(QPaintEvent *pe)
{
    QComboBox::paintEvent(pe);

    QStyleOptionComboBox option;
    option.initFrom(this);
    option.frame = hasFrame();
    QRect r = style()->subControlRect(QStyle::CC_ComboBox, &option, QStyle::SC_ComboBoxEditField, this);
    QPen pen = itemData(currentIndex(), Qt::DecorationRole).value<QPen>();
    QPainter painter(this);
    KPropertyLineStyleItemDelegate::paintItem(&painter, pen, r, option);
    KPropertyWidgetsFactory::paintTopGridLine(this);
}

bool KPropertyLineStyleSelector::addCustomStyle(const QVector<qreal> &style)
{
    return d->model->addCustomStyle(style);
}

void KPropertyLineStyleSelector::setLineStyle(Qt::PenStyle style, const QVector<qreal> &dashes)
{
    int index = d->model->setLineStyle(style, dashes);
    if (index >= 0)
        setCurrentIndex(index);
}

Qt::PenStyle KPropertyLineStyleSelector::lineStyle() const
{
    QPen pen = itemData(currentIndex(), Qt::DecorationRole).value<QPen>();
    return pen.style();
}

QVector<qreal> KPropertyLineStyleSelector::lineDashes() const
{
    QPen pen = itemData(currentIndex(), Qt::DecorationRole).value<QPen>();
    return pen.dashPattern();
}
