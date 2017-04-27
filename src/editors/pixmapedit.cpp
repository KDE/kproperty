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

#include "pixmapedit.h"
#include "utils.h"
#include "KProperty.h"
#include "KPropertyUtils.h"
#include "KPropertyUtils_p.h"

#include <QApplication>
#include <QLabel>
#include <QCursor>
#include <QImage>
#include <QDesktopWidget>
#include <QPixmap>
#include <QEvent>
#include <QKeyEvent>
#include <QFrame>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QFileDialog>
#include <QHBoxLayout>

KPropertyPixmapEditor::KPropertyPixmapEditor(KProperty *prop, QWidget *parent)
        : QWidget(parent)
        , m_property(prop)
{
    setBackgroundRole(QPalette::Base);

    QHBoxLayout *lyr = new QHBoxLayout(this);
    lyr->setContentsMargins(0,0,0,0);

    m_edit = new QLabel(this);
    lyr->addWidget(m_edit);
    m_edit->setContentsMargins(0, 1, 0, 0);
    m_edit->setToolTip(tr("Click to show image preview"));
    m_edit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_edit->setBackgroundRole(QPalette::Base);
    m_edit->setMouseTracking(true);
    m_edit->installEventFilter(this);

    m_button = new QPushButton(this);
    lyr->addWidget(m_button);
    KPropertyUtils::setupDotDotDotButton(m_button, tr("Insert image from file"),
        tr("Inserts image from file"));

    m_popup = new QLabel(nullptr, Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint);
    m_popup->setBackgroundRole(QPalette::Base);
    m_popup->setFrameStyle(QFrame::Plain | QFrame::Box);
    m_popup->setMargin(2);
    m_popup->setLineWidth(1);
    m_popup->hide();

    setFocusProxy(m_edit);
    connect(m_button, SIGNAL(clicked()), this, SLOT(selectPixmap()));
}

KPropertyPixmapEditor::~KPropertyPixmapEditor()
{
    delete m_popup;
}

QVariant KPropertyPixmapEditor::value() const
{
    return m_pixmap;
}

void KPropertyPixmapEditor::setValue(const QVariant &value)
{
    m_pixmap = value.value<QPixmap>();
    if (m_pixmap.isNull() || (m_pixmap.height() <= height())) {
        m_previewPixmap = m_pixmap;
    } else {
        QImage img(m_pixmap.toImage());
        const QSize sz(size() - QSize(0,1));
        if (!QRect(QPoint(0, 0), sz).contains(m_pixmap.rect())) {
            img = img.scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            m_previewPixmap = QPixmap::fromImage(img);//preview pixmap is a bit larger
        } else {
            m_previewPixmap = m_pixmap;
        }
    }
    emit commitData(this);
}

QString KPropertyPixmapEditor::selectPixmapFileName()
{
    /*#ifdef PURE_QT
      QString url = QFileDialog::getOpenFileName();
      if (!url.isEmpty()) {
        m_edit->setPixmap(QPixmap(url));
        emit valueChanged(this);
      }
    #endif*/
    const QString caption(tr("Insert Image From File (for \"%1\" property)").arg(m_property->caption()));
    /*KDE4:
    #ifdef Q_OS_WIN
      QString recentDir;
      QString fileName = Q3FileDialog::getOpenFileName(
        KFileDialog::getStartURL(":lastVisitedImagePath", recentDir).path(),
        convertKFileDialogFilterToQFileDialogFilter(KImageIO::pattern(KImageIO::Reading)),
        this, 0, caption);
    #else*/
    const QUrl url(QFileDialog::getOpenFileUrl(this, caption));
    QString fileName = url.isLocalFile() ? url.toLocalFile() : url.toString();

    //! @todo download the file if remote, then set fileName properly
//#endif
    return fileName;
}

void KPropertyPixmapEditor::selectPixmap()
{
    const QString fileName(selectPixmapFileName());
    if (fileName.isEmpty())
        return;

    QPixmap pm;
    if (!pm.load(fileName)) {
//! @todo err msg
        return;
    }
    setValue(pm);

    /* KDE4:
    #ifdef Q_OS_WIN
      //save last visited path
      QUrl url(fileName);
      if (url.isLocalFile())
        KRecentDirs::add(":lastVisitedImagePath", url.adjusted(QUrl::RemoveFilename|QUrl::StripTrailingSlash).path());
    #endif
    */
}

