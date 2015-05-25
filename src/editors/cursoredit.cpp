/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2008 Jarosław Staniek <staniek@kde.org>

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

#include "cursoredit.h"
#include "KProperty.h"

#include "xpm/blank_cursor.xpm"
#include "xpm/arrow_cursor.xpm"
#include "xpm/bdiag_cursor.xpm"
#include "xpm/busy_cursor.xpm"
#include "xpm/closedhand_cursor.xpm"
#include "xpm/cross_cursor.xpm"
#include "xpm/fdiag_cursor.xpm"
#include "xpm/forbidden_cursor.xpm"
#include "xpm/hand_cursor.xpm"
#include "xpm/ibeam_cursor.xpm"
#include "xpm/openhand_cursor.xpm"
#include "xpm/sizeall_cursor.xpm"
#include "xpm/sizehor_cursor.xpm"
#include "xpm/sizever_cursor.xpm"
#include "xpm/splith_cursor.xpm"
#include "xpm/splitv_cursor.xpm"
#include "xpm/uparrow_cursor.xpm"
#include "xpm/wait_cursor.xpm"
#include "xpm/whatsthis_cursor.xpm"

#include <QMap>
#include <QVariant>
#include <QCursor>
#include <QBitmap>
#include <QDebug>

class CursorListData : public KPropertyListData
{
public:
    CursorListData() : KPropertyListData(keysInternal(), stringsInternal())
    {
    }

    Qt::CursorShape indexToShape(int index) const
    {
        if (index < 0 || index >= keys.count())
            return Qt::ArrowCursor;
        return (Qt::CursorShape)(keys[index].toInt());
    }

    int shapeToIndex(Qt::CursorShape _shape) const
    {
        int index = 0;
        foreach (const QVariant& shape, keys) {
            if (shape.toInt() == _shape)
                return index;
            index++;
        }
        return 0;
    }

    QPixmap pixmapForIndex(int index, const QPalette& pal, bool transparentBackground = false) const
    {
        if (index < 0 || index > 18)
            index = 0;
        QPixmap xpm(m_xpms[index]);
        if (transparentBackground)
            return xpm;
        QPixmap px(xpm.size());
        QColor bg( pal.color(QPalette::Base) ); // paint bg with to avoid invisible black-on-black painting
        bg.setAlpha(127);
        px.fill(bg);
        QPainter p(&px);
        p.drawPixmap(0, 0, xpm);
        return px;
    }
private:
    static QList<QVariant> keysInternal() {
        QList<QVariant> keys;
        keys
        << int(Qt::BlankCursor)
        << int(Qt::ArrowCursor)
        << int(Qt::UpArrowCursor)
        << int(Qt::CrossCursor)
        << int(Qt::WaitCursor)
        << int(Qt::IBeamCursor)
        << int(Qt::SizeVerCursor)
        << int(Qt::SizeHorCursor)
        << int(Qt::SizeBDiagCursor)
        << int(Qt::SizeFDiagCursor)
        << int(Qt::SizeAllCursor)
        << int(Qt::SplitVCursor)
        << int(Qt::SplitHCursor)
        << int(Qt::PointingHandCursor)
        << int(Qt::ForbiddenCursor)
        << int(Qt::WhatsThisCursor)
        << int(Qt::BusyCursor)
        << int(Qt::OpenHandCursor)
        << int(Qt::ClosedHandCursor);
        return keys;
    }

    static QStringList stringsInternal() {
        QStringList strings;
        strings << QObject::tr("No cursor", "Mouse Cursor Shape") //0
        << QObject::tr("Arrow", "Mouse Cursor Shape") //1
        << QObject::tr("Up arrow", "Mouse Cursor Shape") //2
        << QObject::tr("Cross", "Mouse Cursor Shape") //3
        << QObject::tr("Waiting", "Mouse Cursor Shape") //4
        << QObject::tr("Text cursor", "Mouse Cursor Shape") //5
        << QObject::tr("Size vertical", "Mouse Cursor Shape") //6
        << QObject::tr("Size horizontal", "Mouse Cursor Shape") //7
        << QObject::tr("Size slash", "Mouse Cursor Shape") //8
        << QObject::tr("Size backslash", "Mouse Cursor Shape") //9
        << QObject::tr("Size all", "Mouse Cursor Shape") //10
        << QObject::tr("Split vertical", "Mouse Cursor Shape") //11
        << QObject::tr("Split horizontal", "Mouse Cursor Shape") //12
        << QObject::tr("Pointing hand", "Mouse Cursor Shape") //13
        << QObject::tr("Forbidden", "Mouse Cursor Shape") //14
        << QObject::tr("What's this?", "Mouse Cursor Shape") //15
        << QObject::tr("Busy", "Mouse Cursor Shape") //16
        << QObject::tr("Open hand", "Mouse Cursor Shape") //17
        << QObject::tr("Closed hand", "Mouse Cursor Shape"); //18
        return strings;
    }
    static const char * const * const m_xpms[];
};

