/* This file is part of the KDE project
   Copyright (C) 2010-2015 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_LINESTYLEEDIT_H
#define KPROPERTY_LINESTYLEEDIT_H

#include "KPropertyWidgetsFactory.h"
#include "KPropertyLineStyleSelector_p.h"

//! Line style combo box (Property::LineStyle, equivalent of Qt::PenStyle)
//! @todo enable transparency selection
//! @todo add transparency option
//! @todo reimplement view using KColorCells
class KPROPERTYWIDGETS_EXPORT KPropertyLineStyleComboEditor : public KPropertyLineStyleSelector
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)

public:
    explicit KPropertyLineStyleComboEditor(QWidget *parent = 0);

    ~KPropertyLineStyleComboEditor();

    QVariant value() const;

Q_SIGNALS:
    void commitData(QWidget * editor);

public Q_SLOTS:
    void setValue(const QVariant &value);

protected Q_SLOTS:
    void slotValueChanged(int index);
};

class KPROPERTYWIDGETS_EXPORT KPropertyLineStyleComboDelegate : public KPropertyEditorCreatorInterface,
                                                                public KPropertyValuePainterInterface,
                                                                public KPropertyValueDisplayInterface
{
public:
    KPropertyLineStyleComboDelegate() {options.removeBorders = false;}

    virtual QWidget * createEditor( int type, QWidget *parent,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    virtual void paint( QPainter * painter,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    virtual QString valueToString(const QVariant& value, const QLocale &locale) const;
};

#endif
