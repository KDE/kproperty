/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
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

#ifndef KPROPERTYURLEDITOR_H
#define KPROPERTYURLEDITOR_H

#include "KPropertyWidgetsFactory.h"
#include "KPropertyGenericSelectionEditor.h"

#include <QUrl>

//! Editor for Url type
class KPROPERTYWIDGETS_EXPORT KPropertyUrlEditor : public KPropertyGenericSelectionEditor
{
    Q_PROPERTY(QUrl value READ value WRITE setValue USER true)
    Q_OBJECT

public:
    explicit KPropertyUrlEditor(const KProperty &property, QWidget *parent = nullptr);

    ~KPropertyUrlEditor() override;

    virtual QUrl value() const;

Q_SIGNALS:
    void commitData(QWidget * editor);

public Q_SLOTS:
    virtual void setValue(const QUrl &value);

protected Q_SLOTS:
    void selectButtonClicked() override;

private:
    Q_DISABLE_COPY(KPropertyUrlEditor)
    class Private;
    QScopedPointer<Private> const d;
};

//! Delegate for Url type
class KPROPERTYWIDGETS_EXPORT KPropertyUrlDelegate : public KPropertyEditorCreatorInterface,
                                                     public KPropertyValueDisplayInterface
{
public:
    KPropertyUrlDelegate();

    QWidget *createEditor(int type, QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    QString valueToString(const QVariant &value, const QLocale &locale) const override;
};

#endif
