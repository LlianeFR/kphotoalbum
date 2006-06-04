/* Copyright (C) 2003-2005 Jesper K. Pedersen <blackie@kde.org>

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

#ifndef CONTENTFOLDER_H
#define CONTENTFOLDER_H
#include "Folder.h"

namespace Browser
{

class ContentFolder :public Folder {
public:
    ContentFolder( const QString& category, const QString& value, int count,
                   const DB::ImageSearchInfo& info, BrowserWidget* parent );
    virtual FolderAction* action( bool ctrlDown = false );
    virtual QPixmap pixmap();
    virtual QString text() const;
    int compare( Folder* other, int col, bool asc ) const;
    virtual QString countLabel() const;

private:
    QString _category;
    QString _value;
};

class ContentFolderAction :public FolderAction {

public:
    ContentFolderAction( const QString& category, const QString& value,
                         const DB::ImageSearchInfo& info, BrowserWidget* parent );
    virtual void action( BrowserItemFactory* factory );
    virtual bool showsImages() const { return false; }
    virtual bool contentView() const { return false; }
    virtual bool allowSort() const;
    virtual QString title() const;

private:
    QString _category;
    QString _value;
};

}

#endif /* CONTENTFOLDER_H */

