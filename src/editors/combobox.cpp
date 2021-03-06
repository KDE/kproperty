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
#include "KPropertyListData.h"
#include "KPropertyUtils.h"
#include "KPropertyUtils_p.h"
#include "KPropertyWidgetsFactory.h"
#include "kproperty_debug.h"

#include <QCompleter>
#include <QGuiApplication>

KPropertyComboBoxEditorOptions::KPropertyComboBoxEditorOptions()
 : extraValueAllowed(false)
{
}

KPropertyComboBoxEditorOptions::KPropertyComboBoxEditorOptions(
    const KPropertyComboBoxEditorOptions &other)
{
    *this = other;
}

KPropertyComboBoxEditorOptions::~KPropertyComboBoxEditorOptions()
{
    delete iconProvider;
}

KPropertyComboBoxEditorOptions& KPropertyComboBoxEditorOptions::operator=(const KPropertyComboBoxEditorOptions &other)
{
    if (this != &other) {
        if (other.iconProvider) {
            iconProvider = other.iconProvider->clone();
        } else {
            delete iconProvider;
            iconProvider = nullptr;
        }
        extraValueAllowed = other.extraValueAllowed;
    }
    return *this;
}


class Q_DECL_HIDDEN KPropertyComboBoxEditor::Private
{
public:
    Private()
    {
    }
    ~Private()
    {
        delete completer;
    }
    KPropertyListData listData;
    bool setValueEnabled = true;
    KPropertyComboBoxEditorOptions options;
    QCompleter *completer = nullptr;
};

KPropertyComboBoxEditor::KPropertyComboBoxEditor(const KPropertyListData &listData,
                                                 const KPropertyComboBoxEditorOptions &options,
                                                 QWidget *parent)
    : QComboBox(parent), d(new Private)
{
    d->options = options;
    setEditable(d->options.extraValueAllowed);
    setInsertPolicy(QComboBox::NoInsert);
    setAutoCompletion(true);
    setContextMenuPolicy(Qt::NoContextMenu);
    setListData(listData);
    connect(this, SIGNAL(activated(int)), this, SLOT(slotValueChanged(int)));

    int paddingTop = 2;
    int paddingLeft = 3;
    const QString style(parent->style()->objectName());
    if (!KPropertyUtilsPrivate::gridLineColor(this).isValid()) {
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
    delete d;
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
    return !d->listData.keys().isEmpty();
}

QVariant KPropertyComboBoxEditor::value() const
{
    if (!listDataKeysAvailable())
        return QVariant();

    const int idx = currentIndex();
    if (idx < 0 || idx >= d->listData.keys().count()
        || d->listData.names()[idx].toString() != currentText().trimmed())
    {
        if (!d->options.extraValueAllowed || currentText().isEmpty())
            return QVariant();
        return currentText().trimmed(); //trimmed 4 safety
    }
    return d->listData.keys()[idx];
}

void KPropertyComboBoxEditor::setValue(const QVariant &value)
{
    if (!d->setValueEnabled)
        return;
    const int idx = d->listData.keys().isEmpty() ? -1 : d->listData.keys().indexOf(value);
//    kprDebug() << "**********" << idx << "" << value.toString();
    if (idx >= 0 && idx < count()) {
        setCurrentIndex(idx);
    }
    else {
        if (idx < 0) {
            if (d->options.extraValueAllowed) {
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
}

void KPropertyComboBoxEditor::fillValues()
{
    delete d->completer;
    clear();
    if (!listDataKeysAvailable())
        return;

    int index = 0;
    for (const QString &itemName : d->listData.namesAsStringList()) {
        addItem(itemName);
        if (d->options.iconProvider) {
            QIcon icon = d->options.iconProvider->icon(index);
            setItemIcon(index, icon);
        }
        index++;
    }
    if (isEditable()) {
        d->completer = new QCompleter(d->listData.namesAsStringList());
        d->completer->setWidget(this);
    }
}

void KPropertyComboBoxEditor::setListData(const KPropertyListData & listData)
{
    d->listData = listData;
    fillValues();
}

void KPropertyComboBoxEditor::slotValueChanged(int)
{
    emit commitData( this );
}

void KPropertyComboBoxEditor::paintEvent( QPaintEvent * event )
{
    QComboBox::paintEvent(event);
    KPropertyWidgetsFactory::paintTopGridLine(this);
}

//-----------------------

KPropertyComboBoxDelegate::KPropertyComboBoxDelegate()
{
    options()->setBordersVisible(true);
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
    const int idx = listData->keys().indexOf(property->value());
    //kprDebug() << "idx==" << idx;
    if (idx == -1) {
      if (!property->option("extraValueAllowed").toBool())
        return QString();
      else
        return property->value().toString();
    }
    return property->listData()->names()[idx].toString();
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
        return nullptr;
    }
    KPropertyComboBoxEditorOptions options;
    options.extraValueAllowed = property->option("extraValueAllowed", false).toBool();
    KPropertyComboBoxEditor *cb = new KPropertyComboBoxEditor(*property->listData(), options, parent);
    return cb;
}
