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
#include "SelectionInteraction.h"
#include <qtimer.h>
//Added by qt3to4:
#include <QMouseEvent>
#include "ThumbnailWidget.h"
#include <qcursor.h>
#include <qapplication.h>
#include <kurl.h>
#include <kdebug.h>
#include <MainWindow/Window.h>

ThumbnailView::SelectionInteraction::SelectionInteraction( ThumbnailWidget* view )
    :_view( view ), _dragInProgress( false ), _dragSelectionInProgress( false )
{
    _dragTimer = new QTimer( this );
    connect( _dragTimer, SIGNAL( timeout() ), this, SLOT( handleDragSelection() ) );
}


void ThumbnailView::SelectionInteraction::mousePressEvent( QMouseEvent* event )
{
    _mousePressWasOnIcon = isMouseOverIcon( event->pos() );
    _mousePressPos = _view->viewportToContents( event->pos() );
    QString file = _view->fileNameAtCoordinate( event->pos(), ViewportCoordinates );

    if ( deselectSelection( event ) && !_view->_selectedFiles.contains( file ) )
        clearSelection();

    if ( !file.isNull() ) {
        if ( event->modifiers() & Qt::ShiftModifier )
            _view->selectAllCellsBetween( _view->positionForFileName( _view->_currentItem ),
                                          _view->cellAtCoordinate( event->pos(), ViewportCoordinates ) );

        _originalSelectionBeforeDragStart = _view->_selectedFiles;

        // When control is pressed selection of the file should be
        // toggled. This is done in the release event, not here.
        if ( !( event->modifiers() & Qt::ControlModifier ) )
            // Otherwise add file to selected files.
            _view->_selectedFiles.insert( file );

        _view->_currentItem = file;
        _view->updateCell( file );
    }
    _view->possibleEmitSelectionChanged();

}


void ThumbnailView::SelectionInteraction::mouseMoveEvent( QMouseEvent* event )
{
    if ( !(event->buttons() & Qt::LeftButton ) )
        return;

    if ( _mousePressWasOnIcon &&
         (_view->viewportToContents(event->pos()) - _mousePressPos ).manhattanLength() > QApplication::startDragDistance() )
        startDrag();

    else {
        handleDragSelection();
        if ( event->pos().y() < 0 || event->pos().y() > _view->height() )
            _dragTimer->start( 100 );
        else
            _dragTimer->stop();
    }
    _view->possibleEmitSelectionChanged();
}


void ThumbnailView::SelectionInteraction::mouseReleaseEvent( QMouseEvent* event )
{
    QString file = _view->fileNameAtCoordinate( event->pos(), ViewportCoordinates );
    if ( (event->modifiers() & Qt::ControlModifier) &&
         !(event->modifiers() & Qt::ShiftModifier) ) { // toggle selection of file
        if ( _view->_selectedFiles.contains( file ) && (event->button() & Qt::LeftButton) )
            _view->_selectedFiles.erase( file );
        else
            _view->_selectedFiles.insert( file );
        _view->updateCell( file );
    }
    else {
        if ( !_dragSelectionInProgress &&
             deselectSelection( event ) && _view->_selectedFiles.contains( file ) ) {
            // Unselect everything but the file
            StringSet oldSelection = _view->_selectedFiles;
            oldSelection.erase( file );
            _view->_selectedFiles.clear();
            _view->_selectedFiles.insert( file );
            _originalSelectionBeforeDragStart.clear();
            _originalSelectionBeforeDragStart.insert( file );
            for( StringSet::const_iterator it = oldSelection.begin(); it != oldSelection.end(); ++it ) {
                _view->updateCell( *it );
            }
        }
    }

    _dragSelectionInProgress = false;

    _dragTimer->stop();
}


void ThumbnailView::SelectionInteraction::handleDragSelection()
{
    _dragSelectionInProgress = true;

    Cell pos1;
    Cell pos2;
    calculateSelection( &pos1, &pos2 );

    _view->_currentItem = _view->fileNameInCell( pos2 );

    // Auto scroll
    QPoint viewportPos = _view->viewport()->mapFromGlobal( QCursor::pos() );
    if ( viewportPos.y() < 0 )
        _view->scrollBy( 0, viewportPos.y()/2 );
    else if ( viewportPos.y() > _view->height() )
        _view->scrollBy( 0, (viewportPos.y() - _view->height())/3 );

    StringSet oldSelection = _view->_selectedFiles;
    _view->_selectedFiles = _originalSelectionBeforeDragStart;
    _view->selectAllCellsBetween( pos1, pos2, false );

    for( StringSet::const_iterator it = oldSelection.begin(); it != oldSelection.end(); ++it ) {
        if ( !_view->_selectedFiles.contains( *it ) )
            _view->updateCell( *it );
    }

    for( StringSet::const_iterator it = _view->_selectedFiles.begin(); it != _view->_selectedFiles.end(); ++it ) {
        if ( !oldSelection.contains( *it ) )
            _view->updateCell( *it );
    }

}

