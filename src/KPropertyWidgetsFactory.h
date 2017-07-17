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

#include "KProperty.h"
#include "KPropertyFactory.h"
#include "KPropertyUtils.h"

#include <QObject>
#include <QVariant>
#include <QHash>
#include <QLabel>
#include <QPainter>
#include <QStyleOptionViewItem>

/*! @brief Options for altering the editor widget creation process

  @see KPropertyEditorCreatorInterface::createEditor().
  @since 3.1
*/
class KPROPERTYWIDGETS_EXPORT KPropertyEditorCreatorOptions {
public:
    KPropertyEditorCreatorOptions();

    KPropertyEditorCreatorOptions(const KPropertyEditorCreatorOptions &other);

    ~KPropertyEditorCreatorOptions();

    /*! In order to have better look of the widget within the property editor view,
        we are usually hiding borders from the widget (see FactoryManager::createEditor())
        and adding 1 pixel 'gray border' on the top. Default value of bordersVisible is @c false. */
    bool bordersVisible() const;

    void setBordersVisible(bool visible);

    //! Assigns @a other to this KPropertyEditorCreatorOptions
    KPropertyEditorCreatorOptions& operator=(const KPropertyEditorCreatorOptions &other);

    //! @return true if these options have exactly the same values options as @a other
    bool operator==(const KPropertyEditorCreatorOptions &other) const;

    //! @return true if these options differs in at least one value from @a other
    bool operator!=(const KPropertyEditorCreatorOptions &other) const { return !operator==(other); }

private:
    class Private;
    Private * const d;
};

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

    //! Options for editor creating
    //! @since 3.1
    const KPropertyEditorCreatorOptions *options() const;

    //! @overload
    KPropertyEditorCreatorOptions *options();

private:
    Q_DISABLE_COPY(KPropertyEditorCreatorInterface)
    class Private;
    Private * const d;
};

class KPROPERTYWIDGETS_EXPORT KPropertyValuePainterInterface
{
public:
    KPropertyValuePainterInterface();
    virtual ~KPropertyValuePainterInterface();
    virtual void paint( QPainter * painter,
        const QStyleOptionViewItem & option, const QModelIndex & index ) const = 0;

    //! A helper that draws text obtained from index (EditRole data) on painter @a painter.
    //! iface->valueToString() is used to convert value to string.
    //! @since 3.1
    static void paint(const KPropertyValueDisplayInterface *iface, QPainter *painter,
                      const QStyleOptionViewItem &option, const QModelIndex &index);
};

//! Label widget that can be used for displaying text-based read-only items
//! Used in KPropertyLabelCreator.
class KPROPERTYWIDGETS_EXPORT KPropertyLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)
public:
    KPropertyLabel(QWidget *parent, const KProperty *property, const KPropertyValueDisplayInterface *iface);
    QVariant value() const;
Q_SIGNALS:
    void commitData( QWidget * editor );
public Q_SLOTS:
    void setValue(const QVariant& value);

protected:
    void paintEvent( QPaintEvent * event ) override;

private:
    const KProperty *m_property;
    const KPropertyValueDisplayInterface *m_iface;
    QVariant m_value;
};

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
        KProperty *prop = KPropertyUtils::propertyForIndex(index);
        return new Widget(parent, prop, this);
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        KPropertyValuePainterInterface::paint(this, painter, option, index);
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

    Q_DISABLE_COPY(KPropertyWidgetsFactory)
    class Private;
    Private * const d;
};

#endif
