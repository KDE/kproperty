/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2005-2015 Jarosław Staniek <staniek@kde.org>

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

#include "fontedit.h"
#include "fontedit_p.h"
#include "utils.h"
#include "kproperty_debug.h"

#include <QObject>
#include <QPushButton>
#include <QPainter>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QFontDatabase>
#include <QEvent>
#include <QHBoxLayout>
#include <QApplication>
#include <QPushButton>
#include <QFontDialog>

KPropertyFontEditRequester::KPropertyFontEditRequester(QWidget *parent)
        : QWidget(parent)
        , m_paletteChangedEnabled(true)
{
    setBackgroundRole(QPalette::Base);
    QHBoxLayout *lyr = new QHBoxLayout(this);
    lyr->setContentsMargins(0,0,0,0);
    lyr->setSpacing( 1 );
    lyr->addStretch(1);
    m_button = new QPushButton(this);
    setFocusProxy(m_button);
    KPropertyUtils::setupDotDotDotButton(m_button,
        tr("Click to select a font"),
        tr("Selects font"));
    connect( m_button, SIGNAL( clicked() ), SLOT( slotSelectFontClicked() ) );
    lyr->addWidget(m_button);
    setValue(qApp->font());
}

QFont KPropertyFontEditRequester::value() const
{
    return m_font;
}

void KPropertyFontEditRequester::setValue(const QFont &value)
{
    //kprDebug() << QFontDatabase().families();
    m_font = value;
}

void KPropertyFontEditRequester::slotSelectFontClicked()
{
    bool ok;
    QFont font;
    font = QFontDialog::getFont(&ok, m_font, parentWidget());
    if (ok) {
        m_font = font;
        setValue(m_font);
        emit commitData(this);
    }
}

bool KPropertyFontEditRequester::event( QEvent * event )
{
    return QWidget::event(event);
}

// -----------

KPropertyFontDelegate::KPropertyFontDelegate()
{
}

QWidget * KPropertyFontDelegate::createEditor( int type, QWidget *parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Q_UNUSED(type);
    Q_UNUSED(option);
    Q_UNUSED(index);
    return new KPropertyFontEditRequester(parent);
}

void KPropertyFontDelegate::paint( QPainter * painter,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    painter->save();
    const QFont origFont( painter->font() );
    QFont f( index.data(Qt::EditRole).value<QFont>() );
    if (option.font.pointSize() > 0)
        f.setPointSize(option.font.pointSize());
    else if (option.font.pixelSize() > 0)
        f.setPixelSize(option.font.pixelSize());
    painter->setFont( f );
    QRect rect( option.rect );
    rect.setLeft( rect.left() + 1 );
    const QString txt( QObject::tr("Abc", "Font sample for property editor item, typically \"Abc\"") );
    painter->drawText( rect, Qt::AlignLeft | Qt::AlignVCenter,
        QObject::tr("Abc", "Font sample for property editor item, typically \"Abc\"") );

    rect.setLeft(rect.left() + 5 + painter->fontMetrics().width( txt ));
    painter->setFont(origFont);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, valueToString(index.data(Qt::EditRole), QLocale()));
    painter->restore();
}

QString KPropertyFontDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    const QFont f(value.value<QFont>());
    qreal size = f.pointSizeF();
    QString unit;
    if (size == -1) {
        size = f.pixelSize();
        unit = QLatin1String("px");
    }
    else {
        unit = QLatin1String("pt");
    }
    QStringList list;
    list << f.family();
    const bool translate = locale.language() == QLocale::C;
    list << (translate ? QObject::tr("%1%2", "<fontsize><unit>, e.g. 12pt").arg(size).arg(unit)
                      : QString::fromLatin1("%1%2").arg(size).arg(unit));
    if (f.bold()) {
        list << (translate ? QObject::tr("bold", "bold font") : QLatin1String("bold"));
    }
    if (f.italic()) {
        list << (translate ? QObject::tr("italic", "italic font") : QLatin1String("italic"));
    }
    if (f.strikeOut()) {
        list << (translate ? QObject::tr("strikeout", "strikeout font") : QLatin1String("strikeout"));
    }
    if (f.underline()) {
        list << (translate ? QObject::tr("underline", "underline font") : QLatin1String("underline"));
    }
    return QLocale::c().createSeparatedList(list); // yes, C locale, we just want ',' separator.
}
