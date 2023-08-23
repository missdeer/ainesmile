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
    auto      iter = std::find_if(m_files.begin(), m_files.end(), [&](const QString &f) { return QFileInfo(f) == fileInfo; });
    if (m_files.end() != iter)
    {
        *iter = newFile;
    }
}

bool RecentFiles::addFile(const QString &file)
{
    if (!exists(m_files, file))
    {
        m_files.append(file);
        while (m_files.size() > 10)
        {
            m_files.removeFirst();
        }
        emit addRecentFile(file);
        return true;
    }

    return false;
}

void RecentFiles::clearFiles()
{
    m_files.clear();
}

QStringList &RecentFiles::recentFiles()
{
    return m_files;
}

void RecentFiles::sync()
{
    QString filePath = Config::instance()->getConfigDirPath();
    filePath.append("/recent");
    boost::property_tree::ptree ptree;

    std::for_each(
        m_files.begin(), m_files.end(), [&ptree](const QString &filePath) { ptree.add("ainesmile.recentfiles.file", filePath.toStdString()); });
    try
    {
        boost::property_tree::write_json(filePath.toStdString(), ptree);
    }
    catch (boost::property_tree::ptree_error &)
    {
    }
}

bool RecentFiles::exists(const QStringList &container, const QString &file)
{
    QFileInfo fileInfo(file);
    auto iter = std::find_if(container.begin(), container.end(), [&fileInfo](const QString &filePath) { return QFileInfo(filePath) == fileInfo; });
    return container.end() != iter;
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
                    m_files.append(file);
                }
            }
        }
        catch (boost::property_tree::ptree_error &)
        {
        }
    }
}
