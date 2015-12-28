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

#ifndef KPROPERTY_SPINBOX_H
#define KPROPERTY_SPINBOX_H

#include "KPropertyWidgetsFactory.h"

#include <QSpinBox>
#include <QDoubleSpinBox>

//! A delegate supporting Int and UInt types
/*! Note that due to KIntNumInput limitations, for UInt the maximum value
    is INT_MAX, not UINT_MAX.
*/
class KPROPERTYWIDGETS_EXPORT KPropertyIntSpinBox : public QSpinBox
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)

public:
    KPropertyIntSpinBox(const KProperty* prop, QWidget *parent, int itemHeight);
    virtual ~KPropertyIntSpinBox();

    QVariant value() const;

//    virtual void setProperty(const Property *prop);

Q_SIGNALS:
    void commitData(QWidget* editor);

public Q_SLOTS:
    void setValue(const QVariant& value);

//! @todo    virtual bool eventFilter(QObject *o, QEvent *e);
/*    QLineEdit * lineEdit() const {
        return KIntSpinBox::lineEdit();
    }*/
protected Q_SLOTS:
    void slotValueChanged(int value);

private:
    bool m_unsigned;
};

//! Double editor
class KPROPERTYWIDGETS_EXPORT KPropertyDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue USER true)

public:
    KPropertyDoubleSpinBox(const KProperty* prop, QWidget *parent, int itemHeight);
    virtual ~KPropertyDoubleSpinBox();

//    virtual bool eventFilter(QObject *o, QEvent *e);
/*    QLineEdit * lineEdit() const {
        return QDoubleSpinBox::lineEdit();
    }*/

    double value() const;

Q_SIGNALS:
    void commitData(QWidget* editor);

public Q_SLOTS:
    void setValue(double value);

protected Q_SLOTS:
    void slotValueChanged(double value);

protected:
    //! Used to fix height of the internal spin box
    virtual void resizeEvent( QResizeEvent * event );

    class Private;
    Private * const d;
};

//! A delegate supporting Int, UInt, LongLong and ULongLong types
class KPROPERTYWIDGETS_EXPORT KPropertyIntSpinBoxDelegate : public KPropertyEditorCreatorInterface,
                                                            public KPropertyValueDisplayInterface
{
public:
    KPropertyIntSpinBoxDelegate();

    virtual QString propertyValueToString(const KProperty* prop, const QLocale &locale) const;

    virtual QString valueToString(const QVariant& value, const QLocale &locale) const;

    virtual QWidget * createEditor( int type, QWidget *parent,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

class KPROPERTYWIDGETS_EXPORT KPropertyDoubleSpinBoxDelegate : public KPropertyEditorCreatorInterface,
                                                               public KPropertyValueDisplayInterface
{
public:
    KPropertyDoubleSpinBoxDelegate();

    virtual QString propertyValueToString(const KProperty* prop, const QLocale &locale) const;

    virtual QString valueToString(const QVariant& value, const QLocale &locale) const;

    virtual QWidget * createEditor( int type, QWidget *parent,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

#endif
