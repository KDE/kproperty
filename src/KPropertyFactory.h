/* This file is part of the KDE project
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

#ifndef KPROPERTY_FACTORY_H
#define KPROPERTY_FACTORY_H

#include "kproperty_export.h"
#include "KProperty.h"
#include <QObject>
#include <QVariant>
#include <QHash>
#include <QLabel>
#include <QPainter>
#include <QStyleOptionViewItem>

class QStyleOptionViewItem;
class QModelIndex;

//! An interface for for composed property handlers
/*! You have to subclass KComposedPropertyInterface to override the behaviour of a property type.\n
  In the constructor, you should create the child properties (if needed).
  Then, you need to implement the functions concerning values.\n

  Example implementation of composed properties can be found in editors/ directory.
*/
class KPROPERTY_EXPORT KComposedPropertyInterface
{
public:
    explicit KComposedPropertyInterface(KProperty *parent);
    virtual ~KComposedPropertyInterface();

    /*! This function modifies the child properties for parent value @a value.
     It is called by @ref Property::setValue() when
     the property is composed.
    You don't have to modify the property value, it is done by Property class.
    Note that when calling Property::setValue, you <b>need</b> to set
    useComposedProperty (the third parameter) to false, or there will be infinite recursion. */
    virtual void setValue(KProperty *property, const QVariant &value, bool rememberOldValue) = 0;

    void childValueChangedInternal(KProperty *child, const QVariant &value, bool rememberOldValue) {
      if (m_childValueChangedEnabled)
        childValueChanged(child, value, rememberOldValue);
    }

    void setChildValueChangedEnabled(bool set) { m_childValueChangedEnabled = set; }

protected:
    virtual void childValueChanged(KProperty *child, const QVariant &value, bool rememberOldValue) = 0;

    /*! This method emits the \a Set::propertyChanged() signal for all
    sets our parent-property is registered in. */
    void emitPropertyChanged();
    bool m_childValueChangedEnabled : 1;
};

class KPROPERTY_EXPORT KComposedPropertyCreatorInterface
{
public:
    KComposedPropertyCreatorInterface();

    virtual ~KComposedPropertyCreatorInterface();

    virtual KComposedPropertyInterface* createComposedProperty(KProperty *parent) const = 0;
};

//! An interface for editor widget creators.
/*! Options can be set in the options attribute in order to customize
    widget creation process. Do this in the EditorCreatorInterface constructor.
*/
class KPROPERTY_EXPORT KPropertyEditorCreatorInterface
{
public:
    KPropertyEditorCreatorInterface();

    virtual ~KPropertyEditorCreatorInterface();

    virtual QWidget * createEditor( int type, QWidget *parent,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const = 0;

    /*! Options for altering the editor widget creation process,
        used by KPropertyFactoryManager::createEditor(). */
    class Options {
    public:
        Options();
        /*! In order to have better look of the widget within the property editor view,
            we usually remove borders from the widget (see FactoryManager::createEditor()).
            and adding 1 pixel 'gray border' on the top. Default value is true. */
        bool removeBorders;
    };

    //! Options for altering the editor widget creation process
    Options options;
};

class KPROPERTY_EXPORT KPropertyValuePainterInterface
{
public:
    KPropertyValuePainterInterface();
    virtual ~KPropertyValuePainterInterface();
    virtual void paint( QPainter * painter,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const = 0;
};

class KPROPERTY_EXPORT KPropertyValueDisplayInterface
{
public:
    KPropertyValueDisplayInterface();
    virtual ~KPropertyValueDisplayInterface();
    virtual QString displayTextForProperty( const KProperty* property ) const
        { return displayText(property->value()); }
    virtual QString displayText( const QVariant& value ) const
        { return value.toString(); }
};

//! Label widget that can be used for displaying text-based read-only items
//! Used in LabelCreator.
class KPROPERTY_EXPORT KPropertyLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)
public:
    KPropertyLabel(QWidget *parent, const KPropertyValueDisplayInterface *iface);
    QVariant value() const;
Q_SIGNALS:
    void commitData( QWidget * editor );
public Q_SLOTS:
    void setValue(const QVariant& value);

protected:
    virtual void paintEvent( QPaintEvent * event );

private:
    const KPropertyValueDisplayInterface *m_iface;
    QVariant m_value;
};

