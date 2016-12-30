/* This file is part of the KDE project
   Copyright (C) 2010-2016 Jarosław Staniek <staniek@kde.org>

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
#include "combobox.h"
#include "KPropertyUtils_p.h"

#include <QEvent>
#include <QHBoxLayout>

#include <KColorCombo>
#include <KColorCollection>

Q_GLOBAL_STATIC_WITH_ARGS(KColorCollection, g_oxygenColors, (QLatin1String("Oxygen.colors")))

static QString colorToName(const QColor &color, const QLocale &locale)
{
    if (!color.isValid()) {
        return locale.language() == QLocale::C ? QString::fromLatin1("#invalid")
                                     : QObject::tr("#invalid", "Invalid color");
    }
    return color.alpha() == 255 ? color.name(QColor::HexRgb) : color.name(QColor::HexArgb);
}

//! @brief Paints color code
//! Useful for caching the font.
//! @internal
class ColorCodePainter
{
public:
    explicit ColorCodePainter(const QFont &font)
        : m_font(font)
    {
        m_font.setFamily(QLatin1String("courier"));
    }
    void paint(QPainter *painter, const QRect &rect, const QColor &color) {
        painter->setPen(KPropertyUtilsPrivate::contrastColor(color));
        painter->setFont(m_font);
        painter->drawText(rect, Qt::AlignCenter, colorToName(color, QLocale()));
    }

private:
    QFont m_font;
};

// -------------------

class Q_DECL_HIDDEN KPropertyColorComboEditor::Private
{
public:
    Private() {}
    KColorCombo *combo;
    QWidget *colorCodeOverlay;
    QScopedPointer<ColorCodePainter> colorCodePainter;
};

KPropertyColorComboEditor::KPropertyColorComboEditor(QWidget *parent)
        : QWidget(parent)
        , d(new Private)
{
    installEventFilter(this); // handle size of the combo
    d->colorCodePainter.reset(new ColorCodePainter(font()));
    d->combo = new KColorCombo(this);
    connect(d->combo, SIGNAL(activated(QColor)), this, SLOT(slotValueChanged(QColor)));
    d->combo->installEventFilter(this); // handle size of the overlay
    d->colorCodeOverlay = new QWidget(d->combo, Qt::CustomizeWindowHint | Qt::WindowTransparentForInput);
    d->colorCodeOverlay->raise();
    d->colorCodeOverlay->installEventFilter(this); // handle painting of the overlay
    QList< QColor > colors;
    const int oxygenColorsCount = g_oxygenColors->count();
    for (int i = 0; i < oxygenColorsCount; i++) {
        colors += g_oxygenColors->color(i);
    }
    d->combo->setColors(colors);
    setFocusProxy(d->combo);

    int paddingTop = 1;
    if (!KPropertyUtilsPrivate::gridLineColor(this).isValid()) {
        d->combo->setFrame(false);
        paddingTop = 0;
    }
    QString styleSheet = QString::fromLatin1("KPropertyColorComboEditor { \
        %1; \
        padding-top: %2px; padding-left: 1px; }")
        .arg(KPropertyComboBoxEditor::borderSheet(this))
        .arg(paddingTop);
    setStyleSheet(styleSheet);
}

KPropertyColorComboEditor::~KPropertyColorComboEditor()
{
    delete d;
}

QVariant KPropertyColorComboEditor::value() const
{
    return d->combo->color();
}

void KPropertyColorComboEditor::setValue(const QVariant &value)
{
    d->combo->setColor(value.value<QColor>());
}

void KPropertyColorComboEditor::slotValueChanged(const QColor&)
{
    emit commitData(this);
}

bool KPropertyColorComboEditor::eventFilter(QObject *o, QEvent *e)
{
    const bool result = QWidget::eventFilter(o, e);
    if (o == d->colorCodeOverlay) {
        if (e->type() == QEvent::Paint) {
            QPainter painter(d->colorCodeOverlay);
             d->colorCodePainter->paint(&painter, d->colorCodeOverlay->rect(), d->combo->color());
        }
    } else if (o == d->combo) {
        if (e->type() == QEvent::Resize) {
            d->colorCodeOverlay->setGeometry(0, 0, d->combo->width(), d->combo->height());
        }
    } else if (o == this) {
        if (e->type() == QEvent::Resize) {
            d->combo->setGeometry(0, 0, width(), height()+1);
        }
    }
    return result;
}

// -------------------

class Q_DECL_HIDDEN KPropertyColorComboDelegate::Private
{
public:
    Private() {}
    QScopedPointer<ColorCodePainter> colorCodePainter;
};

KPropertyColorComboDelegate::KPropertyColorComboDelegate()
    : d(new Private)
{
}

KPropertyColorComboDelegate::~KPropertyColorComboDelegate()
{
    delete d;
}

QWidget * KPropertyColorComboDelegate::createEditor(int type, QWidget *parent,
    const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(type)
    Q_UNUSED(option)
    Q_UNUSED(index)
    return new KPropertyColorComboEditor(parent);
}

void KPropertyColorComboDelegate::paint( QPainter * painter,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const KPropertyUtilsPrivate::PainterSaver saver(painter);
    const QBrush b(index.data(Qt::EditRole).value<QColor>());
    painter->setBrush(b);
    painter->setPen(QPen(Qt::NoPen));
    painter->drawRect(option.rect);
    if (!d->colorCodePainter) {
        d->colorCodePainter.reset(new ColorCodePainter(option.font));
    }
    d->colorCodePainter->paint(painter, option.rect, b.color());
}

QString KPropertyColorComboDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    return colorToName(value.value<QColor>(), locale);
}
