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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <config-kpa-kipi.h>
#ifdef HASKIPI
#include "Plugins/ImageInfo.h"
#include "DB/ImageDB.h"
#include "DB/ImageInfo.h"
#include "DB/Category.h"
//Added by qt3to4:
#include <Q3ValueList>
Plugins::ImageInfo::ImageInfo( KIPI::Interface* interface, const KUrl& url )
    : KIPI::ImageInfoShared( interface, url )
{
    _info = DB::ImageDB::instance()->info( _url.path() );
}

QString Plugins::ImageInfo::title()
{
    if ( _info )
        return _info->label();
    else
        return QString::null;
}

QString Plugins::ImageInfo::description()
{
    if ( _info )
        return _info->description();
    else
        return QString::null;
}

QMap<QString,QVariant> Plugins::ImageInfo::attributes()
{
    QMap<QString,QVariant> res;
    if ( _info ) {
        for( QMap<QString,Utilities::StringSet>::Iterator it = _info->_categoryInfomation.begin(); it != _info->_categoryInfomation.end(); ++it ) {
            res.insert( it.key(), QVariant( QStringList( it.value().toList() ) ) );
        }
    }

    // Flickr plug-in expects the item tags, so we better give them.
    QString text;
    Q3ValueList<DB::CategoryPtr> categories = DB::ImageDB::instance()->categoryCollection()->categories();
    QStringList tags;
    for( Q3ValueList<DB::CategoryPtr>::Iterator categoryIt = categories.begin(); categoryIt != categories.end(); ++categoryIt ) {
        QString categoryName = (*categoryIt)->name();
        if ( (*categoryIt)->doShow() ) {
            Utilities::StringSet items = _info->itemsOfCategory( categoryName );
            for( Utilities::StringSet::Iterator it = items.begin(); it != items.end(); ++it ) {
                tags.append( *it );
            }
        }
    }
    QString key = QString::fromLatin1( "tags" );
    res.insert( key, QVariant( tags ) );
    return res;
}

void Plugins::ImageInfo::setTitle( const QString& name )
{
    if ( _info )
        _info->setLabel( name );
}

void Plugins::ImageInfo::setDescription( const QString& description )
{
    if ( _info )
        _info->setDescription( description );
}

void Plugins::ImageInfo::clearAttributes()
{
    _info->_categoryInfomation.clear();
}

void Plugins::ImageInfo::addAttributes( const QMap<QString,QVariant>& map )
{
    if ( _info ) {
        for( QMap<QString,QVariant>::ConstIterator it = map.begin(); it != map.end(); ++it ) {
            QStringList list = it.value().toStringList();
            _info->addCategoryInfo( it.key(), list );
        }
    }
}

int Plugins::ImageInfo::angle()
{
    if ( _info )
        return _info->angle();
    else
        return 0;
}

void Plugins::ImageInfo::setAngle( int angle )
{
    if ( _info )
        _info->setAngle( angle );
}

QDateTime Plugins::ImageInfo::time( KIPI::TimeSpec what )
{
    if ( _info ) {
        if ( what == KIPI::FromInfo ) {
            return _info->date().start() ;
        }
        else
            return _info->date().end();
    }
    else
        return KIPI::ImageInfoShared::time( what );
}

bool Plugins::ImageInfo::isTimeExact()
{
    if ( !_info )
        return true;
    return _info->date().hasValidTime();
}

void Plugins::ImageInfo::setTime( const QDateTime& time, KIPI::TimeSpec spec )
{
    if ( !_info )
        return;
    if ( spec == KIPI::FromInfo ) {
        _info->setDate( DB::ImageDate( time, time ) );
    }
    else {
        DB::ImageDate date = _info->date();
        _info->setDate( DB::ImageDate( date.start(), time ) );
    }
}

void Plugins::ImageInfo::cloneData( ImageInfoShared* other )
{
    ImageInfoShared::cloneData( other );
    if ( _info ) {
        Plugins::ImageInfo* inf = static_cast<Plugins::ImageInfo*>( other );
        _info->setDate( inf->_info->date() );
    }
}

#endif // KIPI
