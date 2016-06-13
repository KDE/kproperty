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

#include "combobox.h"
#include "KPropertyEditorView.h"
#include "kproperty_debug.h"
#include "KPropertyWidgetsFactory.h"
#include "KPropertyUtils.h"
#include "KPropertyUtils_p.h"

#include <QCompleter>
#include <QGuiApplication>

KPropertyComboBoxEditor::Options::Options()
 : iconProvider(0)
 , extraValueAllowed(false)
{
}

KPropertyComboBoxEditor::Options::Options(const KPropertyComboBoxEditor::Options& other)
{
    *this = other;
    if (other.iconProvider)
        iconProvider = other.iconProvider->clone();
}

KPropertyComboBoxEditor::Options::~Options()
{
    delete iconProvider;
}

KPropertyComboBoxEditor::KPropertyComboBoxEditor(const KPropertyListData& listData, const Options& options, QWidget *parent)
        : QComboBox(parent)
        , m_setValueEnabled(true)
        , m_options(options)
{
//    QHBoxLayout *l = new QHBoxLayout(this);
//    l->setMargin(0);
//    l->setSpacing(0);
//    m_edit = new KComboBox(this);
//    m_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    m_edit->setMinimumHeight(5);
    //setPlainWidgetStyle(m_edit);

//    l->addWidget(m_edit);

    setEditable( m_options.extraValueAllowed );
    setInsertPolicy(QComboBox::NoInsert);
//    m_edit->setMinimumSize(10, 0); // to allow the combo to be resized to a small size
    setAutoCompletion(true);
    setContextMenuPolicy(Qt::NoContextMenu);

//    if (listData)
    setListData(listData);
//    if (property->listData()) {
  //      fillValues(property);
    //}
//not needed for combo setLeavesTheSpaceForRevertButton(true);

//    setFocusWidget(m_edit);
    connect(this, SIGNAL(activated(int)), this, SLOT(slotValueChanged(int)));

    int paddingTop = 2;
    int paddingLeft = 3;
    const QString style(parent->style()->objectName());
    if (!KPropertyUtils::gridLineColor(this).isValid()) {
        setFrame(false);
        paddingTop = 0;
    }
    if (style == QLatin1String("windowsvista") || style == QLatin1String("fusion")) {
        paddingLeft = 2;
    }

    //Set the stylesheet to a plain style
    QString styleSheet = QString::fromLatin1("QComboBox { \
        %1 \
        padding-top: %2px; padding-left: %3px; }").arg(borderSheet(this)).arg(paddingTop).arg(paddingLeft);
    setStyleSheet(styleSheet);
}

KPropertyComboBoxEditor::~KPropertyComboBoxEditor()
{
}

//static
QString KPropertyComboBoxEditor::borderSheet(const QWidget *widget)
{
    Q_ASSERT(widget);
    const QString style(widget->parentWidget() ? widget->parentWidget()->style()->objectName() : QString());
    if (style == QLatin1String("windowsvista") // a hack
        || style == QLatin1String("fusion"))
    {
        return QString();
    }
    return QLatin1String("border: 0px; ");
}

bool KPropertyComboBoxEditor::listDataKeysAvailable() const
{
    if (m_listData.keys.isEmpty()) {
        kprWarning() << "property listData not available!";
        return false;
    }
    return true;
}

QVariant KPropertyComboBoxEditor::value() const
{
    if (!listDataKeysAvailable())
        return QVariant();

    const int idx = currentIndex();
    if (idx < 0 || idx >= (int)m_listData.keys.count() || m_listData.names[idx] != currentText().trimmed()) {
        if (!m_options.extraValueAllowed || currentText().isEmpty())
            return QVariant();
        return QVariant(currentText().trimmed());//trimmed 4 safety
    }
    return QVariant(m_listData.keys[idx]);
}

