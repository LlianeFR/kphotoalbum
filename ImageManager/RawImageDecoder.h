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
#ifndef RAWIMAGEDECODER_H
#define RAWIMAGEDECODER_H

#include "ImageDecoder.h"
#include <QSet>
#include <QStringList>

namespace DB { class FileName; }

namespace ImageManager
{

class RAWImageDecoder : public ImageDecoder {
public:
    virtual bool _decode(QImage *img, const DB::FileName& imageFile, QSize* fullSize, int dim=-1);
	virtual bool _mightDecode( const QString& imageFile );
	virtual bool _skipThisFile( const QSet<QString>& loadedFiles, const QString& imageFile ) const;
    static bool isRAW( const DB::FileName& imageFile );

private:
	bool _fileExistsWithExtensions( const QString& fileName, const QStringList& extensionList ) const;
	static bool _fileEndsWithExtensions( const QString& fileName, const QStringList& extensionList );
	bool _fileIsKnownWithExtensions( const QSet<QString>& files, const QString& fileName, const QStringList& extensionList ) const;
	static void _initializeExtensionLists( QStringList& rawExtensions, QStringList& standardExtensions, QStringList& ignoredExtensions );
};

}

#endif /* RAWIMAGEDECODER_H */
