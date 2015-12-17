/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2008-2015 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_CURSOREDIT_H
#define KPROPERTY_CURSOREDIT_H

#include "combobox.h"

class KPROPERTYWIDGETS_EXPORT KPropertyCursorEditor : public KPropertyComboBoxEditor
{
    Q_OBJECT
    Q_PROPERTY(QCursor value READ cursorValue WRITE setCursorValue USER true)

public:
    explicit KPropertyCursorEditor(QWidget *parent = 0);
    virtual ~KPropertyCursorEditor();

    virtual QCursor cursorValue() const;
    virtual void setCursorValue(const QCursor &value);
};

class KPROPERTYWIDGETS_EXPORT KPropertyCursorDelegate : public KPropertyEditorCreatorInterface,
                                                        public KPropertyValuePainterInterface,
                                                        public KPropertyValueDisplayInterface
{
public:
    KPropertyCursorDelegate();

    virtual QWidget * createEditor( int type, QWidget *parent,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    virtual void paint( QPainter * painter,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    virtual QString valueToString(const QVariant& value, const QLocale &locale) const;
};

#endif
