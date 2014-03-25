#ifndef REMOTECONTROL_SEARCHINFO_H
#define REMOTECONTROL_SEARCHINFO_H

#include <QStack>
#include <QString>
#include <QDataStream>

#include <tuple>

namespace RemoteControl {

class SearchInfo
{
public:
    void addCategory(const QString& category);
    void addValue(const QString& value);
    void pop();
    void clear();
    QString currentCategory() const;
    QList<std::tuple<QString,QString>> values() const;

    friend QDataStream& operator<<(QDataStream& stream, const SearchInfo& searchInfo);
    friend QDataStream& operator>>(QDataStream& stream, SearchInfo& searchInfo);

private:
    QStack<QString> m_categories;
    QStack<QString> m_values;
};

QDataStream& operator<<(QDataStream& stream, const SearchInfo& searchInfo);
QDataStream& operator>>(QDataStream& stream, SearchInfo& searchInfo);

} // namespace RemoteControl

#endif // REMOTECONTROL_SEARCHINFO_H
