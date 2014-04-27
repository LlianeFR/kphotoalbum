#include "RemoteInterface.h"
#include "Client.h"
#include "RemoteCommand.h"
#include "ImageStore.h"

#include <QTcpSocket>
#include <qimage.h>
#include <QLabel>
#include <QBuffer>
#include <QDataStream>
#include <ScreenInfo.h>
#include "Action.h"
#include <QCoreApplication>
#include <memory>
#include "ImageDetails.h"

using namespace RemoteControl;

RemoteInterface::RemoteInterface()
    : m_categories(new CategoryModel(this)), m_categoryItems(new ThumbnailModel(this)), m_thumbnailModel(new ThumbnailModel(this)),
      m_discoveryModel(new DiscoveryModel(this))
{
    m_connection = new Client;
    connect(m_connection, SIGNAL(gotCommand(RemoteCommand)), this, SLOT(handleCommand(RemoteCommand)));
    connect(m_connection, &Client::connectionChanged,this, &RemoteInterface::connectionChanged);
    connect(m_connection, &Client::gotConnected, this, &RemoteInterface::requestInitialData);
    connect(&ScreenInfo::instance(), &ScreenInfo::overviewIconSizeChanged, this, &RemoteInterface::requestHomePageImages);
    qRegisterMetaType<RemoteControl::CategoryModel*>("RemoteControl::CategoryModel*");
    qRegisterMetaType<RemoteControl::ThumbnailModel*>("ThumbnailModel*");
    qRegisterMetaType<RemoteControl::DiscoveryModel*>("DiscoveryModel*");
}

void RemoteInterface::setCurrentPage(Page page)
{
    if (m_currentPage != page) {
        m_currentPage = page;
        emit currentPageChanged();
    }
}

void RemoteInterface::setListCategoryValues(const QStringList& values)
{
    if (m_listCategoryValues != values) {
        m_listCategoryValues = values;
        emit listCategoryValuesChanged();
    }
}

void RemoteInterface::requestHomePageImages()
{
    m_connection->sendCommand(RequestHomePageImages(ScreenInfo::instance().overviewIconSize()));
}

void RemoteInterface::setHomePageImages(const HomePageData& command)
{
    m_homeImage = command.homeIcon;
    emit homeImageChanged();

    m_kphotoalbumImage = command.kphotoalbumIcon;
    emit kphotoalbumImageChange();

    m_discoveryImage = command.discoverIcon;
    emit discoverImageChanged();
}

RemoteInterface& RemoteInterface::instance()
{
    static RemoteInterface interface;
    return interface;
}

bool RemoteInterface::isConnected() const
{
    return m_connection->isConnected();
}

void RemoteInterface::sendCommand(const RemoteCommand& command)
{
    m_connection->sendCommand(command);
}

QString RemoteInterface::currentCategory() const
{
    return m_search.currentCategory();
}

QImage RemoteInterface::discoveryImage() const
{
    return m_discoveryImage;
}

void RemoteInterface::goHome()
{
    requestInitialData();
}

void RemoteInterface::goBack()
{
    if(m_history.canGoBack())
        m_history.goBackward();
    else
        qApp->quit();
}

void RemoteInterface::goForward()
{
    if (m_history.canGoForward())
        m_history.goForward();
}

void RemoteInterface::selectCategory(const QString& category, int type)
{
    m_search.addCategory(category);
    m_history.push(std::unique_ptr<Action>(new ShowCategoryValueAction(m_search, static_cast<CategoryViewType>(type))));
}

void RemoteInterface::selectCategoryValue(const QString& value)
{
    m_search.addValue(value);
    m_history.push(std::unique_ptr<Action>(new ShowOverviewAction(m_search)));
}

void RemoteInterface::showThumbnails()
{
    m_history.push(std::unique_ptr<Action>(new ShowThumbnailsAction(m_search)));
}

void RemoteInterface::showImage(int imageId)
{
    m_history.push(std::unique_ptr<Action>(new ShowImagesAction(imageId, m_search)));
}

void RemoteInterface::showDiscoveredImage(int imageId)
{
    m_history.push(std::unique_ptr<Action>(new ShowDiscoveredImage(imageId)));
}

void RemoteInterface::requestDetails(int imageId)
{
    m_connection->sendCommand(RequestDetails(imageId));
}

void RemoteInterface::activateSearch(const QString& search)
{
    QStringList list = search.split(";;;");
    QString category = list[0];
    QString item = list[1];
    SearchInfo result;
    result.addCategory(category);
    result.addValue(item);
    m_history.push(std::unique_ptr<Action>(new ShowThumbnailsAction(result)));
}

void RemoteInterface::doDiscover()
{
    m_history.push(std::unique_ptr<Action>(new DiscoverAction(m_search)));
}

void RemoteInterface::setCurrentView(int imageId)
{
    emit jumpToImage(m_thumbnailModel->indexOf(imageId));
}

void RemoteInterface::requestInitialData()
{
    m_history.push(std::unique_ptr<Action>(new ShowOverviewAction({})));
}

void RemoteInterface::handleCommand(const RemoteCommand& command)
{
    if (command.id() == ImageUpdateCommand::id())
        updateImage(static_cast<const ImageUpdateCommand&>(command));
    else if (command.id() == CategoryListCommand::id())
        updateCategoryList(static_cast<const CategoryListCommand&>(command));
    else if (command.id() == SearchResultCommand::id())
        gotSearchResult(static_cast<const SearchResultCommand&>(command));
    else if (command.id() == TimeCommand::id())
        ; // Used for debugging, it will print time stamp when decoded
    else if (command.id() == ImageDetailsCommand::id())
        ImageDetails::instance().setData(static_cast<const ImageDetailsCommand&>(command));
    else if (command.id() == CategoryItems::id())
        setListCategoryValues(static_cast<const CategoryItems&>(command).items);
    else if (command.id() == HomePageData::id())
        setHomePageImages(static_cast<const HomePageData&>(command));
    else
        qFatal("Unhandled command");
}

void RemoteInterface::updateImage(const ImageUpdateCommand& command)
{
    ImageStore::instance().updateImage(command.imageId, command.image, command.label, command.type);
}

void RemoteInterface::updateCategoryList(const CategoryListCommand& command)
{
    ScreenInfo::instance().setCategoryCount(command.categories.count());
    m_categories->setCategories(command.categories);
}

void RemoteInterface::gotSearchResult(const SearchResultCommand& result)
{
    if (result.type == SearchType::Images) {
        m_activeThumbnailModel->setImages(result.result);
    }
    else if (result.type == SearchType::CategoryItems) {
        m_categoryItems->setImages(result.result);
    }
}
