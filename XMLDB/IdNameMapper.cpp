#include "IdNameMapper.h"

void DB::IdNameMapper::add( const QString& fileName )
{
    const int id = ++_maxId;
    _idTofileName.insert( id, fileName );
    _fileNameToId.insert( fileName, id );
    Q_ASSERT( !fileName.startsWith(QLatin1String("/")));
}

int DB::IdNameMapper::operator[](const QString& fileName ) const
{
    Q_ASSERT( !fileName.startsWith( QLatin1String("/") ) );
    Q_ASSERT( _fileNameToId.contains( fileName ) );
    return _fileNameToId[fileName];
}

QString DB::IdNameMapper::operator[]( int id ) const
{
    Q_ASSERT( _idTofileName.contains( id ) );
    return _idTofileName[id];
}

void DB::IdNameMapper::remove( int id )
{
    Q_ASSERT( _idTofileName.contains( id ) );
    _fileNameToId.remove( _idTofileName[id] );
    _idTofileName.remove( id );
}

void DB::IdNameMapper::remove( const QString& fileName )
{
    Q_ASSERT( _fileNameToId.contains( fileName ) );
    Q_ASSERT( !fileName.startsWith( QLatin1String("/") ) );
    _idTofileName.remove( _fileNameToId[fileName] );
    _fileNameToId.remove( fileName );
}

DB::IdNameMapper::IdNameMapper()
    :_maxId(0)
{
}