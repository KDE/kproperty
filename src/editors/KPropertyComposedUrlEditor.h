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

#ifndef KPROPERTYCOMPOSEDURLEDITOR_H
#define KPROPERTYCOMPOSEDURLEDITOR_H

#include "KPropertyComposedUrl.h"
#include "KPropertyGenericSelectionEditor.h"

#include <QScopedPointer>

class KProperty;
class KPropertyUrlEditorPrivate;

/**
 * Editor for ComposedUrl type
 *
 * @since 3.2
 */
class KPROPERTYWIDGETS_EXPORT KPropertyComposedUrlEditor : public KPropertyGenericSelectionEditor
{
    Q_PROPERTY(KPropertyComposedUrl value READ value WRITE setValue USER true)
    Q_OBJECT

public:
    explicit KPropertyComposedUrlEditor(const KProperty &property, QWidget *parent = nullptr);
    ~KPropertyComposedUrlEditor() override;

    virtual KPropertyComposedUrl value() const;

Q_SIGNALS:
    void commitData(QWidget * editor);

public Q_SLOTS:
    virtual void setValue(const KPropertyComposedUrl &value);

protected Q_SLOTS:
    void selectButtonClicked() override;

protected:
    bool eventFilter(QObject *o, QEvent *event) override;

private:
    Q_DISABLE_COPY(KPropertyComposedUrlEditor)
    QScopedPointer<KPropertyUrlEditorPrivate> const d;
};

#endif // KPROPERTYCOMPOSEDURLEDITOR_H
