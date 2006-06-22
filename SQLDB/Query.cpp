#include "Query.h"
#include "DB/MemberMap.h"
#include "DB/ImageDB.h"
#include "DB/ImageSearchInfo.h"
#include <qsqlquery.h>
#include "Settings/SettingsData.h"
#include "QueryUtil.h"
#include "QueryHelper.h"
#include <kdebug.h>
#include "config.h" // HASKEXIDB

QValueList<int> SQLDB::filesMatchingQuery( const DB::ImageSearchInfo& info )
{
    QValueList< QValueList< DB::OptionSimpleMatcher*> > matches = info.query();
    // matches is in Disjunctive Normal Form ( OR(AND(a,b),AND(c,d)) )

    if ( matches.count() == 0 ) {
#ifndef HASKEXIDB
        return allImages();
#else
        return QueryHelper::instance()->allMediaItemIds();
#endif
    }

    // TODO: rest
#ifdef HASKEXIDB
    return QueryHelper::instance()->allMediaItemIds();
#endif

    QValueList<int> result;
    for( QValueList< QValueList<DB::OptionSimpleMatcher*> >::Iterator it = matches.begin(); it != matches.end(); ++it ) {
        result = mergeUniqly( result, runCategoryQuery( *it ) );
    }

    return result;
}

QValueList<int> SQLDB::runCategoryQuery( QValueList<DB::OptionSimpleMatcher*> matches )
{
    QValueList< DB::OptionSimpleMatcher*> possitiveList;
    QValueList< DB::OptionSimpleMatcher*> negativeList;
    split( matches, possitiveList, negativeList );

    // Prefix: SELECT q0.fileId from imagecategoryinfo q0, imagecategoryinfo q1, ... WHERE q0.fileId = q1.fileId and q0.fileId = q1.fileId
    // Query: q1.categoryId = 3 AND q1.value = "Jesper" and ...

    // Positive part of the query
    QStringList positiveQuery;
    int idx = 0;
    QMap<QString,QStringList> matchedValues;
    for( QValueList<DB::OptionSimpleMatcher*>::Iterator it = possitiveList.begin(); it != possitiveList.end(); ++it, ++idx ) {
        DB::OptionValueMatcher* valueMatcher = static_cast<DB::OptionValueMatcher*>( *it );
        positiveQuery << buildValue( valueMatcher->_category, values( valueMatcher), idx, false );
        matchedValues[valueMatcher->_category] += values( valueMatcher );
    }


    // Negative query
    QStringList negativeQuery;
    idx = 0;
    for( QValueList<DB::OptionSimpleMatcher*>::Iterator it = negativeList.begin(); it != negativeList.end(); ++it, ++idx ) {
        DB::OptionValueMatcher* valueMatcher;
        if ( ( valueMatcher = dynamic_cast<DB::OptionValueMatcher*>( *it ) ) ) {
            negativeQuery << buildValue( valueMatcher->_category, values( valueMatcher), idx, false );
        }
        else
            negativeQuery << buildValue( (*it)->_category, matchedValues[(*it)->_category ], idx, true );
    }

    QValueList<int> positive;
    QValueList<int> negative;

    if ( possitiveList.count() > 0 )
        positive = runAndReturnIntList( QString::fromLatin1( "%1 %2 %3" )
                                     .arg( buildQueryPrefix( possitiveList.count(), 0 ) )
                                     .arg( possitiveList.count() > 1 ? QString::fromLatin1( " AND " ) : QString::null )
                                     .arg( positiveQuery.join( QString::fromLatin1( " AND " ) ) ) );
    else
        positive = allImages();

    if ( negativeList.count() > 0 )
        negative = runAndReturnIntList( QString::fromLatin1( "%1 %2 (%3)" )
                                     .arg( buildQueryPrefix( negativeList.count(), 0 ) )
                                     .arg( negativeList.count() > 1 ? QString::fromLatin1( " AND " ) : QString::null )
                                     .arg( negativeQuery.join( QString::fromLatin1( " OR " ) ) ) );

    return listSubstract( positive, negative );
}


