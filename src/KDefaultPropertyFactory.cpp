/* This file is part of the KDE project
   Copyright (C) 2008 Jarosław Staniek <staniek@kde.org>

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

#include "KDefaultPropertyFactory.h"
#include "config-kproperty.h"
/*
#include "customproperty.h"*/
#include "editors/booledit.h"
#include "editors/combobox.h"
#include "editors/cursoredit.h"
#include "editors/dateedit.h"
#include "editors/datetimeedit.h"
// #include "editors/dummywidget.h"
#include "editors/pixmapedit.h"
#include "editors/pointedit.h"
#include "editors/pointfedit.h"
#include "editors/fontedit.h"
#include "editors/rectedit.h"
#include "editors/sizeedit.h"
#include "editors/sizefedit.h"
#include "editors/sizepolicyedit.h"
#include "editors/spinbox.h"
/*#include "stringlistedit.h"*/
#include "editors/linestyleedit.h"
#include "editors/KPropertyStringEditor.h"
// #include "symbolcombo.h"
#include "editors/timeedit.h"
// #include "urledit.h"
#ifdef KPROPERTY_KF
# include "editors/coloredit.h"
#endif

KDefaultPropertyFactory::KDefaultPropertyFactory()
 : KPropertyWidgetsFactory()
{
    addEditor( KProperty::Bool, new KPropertyBoolDelegate );
    addEditor( KProperty::Cursor, new KPropertyCursorDelegate );
    addEditor( KProperty::Date, new KPropertyDateDelegate );
    addEditor( KProperty::DateTime, new KPropertyDateTimeDelegate );
    addEditor( KProperty::Double, new KPropertyDoubleSpinBoxDelegate );
    addEditor( KProperty::Font, new KPropertyFontDelegate );
    addEditor( KProperty::Int, new KPropertyIntSpinBoxDelegate );
    addEditor( KProperty::LineStyle, new KPropertyLineStyleComboDelegate );
//! @todo addEditor( KProperty::LongLong, new LongLongSpinBoxDelegate );
    addEditor( KProperty::Pixmap, new KPropertyPixmapDelegate );
    addEditor( KProperty::Point, new KPropertyPointDelegate );
    addEditor( KProperty::PointF, new KPropertyPointFDelegate );
    addEditor( KProperty::Rect, new KPropertyRectDelegate );
//! @todo    addEditor( KProperty::RectF, new RectFDelegate );
    addEditor( KProperty::Size, new KPropertySizeDelegate );
    addEditor( KProperty::SizeF, new KPropertySizeFDelegate );
    addEditor( KProperty::SizePolicy, new KPropertySizePolicyDelegate );
    addEditor( KProperty::String, new KPropertyStringDelegate );
    addEditor( KProperty::Time, new KPropertyTimeDelegate );
//! @todo    addEditor( KProperty::UInt, new IntSpinBoxDelegate );
//! @todo addEditor( KProperty::ULongLong, new LongLongSpinBoxDelegate );
    addEditor( KProperty::ValueFromList, new KPropertyComboBoxDelegate );
#ifdef KPROPERTY_KF
    addEditor( KProperty::Color, new KPropertyColorComboDelegate );
#endif
}

KDefaultPropertyFactory::~KDefaultPropertyFactory()
{
}
