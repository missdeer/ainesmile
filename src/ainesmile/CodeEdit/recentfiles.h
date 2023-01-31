#ifndef RECENTFILES_H
#define RECENTFILES_H

#include <boost/property_tree/ptree.hpp>

#include <QObject>
#include <QStringList>


class RecentFiles : public QObject
{
    Q_OBJECT
signals:
    void addRecentFile(const QString &file);

private:
    QStringList files_;
    void        init();
    bool        exists(const QStringList &container, const QString &file);

public:
    RecentFiles();
    ~RecentFiles() override;
    void         replaceFile(const QString &originalFile, const QString &newFile);
    bool         addFile(const QString &file);
    void         clearFiles();
    QStringList &recentFiles();
    void         sync();
};

#endif // RECENTFILES_H
