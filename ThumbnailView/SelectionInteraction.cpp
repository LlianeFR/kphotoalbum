/* Copyright (C) 2003-2010 Jesper K. Pedersen <blackie@kde.org>

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
#include "SelectionInteraction.h"
#include "ThumbnailModel.h"
#include "ThumbnailFactory.h"
#include "CellGeometry.h"

#include <qtimer.h>
#include <QMouseEvent>
#include <qcursor.h>
#include <qapplication.h>
#include <kurl.h>

#include "MainWindow/Window.h"
#include "ThumbnailWidget.h"

ThumbnailView::SelectionInteraction::SelectionInteraction( ThumbnailFactory* factory )
    : ThumbnailComponent( factory ),
      _dragInProgress( false ), _dragSelectionInProgress( false )
{
    _dragTimer = new QTimer( this );
    connect( _dragTimer, SIGNAL( timeout() ), this, SLOT( handleDragSelection() ) );
}


bool ThumbnailView::SelectionInteraction::mousePressEvent( QMouseEvent* event )
{
    _mousePressPos = event->pos();
    DB::ResultId mediaId = widget()->mediaIdUnderCursor();
    _isMouseDragOperation = widget()->isSelected( mediaId ) & !( event->modifiers() );
    return _isMouseDragOperation;

#ifdef KDAB_TEMPORARILY_REMOVED
    if ( deselectSelection( event ) && !model()->isSelected( mediaId ) )
        model()->clearSelection();

    if ( !mediaId.isNull() ) {
        if ( event->modifiers() & Qt::ShiftModifier )
            model()->selectRange( model()->currentItem().isNull() ? Cell() : model()->positionForMediaId( model()->currentItem() ),
                                          widget()->cellAtCoordinate( event->pos(), ViewportCoordinates ) );

        _originalSelectionBeforeDragStart = model()->selectionSet();

        // When control is pressed selection of the file should be
        // toggled. This is done in the release event, not here.
        if ( !( event->modifiers() & Qt::ControlModifier ) )
            // Otherwise add file to selected files.
            model()->select( mediaId );

        model()->setCurrentItem( mediaId );
        model()->updateCell( mediaId );
    }
#endif //KDAB_TEMPORARILY_REMOVED
}


bool ThumbnailView::SelectionInteraction::mouseMoveEvent( QMouseEvent* event )
{
    if ( _isMouseDragOperation ) {
        if ( (_mousePressPos - event->pos()).manhattanLength() > QApplication::startDragDistance() )
            startDrag();
        return true;
    }

#ifdef KDAB_TEMPORARILY_REMOVED
    if ( !(event->buttons() & Qt::LeftButton ) )
        return;

    else {
        handleDragSelection();
        if ( event->pos().y() < 0 || event->pos().y() > widget()->height() )
            _dragTimer->start( 100 );
        else
            _dragTimer->stop();
    }
#endif //KDAB_TEMPORARILY_REMOVED
    return false;
}


bool ThumbnailView::SelectionInteraction::mouseReleaseEvent( QMouseEvent* event )
{
// PENDING(kdab) Review
#ifdef KDAB_TEMPORARILY_REMOVED
    DB::ResultId mediaId = model()->imageAt( event->pos(), ViewportCoordinates );
    if ( (event->modifiers() & Qt::ControlModifier) &&
         !(event->modifiers() & Qt::ShiftModifier) ) { // toggle selection of file
        if ( (event->button() & Qt::LeftButton) )
            model()->toggleSelection( mediaId );
    }
    else {
        if ( !_dragSelectionInProgress &&
             deselectSelection( event ) && model()->isSelected( mediaId ) ) {
            model()->clearSelection();
            model()->select( mediaId );

            _originalSelectionBeforeDragStart.clear();
            _originalSelectionBeforeDragStart.insert( mediaId );
        }
    }

    _dragSelectionInProgress = false;

    _dragTimer->stop();
#else // KDAB_TEMPORARILY_REMOVED
    Q_UNUSED(event);
#endif // KDAB_TEMPORARILY_REMOVED
    return false;
}


void ThumbnailView::SelectionInteraction::handleDragSelection()
{
// PENDING(kdab) Review
#ifdef KDAB_TEMPORARILY_REMOVED
    _dragSelectionInProgress = true;

    Cell pos1;
    Cell pos2;
    calculateSelection( &pos1, &pos2 );

    model()->setCurrentItem( pos2 );

    // Auto scroll
    QPoint viewportPos = widget()->viewport()->mapFromGlobal( QCursor::pos() );
    if ( viewportPos.y() < 0 )
        widget()->scrollBy( 0, viewportPos.y()/2 );
    else if ( viewportPos.y() > widget()->height() )
        widget()->scrollBy( 0, (viewportPos.y() - widget()->height())/3 );

    model()->setSelection(_originalSelectionBeforeDragStart );
    model()->selectRange( pos1, pos2 );
#else // KDAB_TEMPORARILY_REMOVED
    return ;
#endif // KDAB_TEMPORARILY_REMOVED
}

void ThumbnailView::SelectionInteraction::startDrag()
{
    _dragInProgress = true;
    KUrl::List urls;
    Q_FOREACH(DB::ImageInfoPtr info, widget()->selection().fetchInfos()) {
        const QString fileName = info->fileName(DB::AbsolutePath);
        urls.append( fileName );
    }
    QDrag* drag = new QDrag( MainWindow::Window::theMainWindow() );
    QMimeData* data = new QMimeData;
    urls.populateMimeData(data);
    drag->setMimeData( data );

    drag->exec(Qt::ActionMask);

    widget()->_mouseHandler = &(widget()->_mouseTrackingHandler);
    _dragInProgress = false;
}

bool ThumbnailView::SelectionInteraction::isDragging() const
{
    return _dragInProgress;
}

#ifdef KDAB_TEMPORARILY_REMOVED
void ThumbnailView::SelectionInteraction::calculateSelection( Cell* pos1, Cell* pos2 )
{
    *pos1 = widget()->cellAtCoordinate( _mousePressPos, ContentsCoordinates );
    QPoint viewportPos = widget()->viewport()->mapFromGlobal( QCursor::pos() );
    *pos2 = widget()->cellAtCoordinate( viewportPos, ViewportCoordinates );

    if ( *pos1 < *pos2 ) {
        if ( atRightSide( _mousePressPos ) )
            *pos1 = nextCell( *pos1 );
        if ( atLeftSide( widget()->viewportToContents( viewportPos ) ) )
            *pos2 = prevCell( *pos2 );
    }
    else if ( *pos1 > *pos2 ) {
        if ( atLeftSide( _mousePressPos ) ){
            *pos1 = prevCell( *pos1 );
        }
        if ( atRightSide( widget()->viewportToContents( viewportPos ) ) ) {
            *pos2 = nextCell( *pos2 );
        }
    }

    // Selecting to the right of the thumbnailview result in a position at cols(), though we only have 0..cols()-1
    if( pos1->col() == widget()->numCols() )
        pos1->col()--;
    if( pos2->col() == widget()->numCols() )
        pos2->col()--;
}
#endif //KDAB_TEMPORARILY_REMOVED

bool ThumbnailView::SelectionInteraction::atLeftSide( const QPoint& contentCoordinates )
{
    QRect rect = iconRect( contentCoordinates, ContentsCoordinates );
    return contentCoordinates.x() < rect.left();
}

bool ThumbnailView::SelectionInteraction::atRightSide( const QPoint& contentCoordinates )
{
    QRect rect = iconRect( contentCoordinates, ContentsCoordinates );
    return contentCoordinates.x() > rect.right();
}

#ifdef KDAB_TEMPORARILY_REMOVED
ThumbnailView::Cell ThumbnailView::SelectionInteraction::prevCell( const Cell& cell )
{
    Cell res( cell.row(), cell.col() -1 );
    if ( res.col() == -1 )
        res = Cell( cell.row()-1, widget()->numCols()-1 );
    if ( res < Cell(0,0) )
        return Cell(0,0);
    else
        return res;
}

ThumbnailView::Cell ThumbnailView::SelectionInteraction::nextCell( const Cell& cell )
{
    Cell res( cell.row(), cell.col()+1 );
    if ( res.col() == widget()->numCols() )
        res = Cell( cell.row()+1, 0 );
    if ( res > widget()->lastCell() )
        return widget()->lastCell();
    else
        return res;
}
#endif //KDAB_TEMPORARILY_REMOVED

QRect ThumbnailView::SelectionInteraction::iconRect( const QPoint& coordinate, CoordinateSystem system ) const
{
// PENDING(kdab) Review
#ifdef KDAB_TEMPORARILY_REMOVED
    Cell pos = widget()->cellAtCoordinate( coordinate, system );
    QRect cellRect = const_cast<ThumbnailWidget*>(widget())->cellGeometry(pos.row(), pos.col() );
    QRect iconRect = cellGeometryInfo()->iconGeometry( pos.row(), pos.col() );

    // map iconRect from local coordinates within the cell to the coordinates requires
    iconRect.translate( cellRect.x(), cellRect.y() );

    return iconRect;
#else // KDAB_TEMPORARILY_REMOVED
    Q_UNUSED(coordinate);
    Q_UNUSED(system);
    return QRect();
#endif // KDAB_TEMPORARILY_REMOVED
}

bool ThumbnailView::SelectionInteraction::deselectSelection( const QMouseEvent* event ) const
{
// PENDING(kdab) Review
#ifdef KDAB_TEMPORARILY_REMOVED
// If control or shift is pressed down then do not deselect.
    if  ( event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier) )
        return false;

    // right mouse button on a selected image should not clear
    if ( (event->button() & Qt::RightButton) && model()->isSelected( model()->imageAt( event->pos(), ViewportCoordinates ) ) )
        return false;

    // otherwise deselect
    return true;
#else // KDAB_TEMPORARILY_REMOVED
    Q_UNUSED(event);
    return false;
#endif // KDAB_TEMPORARILY_REMOVED
}


#include "SelectionInteraction.moc"
