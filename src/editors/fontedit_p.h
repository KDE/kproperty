/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2005-2009 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_FONTEDIT_P_H
#define KPROPERTY_FONTEDIT_P_H

#include <QWidget>

class QPushButton;

//! @internal
//! Reimplemented to better button and label's positioning
class KPropertyFontEditRequester : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QFont value READ value WRITE setValue USER true)
public:
    explicit KPropertyFontEditRequester(QWidget *parent);

    QFont value() const;

public Q_SLOTS:
    void setValue(const QFont &value);

Q_SIGNALS:
    void commitData(QWidget *editor);

protected Q_SLOTS:
    void slotSelectFontClicked();

protected:
    bool event(QEvent *event) override;

    QPushButton *m_button;
    QFont m_font;
    bool m_paletteChangedEnabled;
};

#endif
