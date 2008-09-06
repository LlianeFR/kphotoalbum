/* Copyright (C) 2003-2006 Jesper K. Pedersen <blackie@kde.org>
   Copyright (C) 2007 Tuomas Suutari <thsuut@utu.fi>

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

#ifndef SQLDB_DATABASE_H
#define SQLDB_DATABASE_H

#include "DB/ImageDB.h"
#include "DatabaseAddress.h"
#include "Connection.h"
#include "SQLMemberMap.h"
#include "SQLCategoryCollection.h"
#include "SQLImageInfoCollection.h"
#include "SQLMD5Map.h"
#include "QueryHelper.h"

namespace SQLDB {
    class Database  :public DB::ImageDB {
        Q_OBJECT

    public:
        explicit Database(const DatabaseAddress& address);

        OVERRIDE bool operator==(const DB::ImageDB& other) const;
        OVERRIDE uint totalCount() const;
        DB::MediaCount count(const DB::ImageSearchInfo& searchInfo);
        OVERRIDE DB::ResultPtr search( const DB::ImageSearchInfo&, bool requireOnDisk = false ) const;

        OVERRIDE void renameCategory( const QString& oldName, const QString newName );

        OVERRIDE QMap<QString, uint> classify(const DB::ImageSearchInfo& info,
                                             const QString& category,
                                             DB::MediaType typemask);
        OVERRIDE DB::ResultPtr images();
        OVERRIDE void addImages( const DB::ImageInfoList& images );

        OVERRIDE void addToBlockList( const QStringList& list );
        OVERRIDE bool isBlocking( const QString& fileName );
        OVERRIDE void deleteList( const QStringList& list );
        OVERRIDE DB::ImageInfoPtr info( const QString& fileName, DB::PathType ) const;
        OVERRIDE DB::ImageInfoPtr info( const DB::ResultId& );

        OVERRIDE DB::MemberMap& memberMap();
        OVERRIDE void save( const QString& fileName, bool isAutoSave );
        OVERRIDE DB::MD5Map* md5Map();
        OVERRIDE void sortAndMergeBackIn( const QStringList& fileList );
        OVERRIDE DB::CategoryCollection* categoryCollection();
        OVERRIDE KSharedPtr<DB::ImageDateCollection> rangeCollection();
        OVERRIDE void reorder( const QString& item, const QStringList& cutList, bool after );
        OVERRIDE QString findFirstItemInRange(const DB::ImageDate& range,
                                             bool includeRanges,
                                             const QStringList& images) const;
        OVERRIDE void cutToClipboard( const QStringList& list );
        OVERRIDE QStringList pasteFromCliboard( const QString& afterFile );
        OVERRIDE bool isClipboardEmpty();
        OVERRIDE QStringList CONVERT( const DB::ResultPtr& );

        OVERRIDE bool stack(const QStringList& files);
        OVERRIDE void unstack(const QStringList& files);
        OVERRIDE QStringList getStackFor(const QString& referenceFile) const;

    protected slots:
        OVERRIDE void lockDB( bool lock, bool exclude );

    protected:
        DB::ResultPtr imageList();


    private:
        DatabaseAddress _address;
        ConnectionSPtr _connection;
        QueryHelper _qh;
        SQLCategoryCollection _categoryCollection;
        SQLMemberMap _members;
        SQLImageInfoCollection _infoCollection;
        SQLMD5Map _md5map;

        // No copying or assignment
        Database(const Database&);
        Database& operator=(const Database&);
    };
}

#endif /* SQLDB_DATABASE_H */

