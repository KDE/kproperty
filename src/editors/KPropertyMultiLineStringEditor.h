/* This file is part of the KDE project
   Copyright (C) 2016 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_MULTILINESTRINGEDITOR_H
#define KPROPERTY_MULTILINESTRINGEDITOR_H

#include "KPropertyWidgetsFactory.h"

//! Editor for string type supporting multiple lines of plain text
//! @since 3.1
class KPROPERTYWIDGETS_EXPORT KPropertyMultiLineStringEditor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString value READ value WRITE setValue USER true)
public:
    explicit KPropertyMultiLineStringEditor(QWidget *parent = nullptr);

    ~KPropertyMultiLineStringEditor() override;

    QString value() const;

Q_SIGNALS:
    void commitData(QWidget *editor);

public Q_SLOTS:
    void setValue(const QString &value);

private Q_SLOTS:
    void slotTextChanged();

protected:
    bool eventFilter(QObject *o, QEvent *ev) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent * event) Q_DECL_OVERRIDE;

private:
    class Private;
    Private * const d;
};

#endif
