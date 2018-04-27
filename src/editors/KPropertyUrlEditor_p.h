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

#ifndef KPROPERTYURLEDITORPRIVATE_H
#define KPROPERTYURLEDITORPRIVATE_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QVariant>

class KPropertyGenericSelectionEditor;
class KProperty;

class QEvent;
class QLineEdit;
class QLocale;

/*!
 *  A private implementation of KPropertyUrlEditor and KPropertyComposedUrlEditor
 *
 *  @warning This file is not a part of public API and must not be used by clients
 */
class KPropertyUrlEditorPrivate : public QObject
{
    Q_OBJECT

public:
    KPropertyUrlEditorPrivate(KPropertyGenericSelectionEditor *editor, const KProperty &property);

    //! Save a new value as a variant. Additionally allow saving regular URLs
    //! if the underlying type is KPropertyComposedUrl
    void setValue(const QVariant &newValue);

    //! Update the line edit after setting a new value
    void updateLineEdit(const QString &textToDisplay);

    //! @return @c true is @a url is valid for the property options
    //! @param[in,out] url - the URL to check and update
    bool checkAndUpdate(QUrl *url) const;

    //! Get a new URL value either from a custom or the built-in dialog
    QUrl getUrl();

    //! Process an input event from the editor
    void processEvent(QObject *o, QEvent *event);

private:
    //! @return @c true if @a event is a key press event for Enter or Return key
    bool enterPressed(QEvent *event) const;

    QString fixUp(const QString &path, bool isRelative) const;

Q_SIGNALS:
    void commitData();

public:
    QVariant value;
    bool isComposedUrl;
    QString savedText;
    QLineEdit *lineEdit;
    QByteArray fileMode;
    bool confirmOverwrites;
    QByteArray propertyName;

private:
    KPropertyGenericSelectionEditor *const m_editor;
};

#endif
