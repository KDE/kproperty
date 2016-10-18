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

#include "coloredit.h"
#include "KPropertyUtils_p.h"

#include <QColor>

#ifdef KPROPERTY_KF
#include <KColorCollection>

Q_GLOBAL_STATIC_WITH_ARGS(KColorCollection, g_oxygenColors, (QLatin1String("Oxygen.colors")))

KPropertyColorComboEditor::KPropertyColorComboEditor(QWidget *parent)
        : KColorCombo(parent)
{
    connect(this, SIGNAL(activated(QColor)), this, SLOT(slotValueChanged(QColor)));

    QList< QColor > colors;
    const int oxygenColorsCount = g_oxygenColors->count();
    for (int i = 0; i < oxygenColorsCount; i++) {
        colors += g_oxygenColors->color(i);
    }
    setColors(colors);

    int paddingTop = 1;
    if (!KPropertyUtils::gridLineColor(this).isValid()) {
        setFrame(false);
        paddingTop = 0;
    }
    QString styleSheet = QString::fromLatin1("QComboBox { \
        border: 1px; \
        padding-top: %1px; padding-left: 1px; }").arg(paddingTop);
    setStyleSheet(styleSheet);
}

KPropertyColorComboEditor::~KPropertyColorComboEditor()
{
}

QVariant KPropertyColorComboEditor::value() const
{
    return color();
}

void KPropertyColorComboEditor::setValue(const QVariant &value)
{
    setColor(value.value<QColor>());
}

void KPropertyColorComboEditor::slotValueChanged(const QColor&)
{
    emit commitData(this);
}
#endif // KPROPERTY_KF

QWidget * KPropertyColorComboDelegate::createEditor(int type, QWidget *parent,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(type)
    Q_UNUSED(option)
    Q_UNUSED(index)
#ifdef KPROPERTY_KF
    return new KPropertyColorComboEditor(parent);
#else
    return KPropertyEditorCreatorInterface::createEditor(type, parent, option, index);
#endif
}

static QString colorToName(const QColor &color, const QLocale &locale)
{
    if (!color.isValid()) {
        return locale.language() == QLocale::C ? QString::fromLatin1("#invalid")
                                     : QObject::tr("#invalid", "Invalid color");
    }
    return color.alpha() == 255 ? color.name(QColor::HexRgb) : color.name(QColor::HexArgb);
}

void KPropertyColorComboDelegate::paint( QPainter * painter,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const KPropertyUtils::PainterSaver saver(painter);
    const QBrush b(index.data(Qt::EditRole).value<QColor>());
    painter->setBrush(b);
    painter->setPen(QPen(Qt::NoPen));
    painter->drawRect(option.rect);
    painter->setBrush(KPropertyUtils::contrastColor(b.color()));
    painter->setPen(KPropertyUtils::contrastColor(b.color()));
    QFont f(option.font);
    f.setFamily(QLatin1String("courier"));
    painter->setFont(f);
    painter->drawText(option.rect, Qt::AlignCenter, colorToName(b.color(), QLocale()));
}

QString KPropertyColorComboDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    return colorToName(value.value<QColor>(), locale);
}
