/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>
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

#include "KPropertyStringEditor.h"

KPropertyStringEditor::KPropertyStringEditor(QWidget *parent)
 : QLineEdit(parent)
 , m_slotTextChangedEnabled(true)
{
    setFrame(false);
    setContentsMargins(0,1,0,0);
    setClearButtonEnabled(true);
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(slotTextChanged(const QString&)));
}

KPropertyStringEditor::~KPropertyStringEditor()
{
}

QString KPropertyStringEditor::value() const
{
    return text();
}

void KPropertyStringEditor::setValue(const QString& value)
{
    m_slotTextChangedEnabled = false;
    setText(value);
    m_slotTextChangedEnabled = true;
/*    deselect();
    end(false);*/
}

void KPropertyStringEditor::slotTextChanged( const QString & text )
{
    Q_UNUSED(text)
    if (!m_slotTextChangedEnabled)
        return;
    emit commitData(this);
}

QWidget* KPropertyStringDelegate::createEditor( int type, QWidget *parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Q_UNUSED(type);
    Q_UNUSED(option);
    Q_UNUSED(index);
    return new KPropertyStringEditor(parent);
}
