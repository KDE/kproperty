/* This file is part of the KDE project
   Copyright (C) 2018 Jarosław Staniek <staniek@kde.org>

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

#ifndef KPROPERTYEDITORITEMEVENT_H
#define KPROPERTYEDITORITEMEVENT_H

#include "kpropertywidgets_export.h"

#include <QVariant>

#include <QScopedPointer>

class KProperty;
class QVariant;

/**
 * @brief The KPropertyEditorItemEvent class describes request for handling a single property
 *        editor widget item's event
 *
 * Handling editor events is useful to customize behavior of the property editors.
 *
 * For example Url property editor can offer overriding the default file dialog.
 *
 * Currently supported events are:
 *  - "getOpenFileUrl" for editors of KProperty::Url. Overrides the default file dialog for files
 *    or directories. For this event own file dialog can be displayed. In this case event's result
 *    should be set to result of the file dialog. Note that even if the dialog has been canceled
 *    result should be set (to empty QUrl), otherwise the property editor will not "realize" that
 *    the dialog has been overriden and will display the built-in dialog.
 *    It is recommended to take values of the "fileMode" and "confirmOverwrites" property options
 *    into account and display file dialog in appropriate mode. See KProperty::setOption() for
 *    documentation of property options.
 *    This event provides the following parameters: "url" equal to QUrl value of the editor for use
 *    in the dialog as default value; "caption" equal to the default window's caption for
 *    the dialog, depending on variant of the dialog, e.g. "Select Existing File" if "fileMode"
 *    option is "existingfile".
 *
 * @see KPropertyEditorView::handlePropertyEditorItemEvent()
 *
 * @since 3.2
 */
class KPROPERTYWIDGETS_EXPORT KPropertyEditorItemEvent
{
public:
    /**
     * Creates a new request object.
     *
     * @param property Property related to the request
     * @param name Name of the event
     * @param parameters Parameters of the event
     */
    KPropertyEditorItemEvent(const KProperty &property, const QString &name,
                             const QVariantMap &parameters);

    ~KPropertyEditorItemEvent();

    /**
     * @brief Returns property assiciated with editor which requests the override
     */
    const KProperty *property() const;

    /**
     * @brief Returns name of event that requests the override
     */
    QString name() const;

    /**
     * @brief Returns zero or more parameters associated with this specific event
     *
     * See description of the KPropertyEditorItemEvent class for parameters supported by given
     * events.
     */
    QVariantMap parameters() const;

    /**
     * @brief Sets result of the event
     *
     * In order to override behavior of the editor users have to call this method even if the
     * result is null. Editor will note that and accept the override. For example, URL editor will
     * not attempt to display its built-in file dialog assuming that custom dialog has been used.
     *
     * Once setResult() is called for given event object, there is no way to unset the result.
     * it is only possible to replace the result with other.
     */
    void setResult(const QVariant &result);

    /**
     * @brief Returns result set by the user of the editor
     *
     * It is null until user sets result with setResult().
     *
     * @see hasResult()
     */
    QVariant result() const;

    /**
     * @brief Returns @c true if event's result has been set with setResult()
     *
     * @see result()
     */
    bool hasResult() const;

private:
    Q_DISABLE_COPY(KPropertyEditorItemEvent)
    class Private;
    QScopedPointer<Private> const d;
};

#endif