/**
 * Returns whether the point viewportPos is on top of the pixmap
 */
bool ThumbnailView::SelectionInteraction::isMouseOverIcon( const QPoint& viewportPos ) const
{
    QRect rect = iconRect( viewportPos, ViewportCoordinates );
    return rect.contains( _view->viewportToContents(viewportPos) );
}

void ThumbnailView::SelectionInteraction::startDrag()
{
    _dragInProgress = true;
    QList<QUrl> l;
    QStringList selected = _view->selection();
    for( QStringList::Iterator fileIt = selected.begin(); fileIt != selected.end(); ++fileIt ) {
        l.append( QUrl(*fileIt) );
    }
    QDrag* drag = new QDrag( MainWindow::Window::theMainWindow() );
    QMimeData* data = new QMimeData;
    data->setUrls( l );
    drag->setMimeData( data );

    drag->exec(Qt::ActionMask);

    _view->_mouseHandler = &(_view->_mouseTrackingHandler);
    _dragInProgress = false;
}

bool ThumbnailView::SelectionInteraction::isDragging() const
{
    return _dragInProgress;
}

void ThumbnailView::SelectionInteraction::calculateSelection( Cell* pos1, Cell* pos2 )
{
    *pos1 = _view->cellAtCoordinate( _mousePressPos, ContentsCoordinates );
    QPoint viewportPos = _view->viewport()->mapFromGlobal( QCursor::pos() );
    *pos2 = _view->cellAtCoordinate( viewportPos, ViewportCoordinates );

    if ( *pos1 < *pos2 ) {
        if ( atRightSide( _mousePressPos ) )
            *pos1 = nextCell( *pos1 );
        if ( atLeftSide( _view->viewportToContents( viewportPos ) ) )
            *pos2 = prevCell( *pos2 );
    }
    else if ( *pos1 > *pos2 ) {
        if ( atLeftSide( _mousePressPos ) ){
            *pos1 = prevCell( *pos1 );
        }
        if ( atRightSide( _view->viewportToContents( viewportPos ) ) ) {
            *pos2 = nextCell( *pos2 );
        }
    }

    // Selecting to the right of the thumbnailview result in a position at cols(), though we only have 0..cols()-1
    if( pos1->col() == _view->numCols() )
        pos1->col()--;
    if( pos2->col() == _view->numCols() )
        pos2->col()--;
}

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

ThumbnailView::Cell ThumbnailView::SelectionInteraction::prevCell( const Cell& cell )
{
    Cell res( cell.row(), cell.col() -1 );
    if ( res.col() == -1 )
        res = Cell( cell.row()-1, _view->numCols()-1 );
    if ( res < Cell(0,0) )
        return Cell(0,0);
    else
        return res;
}

ThumbnailView::Cell ThumbnailView::SelectionInteraction::nextCell( const Cell& cell )
{
    Cell res( cell.row(), cell.col()+1 );
    if ( res.col() == _view->numCols() )
        res = Cell( cell.row()+1, 0 );
    if ( res > _view->lastCell() )
        return _view->lastCell();
    else
        return res;
}

QRect ThumbnailView::SelectionInteraction::iconRect( const QPoint& coordinate, CoordinateSystem system ) const
{
    Cell pos = _view->cellAtCoordinate( coordinate, system );
    QRect cellRect = const_cast<ThumbnailWidget*>(_view)->cellGeometry(pos.row(), pos.col() );
    QRect iconRect = _view->iconGeometry( pos.row(), pos.col() );

    // map iconRect from local coordinates within the cell to the coordinates requires
    iconRect.translate( cellRect.x(), cellRect.y() );

    return iconRect;
}

bool ThumbnailView::SelectionInteraction::deselectSelection( const QMouseEvent* event ) const
{
// If control or shift is pressed down then do not deselect.
    if  ( event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier) )
        return false;

    // right mouse button on a selected image should not clear
    if ( (event->button() & Qt::RightButton) && _view->_selectedFiles.contains( _view->fileNameAtCoordinate( event->pos(), ViewportCoordinates ) ) )
        return false;

    // otherwise deselect
    return true;
}

void ThumbnailView::SelectionInteraction::clearSelection()
{
    // Unselect every thing
    StringSet oldSelection = _view->_selectedFiles;
    _view->_selectedFiles.clear();
    _originalSelectionBeforeDragStart.clear();
    for( StringSet::const_iterator it = oldSelection.begin(); it != oldSelection.end(); ++it ) {
        _view->updateCell( *it );
    }
}

#include "SelectionInteraction.moc"
