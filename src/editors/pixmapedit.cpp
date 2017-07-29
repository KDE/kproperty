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

static const int POPUP_MARGIN = 2;

class Q_DECL_HIDDEN KPropertyPixmapEditor::Private
{
public:
    Private()
    {
    }
    ~Private()
    {
        delete popup;
    }

    QLabel *edit;
    QLabel *popup;
    QPushButton *button;
    KProperty *property;
    //! @todo    QVariant recentlyPainted;
    QPixmap pixmap;
    //! @todo QPixmap scaledPixmap
    QPixmap previewPixmap;
};

KPropertyPixmapEditor::KPropertyPixmapEditor(KProperty *prop, QWidget *parent)
        : QWidget(parent), d(new Private)
{
    d->property = prop;
    setBackgroundRole(QPalette::Base);

    QHBoxLayout *lyr = new QHBoxLayout(this);
    lyr->setContentsMargins(0,0,0,0);

    d->edit = new QLabel(this);
    lyr->addWidget(d->edit);
    d->edit->setContentsMargins(0, 1, 0, 0);
    d->edit->setToolTip(tr("Click to show image preview"));
    d->edit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    d->edit->setBackgroundRole(QPalette::Base);
    d->edit->setMouseTracking(true);

    d->button = new QPushButton(this);
    lyr->addWidget(d->button);
    KPropertyUtils::setupDotDotDotButton(d->button, tr("Insert image from file"),
        tr("Inserts image from file"));

    d->popup = new QLabel(nullptr, Qt::ToolTip);
    d->popup->setBackgroundRole(QPalette::ToolTipBase);
    d->popup->setForegroundRole(QPalette::ToolTipText);
    d->popup->setFrameStyle(QFrame::Plain | QFrame::Box);
    d->popup->setMargin(POPUP_MARGIN);
    d->popup->setLineWidth(1);
    d->popup->hide();

    setFocusProxy(d->edit);
    connect(d->button, SIGNAL(clicked()), this, SLOT(selectPixmap()));

    d->edit->installEventFilter(this);
    installEventFilter(this);
}

KPropertyPixmapEditor::~KPropertyPixmapEditor()
{
    delete d;
}

QVariant KPropertyPixmapEditor::value() const
{
    return d->pixmap;
}

void KPropertyPixmapEditor::setValue(const QVariant &value)
{
    d->pixmap = value.value<QPixmap>();
    if (d->pixmap.isNull() || (d->pixmap.height() <= height())) {
        d->previewPixmap = d->pixmap;
    } else {
        QImage img(d->pixmap.toImage());
        const QSize sz(size() - QSize(0,1));
        if (!QRect(QPoint(0, 0), sz).contains(d->pixmap.rect())) {
            img = img.scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            d->previewPixmap = QPixmap::fromImage(img);//preview pixmap is a bit larger
        } else {
            d->previewPixmap = d->pixmap;
        }
    }
    emit commitData(this);
}

