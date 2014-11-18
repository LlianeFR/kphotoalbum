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

#include "SettingsDialog.h"
#include "DatabaseBackendPage.h"
#include "ExifPage.h"
#include "PluginsPage.h"
#include "ViewerPage.h"
#include "FileVersionDetectionPage.h"
#include "ThumbnailsPage.h"
#include "GeneralPage.h"
#include "TagGroupsPage.h"
#include "CategoryPage.h"
#include "SettingsDialog.moc"
#include <QDebug>

#include <klocale.h>
#include <kglobal.h>
#include "Utilities/ShowBusyCursor.h"

#include "config-kpa-kipi.h"

#include "config-kpa-kface.h"
#ifdef HAVE_KFACE
#include "FaceManagementPage.h"
#endif

struct Data
{
    QString title;
    const char* icon;
    QWidget* widget;
};

Settings::SettingsDialog::SettingsDialog( QWidget* parent)
     :KPageDialog( parent )
{
    _generalPage = new Settings::GeneralPage(this);
    _fileVersionDetectionPage = new Settings::FileVersionDetectionPage(this);
    _thumbnailsPage = new Settings::ThumbnailsPage(this);
    _categoryPage = new Settings::CategoryPage(this);
    _tagGroupsPage = new Settings::TagGroupsPage(this);
    _viewerPage = new Settings::ViewerPage(this);

#ifdef HASKIPI
    _pluginsPage = new Settings::PluginsPage(this);
#endif

#ifdef HAVE_EXIV2
    _exifPage = new Settings::ExifPage(this);
#endif

#ifdef HAVE_KFACE
    _faceManagementPage = new Settings::FaceManagementPage(this);
#endif

    _databaseBackendPage = new Settings::DatabaseBackendPage(this);


    Data data[] = {
        { i18n("General"), "kphotoalbum", _generalPage },
        { i18n("File Searching & Versions"), "system-search", _fileVersionDetectionPage },
        { i18n("Thumbnail View" ), "view-list-icons", _thumbnailsPage },
        { i18n("Categories"), "user-identity", _categoryPage },
        { i18n("Tag Groups" ), "edit-copy", _tagGroupsPage },
        { i18n("Viewer" ), "document-preview", _viewerPage },
#ifdef HASKIPI
        { i18n("Plugins" ), "preferences-plugin", _pluginsPage },
#endif

#ifdef HAVE_EXIV2
        { i18n("EXIF/IPTC Information" ), "document-properties", _exifPage },
#endif
        { i18n("Database backend"), "system-file-manager", _databaseBackendPage },
#ifdef HAVE_KFACE
        { i18n("Face management" ), "edit-find-user", _faceManagementPage },
#endif
        { QString(), "", 0 }
    };

    int i = 0;
    while ( data[i].widget != 0 ) {
        KPageWidgetItem* page = new KPageWidgetItem( data[i].widget, data[i].title );
        page->setHeader( data[i].title );
        page->setIcon( KIcon( QString::fromLatin1( data[i].icon ) ) );
        addPage( page );
        ++i;
    }


    setButtons( KDialog::Ok | KDialog::Cancel | KDialog::Apply );
    setCaption( i18n( "Settings" ) );

    connect(_categoryPage, SIGNAL(currentCategoryNameChanged(QString,QString)),
            _tagGroupsPage, SLOT(categoryRenamed(QString,QString)));
    connect(this, SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)),
            _tagGroupsPage, SLOT(slotPageChange()));
#ifdef HAVE_KFACE
    connect(this, SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)),
            _faceManagementPage, SLOT(slotPageChange(KPageWidgetItem*)));
#endif
    connect( this, SIGNAL(applyClicked()), this, SLOT(slotMyOK()) );
    connect( this, SIGNAL(okClicked()), this, SLOT(slotMyOK()) );
    connect(this, SIGNAL(cancelClicked()), this, SLOT(slotMyCancel()));
}

void Settings::SettingsDialog::show()
{
    Settings::SettingsData* opt = Settings::SettingsData::instance();

    _generalPage->loadSettings( opt );
    _fileVersionDetectionPage->loadSettings( opt );
    _thumbnailsPage->loadSettings(opt);
    _tagGroupsPage->loadSettings();
    _databaseBackendPage->loadSettings(opt);
    _viewerPage->loadSettings(opt);

#ifdef HASKIPI
    _pluginsPage->loadSettings(opt);
#endif

    _categoryPage->loadSettings(opt);

#ifdef HAVE_EXIV2
    _exifPage->loadSettings( opt );
#endif

#ifdef HAVE_KFACE
    _faceManagementPage->loadSettings(opt);
#endif

    _categoryPage->enableDisable( false );

    KDialog::show();
}

// KDialog has a slotOK which we do not want to override.
void Settings::SettingsDialog::slotMyOK()
{
    Utilities::ShowBusyCursor dummy;
    Settings::SettingsData* opt = Settings::SettingsData::instance();

    // Must be before I save to the backend.
    if ( _thumbnailsPage->thumbnailSizeChanged(opt) )
        emit thumbnailSizeChanged();

    _categoryPage->resetInterface();
    _generalPage->saveSettings( opt );
    _fileVersionDetectionPage->saveSettings( opt );
    _thumbnailsPage->saveSettings(opt);
    _categoryPage->saveSettings( opt, _tagGroupsPage->memberMap() );
    _tagGroupsPage->saveSettings();
    _viewerPage->saveSettings( opt );

#ifdef HASKIPI
    _pluginsPage->saveSettings( opt );
#endif

#ifdef HAVE_EXIV2
    _exifPage->saveSettings(opt);
#endif

#ifdef HAVE_KFACE
    _faceManagementPage->saveSettings(opt);
    _faceManagementPage->clearDatabaseEntries();
#endif

    _databaseBackendPage->saveSettings(opt);

    emit changed();
    KGlobal::config()->sync();
}

void Settings::SettingsDialog::showBackendPage()
{
    setCurrentPage(_backendPage);
}

void Settings::SettingsDialog::slotMyCancel()
{
    _tagGroupsPage->discardChanges();
}

// vi:expandtab:tabstop=4 shiftwidth=4:
