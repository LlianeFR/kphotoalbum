/*
  Copyright (C) 2007 Tuomas Suutari <thsuut@utu.fi>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program (see the file COPYING); if not, write to the
  Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
  MA 02110-1301 USA.
*/

#ifndef SQLDB_DATABASEMANAGERS_H
#define SQLDB_DATABASEMANAGERS_H

#include "DatabaseManager.h"
#include "ConnectionParameters.h"
#include <kexidb/driver.h>
#include <kexidb/connection.h>

namespace SQLDB
{
    class KexiDBDatabaseManager: public DatabaseManager
    {
    public:
        KexiDBDatabaseManager(const ConnectionParameters& connParams,
	                      const QString& driverName,
                              KexiDB::Driver* driver);

	~KexiDBDatabaseManager();

        virtual QStringList databases() const;

        virtual bool databaseExists(const QString& databaseName) const;

        virtual void createDatabase(const QString& databaseName,
                                    const Schema::DatabaseSchema& schema);

        virtual DatabaseConnection
        connectToDatabase(const QString& databaseName);

    private:
        KexiDBDatabaseManager(const KexiDBDatabaseManager&);
        void operator=(const KexiDBDatabaseManager&);

        KexiDB::Connection* createConnection();

        ConnectionParameters _connParams;
	QString _driverName;
        KexiDB::Driver* _driver;
        KexiDB::Connection* _conn;
    };
}

#endif /* SQLDB_DATABASEMANAGERS_H */