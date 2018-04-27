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

#include "KPropertyComposedUrlEditor.h"
#include "KPropertyUrlEditor.h" // KPropertyUrlDelegate
#include "KPropertyUrlEditor_p.h"

#include <QLineEdit>

KPropertyComposedUrlEditor::KPropertyComposedUrlEditor(const KProperty &property, QWidget *parent)
    : KPropertyGenericSelectionEditor(parent)
    , d(new KPropertyUrlEditorPrivate(this, property))
{
    setMainWidget(d->lineEdit);
    connect(d.data(), &KPropertyUrlEditorPrivate::commitData, this,
            [this] { emit commitData(this); });
}

KPropertyComposedUrlEditor::~KPropertyComposedUrlEditor() {}

KPropertyComposedUrl KPropertyComposedUrlEditor::value() const
{
    return d->value.value<KPropertyComposedUrl>();
}

void KPropertyComposedUrlEditor::setValue(const KPropertyComposedUrl &value)
{
    d->setValue(QVariant::fromValue(value));
    const KPropertyUrlDelegate delegate;
    d->updateLineEdit(delegate.valueToString(d->value, locale()));
}

void KPropertyComposedUrlEditor::selectButtonClicked()
{
    QUrl url = d->getUrl();
    if (url.isValid() && d->checkAndUpdate(&url)) {
        KPropertyComposedUrl composedUrl = value();
        composedUrl.setAbsoluteUrl(url);
        setValue(composedUrl);
        emit commitData(this);
    }
}

bool KPropertyComposedUrlEditor::eventFilter(QObject *o, QEvent *event)
{
    d->processEvent(o, event);
    return KPropertyGenericSelectionEditor::eventFilter(o, event);
}