const char * const * const CursorListData::m_xpms[] =
{
    blank_cursor_xpm,
    arrow_cursor_xpm,
    uparrow_cursor_xpm,
    cross_cursor_xpm,
    wait_cursor_xpm,
    ibeam_cursor_xpm,
    sizever_cursor_xpm,
    sizehor_cursor_xpm,
    bdiag_cursor_xpm,
    fdiag_cursor_xpm,
    sizeall_cursor_xpm,
    splitv_cursor_xpm,
    splith_cursor_xpm,
    hand_cursor_xpm,
    forbidden_cursor_xpm,
    whatsthis_cursor_xpm,
    busy_cursor_xpm,
    openhand_cursor_xpm,
    closedhand_cursor_xpm
};

Q_GLOBAL_STATIC(CursorListData, s_cursorListData)

//----------------------
class CursorIconProvider : public KPropertyComboBoxEditor::Options::IconProviderInterface
{
public:
    explicit CursorIconProvider(QWidget* parent) : m_parent(parent) {}
    virtual QIcon icon(int index) const
    {
          return s_cursorListData->pixmapForIndex(index, m_parent->palette());
    }
    virtual IconProviderInterface* clone() const
    {
        return new CursorIconProvider(m_parent);
    }
    QWidget* m_parent;
};

//----------------------

static KPropertyComboBoxEditor::Options initComboBoxOptions(QWidget* parent)
{
    KPropertyComboBoxEditor::Options options;
    options.iconProvider = new CursorIconProvider(parent);
    return options;
}

KPropertyCursorEditor::KPropertyCursorEditor(QWidget *parent)
        : KPropertyComboBoxEditor(*s_cursorListData, initComboBoxOptions( this ), parent)
{
}

KPropertyCursorEditor::~KPropertyCursorEditor()
{
}

QCursor KPropertyCursorEditor::cursorValue() const
{
    return QCursor((Qt::CursorShape)KPropertyComboBoxEditor::value().toInt());
}

void KPropertyCursorEditor::setCursorValue(const QCursor &value)
{
    KPropertyComboBoxEditor::setValue( (int)(value.shape()) );
}

//---------------

KPropertyCursorDelegate::KPropertyCursorDelegate()
{
    options.removeBorders = false;
}

QWidget * KPropertyCursorDelegate::createEditor( int type, QWidget *parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Q_UNUSED(type);
    Q_UNUSED(option);
    Q_UNUSED(index);
    return new KPropertyCursorEditor(parent);
}

void KPropertyCursorDelegate::paint( QPainter * painter,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    painter->save();
    int comboIndex = s_cursorListData->shapeToIndex( index.data(Qt::EditRole).value<QCursor>().shape() );
    int pmSize = (option.rect.height() >= 32) ? 32 : 16;
    const QPixmap pm( s_cursorListData->pixmapForIndex(comboIndex, option.palette)
        .scaled(pmSize, pmSize, Qt::KeepAspectRatio, Qt::SmoothTransformation) );
    QPoint pmPoint(option.rect.topLeft());
    pmPoint.setX(pmPoint.x() + 2);
    painter->drawPixmap(pmPoint, pm);
    QRect r(option.rect);
    r.setLeft(2 + r.left() + 1 + pm.width());
    painter->drawText(r, Qt::AlignVCenter | Qt::AlignLeft,
        s_cursorListData->names[ comboIndex ] );
    painter->restore();
}
