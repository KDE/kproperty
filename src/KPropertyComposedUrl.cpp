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

#include "KPropertyComposedUrl.h"

#include <tuple> // std::tie

class Q_DECL_HIDDEN KPropertyComposedUrl::Private
{
public:
    Private() {}
    Private(const Private &other) { copy(other); }
    Private(const QUrl &_baseUrl, const QUrl &_url)
        : baseUrl(_baseUrl)
        , url(_url)
    {
    }

#define KPropertyComposedUrlPrivateArgs(o) std::tie(o.baseUrl, o.url)
    void copy(const Private &other)
    {
        KPropertyComposedUrlPrivateArgs((*this)) = KPropertyComposedUrlPrivateArgs(other);
    }

    bool operator==(const Private &other) const
    {
        return KPropertyComposedUrlPrivateArgs((*this)) == KPropertyComposedUrlPrivateArgs(other);
    }
#undef KPropertyComposedUrlPrivateArgs

    //! Base URL for relative URLs, not used if 'url' is absolute but still stored
    //! as it will be needed as soon as 'url' is changed to relative
    QUrl baseUrl;
    //! contains an absolute URL, or just a path for relative URLs
    QUrl url;
};

KPropertyComposedUrl::KPropertyComposedUrl()
    : d(new Private)
{
}

KPropertyComposedUrl::KPropertyComposedUrl(const QUrl &baseUrl, const QString &relativePath)
    : KPropertyComposedUrl()
{
    setBaseUrl(baseUrl);
    setRelativePath(relativePath);
}

KPropertyComposedUrl::KPropertyComposedUrl(const QUrl &baseUrl, const QUrl &absoluteUrl)
    : KPropertyComposedUrl()
{
    setBaseUrl(baseUrl);
    setAbsoluteUrl(absoluteUrl);
}

KPropertyComposedUrl::KPropertyComposedUrl(const KPropertyComposedUrl &other)
    : d(new Private(*other.d))
{
}

KPropertyComposedUrl::~KPropertyComposedUrl()
{
    delete d;
}

KPropertyComposedUrl &KPropertyComposedUrl::operator=(const KPropertyComposedUrl &other)
{
    if (this != &other) {
        d->copy(*other.d);
    }
    return *this;
}

bool KPropertyComposedUrl::operator==(const KPropertyComposedUrl &other) const
{
    return *d == *other.d;
}

QUrl KPropertyComposedUrl::value() const
{
    if (!isValid()) {
        return QUrl();
    }

    if (d->url.isRelative()) {
        QUrl baseUrl = d->baseUrl;

        //! append necessary '/'
        if (!baseUrl.path().endsWith(QLatin1String("/"))) {
            baseUrl.setPath(baseUrl.path() + QLatin1String("/"));
        }

        return baseUrl.resolved(d->url);
    } else {
        return d->url;
    }
}

QUrl KPropertyComposedUrl::baseUrl() const
{
    return d->baseUrl;
}

void KPropertyComposedUrl::setBaseUrl(const QUrl &url)
{
    if (!url.isValid() || url.isRelative()) {
        d->baseUrl.clear();
        return;
    }

    d->baseUrl = url;
}

QUrl KPropertyComposedUrl::absoluteUrl() const
{
    return d->url.isRelative() ? QUrl() : d->url;
}

void KPropertyComposedUrl::setAbsoluteUrl(const QUrl &absoluteUrl)
{
    d->url.clear();

    if (absoluteUrl.isValid() && !absoluteUrl.isRelative()) {
        d->url = absoluteUrl;
    }
}

QString KPropertyComposedUrl::relativePath() const
{
    return d->url.isRelative() ? d->url.path() : QString();
}

void KPropertyComposedUrl::setRelativePath(const QString &relativePath)
{
    d->url.clear();

    if (!relativePath.isEmpty()) {
        d->url.setPath(relativePath);
    }
}

bool KPropertyComposedUrl::isValid() const
{
    // we should have both URLs anyways
    return d->baseUrl.isValid() && d->url.isValid();
}

QDebug operator<<(QDebug dbg, const KPropertyComposedUrl &p)
{
    dbg.nospace() << "KPropertyComposedUrl("
                  << "baseUrl=" << p.baseUrl() << " relativePath=" << p.relativePath()
                  << " absoluteUrl=" << p.absoluteUrl() << ")";
    return dbg.space();
}
