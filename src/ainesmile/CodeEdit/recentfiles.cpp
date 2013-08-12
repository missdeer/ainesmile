#include <boost/foreach.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <QFile>
#include <QFileInfo>
#include "config.h"
#include "recentfiles.h"

bool RecentFiles::exists(const QStringList &container, const QString &file)
{
    QFileInfo fileInfo(file);
    Q_FOREACH( const QString& f, container)
    {
        QFileInfo fi(f);
        if (fi == fileInfo)
            return true;
    }

    return false;
}

RecentFiles::RecentFiles()
{
    QString filePath = Config::instance()->getDataDirPath();
    filePath.append("/recent");
    if (QFile::exists(filePath))
    {
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(filePath.toStdString(), pt);
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v,
                      pt.get_child("ainesmile.recentfiles"))
        {
            QString file(v.second.data().c_str());
            files_.append(file);
        }
    }
}

RecentFiles::~RecentFiles()
{
    sync();
}

bool RecentFiles::addFile(const QString &file)
{
    if (!exists(files_, file))
    {
        files_.append(file);
        while(files_.size() > 10)
            files_.removeFirst();
        emit addRecentFile(file);
        return true;
    }

    return false;
}

QStringList& RecentFiles::recentFiles()
{
    return files_;
}

bool RecentFiles::addProject(const QString &project)
{
    if (!exists(projects_, project))
    {
        projects_.append(project);
        while(projects_.size() > 10)
            projects_.removeFirst();
        emit addRecentProject(project);
        return true;
    }
    return false;
}

QStringList &RecentFiles::recentProjects()
{
    return projects_;
}

void RecentFiles::sync()
{
    QString filePath = Config::instance()->getDataDirPath();
    filePath.append("/recent");
    boost::property_tree::ptree pt;
    for (QList<QString>::const_iterator it = files_.constBegin();
         files_.constEnd() != it;
         ++it)
    {
        pt.add("ainesmile.recentfiles.file", it->toStdString());
    }
    boost::property_tree::write_json(filePath.toStdString(), pt);
}
