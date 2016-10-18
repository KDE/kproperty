/* This file is part of the KDE project
   Copyright (C) 2016 Jarosław Staniek <staniek@kde.org>

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

#include "KPropertyMultiLineStringEditor.h"
#include "KPropertyEditorView.h"
#include <QScrollBar>

class Q_DECL_HIDDEN KPropertyMultiLineStringEditor::Private
{
public:
    Private() {}
    bool slotTextChangedEnabled = true;
};

KPropertyMultiLineStringEditor::KPropertyMultiLineStringEditor(QWidget *parent)
 : QPlainTextEdit(parent)
 , d(new Private)
{
    setFrameStyle(0);
    setTabChangesFocus(true);
    setContentsMargins(0,0,0,0);
    document()->setDocumentMargin(1);
    connect(this, &QPlainTextEdit::textChanged, this, &KPropertyMultiLineStringEditor::slotTextChanged);
    setViewportMargins(2, 1, 0, 0);
    verticalScrollBar()->installEventFilter(this);
}

KPropertyMultiLineStringEditor::~KPropertyMultiLineStringEditor()
{
    delete d;
}

QString KPropertyMultiLineStringEditor::value() const
{
    return toPlainText();
}

void KPropertyMultiLineStringEditor::setValue(const QString& value)
{
    d->slotTextChangedEnabled = false;
    setPlainText(value);
    d->slotTextChangedEnabled = true;
}

void KPropertyMultiLineStringEditor::slotTextChanged()
{
    if (!d->slotTextChangedEnabled) {
        return;
    }
    emit commitData(this);
}

bool KPropertyMultiLineStringEditor::eventFilter(QObject *o, QEvent *ev)
{
    const bool result = QPlainTextEdit::eventFilter(o, ev);
    if (o == verticalScrollBar() && ev->type() == QEvent::Paint) {
        KPropertyWidgetsFactory::paintTopGridLine(qobject_cast<QWidget*>(o));
    }
    return result;
}
