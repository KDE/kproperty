/* This file is part of the KDE project
   Copyright (C) 2018 Jarosław Staniek <staniek@kde.org>
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

#ifndef KPROPERTYCOMPOSEDURL_H
#define KPROPERTYCOMPOSEDURL_H

#include "kpropertycore_export.h"

#include <QDebug>
#include <QMetaType>
#include <QUrl>

/**
 * @brief A data structure that composes absolute and relative URLs.
 *
 * @since 3.2
 */
class KPROPERTYCORE_EXPORT KPropertyComposedUrl
{
public:
    /**
     * Constructs empty and invalid composed URL
     */
    KPropertyComposedUrl();

    /**
     * Constructs composed URL with specified base URL and path relative to the base URL
     *
     * @a baseUrl must be a valid absolute URL, otherwise the entire composed URL is invalid.
     * @a relativePath must be proper relative path. It is not important if it points to existing
     *    filesystem object.
     */
    KPropertyComposedUrl(const QUrl &baseUrl, const QString &relativePath = QString());

    /**
     * Constructs composed URL with specified base URL and independent absolute URL
     *
     * @a baseUrl must be absolute or empty URL, otherwise the entire composed URL is invalid.
     * @a absoluteUrl must be absolute URL. It is not important if it points to existing filesystem
     * object.
     *
     * Because @a absoluteUrl carries all needed information, @a baseUrl is ignored in this variant
     * of constructor. However @a baseUrl is still remembered and will be used if this object's
     * value will be changed to relative path using setRelativePath().
     */
    KPropertyComposedUrl(const QUrl &baseUrl, const QUrl &absoluteUrl);

    /**
     *  Constructs a copy of @a other
     */
    KPropertyComposedUrl(const KPropertyComposedUrl &other);

    ~KPropertyComposedUrl();

    /**
     *  Assigns @a other to this KPropertyComposedUrl
     */
    KPropertyComposedUrl &operator=(const KPropertyComposedUrl &other);

    /**
     * Return @c true if this KPropertyComposedUrl equals to @a other
     *
     * Two KPropertyComposedUrl objects are equal if they have the same values.
     * @see value()
     */
    bool operator==(const KPropertyComposedUrl &other) const;

    /**
     * Return @c true if this KPropertyComposedUrl does not equal to @a other
     */
    bool operator!=(const KPropertyComposedUrl &other) const { return !operator==(other); }

    /**
     * Returns URL value computed from base URL, absolute URL and relative path, whichever is
     * defined.
     *
     * - If the KPropertyComposedUrl is invalid, empty QUrl is returned
     * - If absolute path is present, it is returned
     * - If base URL and relative path are present, URL with added relative path is returned
     */
    QUrl value() const;

    /**
     * Returns the base URL (absolute) or empty URL if there is no absolute base URL specified
     */
    QUrl baseUrl() const;

    /**
     * Sets base URL (absolute)
     *
     * If @a baseUrl is not valid and absolute, base URL is cleared.
     * In any case relative path (relativePath()) and absolute URL (absoluteUrl()) are not modified.
     */
    void setBaseUrl(const QUrl &url);

    /**
     * Returns absolute URL that has been set for this object
     *
     * Empty URL is returned if absolute URL is not specified. In this case relative path can still
     * be present. If absolute URL is present, relative path is empty.
     */
    QUrl absoluteUrl() const;

    /**
     * Sets a new absolute URL
     *
     * If @a absoluteUrl is not valid and absolute, it is cleared.
     * If @a absoluteUrl is absolute, relative path (relativePath()) is cleared.
     */
    void setAbsoluteUrl(const QUrl &absoluteUrl);

    /**
     * Returns relative path that has been set for this object
     *
     * Empty string is returned if there is no path assigned. Non-empty path can be used to
     * calculate value by concatenating it with the base URL. If relativePath() is present
     * absoluteUrl() is empty .
     */
    QString relativePath() const;

    /**
     * Sets a new relative path for this object
     *
     * If @a relativePath is not a valid or empty path, it is cleared.
     * If @a relativePath is empty or valid, absolute URL is cleared.
     */
    void setRelativePath(const QString &relativePath);

    /**
     * Return @c true if the URL value that can be computed (from base URL, absolute URL and
     * relative path) is valid
     *
     * @see value() for description of how value is computed.
     */
    bool isValid() const;

private:
    class Private;
    Private *const d;
};

Q_DECLARE_METATYPE(KPropertyComposedUrl)

//! qDebug() stream operator. Writes KPropertyComposedUrl to the debug output.
KPROPERTYCORE_EXPORT QDebug operator<<(QDebug dbg, const KPropertyComposedUrl &p);

#endif // KPROPERTYCOMPOSEDURL_H
