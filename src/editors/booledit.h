/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2006-2008 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_BOOLEDIT_H
#define KPROPERTY_BOOLEDIT_H

#include "combobox.h"
#include "KPropertyFactory.h"

#include <QEvent>
#include <QPixmap>
#include <QResizeEvent>
#include <QToolButton>

//! A bool editor supporting two states: true and false.
/*! For null values, false is displayed.
*/
class KPROPERTY_EXPORT KPropertyBoolEditor : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(bool value READ value WRITE setValue USER true)

public:
    explicit KPropertyBoolEditor(const KProperty *prop, QWidget *parent = 0);

    ~KPropertyBoolEditor();

    bool value() const;

    static void draw(QPainter *p, const QRect &r, const QVariant &value,
                     const QString& text, bool threeState);
Q_SIGNALS:
    void commitData(QWidget* editor);

public Q_SLOTS:
    void setValue(bool value);

protected Q_SLOTS:
    void  slotValueChanged(bool state);

protected:
    virtual void paintEvent( QPaintEvent * event );
//    virtual void setReadOnlyInternal(bool readOnly);
//    void setState(bool state);
//    virtual void resizeEvent(QResizeEvent *ev);
    virtual bool eventFilter(QObject* watched, QEvent* e);

private:
    QString m_yesText;
    QString m_noText;
};

//! A bool editor supporting three states: true, false and null.
/*! The editor is implemented as a drop-down list.
*/
class KPROPERTY_EXPORT KPropertyThreeStateBoolEditor : public KPropertyComboBoxEditor
{
    Q_OBJECT
//    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)

public:
    explicit KPropertyThreeStateBoolEditor(const KPropertyListData& listData, QWidget *parent = 0);
    ~KPropertyThreeStateBoolEditor();

    QVariant value() const;
    void setValue(const QVariant &value);

Q_SIGNALS:
    void commitData( QWidget * editor );
};

class KPROPERTY_EXPORT KPropertyBoolDelegate : public KPropertyEditorCreatorInterface,
                                               public KPropertyValuePainterInterface
{
public:
    KPropertyBoolDelegate();

    virtual QWidget * createEditor( int type, QWidget *parent,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    virtual void paint( QPainter * painter,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

#endif
