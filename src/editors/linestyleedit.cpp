/* This file is part of the KDE project
   Copyright (C) 2010-2015 Jarosław Staniek <staniek@kde.org>

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

#include "linestyleedit.h"
#include "KPropertyWidgetsFactory.h"
#include "combobox.h"
#include "KPropertyLineStyleItemDelegate_p.h"
#include "KPropertyUtils_p.h"

#include <QApplication>
#include <QPen>

KPropertyLineStyleComboEditor::KPropertyLineStyleComboEditor(QWidget *parent)
    : KPropertyLineStyleSelector(parent)
{
    connect(this, SIGNAL(activated(int)), this, SLOT(slotValueChanged(int)));

    int paddingTop = 1;
    int paddingLeft = 0;
    const QString style(parent->style()->objectName());
    if (!KPropertyUtilsPrivate::gridLineColor(this).isValid()) {
        setFrame(false);
        paddingTop = 0;
    }
    if (style == QLatin1String("windows") || style == QLatin1String("fusion")) {
        paddingLeft = 3;
    } else if (style == QLatin1String("windowsvista")) {
        paddingLeft = 2;
    }
    QString styleSheet = QString::fromLatin1("KPropertyLineStyleSelector { \
        %1 \
        padding-top: %2px; padding-left: %3px; }").arg(KPropertyComboBoxEditor::borderSheet(this))
                                                  .arg(paddingTop).arg(paddingLeft);
    setStyleSheet(styleSheet);
}

KPropertyLineStyleComboEditor::~KPropertyLineStyleComboEditor()
{
}

QVariant KPropertyLineStyleComboEditor::value() const
{
    return int(lineStyle());
}

static bool hasVisibleStyle(const QVariant &value)
{
    return !value.isNull() && value.canConvert(QVariant::Int) && value.toInt() < Qt::CustomDashLine
            && value.toInt() >= Qt::NoPen;
}

void KPropertyLineStyleComboEditor::setValue(const QVariant &value)
{
    if (!hasVisibleStyle(value)) {
        setLineStyle(Qt::NoPen);
        return;
    }
    setLineStyle(static_cast<Qt::PenStyle>(value.toInt()));
}

void KPropertyLineStyleComboEditor::slotValueChanged(int)
{
    emit commitData(this);
}

KPropertyLineStyleComboDelegate::KPropertyLineStyleComboDelegate()
{
    options.removeBorders = false;
}

QWidget * KPropertyLineStyleComboDelegate::createEditor( int type, QWidget *parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Q_UNUSED(type)
    Q_UNUSED(option)
    Q_UNUSED(index)
    return new KPropertyLineStyleComboEditor(parent);
}

void KPropertyLineStyleComboDelegate::paint( QPainter * painter,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const KPropertyUtilsPrivate::PainterSaver saver(painter);
    Qt::PenStyle penStyle = Qt::NoPen;
    if (hasVisibleStyle(index.data(Qt::EditRole))) {
        penStyle = static_cast<Qt::PenStyle>(index.data(Qt::EditRole).toInt());
    }
    const QWidget *paintedWidget = dynamic_cast<QWidget*>(painter->device());
    const QStyle *style = paintedWidget ? paintedWidget->style() : qApp->style();
    QStyleOptionComboBox cbOption;
    cbOption.rect = option.rect;
    QRect r = style->subControlRect(QStyle::CC_ComboBox, &cbOption, QStyle::SC_ComboBoxEditField, nullptr);
    r.setRight(option.rect.right() - (r.left() - option.rect.left()));
    KPropertyLineStyleItemDelegate::paintItem(painter, QPen(penStyle), r, option);
}

QString KPropertyLineStyleComboDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    Qt::PenStyle style = (value.isNull() || !value.canConvert(QVariant::Int) || value.toInt() > Qt::CustomDashLine
                          || value.toInt() < Qt::NoPen) ? Qt::NoPen : static_cast<Qt::PenStyle>(value.toInt());
    return KPropertyLineStyleItemDelegate::styleName(style, locale);
}
