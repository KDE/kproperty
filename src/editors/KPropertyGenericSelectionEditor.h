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

#ifndef KPROPERTYGENERICSELECTIONEDITOR_H
#define KPROPERTYGENERICSELECTIONEDITOR_H

#include "kpropertywidgets_export.h"

#include <QWidget>
#include <QScopedPointer>

//! A base class for use by editors that have widget on the left and "..." select button on the right
class KPROPERTYWIDGETS_EXPORT KPropertyGenericSelectionEditor : public QWidget
{
    Q_OBJECT
public:
    explicit KPropertyGenericSelectionEditor(QWidget *parent = nullptr);

    ~KPropertyGenericSelectionEditor() override;

public Q_SLOTS:
    //! Sets the visibility of the "..." select button
    void setSelectionButtonVisible(bool set);

protected:
    void setMainWidget(QWidget *widget);

protected Q_SLOTS:
    //! Reimplement to react on clicking the "..." select button
    virtual void selectButtonClicked();

private:
    Q_DISABLE_COPY(KPropertyGenericSelectionEditor)
    class Private;
    QScopedPointer<Private> const d;
};

#endif
