/* Copyright (C) 2003-2009 Jesper K. Pedersen <blackie@kde.org>

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
#ifndef THUMBNAILFACTORY_H
#define THUMBNAILFACTORY_H

namespace ThumbnailView
{
class ThumbnailWidget;
class CellGeometry;
class ThumbnailModel;
class ThumbnailPainter;
class ThumbnailCache;

class ThumbnailFactory
{
public:
    virtual ~ThumbnailFactory() {};
    virtual ThumbnailModel* model() = 0;
    virtual CellGeometry* cellGeometry() = 0;
    virtual ThumbnailWidget* widget() = 0;
    virtual ThumbnailPainter* painter() = 0;
    virtual ThumbnailCache* cache() = 0;
};

}

#endif /* THUMBNAILFACTORY_H */
