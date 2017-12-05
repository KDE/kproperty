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
#include <KPropertyListData>
#include <KPropertyEditorView>

#include <QDate>
#include <QDateTime>
#include <QPixmap>
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
    KProperty *p = nullptr;
    m_set.setReadOnly(m_parser.isSet(m_roOption));
    QByteArray group;
    const bool addGroups = !m_parser.isSet(m_flatOption);
    if (addGroups) {
        group = "BasicGroup";
        m_set.setGroupCaption(group, "Basic Properties");
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

    QStringList name_list({"My Item", "Other Item", "Third Item"}); //strings
    if (singleProperty.isEmpty() || singleProperty=="List") {
        const QStringList keys({"myitem", "otheritem", "3rditem"});
        KPropertyListData *listData = new KPropertyListData(keys, name_list);
        m_set.addProperty(new KProperty("List", listData, "otheritem", "List"), group);
    }

    if (singleProperty.isEmpty() || singleProperty=="List2") {
        // A valueFromList property matching strings with ints (could be any type supported by QVariant)
        QVariantList keys({1, 2, 3});
        KPropertyListData *listData = new KPropertyListData;
        listData->setKeys(keys);
        listData->setNamesAsStringList(name_list);
        m_set.addProperty(new KProperty("List2", listData, 3, "List 2"), group);
    }

//  Complex
    if (addGroups) {
        group = "ComposedGroup";
        m_set.setGroupCaption(group, "Composed Properties");
    }
    if (singleProperty.isEmpty() || singleProperty=="Point") {
        m_set.addProperty(new KProperty("Point", QPoint(3, 4), "Point"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="Size") {
        m_set.addProperty(new KProperty("Size", QSize(10, 20), "Size"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="Rect") {
        m_set.addProperty(new KProperty("Rect", QRect(5,11,100,200), "Rect"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="PointF") {
        m_set.addProperty(new KProperty("PointF", QPointF(3.14, 4.15), "PointF"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="SizeF") {
        m_set.addProperty(new KProperty("SizeF", QSizeF(1.1, 2.45), "SizeF"), group);
    }
    if (singleProperty.isEmpty() || singleProperty == "RectF") {
        m_set.addProperty(
            new KProperty("RectF", QRectF(0.1, 0.5, 10.72, 18.21), "RectF"), group);
    }

//  With suffixes and prefixes
    if (addGroups) {
        group = "PrefixesSuffixesGroup";
        m_set.setGroupCaption(group, "Prefixes & Suffixes");
    }
    if (singleProperty.isEmpty() || singleProperty=="dollars") {
        m_set.addProperty(p = new KProperty("dollars", 100, "Dollars"), group);
        p->setOption("prefix", "$");
    }
    if (singleProperty.isEmpty() || singleProperty == "billions") {
        m_set.addProperty(p = new KProperty("billions", 5.0, "Billions"), group);
        p->setOption("prefix", "£");
        p->setOption("suffix", "bn");
        // default precision == 2 and step == 0.01
    }
    if (singleProperty.isEmpty() || singleProperty == "PointF-mm") {
        m_set.addProperty(
            p = new KProperty("PointF-mm", QPointF(2.5, 3.5), "PointF [mm]"), group);
        p->setOption("suffix", "mm");
        p->setOption("step", 0.1);
        p->setOption("precision", 2);
    }
    if (singleProperty.isEmpty() || singleProperty == "SizeF-dm") {
        m_set.addProperty(
            p = new KProperty("SizeF-dm", QSizeF(7.0, 6.5), "SizeF [dm]"), group);
        p->setOption("suffix", "dm");
        p->setOption("step", 0.001);
        p->setOption("precision", 3);
    }
    if (singleProperty.isEmpty() || singleProperty=="RectF-px") {
        m_set.addProperty(
            p = new KProperty("RectF-px", QRectF(21.2, 22.2, 9.1, 1.0), "RectF [px]"),
            group);
        p->setOption("suffix", "px");
        p->setOption("step", 0.1);
        p->setOption("precision", 1);
    }

//  Limits
    if (addGroups) {
        group = "LimitsGroup";
        m_set.setGroupCaption(group, "Limits");
    }
    if (singleProperty.isEmpty() || singleProperty=="uint") {
        m_set.addProperty(new KProperty("uint", 3, "Unsigned Int", QString(), KProperty::UInt), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="uint+mintext") {
        m_set.addProperty(p = new KProperty("uint+mintext", 0, "Unsigned Int\n+min.text", QString(), KProperty::UInt), group);
        p->setOption("minValueText", "Minimum value");
    }
    if (singleProperty.isEmpty() || singleProperty=="limit0_10") {
        m_set.addProperty(p = new KProperty("limit0_10", 9, "0..10", QString(), KProperty::UInt), group);
        p->setOption("max", 10);
    }
    if (singleProperty.isEmpty() || singleProperty == "limit1_5+mintext") {
        // This is a good test for fixing value out of range: "Could not assign value 9 larger than
        // maximum 5 -- setting to 5" warning is displayed and 5 is assigned.
        m_set.addProperty(p = new KProperty("limit1_5+mintext", 9, "1..5\n+mintext", QString(),
                                            KProperty::UInt), group);
        p->setOption("min", 1);
        p->setOption("max", 5);
        p->setOption("minValueText", "Minimum value");
    }
    if (singleProperty.isEmpty() || singleProperty=="negative-int") {
        m_set.addProperty(p = new KProperty("negative-int", -2, "Negative Int", QString(), KProperty::Int), group);
        p->setOption("max", -1);
    }
    if (singleProperty.isEmpty() || singleProperty=="double-unlimited") {
        m_set.addProperty(p = new KProperty("double-unlimited", -1.11, "Double unlimited"), group);
        p->setOption("min", KPROPERTY_MIN_PRECISE_DOUBLE);
        p->setOption("precision", 1);
        p->setOption("step", 0.1);
    }
    if (singleProperty.isEmpty() || singleProperty=="double-negative") {
        m_set.addProperty(p = new KProperty("double-negative", -0.2, "Double negative"), group);
        p->setOption("min", KPROPERTY_MIN_PRECISE_DOUBLE);
        p->setOption("max", -0.1);
        p->setOption("precision", 1);
        p->setOption("step", 0.1);
    }
    if (singleProperty.isEmpty() || singleProperty=="double-minus1-0+mintext") {
        // This is a good test for fixing value out of range: "Could not assign value -9.0 smaller than
        // minimum -1.0 -- setting to -1.0" warning is displayed and -1.0 is assigned.
        m_set.addProperty(p = new KProperty("double-minus1-0+mintext", -9.0, "Double -1..0\n+mintext"), group);
        p->setOption("min", -1.0);
        p->setOption("max", 0.0);
        p->setOption("precision", 1);
        p->setOption("step", 0.1);
        p->setOption("minValueText", "Minimum value");
    }

//  Appearance
    if (addGroups) {
        group = "AppearanceGroup";
        m_set.setGroupCaption(group, "Appearance Group");
        m_set.setGroupIconName(group, "appearance");
    }
    if (singleProperty.isEmpty() || singleProperty=="Color") {
        m_set.addProperty(new KProperty("Color", palette().color(QPalette::Active, QPalette::Background), "Color"), group);
    }
    if (singleProperty.isEmpty() || singleProperty=="Pixmap") {
        QPixmap pm(QIcon::fromTheme("network-wired").pixmap(QSize(64, 64)));
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
        m_set.addProperty(p = new KProperty("ExistingFile",
            QUrl::fromLocalFile(QCoreApplication::applicationFilePath()), "Existing\nFile"), group);
        p->setOption("fileMode", "existingFile");
    }
    if (singleProperty.isEmpty() || singleProperty=="OverwriteFile") {
        m_set.addProperty(p = new KProperty("OverwriteFile",
            QUrl::fromLocalFile(QDir::homePath() + "/" + QCoreApplication::applicationName() + ".txt"),
                                            "New File or\nOverwrite Existing"), group);
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
    m_editorView->resizeColumnToContents(0);
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
