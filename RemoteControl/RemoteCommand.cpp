#include "RemoteCommand.h"

#include <QBuffer>
#include <QMap>

using namespace RemoteControl;

RemoteCommand::RemoteCommand(const QString& id)
    :m_id(id)
{
}

QString RemoteCommand::id() const
{
    return m_id;
}

RemoteCommand& RemoteCommand::command(const QString& id)
{
    static QMap<QString, RemoteCommand*> map;
    if (map.isEmpty()) {
        QList<RemoteCommand*> commands;
        commands << new NextSlideCommand
                 << new PreviousSlideCommand
                 << new ImageUpdateCommand
                 << new ImageCountUpdateCommand
                 << new CategoryListCommand;

        for (RemoteCommand* command : commands )
             map.insert(command->id(), command);
    }

    return *map[id];
}


NextSlideCommand::NextSlideCommand()
    :RemoteCommand(id())
{
}

QString NextSlideCommand::id()
{
    return QString::fromUtf8("Next Slide");
}


PreviousSlideCommand::PreviousSlideCommand()
    :RemoteCommand(id())
{
}

QString PreviousSlideCommand::id()
{
    return QString::fromUtf8("Previous Slide");
}


ImageUpdateCommand::ImageUpdateCommand(int index, const QImage& image)
    :RemoteCommand(id()), index(index), image(image)
{
}

QString ImageUpdateCommand::id()
{
    return QString::fromUtf8("Image Update");
}

void ImageUpdateCommand::encodeData(QBuffer& buffer) const
{
    QDataStream stream(&buffer);
    stream << index;
    image.save(&buffer,"JPEG");
}

void ImageUpdateCommand::decodeData(QBuffer& buffer)
{
    QDataStream stream(&buffer);
    stream >> index;
    image.load(&buffer, "JPEG");
}


ImageCountUpdateCommand::ImageCountUpdateCommand()
    :RemoteCommand(id())
{
}

QString ImageCountUpdateCommand::id()
{
    return QString::fromUtf8("Image Count Update");
}

void ImageCountUpdateCommand::encodeData(QBuffer& buffer) const
{
    QDataStream stream(&buffer);
    stream << count;
}

void ImageCountUpdateCommand::decodeData(QBuffer& buffer)
{
    QDataStream stream(&buffer);
    stream >> count;
}

CategoryListCommand::CategoryListCommand()
    : RemoteCommand(id())
{
}

QString CategoryListCommand::id()
{
    return QString::fromUtf8("Category List");
}

void CategoryListCommand::encodeData(QBuffer& buffer) const
{
    QDataStream stream(&buffer);
    stream << categories.count();
    for (const Category& category : categories)
        stream << category.name << category.text << category.icon;
    stream << home << kphotoalbum;
}

void CategoryListCommand::decodeData(QBuffer& buffer)
{
    QDataStream stream(&buffer);
    int count;
    stream >> count;
    for (int i=0; i<count; ++i) {
        QString name;
        QString text;
        QImage icon;
        stream >> name >> text >> icon;
        categories.append( {name, text, icon});
    }
    stream >> home >> kphotoalbum;
}
