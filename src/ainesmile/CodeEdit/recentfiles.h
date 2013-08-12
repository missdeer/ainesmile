#ifndef RECENTFILES_H
#define RECENTFILES_H

#include <QObject>
#include <boost/property_tree/ptree.hpp>

class RecentFiles : public QObject
{
    Q_OBJECT
signals:
    void addRecentFile(const QString& file);
    void addRecentProject(const QString& project);
private:
    QList<QString> files_;
    QList<QString> projects_;
public:
    RecentFiles();
    ~RecentFiles();
    void addFile(const QString& file);
    QList<QString>& recentFiles();
    void addProject(const QString& project);
    QList<QString>& recentProjects();
    void sync();
};

#endif // RECENTFILES_H
