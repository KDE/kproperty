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

#include "KPropertyGenericSelectionEditor.h"

#include <QHBoxLayout>
#include <QPushButton>

class KPropertyGenericSelectionEditor::Private
{
public:
    Private() {
    }
    QHBoxLayout *lyr;
    QPushButton *btn;
};

KPropertyGenericSelectionEditor::KPropertyGenericSelectionEditor(QWidget *parent)
        : QWidget(parent), d(new Private)
{
    setAutoFillBackground(true);
    d->lyr = new QHBoxLayout(this);
    d->lyr->setContentsMargins(0, 0, 0, 0);
    d->lyr->setSpacing(2);
    d->btn = new QPushButton(tr("...", "... button"));
    d->btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    d->btn->setFixedWidth(d->btn->fontMetrics().width(d->btn->text() + QLatin1String("   ")));
    d->btn->setFocusPolicy(Qt::NoFocus);
    connect(d->btn, &QPushButton::clicked, this, &KPropertyGenericSelectionEditor::selectButtonClicked);
    d->lyr->addWidget(d->btn);
}

KPropertyGenericSelectionEditor::~KPropertyGenericSelectionEditor()
{
}

void KPropertyGenericSelectionEditor::setMainWidget(QWidget *widget)
{
    if (d->lyr->count() > 1) {
        delete d->lyr->takeAt(0)->widget();
    }
    if (widget) {
        d->lyr->insertWidget(0, widget);
        widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        setFocusProxy(widget);
    }
}

void KPropertyGenericSelectionEditor::setSelectionButtonVisible(bool set)
{
    d->btn->setVisible(set);
}

void KPropertyGenericSelectionEditor::selectButtonClicked()
{
}
