#include "mainview.h"
#include <optionsdialog.h>
#include <qapplication.h>
#include "thumbnailview.h"
#include "thumbnail.h"
#include "imageconfig.h"
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qdict.h>

MainView::MainView( QWidget* parent, const char* name )
    :MainViewUI( parent,  name )
{
    _optionsDialog = 0;
    _imageConfigure = 0;
    load();
}

void MainView::slotExit()
{
    qApp->quit();
}

void MainView::slotOptions()
{
    if ( ! _optionsDialog ) {
        _optionsDialog = new OptionsDialog( this );
        connect( _optionsDialog, SIGNAL( changed() ), thumbNailView, SLOT( reload() ) );
        connect( _optionsDialog, SIGNAL( imagePathChanged() ), this, SLOT( load() ) );
    }
    _optionsDialog->show();
}


void MainView::slotConfigureAllImages()
{
    configureImages( false );
}


void MainView::slotConfigureImagesOneAtATime()
{
    configureImages( true );
}



void MainView::configureImages( bool oneAtATime )
{
    if ( ! _imageConfigure ) {
        _imageConfigure = new ImageConfig( this,  "_imageConfigure" );
    }

    ImageInfoList list;
    for ( QIconViewItem* item = thumbNailView->firstItem(); item; item = item->nextItem() ) {
        if ( item->isSelected() ) {
            ThumbNail* tn = dynamic_cast<ThumbNail*>( item );
            Q_ASSERT( tn );
            list.append( tn->imageInfo() );
        }
    }
    if ( list.count() == 0 )  {
        QMessageBox::warning( this,  tr("No Selection"),  tr("No item selected.") );
    }
    else {
        _imageConfigure->configure( list,  oneAtATime );
        save();
    }
}

void MainView::slotSearch()
{
    if ( ! _imageConfigure ) {
        _imageConfigure = new ImageConfig( this,  "_imageConfigure" );
    }
    int ok = _imageConfigure->search();
    if ( ok == QDialog::Accepted )  {
        _curView.clear();
        for( ImageInfoListIterator it( _images ); *it; ++it ) {
            if ( _imageConfigure->match( *it ) )
                 _curView.append( *it );
        }
        thumbNailView->load( &_curView );
    }
}

void MainView::save()
{
    QMap<QString, QDomDocument> docs;
    for( QPtrListIterator<ImageInfo> it( _images ); *it; ++it ) {
        QString indexDirectory = (*it)->indexDirectory();

        QString outputFile = indexDirectory + "/index.xml";
        if ( !docs.contains( outputFile ) )  {
            QDomDocument tmp;
            tmp.setContent( QString("<Images/>") );
            docs[outputFile] = tmp;
        }
        QDomDocument doc = docs[outputFile];
        QDomElement elm = doc.documentElement();
        elm.appendChild( (*it)->save( doc ) );
    }

    for( QMapIterator<QString,QDomDocument> it= docs.begin(); it != docs.end(); ++it ) {
        QFile out( it.key() );

        if ( !out.open( IO_WriteOnly ) )  {
            qWarning( "Could not open file '%s'", it.key().latin1() );
        }
        else {
            QTextStream stream( &out );
            stream << it.data().toString();
            out.close();
        }
    }
}

void MainView::slotDeleteSelected()
{
    qDebug("NYI!");
}

void MainView::load()
{
    _images.clear();
    _curView.clear();

    QString directory = Options::instance()->imageDirectory();
    if ( directory.isEmpty() )
        return;
    if ( directory.endsWith( "/" ) )
        directory = directory.mid( 0, directory.length()-1 );


    // Load the information from the XML file.
    QDict<void> loadedFiles( 6301 /* a large prime */ );

    QString xmlFile = directory + "/index.xml";
    if ( QFileInfo( xmlFile ).exists() )  {
        QFile file( xmlFile );
        if ( ! file.open( IO_ReadOnly ) )  {
            qWarning( "Couldn't read file %s",  xmlFile.latin1() );
        }
        else {
            QDomDocument doc;
            doc.setContent( &file );
            for ( QDomNode node = doc.documentElement().firstChild(); !node.isNull(); node = node.nextSibling() )  {
                QDomElement elm;
                if ( node.isElement() )
                    elm = node.toElement();
                else
                    continue;

                QString fileName = elm.attribute( "file" );
                if ( fileName.isNull() )
                    qWarning( "Element did not contain a file attirbute" );
                else if ( ! QFileInfo( directory + "/" + fileName ).exists() )
                    qWarning( "File %s didn't exists", fileName.latin1());
                else if ( loadedFiles.find( fileName ) != 0 )
                    qWarning( "XML file contained image %s, more than ones - only first one will be loaded", fileName.latin1());
                else {
                    loadedFiles.insert( directory + "/" + fileName, (void*)0x1 /* void pointer to nothing I never need the value,
                                                                                  just its existsance, must be != 0x0 though.*/ );
                    load( directory, fileName, elm );
                }
            }
        }
    }

    loadExtraFiles( loadedFiles, directory, directory );
    thumbNailView->load( &_images );
}

void MainView::load( const QString& indexDirectory,  const QString& fileName, QDomElement elm )
{
    ImageInfo* info = new ImageInfo( indexDirectory, fileName, elm );
    _images.append(info);
}

void MainView::loadExtraFiles( const QDict<void>& loadedFiles, const QString& indexDirectory, QString directory )
{
    if ( directory.endsWith( "/" ) )
        directory = directory.mid( 0, directory.length()-1 );
    QDir dir( directory );
    QStringList dirList = dir.entryList();
    for( QStringList::Iterator it = dirList.begin(); it != dirList.end(); ++it ) {
        QString file = directory + "/" + *it;
        QFileInfo fi( file );
        if ( (*it) == "." || (*it) == ".." || (*it) == "ThumbNails" || !fi.isReadable() )
                continue;

        if ( fi.isFile() && (loadedFiles.find( file ) == 0) &&
             ( (*it).endsWith( ".jpg" ) || (*it).endsWith( ".jpeg" ) || (*it).endsWith( ".png" ) ||
                 (*it).endsWith( ".tiff" ) || (*it).endsWith( ".gif" ) ) )  {
            QString baseName = file.mid( indexDirectory.length() );

            ImageInfo* info = new ImageInfo( indexDirectory, baseName  );
            _images.append(info);
        }
        else if ( fi.isDir() )  {
            loadExtraFiles( loadedFiles,  indexDirectory, file );
        }
    }
}
