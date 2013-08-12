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
public:
    RecentFiles();
    ~RecentFiles();
    void addFile(const QString& file);
    QStringList& recentFiles();
    void addProject(const QString& project);
    QStringList& recentProjects();
    void sync();
};

#endif // RECENTFILES_H
