/* This file is part of the KDE project
   Copyright (C) 2004-2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2008-2018 Jarosław Staniek <staniek@kde.org>

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

#ifndef WINDOW_H
#define WINDOW_H

#include <QCommandLineParser>
#include <QWidget>
#include <KPropertySet>

class KPropertyEditorView;

/*! @internal
 * @short KPropertyExample application's main window */
class Window : public QWidget
{
    Q_OBJECT
public:
    Window();
    ~Window() override;

protected Q_SLOTS:
    void showGrid(int state);
    void showFrame(int state);

    //! Reaction on changed value of property
    void propertyChanged(KPropertySet& set, KProperty& property);

private:
    void parseCommandLine();

    KPropertySet m_set;
    QCommandLineParser m_parser;
    QCommandLineOption m_flatOption;
    QCommandLineOption m_fontSizeOption;
    QCommandLineOption m_propertyOption;
    QCommandLineOption m_roOption;
    KPropertyEditorView *m_editorView;
    KProperty *m_dynamicToolTipProperty;
};

#endif
