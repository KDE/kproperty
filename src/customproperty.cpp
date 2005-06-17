/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#include "customproperty.h"
#include "property.h"

#include <qsize.h>
#include <qrect.h>
#include <qsizepolicy.h>
#include <qpoint.h>

#ifdef QT_ONLY
// \todo
#else
#include <klocale.h>
#include <kdebug.h>
#endif

namespace KOProperty {

CustomProperty::CustomProperty(Property *parent)
 : m_property(parent)
{}

///////////////  SizeCustomProperty /////////////////////

SizeCustomProperty::SizeCustomProperty(Property *property)
: CustomProperty(property)
{
	if(property && (property->type() == Size) ) {
		QSize s = property->value().toSize();
		property->addChild(new Property("width", i18n("Width"), i18n("Width"), s.width(), Size_Width));
		property->addChild(new Property("height", i18n("Height"), i18n("Height"), s.height(), Size_Height));
	}
}

SizeCustomProperty::~SizeCustomProperty()
{}

bool
SizeCustomProperty::handleValue()
{
	if(!m_property)
		return false;

	switch(m_property->type()) {
		case Size_Width: case Size_Height:
			return true;
		default:
			return false;
	}
}

void
SizeCustomProperty::setValue(const QVariant &value, bool rememberOldValue)
{
	if(!m_property)
		return;

	if(m_property->parent()) {
		QSize s = m_property->parent()->value().toSize();

		if(m_property->type() == Size_Height)
			s.setHeight(value.toInt());
		else if(m_property->type() == Size_Width)
			s.setWidth(value.toInt());

		m_property->parent()->setValue(s, true, false);
	}
	else{
		QSize s = value.toSize();
		m_property->child("width")->setValue(s.width(), rememberOldValue, false);
		m_property->child("height")->setValue(s.height(), rememberOldValue, false);
	}
}

QVariant
SizeCustomProperty::value()
{
	if(!m_property || !m_property->parent())
		return QVariant();

	if(m_property->type() == Size_Height)
		return m_property->parent()->value().toSize().height();
	else if(m_property->type() == Size_Width)
		return m_property->parent()->value().toSize().width();

	return QVariant();
}

///////////////  PointCustomProperty /////////////////////

PointCustomProperty::PointCustomProperty(Property *property)
: CustomProperty(property)
{
	if(property && (property->type() == Point) ) {
		QPoint p = property->value().toPoint();
		property->addChild(new Property("x", i18n("X"), i18n("x"), p.x(), Point_X));
		property->addChild(new Property("y", i18n("Y"), i18n("y"), p.y(), Point_Y));
	}
}

bool
PointCustomProperty::handleValue()
{
	if(!m_property)
		return false;

	switch(m_property->type()) {
		case Point_X: case Point_Y:
			return true;
		default:
			return false;
	}
}

void
PointCustomProperty::setValue(const QVariant &value, bool rememberOldValue)
{
	if(!m_property)
		return;

	if(m_property->parent()) {
		QPoint p = m_property->parent()->value().toPoint();

		if(m_property->type() == Point_X)
			p.setX(value.toInt());
		else if(m_property->type() == Point_Y)
			p.setY(value.toInt());

		m_property->parent()->setValue(p, true, false);
	}
	else {
		QPoint p = value.toPoint();
		m_property->child("x")->setValue(p.x(), rememberOldValue, false);
		m_property->child("y")->setValue(p.y(), rememberOldValue, false);
	}
}

QVariant
PointCustomProperty::value()
{
	if(!m_property || !m_property->parent())
		return QVariant();

	if(m_property->type() == Point_X)
		return m_property->parent()->value().toPoint().x();
	else if(m_property->type() == Point_Y)
		return m_property->parent()->value().toPoint().y();

	return QVariant();
}

///////////////  RectCustomProperty /////////////////////

RectCustomProperty::RectCustomProperty(Property *property)
: CustomProperty(property)
{
	if(property && (property->type() == Rect) ) {
		QRect r = property->value().toRect();
		property->addChild(new Property("x",  i18n("X"), i18n("x"), r.x(), Rect_X));
		property->addChild(new Property("y", i18n("Y"), i18n("y"), r.y(), Rect_Y));
		property->addChild(new Property("width", i18n("Width"), i18n("Width"), r.width(), Rect_Width));
		property->addChild(new Property("height", i18n("Height"), i18n("Height"), r.height(), Rect_Height));
	}
}

bool
RectCustomProperty::handleValue()
{
	if(!m_property)
		return false;

	switch(m_property->type()) {
		case Rect_X: case Rect_Y: case Rect_Width: case Rect_Height:
			return true;
		default:
			return false;
	}
}

void
RectCustomProperty::setValue(const QVariant &value, bool rememberOldValue)
{
	if(!m_property)
		return;

	if(m_property->parent()) {
		QRect r = m_property->parent()->value().toRect();

		if(m_property->type() == Rect_X)
			r.setX(value.toInt());
		else if(m_property->type() == Rect_Y)
			r.setY(value.toInt());
		else if(m_property->type() == Rect_Width)
			r.setWidth(value.toInt());
		else if(m_property->type() == Rect_Height)
			r.setHeight(value.toInt());

		m_property->parent()->setValue(r, true, false);
	}
	else {
		QRect r = value.toRect();
		m_property->child("x")->setValue(r.x(), rememberOldValue, false);
		m_property->child("y")->setValue(r.y(), rememberOldValue, false);
		m_property->child("width")->setValue(r.width(), rememberOldValue, false);
		m_property->child("height")->setValue(r.height(), rememberOldValue, false);
	}
}

QVariant
RectCustomProperty::value()
{
	if(!m_property || !m_property->parent())
		return QVariant();

	if(m_property->type() == Rect_X)
		return m_property->parent()->value().toRect().x();
	else if(m_property->type() == Rect_Y)
		return m_property->parent()->value().toRect().y();
	else if(m_property->type() == Rect_Width)
		return m_property->parent()->value().toRect().width();
	else if(m_property->type() == Rect_Height)
		return m_property->parent()->value().toRect().height();

	return QVariant();
}


///////////////  SizePolicyCustomProperty /////////////////////

SizePolicyCustomProperty::SizePolicyCustomProperty(Property *property)
: CustomProperty(property)
{
	if(property && (property->type() == SizePolicy) ) {
		QMap<QString, QVariant> spValues;
		spValues[i18n("Fixed")] = QSizePolicy::Fixed;
		spValues[i18n("Minimum")] = QSizePolicy::Minimum;
		spValues[i18n("Maximum")] = QSizePolicy::Maximum;
		spValues[i18n("Preferred")] = QSizePolicy::Preferred;
		spValues[i18n("Expanding")] = QSizePolicy::Expanding;
		spValues[i18n("Minimum Expanding")] = QSizePolicy::MinimumExpanding;
		spValues[i18n("Ignored")] = QSizePolicy::Ignored;

		property->addChild(new Property("hSizeType", i18n("Hor. Size Type"),i18n("Horizontal Size Type"),
			spValues, (int)property->value().toSizePolicy().horData(), SizePolicy_HorData));
		property->addChild(new Property("vSizeType", i18n("Ver. Size Type"), i18n("Vertical Size Type"),
			spValues, (int)property->value().toSizePolicy().verData(), SizePolicy_VerData));
		property->addChild(new Property("hStretch", i18n("Hor. Stretch"), i18n("Horizontal Stretch"),
			property->value().toSizePolicy().horStretch(), SizePolicy_HorStretch));
		property->addChild(new Property("vStretch", i18n("Ver. Stretch"), i18n("Vertical Stretch"),
			property->value().toSizePolicy().verStretch(), SizePolicy_VerStretch));
	}
}

bool
SizePolicyCustomProperty::handleValue()
{
	if(!m_property)
		return false;

	switch(m_property->type()) {
		case SizePolicy_HorData: case SizePolicy_VerData: case SizePolicy_HorStretch: case SizePolicy_VerStretch:
			return true;
		default:
			return false;
	}
}

void
SizePolicyCustomProperty::setValue(const QVariant &value, bool rememberOldValue)
{
	if(!m_property)
		return;

	if(m_property->parent()) {
		QSizePolicy v = m_property->parent()->value().toSizePolicy();

		if(m_property->type() == SizePolicy_HorData)
			v.setHorData(QSizePolicy::SizeType(value.toInt()));
		else if(m_property->type() == SizePolicy_VerData)
			v.setVerData(QSizePolicy::SizeType(value.toInt()));
		else if(m_property->type() == SizePolicy_HorStretch)
			v.setHorStretch(value.toInt());
		else if(m_property->type() == SizePolicy_VerStretch)
			v.setVerStretch(value.toInt());

		m_property->parent()->setValue(v, true, false);
	}
	else {
		QSizePolicy v = value.toSizePolicy();
		m_property->child("hSizeType")->setValue(v.horData(), rememberOldValue, false);
		m_property->child("vSizeType")->setValue(v.verData(), rememberOldValue, false);
		m_property->child("hStretch")->setValue(v.horStretch(), rememberOldValue, false);
		m_property->child("vStretch")->setValue(v.verStretch(), rememberOldValue, false);
	}
}

QVariant
SizePolicyCustomProperty::value()
{
	if(!m_property || !m_property->parent())
		return QVariant();

	if(m_property->type() == SizePolicy_HorData)
		return m_property->parent()->value().toSizePolicy().horData();
	else if(m_property->type() == SizePolicy_VerData)
		return m_property->parent()->value().toSizePolicy().verData();
	else if(m_property->type() == SizePolicy_HorStretch)
		return m_property->parent()->value().toSizePolicy().horStretch();
	else if(m_property->type() == SizePolicy_VerStretch)
		return m_property->parent()->value().toSizePolicy().verStretch();

	return QVariant();
}

}
