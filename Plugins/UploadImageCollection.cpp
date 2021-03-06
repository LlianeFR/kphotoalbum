/* Copyright 2012 Jesper K. Pedersen <blackie@kde.org>
  
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License or (at your option) version 3 or any later version
   accepted by the membership of KDE e.V. (or its successor approved
   by the membership of KDE e.V.), which shall act as a proxy
   defined in Section 14 of version 3 of the license.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "UploadImageCollection.h"
#include <Settings/SettingsData.h>

#include <KLocalizedString>

namespace Plugins {

UploadImageCollection::UploadImageCollection(const QString& path)
    :m_path(path)
{
}

QList<QUrl> UploadImageCollection::images()
{
    return QList<QUrl>();
}

QString UploadImageCollection::name()
{
    return QString();
}

QUrl UploadImageCollection::uploadUrl()
{
    return QUrl::fromLocalFile(m_path);
}

QUrl UploadImageCollection::uploadRootUrl()
{
    QUrl url = QUrl::fromLocalFile(Settings::SettingsData::instance()->imageDirectory() );
    return url;
}

QString UploadImageCollection::uploadRootName()
{
    return i18nc("'Name' of the image directory", "Image/Video root directory");
}

} // namespace Plugins
// vi:expandtab:tabstop=4 shiftwidth=4:
