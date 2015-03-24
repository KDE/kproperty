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
#include "combobox.h"

#include <QLayout>
#include <QMap>
#include <QVariant>
#include <QPainter>
#include <QHBoxLayout>

#include <kcombobox.h>
#include <kdebug.h>

#include "property.h"

using namespace KoProperty;

ComboBox::ComboBox(Property *property, QWidget *parent)
        : Widget(property, parent)
        , m_setValueEnabled(true)
{
    QHBoxLayout *l = new QHBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);
    m_edit = new KComboBox(this);
    m_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_edit->setMinimumHeight(5);
    //setPlainWidgetStyle(m_edit);

    l->addWidget(m_edit);
    m_extraValueAllowed = false;

    if (property->option("extraValueAllowed").toBool()) {
        m_extraValueAllowed = true;
    }

    if (m_extraValueAllowed) {
        m_edit->setEditable(true);
    } else {
        m_edit->setEditable(false);
    }

    m_edit->setInsertPolicy(QComboBox::NoInsert);
    m_edit->setMinimumSize(10, 0); // to allow the combo to be resized to a small size
    m_edit->setAutoCompletion(true);
    m_edit->setContextMenuPolicy(Qt::NoContextMenu);

    if (this->property()->listData()) {
        fillBox();
    }
//not needed for combo setLeavesTheSpaceForRevertButton(true);

    setFocusWidget(m_edit);
    connect(m_edit, SIGNAL(activated(int)), this, SLOT(slotValueChanged(int)));
}

ComboBox::~ComboBox()
{
}

QVariant
ComboBox::value() const
{
    if (!property()->listData()) {
        kopropertywarn << "ComboBox::value(): propery listData not available!" << endl;
        return QVariant();
    }
    const int idx = m_edit->currentIndex();
    kDebug(30007) << "**********" << idx;
    if (idx < 0 || idx >= (int)property()->listData()->keys.count() || property()->listData()->names[idx] != m_edit->currentText().trimmed()) {
        if (!m_extraValueAllowed || m_edit->currentText().isEmpty())
            return QVariant();
        return QVariant(m_edit->currentText().trimmed());//trimmed 4 safety
    }
    return QVariant(property()->listData()->keys[idx]);
// if(property()->listData() && property()->listData()->contains(m_edit->currentText()))
//  return (*(property()->valueList()))[m_edit->currentText()];
// return QVariant();
}

void
ComboBox::setValue(const QVariant &value, bool emitChange)
{
    if (!property() || !property()->listData()) {
        kopropertywarn << "ComboBox::value(): propery listData not available!" << endl;
        return;
    }
    if (!m_setValueEnabled)
        return;
    int idx = property()->listData()->keys.indexOf(value.toString());
    kDebug(30007) << "**********" << idx << "" << value.toString();
    if (idx >= 0 && idx < m_edit->count()) {
        m_edit->setCurrentIndex(idx);
    } else {
        if (idx < 0) {
            if (m_extraValueAllowed) {
                m_edit->setCurrentIndex(-1);
                m_edit->setEditText(value.toString());
            }
            kopropertywarn << "ComboBox::setValue(): NO SUCH KEY '" << value.toString()
            << "' (property '" << property()->name() << "')" << endl;
        } else {
            QStringList list;
            for (int i = 0; i < m_edit->count(); i++)
                list += m_edit->itemText(i);
            kopropertywarn << "ComboBox::setValue(): NO SUCH INDEX WITHIN COMBOBOX: " << idx
            << " count=" << m_edit->count() << " value='" << value.toString()
            << "' (property '" << property()->name() << "')\nActual combobox contents: "
            << list << endl;
        }
        m_edit->setItemText(m_edit->currentIndex(), QString::null);
    }

    if (value.isNull())
        return;

// m_edit->blockSignals(true);
// m_edit->setCurrentText(keyForValue(value));
// m_edit->blockSignals(false);
    if (emitChange)
        emit valueChanged(this);
}

void
ComboBox::drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value)
{
    QString txt;
    if (property()->listData()) {
        const int idx = property()->listData()->keys.indexOf(value);
        if (idx >= 0)
            txt = property()->listData()->names[ idx ];
        else if (m_edit->isEditable())
            txt = m_edit->currentText();
    } else if (m_edit->isEditable()) {
        txt = m_edit->currentText();
    }

    Widget::drawViewer(p, cg, r, txt); //keyForValue(value));
// p->eraseRect(r);
// p->drawText(r, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, keyForValue(value));
}

void
ComboBox::fillBox()
{
    m_edit->clear();
    //m_edit->clearContents();

    if (!property())
        return;
    if (!property()->listData()) {
        kopropertywarn << "ComboBox::fillBox(): propery listData not available!" << endl;
        return;
    }

    m_edit->insertItems(0, property()->listData()->names);
    KCompletion *comp = m_edit->completionObject();
    comp->insertItems(property()->listData()->names);
    comp->setCompletionMode(KGlobalSettings::CompletionShell);
}

void
ComboBox::setProperty(Property *prop)
{
    const bool b = (property() == prop);
    m_setValueEnabled = false; //setValue() couldn't be called before fillBox()
    Widget::setProperty(prop);
    m_setValueEnabled = true;
    if (!b)
        fillBox();
    if (prop)
        setValue(prop->value(), false); //now the value can be set
}

void
ComboBox::slotValueChanged(int)
{
    emit valueChanged(this);
}

void
ComboBox::setReadOnlyInternal(bool readOnly)
{
    setVisibleFlag(!readOnly);
}


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


#include "combobox.moc"

