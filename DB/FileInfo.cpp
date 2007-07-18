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
#include "FileInfo.h"
#include <qdatetime.h>
#include <qfileinfo.h>
#include "Utilities/Util.h"
#include "Settings/SettingsData.h"
#ifdef HASEXIV2
#  include "Exif/Info.h"
#endif
#include "Exif/Syncable.h"
#include <kfilemetainfo.h>
#include <kdebug.h>

using namespace DB;

FileInfo FileInfo::read( const QString& fileName )
{
    return FileInfo( fileName );
}

DB::FileInfo::FileInfo( const QString& fileName )
    :_angle(0)
{
#ifdef HASEXIV2
    parseEXIV2( fileName );
#else
    parseKFileMetaInfo( fileName );
#endif

    if ( !_date.isValid() && Settings::SettingsData::instance()->trustTimeStamps() )
        _date = QFileInfo( fileName ).lastModified();
}

#ifdef HASEXIV2
void DB::FileInfo::parseEXIV2( const QString& fileName )
{
    QMap<Exif::Syncable::Kind,QString> _fieldName, _visibleName;
    QMap<Exif::Syncable::Kind,Exif::Syncable::Header> _header;
    Exif::Syncable::fillTranslationTables( _fieldName, _visibleName, _header);

    Exiv2::ExifData exifMap = Exif::Info::instance()->exifData( fileName );
    Exiv2::IptcData iptcMap = Exif::Info::instance()->iptcData( fileName );

    // Orientation
    QValueList<Exif::Syncable::Kind> items = Settings::SettingsData::instance()->orientationSyncing( false );
    for (QValueList<Exif::Syncable::Kind>::const_iterator it = items.begin(); ( it != items.end() ) && ( *it != Exif::Syncable::STOP ); ++it ) {
        switch ( *it ) {
            case Exif::Syncable::EXIF_ORIENTATION:
                if ( exifMap.findKey( Exiv2::ExifKey( std::string( _fieldName[ *it ].ascii() ) ) ) != exifMap.end() ) {
                    const Exiv2::Exifdatum& datum = exifMap[ _fieldName[ *it ].ascii() ];

                    int orientation =  datum.toLong();
                    _angle = orientationToAngle( orientation );
                }
                break;
            default:
                kdDebug(5123) << "Unknown orientation field " << _fieldName[ *it ] << endl;
        }
    }

    // Label
    items = Settings::SettingsData::instance()->labelSyncing( false );
    for (QValueList<Exif::Syncable::Kind>::const_iterator it = items.begin(); ( it != items.end() ) && ( *it != Exif::Syncable::STOP ); ++it ) {
        switch ( _header[ *it ] ) {
            case Exif::Syncable::EXIF:
            {
                Exiv2::ExifData::const_iterator field = exifMap.findKey( Exiv2::ExifKey( _fieldName[ *it ].ascii() ) );
                if ( field == exifMap.end() )
                    kdDebug(5123) << _fieldName[ *it ] << " not found in " << fileName << endl;
                else {
                    _label = Utilities::cStringWithEncoding( (*field).toString().c_str(),
                            Settings::SettingsData::instance()->iptcCharset() );
                }
                break;
            }
            case Exif::Syncable::IPTC:
            {
                Exiv2::IptcData::const_iterator field = iptcMap.findKey( Exiv2::IptcKey( _fieldName[ *it ].ascii() ) );
                if ( field == iptcMap.end() )
                    kdDebug(5123) << _fieldName[ *it ] << " not found in " << fileName << endl;
                else {
                    _label = Utilities::cStringWithEncoding( (*field).toString().c_str(),
                            Settings::SettingsData::instance()->iptcCharset() );

                }
                break;
            }
            case Exif::Syncable::JPEG:
                kdDebug(5123) << "Can't read JPEG value " << _fieldName[ *it ] << " (not implemented yet)" << endl;
                break;
            default:
                kdDebug(5123) << "Unknown label field " << _fieldName[ *it ] << endl;
        }
    }

    // Description
    items = Settings::SettingsData::instance()->descriptionSyncing( false );
    for (QValueList<Exif::Syncable::Kind>::const_iterator it = items.begin(); ( it != items.end() ) && ( *it != Exif::Syncable::STOP ); ++it ) {
        switch ( _header[ *it ] ) {
            case Exif::Syncable::EXIF:
            {
                Exiv2::ExifData::const_iterator field = exifMap.findKey( Exiv2::ExifKey( _fieldName[ *it ].ascii() ) );
                if ( field == exifMap.end() )
                    kdDebug(5123) << _fieldName[ *it ] << " not found in " << fileName << endl;
                else {
                    _description = Utilities::cStringWithEncoding( (*field).toString().c_str(),
                            Settings::SettingsData::instance()->iptcCharset() );
                }
                break;
            }
            case Exif::Syncable::IPTC:
            {
                Exiv2::IptcData::const_iterator field = iptcMap.findKey( Exiv2::IptcKey( _fieldName[ *it ].ascii() ) );
                if ( field == iptcMap.end() )
                    kdDebug(5123) << _fieldName[ *it ] << " not found in " << fileName << endl;
                else {
                    _description = Utilities::cStringWithEncoding( (*field).toString().c_str(),
                            Settings::SettingsData::instance()->iptcCharset() );

                }
                break;
            }
            case Exif::Syncable::JPEG:
                kdDebug(5123) << "Can't read JPEG value " << _fieldName[ *it ] << " (not implemented yet)" << endl;
                break;
            default:
                kdDebug(5123) << "Unknown description field " << _fieldName[ *it ] << endl;
        }
    }

    /*
    // Date
    _date = fetchEXIV2Date( map, "Exif.Photo.DateTimeOriginal" );
    if ( !_date.isValid() ) {
        _date = fetchEXIV2Date( map, "Exif.Photo.DateTimeDigitized" );
        if ( !_date.isValid() )
            _date = fetchEXIV2Date( map, "Exif.Image.DateTime" );
    }
    */
}

