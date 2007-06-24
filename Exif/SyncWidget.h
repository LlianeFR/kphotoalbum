/* Copyright (C) 2007 Jan Kundrát <jkt@gentoo.org>

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
#ifndef EXIFSYNCWIDGET_H
#define EXIFSYNCWIDGET_H

#include <qlistview.h>
#include <qhbox.h>
#include <qpushbutton.h>

namespace Exif{

namespace Syncable {

enum Header { NONE, FILE, JPEG, EXIF, IPTC };

enum Kind { 
    // delimiter for the "don't proccess more records" position
    STOP,
    // JPEG header
    JPEG_COMMENT,
    // image orientation
    EXIF_ORIENTATION, 
    // standard EXIF fields for description/label
    EXIF_DESCRIPTION, EXIF_USER_COMMENT,
    // stuff from Windows XP
    EXIF_XPTITLE, EXIF_XPCOMMENT, EXIF_XPKEYWORDS, EXIF_XPSUBJECT,
    // standard IPTC label/description
    IPTC_HEADLINE, IPTC_CAPTION, IPTC_OBJECT_NAME, IPTC_SUBJECT,
    // IPTC categories
    IPTC_SUPP_CAT, IPTC_KEYWORDS,
    // geographic stuff
    IPTC_LOCATION_CODE, IPTC_LOCATION_NAME, IPTC_CITY,
    IPTC_SUB_LOCATION, IPTC_PROVINCE_STATE, IPTC_COUNTRY_CODE, IPTC_COUNTRY_NAME,
    // file modification time
    FILE_CTIME, FILE_MTIME, EXIF_DATETIME, EXIF_DATETIME_ORIGINAL, EXIF_DATETIME_DIGITIZED
};

void fillTranslationTables( QMap<Kind,QString>& _fieldName, QMap<Kind,QString>& _visibleName, QMap<Kind,Header>& _header);

}

class SyncWidget : public QHBox {
    Q_OBJECT

public:
    SyncWidget( const QString& title, QWidget* parent, const QValueList<Syncable::Kind>& items, const char* name = 0 );
    QValueList<Syncable::Kind> items() const;

protected slots:
    void slotMoveSelectedDown();
    void slotMoveSelectedUp();
    void slotHandleDisabling();

private:
    QValueList<Syncable::Kind> _items;
    QListView* _list;
    QPushButton* _upBut;
    QPushButton* _downBut;
};

}

#endif /* EXIFSYNCWIDGET_H */

