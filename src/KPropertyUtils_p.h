/* This file is part of the KDE project
   Copyright (C) 2010 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_UTILS_P_H
#define KPROPERTY_UTILS_P_H

class QColor;
class QWidget;

namespace KPropertyUtils
{

//! @return contrast color for @a c color.
QColor contrastColor(const QColor& c);

//! @return grid line color defined by a KPropertyEditorView widget contains @a widget
//! Invalid  color is returned if no grid is defined or KPropertyEditorView was not found.
QColor gridLineColor(const QWidget *widget);

}

#endif
