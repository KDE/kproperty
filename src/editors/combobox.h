/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
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

#ifndef KPROPERTY_COMBOBOX_H
#define KPROPERTY_COMBOBOX_H

#include "KPropertyFactory.h"

#include <QComboBox>

class KPROPERTY_EXPORT KPropertyComboBoxEditor : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)

public:
    class Options {
    public:
        class IconProviderInterface {
        public:
            IconProviderInterface() {}
            virtual ~IconProviderInterface() {}
            virtual QIcon icon(int index) const = 0;
            virtual IconProviderInterface* clone() const = 0;
        };
        Options();
        Options(const Options& other);
        ~Options();

        IconProviderInterface *iconProvider;
        bool extraValueAllowed;
    };

//    explicit ComboBox(const Property* property, QWidget *parent = 0);
    KPropertyComboBoxEditor(const KPropertyListData& listData, const Options& options,
             QWidget *parent = 0);

    virtual ~KPropertyComboBoxEditor();

    virtual QVariant value() const;

//    virtual void setProperty(const Property *property);
    void setListData(const KPropertyListData & listData);

Q_SIGNALS:
    void commitData( QWidget * editor );

public Q_SLOTS:
    virtual void setValue(const QVariant &value);

protected Q_SLOTS:
    void slotValueChanged(int value);

protected:
    virtual void paintEvent( QPaintEvent * event );
//    virtual void setReadOnlyInternal(bool readOnly);

    QString keyForValue(const QVariant &value);

    void fillValues();

    bool listDataKeysAvailable() const;

//    KComboBox *m_edit;
//    const Property *m_property;
    KPropertyListData m_listData;
//    QList<QVariant> keys;
    bool m_setValueEnabled;
    Options m_options;
};

class KPROPERTY_EXPORT KPropertyComboBoxDelegate : public KPropertyEditorCreatorInterface,
                                                   public KPropertyValueDisplayInterface
{
public:
    KPropertyComboBoxDelegate();

    virtual QString displayTextForProperty( const KProperty* property ) const;

    virtual QWidget * createEditor( int type, QWidget *parent,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const;

//    virtual void paint( QPainter * painter,
//        const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

#endif
