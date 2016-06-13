/* This file is part of the KDE project
   Copyright (C) 2006-2016 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_UTILS_H
#define KPROPERTY_UTILS_H

#include "kpropertywidgets_export.h"

#include <QWidget>

class KProperty;

//! @short A container widget that can be used to split information into hideable sections
//! for a property editor-like panes.
class KPROPERTYWIDGETS_EXPORT KPropertyGroupWidget : public QWidget
{
public:
    explicit KPropertyGroupWidget(const QString& title, QWidget* parent = 0);
    ~KPropertyGroupWidget();

    void setContents(QWidget* contents);

protected:
    virtual bool event(QEvent * e);

    class Private;
    Private * const d;
};

namespace KPropertyUtils {

//! @return property object for model index @a index or @c nullptr if there is no property
//! referenced by the index.
KPROPERTYWIDGETS_EXPORT KProperty* propertyForIndex(const QModelIndex &index);

} // KPropertyUtils

#endif