QDateTime FileInfo::fetchEXIV2Date( Exiv2::ExifData& map, const char* key )
{
    try
    {
        if ( map.findKey( Exiv2::ExifKey( key ) ) != map.end() ) {
            const Exiv2::Exifdatum& datum = map[key ];
            return QDateTime::fromString( QString::fromLatin1(datum.toString().c_str()), Qt::ISODate );
        }
    }
    catch (...)
    {
    }

    return QDateTime();
}
#endif

void DB::FileInfo::parseKFileMetaInfo( const QString& fileName )
{
    QString tempFileName( fileName );
#ifdef TEMPORARILY_REMOVED
    if ( Util::isCRW( fileName ) ) {
      QString dirName = QFileInfo( fileName ).dirPath();
      QString baseName = QFileInfo( fileName ).baseName();
      tempFileName = dirName + QString::fromLatin1("/") + baseName + QString::fromLatin1( ".thm" );
      QFileInfo tempFile (tempFileName);
      if ( !tempFile.exists() )
          tempFileName = dirName + QString::fromLatin1("/") + baseName + QString::fromLatin1( ".THM" );
    }
#endif

    KFileMetaInfo metainfo( tempFileName );
    if ( metainfo.isEmpty() )
        return;

    // Date.
    if ( metainfo.contains( QString::fromLatin1( "CreationDate" ) ) ) {
        QDate date = metainfo.value( QString::fromLatin1( "CreationDate" )).toDate();
        if ( date.isValid() ) {
            _date.setDate( date );

            if ( metainfo.contains( QString::fromLatin1( "CreationTime" ) ) ) {
                QTime time = metainfo.value(QString::fromLatin1( "CreationTime" )).toTime();
                if ( time.isValid() )
                    _date.setTime( time );
            }
        }
    }

    // Angle
    if ( metainfo.contains( QString::fromLatin1( "Orientation" ) ) )
        _angle = orientationToAngle( metainfo.value( QString::fromLatin1( "Orientation" ) ).toInt() );

    // Description
    if ( metainfo.contains( QString::fromLatin1( "Comment" ) ) )
        _description = metainfo.value( QString::fromLatin1( "Comment" ) ).toString();
}

int DB::FileInfo::orientationToAngle( int orientation )
{
    // FIXME: this needs to be revisited, some of those values actually specify
    // both rotation and flip
    if ( orientation == 1 || orientation == 2 )
        return 0;
    else if ( orientation == 3 || orientation == 4 )
        return 180;
    else if ( orientation == 5 || orientation == 8 )
        return 270;
    else if ( orientation == 6 || orientation == 7 )
        return 90;

    return 0;
}
