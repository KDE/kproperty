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

#include "linestyleedit.h"
#include "editoritem.h"

#include <QPainter>
#include <QPixmap>
#include <QComboBox>
#include <QLayout>
#include <QVariant>
#include <QHBoxLayout>

using namespace KoProperty;

    //! @internal
    static const char *nopen[]={
    "48 16 1 1",
    ". c None",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................"};
    //! @internal
    static const char *solid[]={
    "48 16 2 1",
    ". c None",
    "# c #000000",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    ".###########################################....",
    ".###########################################....",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................"};
    //! @internal
    static const char *dash[]={
    "48 16 2 1",
    ". c None",
    "# c #000000",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    ".#########..#########..#########..##########....",
    ".#########..#########..#########..##########....",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................"};
    //! @internal
    static const char *dot[]={
    "48 16 2 1",
    ". c None",
    "# c #000000",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    ".##..##..##..##..##..##..##..##..##..##..##..##.",
    ".##..##..##..##..##..##..##..##..##..##..##..##.",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................"};
    //! @internal
    static const char *dashdot[]={
    "48 16 2 1",
    ". c None",
    "# c #000000",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    ".#########..##..#########..##..#########..##....",
    ".#########..##..#########..##..#########..##....",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................"};
    //! @internal
    static const char *dashdotdot[]={
    "48 16 2 1",
    ". c None",
    "# c #000000",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    ".#########..##..##..#########..##..##..#####....",
    ".#########..##..##..#########..##..##..#####....",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................",
    "................................................"};


LineStyleEdit::LineStyleEdit(Property *property, QWidget *parent)
 : Widget(property, parent)
{
	QHBoxLayout *l = new QHBoxLayout(this);
	l->setMargin(0);
	l->setSpacing(0);

	m_edit = new QComboBox(this);
	m_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_edit->setMinimumHeight(5);
	setPlainWidgetStyle(m_edit);
	
	l->addWidget(m_edit);

	m_edit->addItem(QIcon(QPixmap(nopen)), "");
	m_edit->addItem(QIcon(QPixmap(solid)), "");
	m_edit->addItem(QIcon(QPixmap(dash)), "");
	m_edit->addItem(QIcon(QPixmap(dot)), "");
	m_edit->addItem(QIcon(QPixmap(dashdot)), "");
	m_edit->addItem(QIcon(QPixmap(dashdotdot)), "");

	setLeavesTheSpaceForRevertButton(true);
	setFocusWidget(m_edit);
	connect(m_edit, SIGNAL(activated(int)), this, SLOT(slotValueChanged(int)));
}

LineStyleEdit::~LineStyleEdit()
{}

QVariant
LineStyleEdit::value() const
{
	return m_edit->currentIndex();
}

void
LineStyleEdit::setValue(const QVariant &value, bool emitChange)
{
	if (!value.canConvert(QVariant::Int))
		return;
	if ((value.toInt() > 5) || (value.toInt() < 0))
		return;

	m_edit->blockSignals(true);
	m_edit->setCurrentIndex(value.toInt());
	m_edit->blockSignals(false);
	if (emitChange)
		emit valueChanged(this);
}

void
LineStyleEdit::drawViewer(QPainter *p, const QColorGroup &, const QRect &r, const QVariant &value)
{
	p->eraseRect(r);

	if (!value.canConvert(QVariant::Int))
		return;

	QPixmap px;
	switch (value.toInt()) {
	case Qt::NoPen:
		px = QPixmap(nopen);
		break;
	case Qt::SolidLine:
		px = QPixmap(solid);
		break;
	case Qt::DashLine:
		px = QPixmap(dash);
		break;
	case Qt::DotLine:
		px = QPixmap(dot);
		break;
	case Qt::DashDotLine:
		px = QPixmap(dashdot);
		break;
	case Qt::DashDotDotLine:
		px = QPixmap(dashdotdot);
		break;
	default:
		return;
	}
	p->drawPixmap(r.left()+KPROPEDITOR_ITEM_MARGIN, r.top()+(r.height()-px.height())/2, px);
}

void
LineStyleEdit::slotValueChanged(int)
{
	emit valueChanged(this);
}

void
LineStyleEdit::setReadOnlyInternal(bool readOnly)
{
	setVisibleFlag(!readOnly);
}

#include "linestyleedit.moc"
