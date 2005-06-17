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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "cursoredit.h"

#include <qmap.h>
#include <qvariant.h>
#include <qcursor.h>

#ifdef QT_ONLY
// \tdo
#else
#include <klocale.h>
#include <kdebug.h>
#endif

#include "property.h"

namespace KOProperty {

QMap<QString, QVariant> *CursorEdit::m_spValues = 0;

CursorEdit::CursorEdit(Property *property, QWidget *parent, const char *name)
: ComboBox(property, parent, name)
{
	if(!m_spValues) {
		m_spValues = new QMap<QString, QVariant>();
		(*m_spValues)[i18n("Arrow")] = Qt::ArrowCursor;
		(*m_spValues)[i18n("Up Arrow")] = Qt::UpArrowCursor;
		(*m_spValues)[i18n("Cross")] = Qt::CrossCursor;
		(*m_spValues)[i18n("Waiting")] = Qt::WaitCursor;
		(*m_spValues)[i18n("iBeam")] = Qt::IbeamCursor;
		(*m_spValues)[i18n("Size Vertical")] = Qt::SizeVerCursor;
		(*m_spValues)[i18n("Size Horizontal")] = Qt::SizeHorCursor;
		(*m_spValues)[i18n("Size Slash")] = Qt::SizeBDiagCursor;
		(*m_spValues)[i18n("Size Backslash")] = Qt::SizeFDiagCursor;
		(*m_spValues)[i18n("Size All")] = Qt::SizeAllCursor;
		(*m_spValues)[i18n("Blank")] = Qt::BlankCursor;
		(*m_spValues)[i18n("Split Vertical")] = Qt::SplitVCursor;
		(*m_spValues)[i18n("Split Horizontal")] = Qt::SplitHCursor;
		(*m_spValues)[i18n("Pointing Hand")] = Qt::PointingHandCursor;
		(*m_spValues)[i18n("Forbidden")] = Qt::ForbiddenCursor;
		(*m_spValues)[i18n("What's this")] = Qt::WhatsThisCursor;
	}

	if(property)
		property->setValueList(*m_spValues);
}

CursorEdit::~CursorEdit()
{
	delete m_spValues;
	m_spValues = 0;
}

QVariant
CursorEdit::value() const
{
	kdDebug() << ComboBox::value().toInt() << endl;
	return ComboBox::value().toInt();
}

/*void
CursorEdit::setValue(const QVariant &value, bool emitChange)
{
	ComboBox::setValue(value.toCursor(), emitChange);
}*/

/*void
CursorEdit::drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value)
{
	ComboBox::drawViewer(p, cg, r, keyForValue(value.toInt()));
}*/

void
CursorEdit::setProperty(Property *prop)
{
	if(prop && prop != property())
		prop->setValueList(*m_spValues);
	ComboBox::setProperty(prop);
}

}

#include "cursoredit.moc"