QString KPropertyPixmapEditor::selectPixmapFileName()
{
    /*#ifdef PURE_QT
      QString url = QFileDialog::getOpenFileName();
      if (!url.isEmpty()) {
        d->edit->setPixmap(QPixmap(url));
        emit valueChanged(this);
      }
    #endif*/
    const QString caption(
        tr("Insert Image From File (for \"%1\" property)").arg(d->property->captionOrName()));
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

static QRect popupGeometry(QWidget *editor, const QSize &pixmapSize)
{
    const QRect screenRect = QApplication::desktop()->availableGeometry(editor);
    const QRect globalRect = QRect(editor->mapToGlobal(QPoint(0, 0)), editor->size()) & screenRect;
    int aboveSpace = std::min(pixmapSize.height(), globalRect.y() - screenRect.top());
    int belowSpace = std::min(pixmapSize.height(), screenRect.bottom() - globalRect.bottom());
    // find max area
    // area3 | area1
    // area2 | area0
    const QVector<int> widths{
        std::min(pixmapSize.width(), screenRect.right() - globalRect.left()),
        std::min(pixmapSize.width(), screenRect.right() - globalRect.left()),
        std::min(pixmapSize.width(), globalRect.right() - screenRect.left()),
        std::min(pixmapSize.width(), globalRect.right() - screenRect.left())
    };
    const std::vector<int> areas{ widths[0] * belowSpace, widths[1] * aboveSpace,
                                  widths[2] * belowSpace, widths[3] * aboveSpace };
    const int areaNumber = std::distance(areas.begin(), std::max_element(areas.begin(), areas.end()));
    QRect rect;
    switch (areaNumber) {
    case 0: {
        int width = double(belowSpace) / pixmapSize.height() * pixmapSize.width();
        if (width > widths[areaNumber]) {
            width = widths[areaNumber];
            belowSpace = double(width) / pixmapSize.width() * pixmapSize.height();
        }
        rect = QRect(globalRect.left(), globalRect.bottom(), width, belowSpace);
        break;
    }
    case 1: {
        int width = double(aboveSpace) / pixmapSize.height() * pixmapSize.width();
        if (width > widths[areaNumber]) {
            width = widths[areaNumber];
            aboveSpace = double(width) / pixmapSize.width() * pixmapSize.height();
        }
        rect = QRect(globalRect.left(), globalRect.top() - aboveSpace, width, aboveSpace);
        break;
    }
    case 2: {
        int width = double(belowSpace) / pixmapSize.height() * pixmapSize.width();
        if (width > widths[areaNumber]) {
            width = widths[areaNumber];
            belowSpace = double(width) / pixmapSize.width() * pixmapSize.height();
        }
        rect = QRect(globalRect.right() - width, globalRect.bottom(), width, belowSpace);
        break;
    }
    case 3: {
        int width = double(aboveSpace) / pixmapSize.height() * pixmapSize.width();
        if (width > widths[areaNumber]) {
            width = widths[areaNumber];
            aboveSpace = double(width) / pixmapSize.width() * pixmapSize.height();
        }
        rect = QRect(globalRect.right() - width, globalRect.top() - aboveSpace, width,
                     aboveSpace);
        break;
    }
    }
    return rect;
}

bool
KPropertyPixmapEditor::eventFilter(QObject *o, QEvent *ev)
{
    if (o == d->edit) {
        if (ev->type() == QEvent::MouseButtonPress && static_cast<QMouseEvent*>(ev)->button() == Qt::LeftButton) {
            if (d->pixmap.height() <= d->edit->height() && d->pixmap.width() <= d->edit->width()) {
                return false; // nothing to preview
            }
            d->popup->setGeometry(popupGeometry(this, d->pixmap.size()));
            d->popup->setPixmap(d->pixmap);
            d->popup->show();
        } else if (ev->type() == QEvent::MouseButtonRelease || ev->type() == QEvent::Hide) {
            if (d->popup->isVisible()) {
                d->popup->hide();
            }
        } else if (ev->type() == QEvent::KeyPress) {
            QKeyEvent* e = static_cast<QKeyEvent*>(ev);
            if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Space) || (e->key() == Qt::Key_Return)) {
                d->button->animateClick();
                return true;
            }
        }
    } else if (o == this) {
        if (ev->type() == QEvent::Resize) {
            d->button->setMaximumWidth(height());
        }
    }
    return QWidget::eventFilter(o, ev);
}

/*
void
PixmapEdit::setReadOnlyInternal(bool readOnly)
{
    d->button->setEnabled(!readOnly);
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
/*    if (d->recentlyPainted != value) {
        d->recentlyPainted = value;
        d->scaledPixmap = value.value<QPixmap>();
        if (d->scaledPixmap.height() > r2.height() || d->scaledPixmap.width() > r2.width()) { //scale down
            QImage img(d->scaledPixmap.toImage());
            img = img.scaled(r2.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            d->scaledPixmap = QPixmap::fromImage(img);
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
