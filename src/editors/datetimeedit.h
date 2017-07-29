/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2012 Friedrich W. H. Kossebau <kossebau@kde.org>
   Copyright (C) 2015 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_DATETIMEEDIT_H
#define KPROPERTY_DATETIMEEDIT_H

#include "KPropertyWidgetsFactory.h"
#include <QDateTimeEdit>

class KPROPERTYWIDGETS_EXPORT KPropertyDateTimeEditor : public QDateTimeEdit
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)

public:
    KPropertyDateTimeEditor(const KProperty* prop, QWidget* parent);
    ~KPropertyDateTimeEditor() override;

    QVariant value() const;

Q_SIGNALS:
    void commitData(QWidget* editor);

public Q_SLOTS:
    void setValue(const QVariant& value);

protected:
    void paintEvent(QPaintEvent *event) override;

protected Q_SLOTS:
    void onDateTimeChanged();

private:
    Q_DISABLE_COPY(KPropertyDateTimeEditor)
    class Private;
    Private * const d;
};

class KPROPERTYWIDGETS_EXPORT KPropertyDateTimeDelegate : public KPropertyEditorCreatorInterface,
                                                          public KPropertyValueDisplayInterface
{
public:
    KPropertyDateTimeDelegate();

    QString valueToString(const QVariant &value, const QLocale &locale) const override;

    QWidget *createEditor(int type, QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
};

#endif
