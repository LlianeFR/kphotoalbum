/* Copyright (C) 2003-2004 Jesper K. Pedersen <blackie@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "browseritemfactory.h"
#include "folder.h"
#include <klocale.h>
BrowserIconViewItemFactory::BrowserIconViewItemFactory( QIconView* view )
    :BrowserItemFactory(), _view( view )
{
}

void BrowserIconViewItemFactory::createItem( Folder* folder )
{
    if ( folder->text().lower().contains( _matchText.lower() ) )
        new BrowserIconItem( _view, folder );
}

BrowserListViewItemFactory::BrowserListViewItemFactory( QListView* view )
    :BrowserItemFactory(), _view( view )
{
}

void BrowserListViewItemFactory::createItem( Folder* folder )
{
    new BrowserListItem( _view, folder );
}

BrowserIconItem::BrowserIconItem( QIconView* view, Folder* folder )
    :QIconViewItem( view ), _folder(folder)
{
    setPixmap( folder->pixmap() );
    int count = folder->count();
    if ( count == -1 )
        setText( folder->text() );
    else
        setText( QString::fromLatin1( "%1 (%2)" ).arg( folder->text() ).arg( count ) );
}

BrowserListItem::BrowserListItem( QListView* view, Folder* folder )
     : QListViewItem( view ), _folder(folder)
{
    setPixmap( 0, folder->pixmap() );
    setText( 0, folder->text() );
    setText( 1, folder->countLabel() );
}

int BrowserListItem::compare( QListViewItem* other, int col, bool asc ) const
{
    return _folder->compare( static_cast<BrowserListItem*>(other)->_folder, col, asc );
}

BrowserIconItem::~BrowserIconItem()
{
    delete _folder;
}

BrowserListItem::~BrowserListItem()
{
    delete _folder;
}

void BrowserIconViewItemFactory::setMatchText( const QString& text )
{
    _matchText = text;
}

