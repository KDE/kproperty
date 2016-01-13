/* This file is part of the KDE project
   Copyright (C) 2008-2016 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTYWIDGETSPLUGINMANAGER_H
#define KPROPERTYWIDGETSPLUGINMANAGER_H

#include "kpropertywidgets_export.h"
#include <QObject>

class QModelIndex;
class QPainter;
class QStyleOptionViewItem;
class QWidget;
class KProperty;
class KPropertyWidgetsFactory;
class KCustomProperty;

class KPROPERTYWIDGETS_EXPORT KPropertyWidgetsPluginManager : public QObject
{
    Q_OBJECT

public:
    //! @return a pointer to the instance of plugin manager
    static KPropertyWidgetsPluginManager* self();

    bool isEditorForTypeAvailable(int type) const;

    QWidget* createEditor(int type, QWidget *parent, const QStyleOptionViewItem & option,
                          const QModelIndex &index) const;

    bool paint(int type, QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    //! Registers factory @a factory. It becomes owned by the manager.
    void registerFactory(KPropertyWidgetsFactory *factory);

private:
    // to get access to the constructor
    friend class KPropertyWidgetsPluginManagerSingleton;

    KPropertyWidgetsPluginManager();

    ~KPropertyWidgetsPluginManager();

    Q_DISABLE_COPY(KPropertyWidgetsPluginManager)
    class Private;
    Private * const d;
};

#endif
