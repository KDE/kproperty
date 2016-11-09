/* This file is part of the KDE project
   Copyright (C) 2010-2016 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_COLOREDIT_H
#define KPROPERTY_COLOREDIT_H

#include "KPropertyWidgetsFactory.h"

//! Color combo box
//! @todo enable transparency selection
//! @todo add transparency option
//! @todo reimplement view using KColorCells
class KPROPERTYWIDGETS_EXPORT KPropertyColorComboEditor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)

public:
    explicit KPropertyColorComboEditor(QWidget *parent = 0);

    ~KPropertyColorComboEditor();

    QVariant value() const;

Q_SIGNALS:
    void commitData( QWidget * editor );

public Q_SLOTS:
    void setValue(const QVariant &value);

protected Q_SLOTS:
    void slotValueChanged(const QColor&);

protected:
    bool eventFilter(QObject *o, QEvent *e) Q_DECL_OVERRIDE;

private:
    class Private;
    Private * const d;
};

class KPROPERTYWIDGETS_EXPORT KPropertyColorComboDelegate : public KPropertyEditorCreatorInterface,
                                                            public KPropertyValuePainterInterface,
                                                            public KPropertyValueDisplayInterface
{
public:
    KPropertyColorComboDelegate();
    ~KPropertyColorComboDelegate();

    virtual QWidget * createEditor( int type, QWidget *parent,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    virtual void paint( QPainter * painter,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    virtual QString valueToString(const QVariant& value, const QLocale &locale) const;
private:
    class Private;
    Private * const d;
};

#endif
