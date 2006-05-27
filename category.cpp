#include "category.h"
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include "imagedb.h"
#include "Settings/Settings.h"
#include "membermap.h"

QPixmap Category::icon( int size ) const
{
    return KGlobal::iconLoader()->loadIcon( iconName(), KIcon::Desktop, size );
}


/**
   If one person from say Denmark sends a database to a person from say germany, then the title of
   Persons, Locations, and Keywords will still be translated correct, when this function is used.
*/
QString Category::text() const
{
    if ( name() == QString::fromLatin1( "Persons" ) )
        return i18n("Persons");
    else if ( name() == QString::fromLatin1( "Locations" ) )
        return i18n("Locations");
    else if ( name() == QString::fromLatin1( "Keywords" ) )
        return i18n("Keywords");
    else if ( name() == QString::fromLatin1( "Folder" ) )
        return i18n("Folder");
    else if ( name() == QString::fromLatin1( "Tokens" ) )
        return i18n("Tokens");
    else
        return name();
}

QStringList Category::itemsInclGroups() const
{
        // values including member groups

    QStringList items = this->items();

    // add the groups to the list too, but only if the group is not there already, which will be the case
    // if it has ever been selected once.
    QStringList groups = ImageDB::instance()->memberMap().groups( name() );
    for( QStringList::Iterator it = groups.begin(); it != groups.end(); ++it ) {
        if ( ! items.contains(  *it ) )
            items << *it ;
    };
    if ( Settings::Settings::instance()->viewSortType() == Settings::SortAlpha )
        items.sort();
    return items;
}

#include "category.moc"
