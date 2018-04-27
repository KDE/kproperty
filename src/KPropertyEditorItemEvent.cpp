/* This file is part of the KDE project
   Copyright (C) 2018 Jarosław Staniek <staniek@kde.org>

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

#include "KPropertyEditorItemEvent.h"

class Q_DECL_HIDDEN KPropertyEditorItemEvent::Private
{
public:
    Private()
    {
    }
    const KProperty *property;
    QString name;
    QVariantMap parameters;
    QVariant result;
    bool hasResult = false;
};

KPropertyEditorItemEvent::KPropertyEditorItemEvent(const KProperty &property, const QString &name,
                                                   const QVariantMap &parameters)
    : d(new Private)
{
    d->property = &property;
    d->name = name;
    d->parameters = parameters;
}

KPropertyEditorItemEvent::~KPropertyEditorItemEvent()
{
}

const KProperty *KPropertyEditorItemEvent::property() const
{
    return d->property;
}

QString KPropertyEditorItemEvent::name() const
{
    return d->name;
}

QVariantMap KPropertyEditorItemEvent::parameters() const
{
    return d->parameters;
}

void KPropertyEditorItemEvent::setResult(const QVariant &result)
{
    d->hasResult = true;
    d->result = result;
}

QVariant KPropertyEditorItemEvent::result() const
{
    return d->result;
}

bool KPropertyEditorItemEvent::hasResult() const
{
    return d->hasResult;
}