bool
KPropertyPixmapEditor::eventFilter(QObject *o, QEvent *ev)
{
    if (o == m_edit) {
        if (ev->type() == QEvent::MouseButtonPress && static_cast<QMouseEvent*>(ev)->button() == Qt::LeftButton) {
            if (m_previewPixmap.height() <= m_edit->height()
                    && m_previewPixmap.width() <= m_edit->width())
                return false;

            m_popup->setPixmap(m_previewPixmap.isNull() ? m_pixmap : m_previewPixmap);
            m_popup->resize(m_previewPixmap.size() + QSize(2*3, 2*3));
            QPoint pos = QCursor::pos() + QPoint(3, 15);
            const QRect screenRect = QApplication::desktop()->availableGeometry(this);
            if ((pos.x() + m_popup->width()) > screenRect.width())
                pos.setX(screenRect.width() - m_popup->width());
            if ((pos.y() + m_popup->height()) > screenRect.height())
                pos.setY(mapToGlobal(QPoint(0, 0)).y() - m_popup->height());
            m_popup->move(pos);
            m_popup->show();
        } else if (ev->type() == QEvent::MouseButtonRelease || ev->type() == QEvent::Hide) {
            if (m_popup->isVisible()) {
                m_popup->hide();
            }
        } else if (ev->type() == QEvent::KeyPress) {
            QKeyEvent* e = static_cast<QKeyEvent*>(ev);
            if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Space) || (e->key() == Qt::Key_Return)) {
                m_button->animateClick();
                return true;
            }
        }
    }
    return QWidget::eventFilter(o, ev);
}

/*
void
PixmapEdit::setReadOnlyInternal(bool readOnly)
{
    m_button->setEnabled(!readOnly);
}*/

//-----------------------

KPropertyPixmapDelegate::KPropertyPixmapDelegate()
{
}

QWidget* KPropertyPixmapDelegate::createEditor( int type, QWidget *parent,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Q_UNUSED(type);
    Q_UNUSED(option);

    KProperty *property = KPropertyUtils::propertyForIndex(index);
    if (!property) {
        return nullptr;
    }
    KPropertyPixmapEditor *pe = new KPropertyPixmapEditor(property, parent);
    return pe;
}

void KPropertyPixmapDelegate::paint( QPainter * painter,
    const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    const KPropertyUtilsPrivate::PainterSaver saver(painter);
    QPixmap pm( index.data(Qt::EditRole).value<QPixmap>() );
    if (!pm.isNull()) {
        if (pm.height() > option.rect.height() || pm.width() > option.rect.width()) { //scale down
            QImage img(pm.toImage());
            img = img.scaled(option.rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            pm = QPixmap::fromImage(img);
        }
//! @todo
/*    if (m_recentlyPainted != value) {
        m_recentlyPainted = value;
        m_scaledPixmap = value.value<QPixmap>();
        if (m_scaledPixmap.height() > r2.height() || m_scaledPixmap.width() > r2.width()) { //scale down
            QImage img(m_scaledPixmap.toImage());
            img = img.scaled(r2.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            m_scaledPixmap = QPixmap::fromImage(img);
        }
    }*/
        painter->drawPixmap(option.rect.topLeft().x(),
                            option.rect.topLeft().y() + (option.rect.height() - pm.height()) / 2, pm);
    }
    QRect r(option.rect);
    r.setLeft(r.left() + pm.width() + 2);
    painter->drawText(r, valueToString(index.data(Qt::EditRole), QLocale()));
}

QString KPropertyPixmapDelegate::valueToString(const QVariant& value, const QLocale &locale) const
{
    const QPixmap pm(value.value<QPixmap>());
    if (pm.isNull()) {
        if (locale.language() == QLocale::C) {
            return QString();
        }
        return QObject::tr("None", "No pixmap");
    }
    if (locale.language() == QLocale::C) {
        return QString::fromLatin1("%1x%2px").arg(pm.width()).arg(pm.height());
    }
    return QObject::tr("%1x%2px").arg(locale.toString(pm.width())).arg(locale.toString(pm.height()));
}