QString SQLDB::buildValue( const QString& category, const QStringList& vals, int idx, bool negate )
{
    QString expression;
    QString prefix;
    if ( idx != -1 )
        prefix = QString::fromLatin1("q%1.").arg( idx );

    if ( !vals.isEmpty() ) {
        for( QStringList::ConstIterator it = vals.begin(); it != vals.end(); ++it ) {
            if ( !expression.isEmpty() )
                expression += QString::fromLatin1( " or " );
            expression += QString::fromLatin1( "%1value = \"%2\"" ).arg( prefix ).arg( *it );
        }
        if ( negate )
            expression = QString::fromLatin1( "!(%1)" ).arg( expression );
        else
            expression = QString::fromLatin1( "(%1)" ).arg( expression );

        return QString::fromLatin1( "%1categoryId = \"%2\" and %3 " )
            .arg(prefix).arg( idForCategory(category) ).arg( expression );
    }
    else
        return QString::fromLatin1( "%1categoryId = \"%2\" " ).arg(prefix).arg( idForCategory(category) );
}

QStringList SQLDB::values( DB::OptionValueMatcher* matcher )
{
    QStringList values;
    values.append( matcher->_option );

    if ( DB::ImageDB::instance()->memberMap().isGroup( matcher->_category, matcher->_option ) )
        values += DB::ImageDB::instance()->memberMap().members( matcher->_category, matcher->_option, true );
    return values;
}

void SQLDB::split( const QValueList<DB::OptionSimpleMatcher*>& input,
                   QValueList<DB::OptionSimpleMatcher*>& positiveList,
                   QValueList<DB::OptionSimpleMatcher*>& negativeList )
{
    for( QValueList<DB::OptionSimpleMatcher*>::ConstIterator it = input.constBegin(); it != input.constEnd(); ++it ) {
        DB::OptionValueMatcher* valueMatcher;
        if ( ( valueMatcher = dynamic_cast<DB::OptionValueMatcher*>( *it ) ) ) {
            if ( valueMatcher->_sign )
                positiveList.append( valueMatcher );
            else
                negativeList.append( valueMatcher );
        }
        else
            negativeList.append( *it );
    }
}

QString SQLDB::buildQueryPrefix( int count, int firstId )
{
    //  SELECT q0.fileId from imagecategoryinfo q0, imagecategoryinfo q1, ... WHERE q0.fileId = q1.fileId and q0.fileId = q1.fileId ....

    QStringList prefixes;
    QStringList matches;

    for ( int i = 0; i < count; ++i ) {
        prefixes << QString::fromLatin1( "imagecategoryinfo q%1" ).arg( i + firstId );
        if ( i != 0 )
            matches << QString::fromLatin1( "q0.fileId = q%1.fileId" ).arg( i + firstId );
    }

    return QString::fromLatin1( "SELECT q%1.fileId from %2 WHERE %3" ).arg(firstId).arg( prefixes.join( QString::fromLatin1( ", " ) ) )
        .arg( matches.join( QString::fromLatin1( " and " ) ) );
}

QValueList<int> SQLDB::mergeUniqly( QValueList<int> l1, QValueList<int> l2)
{
    QValueList<int> result;
    l1 += l2;
    qHeapSort( l1 );

    QValueList<int>::Iterator it = l1.begin();
    int last = *it;
    result.append( last );
    ++it;

    for( ; it != l1.end(); ++it ) {
        if( *it != last ) {
            last = *it;
            result.append( last );
        }
    }
    return result;
}


QValueList<int> SQLDB::listSubstract( QValueList<int> l1, QValueList<int> l2)
{
    qHeapSort(l1);
    qHeapSort(l2);
    QValueList<int>::Iterator it1 = l1.begin();
    QValueList<int>::Iterator it2 = l2.begin();

    QValueList<int> result;

    while ( it1 != l1.end() && it2 != l2.end()) {
        if ( *it1 == *it2 ) {
            ++it1; // There might be duplicates, so do not increment it2
        }

        else if ( *it1 < *it2 ) {
            result.append( *it1 );
            ++it1;
        }
        else {
            ++it2;
        }
    }

    while ( it1 != l1.end() ) {
        result.append( *it1 );
        ++it1;
    }

    return result;
}