void KPropertyComboBoxEditor::setValue(const QVariant &value)
{
    if (!listDataKeysAvailable())
        return;

    if (!m_setValueEnabled)
        return;
    int idx = m_listData.keys.indexOf(value/*.toString()TODO*/);
//    kprDebug() << "**********" << idx << "" << value.toString();
    if (idx >= 0 && idx < count()) {
        setCurrentIndex(idx);
    }
    else {
        if (idx < 0) {
            if (m_options.extraValueAllowed) {
                setCurrentIndex(-1);
                setEditText(value.toString());
            }
            kprWarning() << "NO SUCH KEY:" << value.toString()
                << "property=" << objectName();
        } else {
            QStringList list;
            for (int i = 0; i < count(); i++)
                list += itemText(i);
            kprWarning() << "NO SUCH INDEX WITHIN COMBOBOX:" << idx
                << "count=" << count() << "value=" << value.toString()
                << "property=" << objectName() << "\nActual combobox contents"
                << list;
        }
        setItemText(currentIndex(), QString());
    }

    if (value.isNull())
        return;

//??    if (emitChange)
//??        emit valueChanged(this);
}

void KPropertyComboBoxEditor::fillValues()
{
    clear();
    //m_edit->clearContents();

//    if (!m_property)
//        return;
    if (!listDataKeysAvailable())
        return;

//    m_keys = m_property->listData()->keys;
    int index = 0;
    foreach( const QString& itemName, m_listData.names ) {
        addItem(itemName);
        if (m_options.iconProvider) {
            QIcon icon = m_options.iconProvider->icon(index);
            setItemIcon(index, icon);
        }
        index++;
    }
    QCompleter *comp = new QCompleter(m_listData.names);
    comp->setWidget(this);
}

/*
void ComboBox::setProperty( const Property *property )
{
//    const bool b = (property() == prop);
//    m_setValueEnabled = false; //setValue() couldn't be called before fillBox()
//    Widget::setProperty(prop);
//    m_setValueEnabled = true;
//    if (!b)
//    m_property = property;
    m_listData = *property->listData();
    fillValues();
//    if (prop)
//        setValue(prop->value(), false); //now the value can be set
}*/

void KPropertyComboBoxEditor::setListData(const KPropertyListData & listData)
{
    m_listData = listData;
    fillValues();
}

void KPropertyComboBoxEditor::slotValueChanged(int)
{
//    emit valueChanged(this);
    emit commitData( this );
}

void KPropertyComboBoxEditor::paintEvent( QPaintEvent * event )
{
    QComboBox::paintEvent(event);
    KPropertyWidgetsFactory::paintTopGridLine(this);
}

/*
void ComboBox::setReadOnlyInternal(bool readOnly)
{
    setVisibleFlag(!readOnly);
}*/


/*QString
ComboBox::keyForValue(const QVariant &value)
{
  const QMap<QString, QVariant> *list = property()->valueList();
  Property::ListData *list = property()->listData();

  if (!list)
    return QString();
  int idx = listData->keys.findIndex( value );


  QMap<QString, QVariant>::ConstIterator endIt = list->constEnd();
  for(QMap<QString, QVariant>::ConstIterator it = list->constBegin(); it != endIt; ++it) {
    if(it.data() == value)
      return it.key();
  }
  return QString();
}*/

//-----------------------

KPropertyComboBoxDelegate::KPropertyComboBoxDelegate()
{
    options.removeBorders = false;
}

QString KPropertyComboBoxDelegate::propertyValueToString(const KProperty* property, const QLocale &locale) const
{
    Q_UNUSED(locale)
    KPropertyListData *listData = property->listData();
    if (!listData)
        return property->value().toString();
    if (property->value().isNull())
        return QString();
    //kprDebug() << "property->value()==" << property->value();
    const int idx = listData->keys.indexOf( property->value() );
    //kprDebug() << "idx==" << idx;
    if (idx == -1) {
      if (!property->option("extraValueAllowed").toBool())
        return QString();
      else
        return property->value().toString();
    }
    return property->listData()->names[ idx ];
}

QString KPropertyComboBoxDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    Q_UNUSED(locale)
    return value.toString();
}

QWidget* KPropertyComboBoxDelegate::createEditor( int type, QWidget *parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Q_UNUSED(type);
    Q_UNUSED(option);

    KProperty *property = KPropertyUtils::propertyForIndex(index);
    if (!property) {
        return 0;
    }
    KPropertyComboBoxEditor::Options options;
    options.extraValueAllowed = property->option("extraValueAllowed", false).toBool();
    KPropertyComboBoxEditor *cb = new KPropertyComboBoxEditor(*property->listData(), options, parent);
    return cb;
}

/*void ComboBoxDelegate::paint( QPainter * painter,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
}*/
