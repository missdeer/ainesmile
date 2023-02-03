#include "stdafx.h"

#include "recentfiles.h"
#include "config.h"

RecentFiles::RecentFiles()
{
    init();
}

RecentFiles::~RecentFiles()
{
    sync();
}

void RecentFiles::replaceFile(const QString &originalFile, const QString &newFile)
{
    QFileInfo fileInfo(originalFile);
    auto      iter = std::find_if(files_.begin(), files_.end(), [&](const QString &f) { return QFileInfo(f) == fileInfo; });
    if (files_.end() != iter)
    {
        *iter = newFile;
    }
}

bool RecentFiles::addFile(const QString &file)
{
    if (!exists(files_, file))
    {
        files_.append(file);
        while (files_.size() > 10)
        {
            files_.removeFirst();
        }
        emit addRecentFile(file);
        return true;
    }

    return false;
}

void RecentFiles::clearFiles()
{
    files_.clear();
}

QStringList &RecentFiles::recentFiles()
{
    return files_;
}

void RecentFiles::sync()
{
    QString filePath = Config::instance()->getConfigDirPath();
    filePath.append("/recent");
    boost::property_tree::ptree pt;

    std::for_each(files_.begin(), files_.end(), [&pt](const QString &f) { pt.add("ainesmile.recentfiles.file", f.toStdString()); });
    try
    {
        boost::property_tree::write_json(filePath.toStdString(), pt);
    }
    catch (boost::property_tree::ptree_error &)
    {
    }
}

bool RecentFiles::exists(const QStringList &container, const QString &file)
{
    QFileInfo fileInfo(file);
    auto      it = std::find_if(container.begin(), container.end(), [&fileInfo](const QString &f) { return QFileInfo(f) == fileInfo; });
    return container.end() != it;
}

void RecentFiles::init()
{
    QString filePath = Config::instance()->getConfigDirPath();
    filePath.append("/recent");
    if (QFile::exists(filePath))
    {
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(filePath.toStdString(), pt);
        try
        {
            for (auto &val : pt.get_child("ainesmile.recentfiles"))
            {
                QString file(val.second.data().c_str());
                if (QFile::exists(file))
                {
                    files_.append(file);
                }
            }
        }
        catch (boost::property_tree::ptree_error &)
        {
        }
    }
}
