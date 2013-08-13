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
    void addRecentProject(const QString& project);
private:
    QStringList files_;
    QStringList projects_;
    void init();
    bool exists(const QStringList& container, const QString& file);
public:
    RecentFiles();
    ~RecentFiles();
    bool addFile(const QString& file);
    void clearFiles();
    QStringList& recentFiles();
    bool addProject(const QString& project);
    void clearProjects();
    QStringList& recentProjects();
    void sync();
};

#endif // RECENTFILES_H
