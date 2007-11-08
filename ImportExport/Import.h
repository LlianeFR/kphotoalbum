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

#ifndef IMPORT_H
#define IMPORT_H

#include <kurl.h>
#include <kio/job.h>
#include "Utilities/Util.h"
#include <QPixmap>
#include <Q3ValueList>
#include <QCloseEvent>
#include <KAssistantDialog>

class KTemporaryFile;
class QCheckBox;
class KArchiveDirectory;
class KZip;
class KLineEdit;
class QProgressDialog;

namespace DB
{
    class ImageInfo;
}

namespace ImportExport
{
class Import;
class ImportMatcher;

class ImageRow :public QObject
{
    Q_OBJECT
public:
    ImageRow( DB::ImageInfoPtr info, ImportExport::Import* import, QWidget* parent );
    QCheckBox* _checkbox;
    DB::ImageInfoPtr _info;
    Import* _import;
protected slots:
    void showImage();
};

class Import :public KAssistantDialog {
    Q_OBJECT

public:
    static void imageImport();
    static void imageImport( const KUrl& url );

protected:
    friend class ImageRow;

    void setupPages();
    bool readFile( const QByteArray& data, const QString& fileName );
    void createIntroduction();
    void createImagesPage();
    void createDestination();
    void createCategoryPages();
    ImportMatcher* createCategoryPage( const QString& myCategory, const QString& otherCategory );
    bool copyFilesFromZipFile();
    void copyFromExternal();
    void copyNextFromExternal();
    QPixmap loadThumbnail( QString fileName );
    QByteArray loadImage( const QString& fileName );
    void selectImage( bool on );
    DB::ImageInfoList selectedImages();
    bool init( const QString& fileName );
    void updateDB();
    virtual void closeEvent( QCloseEvent* );

protected slots:
    void slotEditDestination();
    void updateNextButtonState();
    virtual void next();
    void slotFinish();
    void slotSelectAll();
    void slotSelectNone();
    void downloadKimJobCompleted( KIO::Job* );
    void aCopyJobCompleted( KIO::Job* );
    void aCopyFailed( QStringList files );
    void stopCopyingImages();
    void slotHelp();

signals:
    void failedToCopy( QStringList files );

private:
    Import( const QString& file, bool* ok, QWidget* parent );
    Import( const KUrl& url, QWidget* parent );
    ~Import();

    QString _zipFile;
    DB::ImageInfoList _images;
    KLineEdit* _destinationEdit;
    KPageWidgetItem* _destinationPage;
    KPageWidgetItem* _categoryMatcherPage;
    KPageWidgetItem* _dummy;
    ImportMatcher* _categoryMatcher;
    Q3ValueList<ImportMatcher*> _matchers;
    KZip* _zip;
    const KArchiveDirectory* _dir;
    Q3ValueList< ImageRow* > _imagesSelect;
    KTemporaryFile* _tmp;
    bool _externalSource;
    KUrl _kimFile;
    Utilities::UniqNameMap _nameMap;
    bool _finishedPressed;
    int _totalCopied;
    DB::ImageInfoList _pendingCopies;
    QProgressDialog* _progress;
    KIO::FileCopyJob* _job;
    bool _hasFilled;
    QString _baseUrl;
    bool _reportUnreadableFiles;
};

}


#endif /* IMPORT_H */

