/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexipropertyeditorview.h"
#include "keximainwindow.h"

#include <klocale.h>

#include <qlayout.h>


KexiPropertyEditorView::KexiPropertyEditorView(KexiMainWindow *mainWin)
	: KexiViewBase(mainWin, mainWin, "KexiPropertyEditorView")
{
	setCaption(i18n("Properties"));
	//TODO: set a nice icon
	setIcon(*mainWin->icon());

	QHBoxLayout *lyr = new QHBoxLayout(this);
	m_editor = new KexiPropertyEditor(this, true /*AutoSync*/, "propeditor");
	lyr->addWidget(m_editor);
	setFocusProxy(m_editor);
}

KexiPropertyEditorView::~KexiPropertyEditorView()
{
}

QSize KexiPropertyEditorView::sizeHint() const
{
	return QSize(200,200);//m_editor->sizeHint();
}

QSize KexiPropertyEditorView::minimumSizeHint() const
{
	return QSize(200,200);//m_editor->sizeHint();
}

void KexiPropertyEditorView::setGeometry ( const QRect &r )
{
	KexiViewBase::setGeometry(r);
}

void KexiPropertyEditorView::resize (  int w, int h  )
{
	KexiViewBase::resize( w, h );
}



#include "kexipropertyeditorview.moc"
