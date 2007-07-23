/* Copyright (C) 2003-2006 Jesper K. Pedersen <blackie@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <config-kpa.h>
#ifdef HASKIPI
#include "Plugins/ImageCollection.h"
#include "MainWindow/Window.h"
#include "DB/ImageDB.h"
#include "Settings/SettingsData.h"
#include <qfileinfo.h>
#include "DB/ImageInfoList.h"
#include "DB/ImageInfo.h"

Plugins::ImageCollection::ImageCollection( Type tp )
    : _tp( tp )
{
}

QString Plugins::ImageCollection::name()
{
    // This doesn't really make much sence for selection and current album.
    return QString::null;
}

QString Plugins::ImageCollection::comment()
{
    return QString::null;
}

KUrl::List Plugins::ImageCollection::images()
{
    switch ( _tp ) {
    case CurrentAlbum:
        return stringListToUrlList( DB::ImageDB::instance()->currentScope( false ) );

    case CurrentSelection:
        return stringListToUrlList( MainWindow::Window::theMainWindow()->selected() );

    case SubClass:
        qFatal( "The subclass should implement images()" );
        return KUrl::List();
    }
    return KUrl::List();
}

KUrl::List Plugins::ImageCollection::imageListToUrlList( const DB::ImageInfoList& imageList )
{
    KUrl::List urlList;
    for( DB::ImageInfoListConstIterator it = imageList.constBegin(); it != imageList.constEnd(); ++it ) {
        KUrl url;
        url.setPath( (*it)->fileName() );
        urlList.append( url );
    }
    return urlList;
}

KUrl::List Plugins::ImageCollection::stringListToUrlList( const QStringList& list )
{
    KUrl::List urlList;
    for( QStringList::ConstIterator it = list.begin(); it != list.end(); ++it ) {
        KUrl url;
        url.setPath( *it );
        urlList.append( url );
    }
    return urlList;
}

KUrl Plugins::ImageCollection::path()
{
    return commonRoot();
}

KUrl Plugins::ImageCollection::commonRoot()
{
    QString imgRoot = Settings::SettingsData::instance()->imageDirectory();
    const KUrl::List imgs = images();
    if ( imgs.count() == 0 )
        return imgRoot;

    QStringList res = QStringList::split( QString::fromLatin1( "/" ), QFileInfo( imgs[0].path() ).absolutePath(), true );

    for( KUrl::List::ConstIterator it = imgs.begin(); it != imgs.end(); ++it ) {
        QStringList newRes;

        QStringList path = QFileInfo( (*it).path() ).dirPath( true ), true ).split( QString::fromLatin1( "/" ) );
        uint i = 0;
        for ( ; i < qMin( path.size(), res.size() ); ++i ) {
            if ( path[i] == res[i] )
                newRes.append( res[i] );
            else
                break;
        }
        res = newRes;
    }

    QString result = res.join( QString::fromLatin1( "/" ) );
    if ( result.left( imgRoot.length() ) != imgRoot ) {
        result = imgRoot;
    }

    KUrl url;
    url.setPath( result );
    return url;
}

KUrl Plugins::ImageCollection::uploadPath()
{
    return commonRoot();
}

KUrl Plugins::ImageCollection::uploadRoot()
{
    KUrl url;
    url.setPath( Settings::SettingsData::instance()->imageDirectory() );
    return url;
}

#endif // KIPI
