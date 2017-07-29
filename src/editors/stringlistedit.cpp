/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>

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

#include "stringlistedit.h"

#include <QLineEdit>
#include <QDialog>
#include <QPainter>
#include <QVariant>
#include <QPushButton>
#include <QHBoxLayout>

#include "KProperty.h"

class Q_DECL_HIDDEN KPropertyStringListEditor::Private
{
public:
    Private() {
    }
    QLineEdit *edit;
    QStringList list;
    QPushButton *selectButton;
};

KPropertyStringListEditor::KPropertyStringListEditor(KProperty *property, QWidget *parent)
        : Widget(property, parent), d(new Private)
{
    setHasBorders(false);
    QHBoxLayout *l = new QHBoxLayout(this);
    l->setMargin(0);
    l->setSpacing(0);

    d->edit = new QLineEdit(this);
    d->edit->setReadOnly(true);
    d->edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->edit->setMinimumHeight(5);
    l->addWidget(d->edit);

    d->selectButton = new QPushButton(this);
    Utils::setupDotDotDotButton(d->selectButton, tr("Select item"),
        tr("Selects one item"));

    d->selectButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    l->addWidget(d->selectButton);
    setFocusWidget(d->selectButton);

    connect(d->selectButton, SIGNAL(clicked()), this, SLOT(showEditor()));
}

KPropertyStringListEditor::~KPropertyStringListEditor()
{
    delete d;
}

QVariant
KPropertyStringListEditor::value() const
{
    return d->list;
}

void
KPropertyStringListEditor::setValue(const QVariant &value, bool emitChange)
{
    d->list = value.toStringList();
    d->edit->setText(value.toStringList().join(", "));
    if (emitChange)
        emit valueChanged(this);
}

void
KPropertyStringListEditor::drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value)
{
    Widget::drawViewer(p, cg, r, value.toStringList().join(", "));
}

void
KPropertyStringListEditor::showEditor()
{
    QDialog dialog(this->topLevelWidget());
    dialog.setWindowTitle(tr("Edit List of Items", "Window title"));
    dialog.setObjectName("stringlist_dialog");
    dialog.setButtons(QDialog::Ok | QDialog::Cancel);
    dialog.setDefaultButton(QDialog::Ok);
    dialog.setModal(false);
    KEditListBox *edit = new KEditListBox(tr("Contents of %1").arg(property()->captionOrName()),
                                          &dialog, "editlist");
    // PORTING: Verify that widget was added to mainLayout:     dialog.setMainWidget(edit);
    // Add mainLayout->addWidget(edit); if necessary
    edit->insertStringList(d->list);

    if (dialog.exec() == QDialog::Accepted) {
        d->list = edit->items();
        d->edit->setText(d->list.join(", "));
        emit valueChanged(this);
    }
}

void
KPropertyStringListEditor::setReadOnlyInternal(bool readOnly)
{
    d->selectButton->setEnabled(!readOnly);
}