//! Creator returning editor
template<class Widget>
class KPROPERTY_EXPORT KPropertyEditorCreator : public KPropertyEditorCreatorInterface,
                                        public KPropertyValueDisplayInterface,
                                        public KPropertyValuePainterInterface
{
public:
    KPropertyEditorCreator() : KPropertyEditorCreatorInterface() {}

    virtual ~KPropertyEditorCreator() {}

    virtual QWidget * createEditor( int type, QWidget *parent,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        Q_UNUSED(type);
        Q_UNUSED(option);
        Q_UNUSED(index);
        return new Widget(parent, this);
    }

    virtual void paint( QPainter * painter,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        painter->save();
        QRect r(option.rect);
        r.setLeft(r.left()+1);
        painter->drawText( r, Qt::AlignLeft | Qt::AlignVCenter,
            displayText( index.data(Qt::EditRole) ) );
        painter->restore();
    }
};

typedef KPropertyEditorCreator<KPropertyLabel> KPropertyLabelCreator;

//! Creator returning composed property object
template<class ComposedProperty>
class KPROPERTY_EXPORT KComposedPropertyCreator : public KComposedPropertyCreatorInterface
{
public:
    KComposedPropertyCreator() : KComposedPropertyCreatorInterface() {}

    virtual ~KComposedPropertyCreator() {}

    virtual ComposedProperty* createComposedProperty(KProperty *parent) const {
        return new ComposedProperty(parent);
    }
};

class KPROPERTY_EXPORT KPropertyFactory
{
public:
    KPropertyFactory();
    virtual ~KPropertyFactory();
    QHash<int, KComposedPropertyCreatorInterface*> composedPropertyCreators() const;
    QHash<int, KPropertyEditorCreatorInterface*> editorCreators() const;
    QHash<int, KPropertyValuePainterInterface*> valuePainters() const;
    QHash<int, KPropertyValueDisplayInterface*> valueDisplays() const;

    //! Adds editor creator @a creator for type @a type.
    //! The creator becomes owned by the factory.
    void addEditor(int type, KPropertyEditorCreatorInterface *creator);

    void addComposedPropertyCreator( int type, KComposedPropertyCreatorInterface* creator );

    void addPainter(int type, KPropertyValuePainterInterface *painter);

    void addDisplay(int type, KPropertyValueDisplayInterface *display);

    static void paintTopGridLine(QWidget *widget);
    static void setTopAndBottomBordersUsingStyleSheet(QWidget *widget, QWidget* parent,
        const QString& extraStyleSheet = QString());

protected:
    void addEditorInternal(int type, KPropertyEditorCreatorInterface *editor, bool own = true);

    void addComposedPropertyCreatorInternal(int type,
        KComposedPropertyCreatorInterface* creator, bool own = true);

    //! Adds value painter @a painter for type @a type.
    //! The painter becomes owned by the factory.
    void addPainterInternal(int type, KPropertyValuePainterInterface *painter, bool own = true);

    //! Adds value-to-text converted @a painter for type @a type.
    //! The converter becomes owned by the factory.
    void addDisplayInternal(int type, KPropertyValueDisplayInterface *display, bool own = true);

    class Private;
    Private * const d;
};

class KProperty;
class KCustomProperty;

class KPROPERTY_EXPORT KPropertyFactoryManager : public QObject
{
    Q_OBJECT
public:
    bool isEditorForTypeAvailable( int type ) const;

    QWidget * createEditor(
        int type,
        QWidget *parent,
        const QStyleOptionViewItem & option,
        const QModelIndex & index ) const;

    bool paint( int type,
        QPainter * painter,
        const QStyleOptionViewItem & option,
        const QModelIndex & index ) const;

    KComposedPropertyInterface* createComposedProperty(KProperty *parent);

    bool canConvertValueToText( int type ) const;

    bool canConvertValueToText( const KProperty* property ) const;

    QString convertValueToText( const KProperty* property ) const;

    //! Registers factory @a factory. It becomes owned by the manager.
    void registerFactory(KPropertyFactory *factory);

    KCustomProperty* createCustomProperty( KProperty *parent );

    /*! \return a pointer to a factory manager instance.*/
    static KPropertyFactoryManager* self();

    KPropertyFactoryManager();
    ~KPropertyFactoryManager();
private:
    
    class Private;
    Private * const d;
};

#endif
