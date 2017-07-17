/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>
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

#include "KPropertyStringEditor.h"
#include "KPropertyMultiLineStringEditor.h"
#include "KPropertyUtils.h"
#include "KPropertyUtils_p.h"
#include "KPropertyEditorDataModel_p.h"

namespace {
    bool isMultiLine(const KProperty *property) {
        return property->option("multiLine", false).toBool();
    }
}

KPropertyStringEditor::KPropertyStringEditor(QWidget *parent)
 : QLineEdit(parent)
 , m_slotTextChangedEnabled(true)
{
    setFrame(false);
    setContentsMargins(0,1,0,0);
    setClearButtonEnabled(true);
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(slotTextChanged(const QString&)));
}

KPropertyStringEditor::~KPropertyStringEditor()
{
}

QString KPropertyStringEditor::value() const
{
    return text();
}

void KPropertyStringEditor::setValue(const QString& value)
{
    m_slotTextChangedEnabled = false;
    setText(value);
    m_slotTextChangedEnabled = true;
/*    deselect();
    end(false);*/
}

void KPropertyStringEditor::slotTextChanged( const QString & text )
{
    Q_UNUSED(text)
    if (!m_slotTextChangedEnabled)
        return;
    emit commitData(this);
}

KPropertyStringDelegate::KPropertyStringDelegate()
{
}

QWidget* KPropertyStringDelegate::createEditor( int type, QWidget *parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Q_UNUSED(type);
    Q_UNUSED(option);
    KProperty *property = KPropertyUtils::propertyForIndex(index);
    if (!property) {
        return nullptr;
    }
    if (isMultiLine(property)) {
        return new KPropertyMultiLineStringEditor(parent);
    } else {
        return new KPropertyStringEditor(parent);
    }
}

void KPropertyStringDelegate::paint(QPainter *painter,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    KProperty *property = KPropertyUtils::propertyForIndex(index);
    if (!property) {
        return;
    }
    QString string(index.data(Qt::EditRole).toString());
    if (string.isEmpty()) {
        return;
    }
    Qt::Alignment align = Qt::AlignLeft;
    QRect r(option.rect);
    r.setLeft(r.left() + 2);
    r.setTop(r.top() + 1);
    if (isMultiLine(property)) {
        align |= Qt::AlignTop;
        r.setLeft(r.left() + 1);
        const KPropertyEditorDataModel *editorModel
                = qobject_cast<const KPropertyEditorDataModel*>(index.model());
        KPropertySet *propertySet = nullptr;
        if (editorModel) {
            propertySet = editorModel->propertySet();
        }
        const bool readOnly = property->isReadOnly() || (propertySet && propertySet->isReadOnly());
        QBrush fillBrush;
        if ((!(option.state & QStyle::State_Editing) && (option.state & QStyle::State_Selected))
            || ((option.state & QStyle::State_Selected) && readOnly))
        {
            fillBrush = option.palette.highlight();
        } else {
            fillBrush = option.palette.window();
        }
        painter->fillRect(option.rect, fillBrush);
    } else {
        const int newLineIndex = string.indexOf(QLatin1Char('\n'));
        if (newLineIndex >= 0) {
            string.truncate(newLineIndex);
            if (string.isEmpty()) {
                return;
            }
        }
        align |= Qt::AlignVCenter;
    }
    const KPropertyUtilsPrivate::PainterSaver saver(painter);
    painter->drawText(r, align, string);
}

QString KPropertyStringDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    if (locale.language() == QLocale::C) {
        return value.toString();
    }
    return valueToLocalizedString(value);
}
