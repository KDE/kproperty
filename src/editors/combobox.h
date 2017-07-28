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

#ifndef KPROPERTY_COMBOBOX_H
#define KPROPERTY_COMBOBOX_H

#include "KPropertyWidgetsFactory.h"

#include <QComboBox>

class KPROPERTYWIDGETS_EXPORT KPropertyComboBoxEditorIconProviderInterface
{
public:
    KPropertyComboBoxEditorIconProviderInterface() {}
    virtual ~KPropertyComboBoxEditorIconProviderInterface() {}
    virtual QIcon icon(int index) const = 0;
    virtual KPropertyComboBoxEditorIconProviderInterface* clone() const = 0;
};

class KPROPERTYWIDGETS_EXPORT KPropertyComboBoxEditorOptions
{
public:
    KPropertyComboBoxEditorOptions();
    KPropertyComboBoxEditorOptions(const KPropertyComboBoxEditorOptions& other);
    ~KPropertyComboBoxEditorOptions();

    KPropertyComboBoxEditorOptions& operator=(const KPropertyComboBoxEditorOptions &other);

    KPropertyComboBoxEditorIconProviderInterface *iconProvider;
    bool extraValueAllowed;
};

class KPROPERTYWIDGETS_EXPORT KPropertyComboBoxEditor : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)

public:
    KPropertyComboBoxEditor(const KPropertyListData &listData,
                            const KPropertyComboBoxEditorOptions &options,
                            QWidget *parent = nullptr);

    ~KPropertyComboBoxEditor() override;

    virtual QVariant value() const;

    static QString borderSheet(const QWidget *widget);

Q_SIGNALS:
    void commitData( QWidget * editor );

public Q_SLOTS:
    void setListData(const KPropertyListData & listData);

    virtual void setValue(const QVariant &value);

protected Q_SLOTS:
    void slotValueChanged(int value);

protected:
    void paintEvent(QPaintEvent *event) override;

    QString keyForValue(const QVariant &value);

    void fillValues();

    bool listDataKeysAvailable() const;

    Q_DISABLE_COPY(KPropertyComboBoxEditor)
    class Private;
    Private * const d;
};

class KPROPERTYWIDGETS_EXPORT KPropertyComboBoxDelegate : public KPropertyEditorCreatorInterface,
                                                   public KPropertyValueDisplayInterface
{
public:
    KPropertyComboBoxDelegate();

    QString propertyValueToString(const KProperty *property, const QLocale &locale) const override;

    QString valueToString(const QVariant &value, const QLocale &locale) const override;

    QWidget *createEditor(int type, QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
};

#endif
