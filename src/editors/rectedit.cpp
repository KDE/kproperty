/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>

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
#include "rectedit.h"
#include "editoritem.h"

#include <QVariant>
#include <QLayout>
#include <QPainter>
#include <QToolTip>
#include <QLabel>

#include <klocale.h>

#define RECTEDIT_MASK "%1,%2 %3x%4"

using namespace KoProperty;

RectEdit::RectEdit(Property *property, QWidget *parent)
        : Widget(property, parent)
{
    setHasBorders(false);
    m_edit = new QLabel(this);
    m_edit->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QPalette pal = m_edit->palette();
    pal.setColor(QPalette::Window, palette().color(QPalette::Active, QPalette::Base));
    m_edit->setPalette(pal);
    m_edit->setMinimumHeight(5);
    setEditor(m_edit);
// setFocusWidget(m_edit);
}

RectEdit::~RectEdit()
{}

QVariant
RectEdit::value() const
{
    return m_value;
}

void
RectEdit::setValue(const QVariant &value, bool emitChange)
{
    m_value = value;
    m_edit->setText(QString(RECTEDIT_MASK).arg(value.toRect().x()).
                    arg(value.toRect().y()).arg(value.toRect().width()).arg(value.toRect().height()));
    this->setToolTip(i18n("Position: %1, %2\nSize: %3 x %4", value.toRect().x()).
                     arg(value.toRect().y()).arg(value.toRect().width()).arg(value.toRect().height()));

    if (emitChange)
        emit valueChanged(this);
}

void
RectEdit::drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value)
{
    QRect rect(r);
    rect.setBottom(r.bottom() + 1);
    Widget::drawViewer(p, cg, rect,
                       QString(RECTEDIT_MASK).arg(value.toRect().x()).arg(value.toRect().y())
                       .arg(value.toRect().width()).arg(value.toRect().height()));
// p->eraseRect(r);
// p->drawText(r, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
//   QString("[ %1, %2, %3, %4 ]").arg(value.toRect().x()).arg(value.toRect().y())
//   .arg(value.toRect().width()).arg(value.toRect().height()));
}

void
RectEdit::setReadOnlyInternal(bool readOnly)
{
    Q_UNUSED(readOnly);
}

#include "rectedit.moc"
