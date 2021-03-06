/* Copyright (C) 2014-2016 Tobias Leupold <tobias.leupold@web.de>

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

#ifndef COPYPOPUP_H
#define COPYPOPUP_H

// Qt includes
#include <QMenu>
#include <QList>
#include <QUrl>

namespace MainWindow
{

class CopyPopup : public QMenu
{
    Q_OBJECT

public:
    enum CopyType {
        Copy,
        Link
    };

    enum CopyAction {
        CopyCurrent,
        CopyAll,
        LinkCurrent,
        LinkAll
    };

    explicit CopyPopup(QWidget *parent,
                       QUrl &selectedFile,
                       QList<QUrl> &allSelectedFiles,
                       QString &lastTarget,
                       CopyType copyType);

private slots:
    void slotCopy(QAction *action);

private:
    QUrl &m_selectedFile;
    QList<QUrl> &m_allSelectedFiles;
    QString &m_lastTarget;
};

}

#endif // COPYPOPUP_H

// vi:expandtab:tabstop=4 shiftwidth=4:
