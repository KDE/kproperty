/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#include <QFont>
#include <QApplication>
#include <QCommandLineParser>

#include "window.h"

static const char description[] = "An example application for the KProperty library";
static const char version[] = "0.2";

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("KPropertyExample");
    QCoreApplication::setApplicationVersion(version);
    QCommandLineParser parser;

    QCommandLineOption flatOption("flat", QCoreApplication::translate("main",
        "Flat display: do not display groups\n(useful for testing)"));
    parser.addOption(flatOption);

    QCommandLineOption fontSizeOption("font-size", QCoreApplication::translate("main",
        "Set font size to <size> (in points)\n(useful for testing whether editors keep the font settings)"));
    parser.addOption(fontSizeOption);

    QCommandLineOption propertyOption("property", QCoreApplication::translate("main",
        "Display only specified property\n(useful when we want to focus on testing a single\nproperty editor)"));
    parser.addOption(propertyOption);

    QCommandLineOption roOption("ro", QCoreApplication::translate("main",
        "Set all properties as read-only:\n(useful for testing read-only mode)"));
    parser.addOption(roOption);

    parser.process(app);

    Window window;

    bool ok;
    const int fontSize = parser.value("font-size").toInt(&ok);
    if (fontSize > 0 && ok) {
        QFont f(window.font());
        f.setPointSize(fontSize);
        window.setFont(f);
    }
    window.show();
    return app.exec();
}

