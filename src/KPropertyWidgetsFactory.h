/* This file is part of the KDE project
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

#ifndef KPROPERTYWIDGETS_FACTORY_H
#define KPROPERTYWIDGETS_FACTORY_H

#include "kpropertywidgets_export.h"
#include "KProperty.h"
#include "KPropertyFactory.h"

#include <QObject>
#include <QVariant>
#include <QHash>
#include <QLabel>
#include <QPainter>
#include <QStyleOptionViewItem>

//! An interface for editor widget creators.
/*! Options can be set in the options attribute in order to customize
    widget creation process. Do this in the EditorCreatorInterface constructor.
*/
class KPROPERTYWIDGETS_EXPORT KPropertyEditorCreatorInterface
{
public:
    KPropertyEditorCreatorInterface();

    virtual ~KPropertyEditorCreatorInterface();

    virtual QWidget * createEditor(int type, QWidget *parent,
        const QStyleOptionViewItem &option, const QModelIndex &index) const;

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

class KPROPERTYWIDGETS_EXPORT KPropertyValuePainterInterface
{
public:
    KPropertyValuePainterInterface();
    virtual ~KPropertyValuePainterInterface();
    virtual void paint( QPainter * painter,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const = 0;
};

//! Label widget that can be used for displaying text-based read-only items
//! Used in KPropertyLabelCreator.
class KPROPERTYWIDGETS_EXPORT KPropertyLabel : public QLabel
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
    void paintEvent( QPaintEvent * event ) override;

private:
    const KPropertyValueDisplayInterface *m_iface;
    QVariant m_value;
};

KPROPERTYWIDGETS_EXPORT void paintInternal(const KPropertyValueDisplayInterface *iface, QPainter *painter,
                                           const QStyleOptionViewItem & option, const QModelIndex & index);

//! Creator returning editor
template<class Widget>
class KPROPERTYWIDGETS_EXPORT KPropertyEditorCreator : public KPropertyEditorCreatorInterface,
                                        public KPropertyValueDisplayInterface,
                                        public KPropertyValuePainterInterface
{
public:
    KPropertyEditorCreator() : KPropertyEditorCreatorInterface() {}

    ~KPropertyEditorCreator() override {}

    QWidget *createEditor(int type, QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override
    {
        Q_UNUSED(type);
        Q_UNUSED(option);
        Q_UNUSED(index);
        return new Widget(parent, this);
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        paintInternal(this, painter, option, index);
    }
};

typedef KPropertyEditorCreator<KPropertyLabel> KPropertyLabelCreator;


class KPROPERTYWIDGETS_EXPORT KPropertyWidgetsFactory : public KPropertyFactory
{
public:
    KPropertyWidgetsFactory();
    ~KPropertyWidgetsFactory() override;

    QHash<int, KPropertyEditorCreatorInterface*> editorCreators() const;
    QHash<int, KPropertyValuePainterInterface*> valuePainters() const;

    //! Adds editor creator @a creator for type @a type.
    //! The creator becomes owned by the factory.
    void addEditor(int type, KPropertyEditorCreatorInterface *creator);

    void addPainter(int type, KPropertyValuePainterInterface *painter);

    static void paintTopGridLine(QWidget *widget);
    static void setTopAndBottomBordersUsingStyleSheet(QWidget *widget,
                                              const QString& extraStyleSheet = QString());

protected:
    void addEditorInternal(int type, KPropertyEditorCreatorInterface *editor, bool own = true);

    //! Adds value painter @a painter for type @a type.
    //! The painter becomes owned by the factory.
    void addPainterInternal(int type, KPropertyValuePainterInterface *painter, bool own = true);

    class Private;
    Private * const d;
};

#endif
