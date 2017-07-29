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

#include "symbolcombo.h"

#include <QLineEdit>
#include <QPushButton>
#include <QPainter>
#include <QVariant>
#include <QHBoxLayout>

class Q_DECL_HIDDEN KPropertySymbolComboEditor::Private
{
public:
    Private() {
    }
    QLineEdit *edit;
    QPushButton *select;
};

KPropertySymbolComboEditor::KPropertySymbolComboEditor(KProperty *property, QWidget *parent)
        : Widget(property, parent), d(new Private)
{
    setHasBorders(false);
    QHBoxLayout *l = new QHBoxLayout(this);

    d->edit = new QLineEdit(this);
    d->edit->setReadOnly(true);
    d->edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->edit->setMinimumHeight(5);
    d->edit->setMaxLength(1);
    l->addWidget(d->edit);
    d->select = new QPushButton(this);
    Utils::setupDotDotDotButton(d->select, tr("Select symbol"),
        tr("Selects a symbol"));
    d->select->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
    d->select->setMinimumHeight(5);
    l->addWidget(d->select);

    connect(d->select, SIGNAL(clicked()), this, SLOT(selectChar()));
    connect(d->edit, SIGNAL(textChanged(const QString&)), this, SLOT(slotValueChanged(const QString&)));
}

KPropertySymbolComboEditor::~KPropertySymbolComboEditor()
{
    delete d;
}

QVariant
KPropertySymbolComboEditor::value() const
{
    if (!(d->edit->text().isNull()))
        return d->edit->text().at(0).unicode();
    else
        return 0;
}

void
KPropertySymbolComboEditor::setValue(const QVariant &value, bool emitChange)
{
    if (!(value.isNull()))
    {
        d->edit->blockSignals(true);
        d->edit->setText(QChar(value.toInt()));
        d->edit->blockSignals(false);
        if (emitChange)
            emit valueChanged(this);
    }
}

void
KPropertySymbolComboEditor::drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value)
{
    Widget::drawViewer(p, cg, r, QString(QChar(value.toInt())));
}

void
KPropertySymbolComboEditor::selectChar()
{
    QDialog dialog(this->topLevelWidget());
    dialog.setWindowTitle(tr("Select Character", "Window title"));
    dialog.setObjectName("charselect_dialog");
    dialog.setButtons(QDialog::Ok | QDialog::Cancel);
    dialog.setDefaultButton(QDialog::Ok);
    dialog.setModal(false);

    KCharSelect *select = new KCharSelect(&dialog);
    dialog.setObjectName("select_char");
//PORTING: Verify that widget was added to mainLayout:     dialog.setMainWidget(select);
// Add mainLayout->addWidget(select); if necessary

    if (!(d->edit->text().isNull()))
        select->setCurrentChar(d->edit->text().at(0));

    if (dialog.exec() == QDialog::Accepted)
        d->edit->setText(select->currentChar());
}

void
KPropertySymbolComboEditor::slotValueChanged(const QString&)
{
    emit valueChanged(this);
}

void
KPropertySymbolComboEditor::setReadOnlyInternal(bool readOnly)
{
    d->select->setEnabled(!readOnly);
}
