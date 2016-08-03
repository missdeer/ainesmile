#ifndef RECENTFILES_H
#define RECENTFILES_H

#include <QObject>
#include <QStringList>
#include <boost/property_tree/ptree.hpp>

class RecentFiles : public QObject
{
    Q_OBJECT
signals:
    void addRecentFile(const QString& file);
private:
    QStringList files_;
    void init();
    bool exists(const QStringList& container, const QString& file);
public:
    RecentFiles();
    ~RecentFiles();
    void replaceFile(const QString& originalFile, const QString& newFile);
    bool addFile(const QString& file);
    void clearFiles();
    QStringList& recentFiles();
    void sync();
};

#endif // RECENTFILES_H
