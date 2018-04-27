/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2016-2018 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2018 Dmitry Baryshev <dmitrymq@gmail.com>

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

#include "KPropertyUrlEditor.h"
#include "KPropertyComposedUrl.h"
#include "KPropertyComposedUrlEditor.h"
#include "KPropertyEditorView.h"
#include "KPropertyUrlEditor_p.h"
#include "KPropertyUtils.h"
#include "kproperty_debug.h"

#include <QDir>
#include <QLineEdit>

KPropertyUrlEditor::KPropertyUrlEditor(const KProperty &property, QWidget *parent)
    : KPropertyGenericSelectionEditor(parent)
    , d(new KPropertyUrlEditorPrivate(this, property))
{
    setMainWidget(d->lineEdit);
    connect(d.data(), &KPropertyUrlEditorPrivate::commitData, this,
            [this] { emit commitData(this); });
}

KPropertyUrlEditor::~KPropertyUrlEditor() {}

QUrl KPropertyUrlEditor::value() const
{
    return d->value.toUrl();
}

void KPropertyUrlEditor::setValue(const QUrl &value)
{
    d->setValue(value);
    const KPropertyUrlDelegate delegate;
    d->updateLineEdit(delegate.valueToString(d->value, locale()));
}

void KPropertyUrlEditor::selectButtonClicked()
{
    QUrl url = d->getUrl();
    if (url.isValid() && d->checkAndUpdate(&url)) {
        setValue(url);
        emit commitData(this);
    }
}

bool KPropertyUrlEditor::eventFilter(QObject *o, QEvent *event)
{
    d->processEvent(o, event);
    return KPropertyGenericSelectionEditor::eventFilter(o, event);
}

// ----

KPropertyUrlDelegate::KPropertyUrlDelegate() {}

QWidget *KPropertyUrlDelegate::createEditor(int type, QWidget *parent,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    Q_UNUSED(option)

    const KProperty *prop = KPropertyUtils::propertyForIndex(index);

    if (type == KProperty::Url) {
        return new KPropertyUrlEditor(prop ? *prop : KProperty(), parent);
    } else if (type == KProperty::ComposedUrl) {
        return new KPropertyComposedUrlEditor(prop ? *prop : KProperty(), parent);
    }

    return nullptr;
}

QString KPropertyUrlDelegate::valueToString(const QVariant &value, const QLocale &locale) const
{
    QUrl url;

    if (value.canConvert<QUrl>()) {
        url = value.toUrl();
    } else if (value.canConvert<KPropertyComposedUrl>()) {
        const KPropertyComposedUrl composedUrl = value.value<KPropertyComposedUrl>();

        if (composedUrl.isValid()) {
            if (!composedUrl.relativePath().isEmpty()) {
                QUrl urlWithPath;
                urlWithPath.setPath(composedUrl.relativePath());
                url = urlWithPath;
            } else {
                url = composedUrl.absoluteUrl();
            }
        } else {
            return QString();
        }
    } else {
        return QString();
    }

    QString s;

    if (url.isLocalFile()) {
        s = QDir::toNativeSeparators(url.toLocalFile());
        // TODO this assumes local files only
    } else if (url.isRelative()) {
        s = QDir::toNativeSeparators(url.toString());
    } else {
        s = url.toString();
    }

    if (locale.language() == QLocale::C) {
        return s;
    }
    return valueToLocalizedString(s);
}
