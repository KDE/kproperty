/* This file is part of the KDE project
   Copyright (C) 2009-2017 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTY_PROPERTY_P_H
#define KPROPERTY_PROPERTY_P_H

#include "KPropertySet.h"
#include <QPointer>

//! Default value for "step" option. Used for spin boxes, etc.
#define KPROPERTY_DEFAULT_DOUBLE_VALUE_STEP 0.01

//! @internal
class Q_DECL_HIDDEN KProperty::Private
{
public:
    explicit Private(KProperty *prop);

    void setCaptionForDisplaying(const QString& captionForDisplaying);

    ~Private();

    //! @return a value for option @a name or null value if there is no such option set.
    inline QVariant option(const char* name, const QVariant& defaultValue) const
    {
        if (options.contains(name))
            return options[name];
        return parent ? parent->option(name, defaultValue) : defaultValue;
    }

    //! @return true if value of this property differs from @a otherValue
    bool valueDiffersInternal(const QVariant &otherValue, KProperty::ValueOptions options);

    //! Sets value of the property to @a newValue
    bool setValueInternal(const QVariant &newValue, KProperty::ValueOptions valueOptions);

    /*! Adds @a prop as a child of this property.
     The children will be owned by this property. */
    void addChild(KProperty *prop);

    /*! Adds @a set to this property. */
    void addSet(KPropertySet *newSet);

    /*! Adds related property for this property. */
    void addRelatedProperty(KProperty *property);

    /*! This method emits the @a KPropertySet::propertyChanged() signal.
    KProperty::setValue() calls this method if the value has been changed. */
    void emitPropertyChanged();

    KProperty * const q;
    int type;
    QByteArray name;
    QString captionForDisplaying;
    QString* caption;
    QString description;
    QVariant value;
    QVariant oldValue;
    /*! The string-to-value correspondence list of the property.*/
    KPropertyListData* listData;
    QString iconName;

    bool changed;
    bool storable;
    bool readOnly;
    bool visible;
    KProperty::ValueSyncPolicy valueSyncPolicy = KProperty::ValueSyncPolicy::Editor;
    QMap<QByteArray, QVariant> options;

    KComposedPropertyInterface *composed;
    //! Flag used to allow composed property to use setValue() without causing recursion
    bool useComposedProperty;

    //! Used when a single set is assigned for the property
    QPointer<KPropertySet> set;
    //! Used when multiple sets are assigned for the property
    QList< QPointer<KPropertySet> > *sets;

    KProperty  *parent;
    QList<KProperty*>  *children;
    //! List of properties with the same name (when intersecting buffers)
    QList<KProperty*>  *relatedProperties;
};

#endif
