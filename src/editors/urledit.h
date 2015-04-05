/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>

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

#ifndef KPROPERTY_URLEDIT_H
#define KPROPERTY_URLEDIT_H

#include "Factory.h"

class KUrlRequester;

namespace KoProperty
{

class KPROPERTY_EXPORT URLEdit : public Widget
{
    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)
    Q_OBJECT

public:
    explicit URLEdit(Property *property, QWidget *parent = 0);
    virtual ~URLEdit();

    virtual QVariant value() const;

    virtual void setProperty(Property *property);

Q_SIGNALS:
    void commitData( QWidget * editor );

public Q_SLOTS:
    virtual void setValue(const QVariant &value);

protected Q_SLOTS:
    void slotValueChanged(const QString &url);

protected:
    virtual void setReadOnlyInternal(bool readOnly);

private:
    KUrlRequester *m_edit;
};

}

#endif
