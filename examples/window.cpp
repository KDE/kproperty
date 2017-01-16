/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2008-2017 Jarosław Staniek <staniek@kde.org>

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

#include "window.h"

#include <KProperty>
#include <KPropertyEditorView>
#include <KPropertyUnit_p.h>

#include <QDate>
#include <QDateTime>
#include <QPixmap>
#include <QStringList>
#include <QCursor>
#include <QApplication>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QIcon>
#include <QCommandLineParser>
#include <QDebug>
#include <QDate>
#include <QCheckBox>
#include <QUrl>
#include <QDir>

Window::Window()
        : QWidget()
        , m_set(this)
        , m_flatOption("flat", QCoreApplication::translate("main",
                       "Flat display: do not display groups\n(useful for testing)"))
        , m_fontSizeOption("font-size", QCoreApplication::translate("main",
                           "Set font size to <size> (in points)\n(useful for testing whether editors keep the font settings)"),
                           QCoreApplication::translate("main", "size"))
        , m_propertyOption("property", QCoreApplication::translate("main",
                           "Display only specified property\n(useful when we want to focus on testing a single\nproperty editor)"),
                           QCoreApplication::translate("main", "name"))
        , m_roOption("ro", QCoreApplication::translate("main",
                     "Set all properties as read-only:\n(useful for testing read-only mode)"))
{
    setObjectName("kpropertyexamplewindow");
    setWindowIcon(QIcon::fromTheme("document-properties"));
    m_parser.setApplicationDescription(QCoreApplication::translate("main", "An example application for the KProperty library."));
    m_parser.addHelpOption();
    m_parser.addVersionOption();
    parseCommandLine();

    const QString singleProperty = m_parser.value(m_propertyOption);
    bool ok;
    const int fontSize = m_parser.value(m_fontSizeOption).toInt(&ok);
    if (fontSize > 0 && ok) {
        QFont f(font());
        f.setPointSize(fontSize);
        setFont(f);
    }

    /*  First, create a KPropertySet which will hold the properties.  */
    KProperty *p = 0;
    m_set.setReadOnly(m_parser.isSet(m_roOption));
    QByteArray group;
    const bool addGroups = !m_parser.isSet(m_flatOption);
    if (addGroups) {
        group = "SimpleGroup";
        m_set.setGroupCaption(group, "Simple Group");
    }
    if (singleProperty.isEmpty() || singleProperty=="String") {
        m_set.addProperty(p = new KProperty("String", "String"), group);
        p->setValueSyncPolicy(KProperty::ValueSyncPolicy::Auto);
        p->setReadOnly(false); // this should not work:
                               // - not needed if the property set is read-write
                               // - ignored if the property set is read-only
    }
    if (singleProperty.isEmpty() || singleProperty=="MultiLine") {
        m_set.addProperty(p = new KProperty("MultiLine", "Multi\nLine\nContent"), group);
        p->setValueSyncPolicy(KProperty::ValueSyncPolicy::Auto);
        p->setOption("multiLine", true);
    }

    if (singleProperty.isEmpty() || singleProperty=="Int") {
        m_set.addProperty(new KProperty("Int", 2, "Int"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="Double") {
        m_set.addProperty(p = new KProperty("Double", 3.14159, "Double"), group);
        p->setOption("precision", 4); // will round to 3.1416
    }
    if (singleProperty.isEmpty() || singleProperty=="cm") {
        const qreal cm = 1.0; // 28.3465058 points
        const qreal points = KPropertyUnit(KPropertyUnit::Centimeter).fromUserValue(cm);
        m_set.addProperty(p = new KProperty("cm", points, "Double cm"), group);
        p->setOption("unit", "cm");
        // default precision == 2
    }
    if (singleProperty.isEmpty() || singleProperty=="Bool") {
        m_set.addProperty(new KProperty("Bool", QVariant(true), "Bool"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="NullBool") {
        m_set.addProperty(new KProperty("NullBool", QVariant(), "Null Bool", QString(), KProperty::Bool), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="3-State") {
        m_set.addProperty(p = new KProperty("3-State", QVariant(), "3 States", QString(),
                                            KProperty::Bool), group);
        p->setOption("3State", true);
    }
    if (singleProperty.isEmpty() || singleProperty=="Date") {
        m_set.addProperty(p = new KProperty("Date", QDate::currentDate(), "Date"), group);
        p->setIconName("date");
    }
    if (singleProperty.isEmpty() || singleProperty=="Time") {
        m_set.addProperty(new KProperty("Time", QTime::currentTime(), "Time"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="DateTime") {
        m_set.addProperty(new KProperty("DateTime", QDateTime::currentDateTime(), "Date/Time"), group);
    }

    QStringList name_list; //strings
    name_list << "My Item" << "Other Item" << "Third Item";
    if (singleProperty.isEmpty() || singleProperty=="List") {
        QStringList list;//keys
        list << "myitem" << "otheritem" << "3rditem";
        m_set.addProperty(new KProperty("List", list, name_list, "otheritem", "List"), group);
    }

    if (singleProperty.isEmpty() || singleProperty=="List2") {
        // A valueFromList property matching strings with ints (could be any type supported by QVariant)
        QList<QVariant> keys;
        keys.append(1);
        keys.append(2);
        keys.append(3);
        KPropertyListData *listData = new KPropertyListData(keys, name_list);
        m_set.addProperty(new KProperty("List2", listData, 3, "List 2"), group);
    }

//  Complex
    if (addGroups) {
        group = "ComplexGroup";
        m_set.setGroupCaption(group, "Complex Group");
    }
    if (singleProperty.isEmpty() || singleProperty=="Rect") {
        m_set.addProperty(new KProperty("Rect", QRect(5,11,100,200), "Rect"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="Point") {
        m_set.addProperty(new KProperty("Point", QPoint(3, 4), "Point"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="Size") {
        m_set.addProperty(new KProperty("Size", QSize(10, 20), "Size"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="RectF") {
        m_set.addProperty(new KProperty("RectF", QRectF(0.1, 0.5, 10.72, 18.21), "RectF"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="PointF") {
        m_set.addProperty(new KProperty("PointF", QPointF(3.14, 4.15), "PointF"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="SizeF") {
        m_set.addProperty(new KProperty("SizeF", QSizeF(1.1, 2.45), "SizeF"), group);
    }

//  Appearance
    if (addGroups) {
        group = "Appearance Group";
        m_set.setGroupCaption(group, "Appearance Group");
        m_set.setGroupIconName(group, "appearance");
    }
    if (singleProperty.isEmpty() || singleProperty=="Color") {
        m_set.addProperty(new KProperty("Color", palette().color(QPalette::Active, QPalette::Background), "Color"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="Pixmap") {
        QPixmap pm(QIcon::fromTheme("network-wired").pixmap(QSize(16,16)));
        m_set.addProperty(p = new KProperty("Pixmap", pm, "Pixmap"), group);
        p->setIconName("kpaint");
    }
    if (singleProperty.isEmpty() || singleProperty=="Font") {
        QFont myFont("Times New Roman", 12);
        myFont.setUnderline(true);
        m_set.addProperty(p = new KProperty("Font", myFont, "Font"), group);
        p->setIconName("fonts");
    }
    if (singleProperty.isEmpty() || singleProperty=="Cursor") {
        m_set.addProperty(new KProperty("Cursor", QCursor(Qt::WaitCursor), "Cursor"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="LineStyle") {
        m_set.addProperty(new KProperty("LineStyle", 3, "Line Style", QString(),
                                        KProperty::LineStyle), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="SizePolicy") {
        QSizePolicy sp(sizePolicy());
        sp.setHorizontalStretch(1);
        sp.setVerticalStretch(2);
        m_set.addProperty(new KProperty("SizePolicy", sp, "Size Policy"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="Invisible") {
        m_set.addProperty(p = new KProperty("Invisible", "I am invisible", "Invisible"), group);
        p->setVisible(false);
    }
    if (singleProperty.isEmpty() || singleProperty=="Url") {
        m_set.addProperty(p = new KProperty("Url", QUrl("https://community.kde.org/KProperty"), "Url"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="ExistingFile") {
        m_set.addProperty(p = new KProperty("ExistingFile", QUrl::fromLocalFile(QDir::homePath()), "Existing File"), group);
        p->setOption("fileMode", "existingFile");
    }
    if (singleProperty.isEmpty() || singleProperty=="OverwriteFile") {
        m_set.addProperty(p = new KProperty("OverwriteFile", QUrl::fromLocalFile(QDir::homePath()), "Overwrite File"), group);
        p->setOption("fileMode", "existingFile");
        p->setOption("confirmOverwrites", true);
    }
    if (singleProperty.isEmpty() || singleProperty=="Dir") {
        m_set.addProperty(p = new KProperty("Dir", QUrl::fromLocalFile(QDir::homePath()), "Dir"), group);
        p->setOption("fileMode", "dirsOnly");
    }
    if (singleProperty.isEmpty() || singleProperty=="ReadOnly") {
        m_set.addProperty(p = new KProperty("Read-Only", "Read-only string"), group);
        p->setReadOnly(true);
    }

    QVBoxLayout *lyr = new QVBoxLayout(this);
    m_editorView = new KPropertyEditorView(this);
    lyr->addWidget(m_editorView);
    m_editorView->changeSet(&m_set);
    lyr->addSpacing(lyr->spacing());

    QHBoxLayout *hlyr = new QHBoxLayout;
    lyr->addLayout(hlyr);

    QCheckBox *showGrid = new QCheckBox("Show grid");
    showGrid->setChecked(true);
    connect(showGrid, &QCheckBox::stateChanged, this, &Window::showGrid);
    hlyr->addWidget(showGrid);

    QCheckBox *showFrame = new QCheckBox("Show frame");
    showFrame->setChecked(true);
    connect(showFrame, &QCheckBox::stateChanged, this, &Window::showFrame);
    hlyr->addWidget(showFrame);

    QCheckBox *showGroups = new QCheckBox("Show groups");
    if (addGroups) {
        connect(showGroups, &QCheckBox::toggled, m_editorView, &KPropertyEditorView::setGroupsVisible);
        showGroups->setChecked(m_editorView->groupsVisible());
    } else {
        showGroups->setEnabled(false);
    }
    hlyr->addWidget(showGroups);

    QCheckBox *readOnly = new QCheckBox("Read-only");
    connect(readOnly, &QCheckBox::toggled, &m_set, &KPropertySet::setReadOnly);
    readOnly->setChecked(m_set.isReadOnly());
    hlyr->addWidget(readOnly);

    hlyr->addStretch(1);
    resize(400, qApp->desktop()->height() - 200);
    m_editorView->setFocus();
    qDebug() << m_set;
}

Window::~Window()
{
}

void Window::parseCommandLine()
{
    m_parser.addOption(m_flatOption);
    m_parser.addOption(m_fontSizeOption);
    m_parser.addOption(m_propertyOption);
    m_parser.addOption(m_roOption);
    m_parser.process(*(QCoreApplication::instance()));
}

void Window::showGrid(int state)
{
    m_editorView->setGridLineColor(
        state == Qt::Checked ? KPropertyEditorView::defaultGridLineColor() : QColor());
}

void Window::showFrame(int state)
{
    m_editorView->setFrameStyle(state == Qt::Checked ? QFrame::Box : QFrame::NoFrame);
}
