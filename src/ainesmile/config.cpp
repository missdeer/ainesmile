#include <QApplication>
#include <QDir>
#include <QDesktopServices>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QStandardPaths>
#endif
#include <QFileInfo>
#include <QtXml>
#include <QRegExp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "config.h"

Config* Config::instance_ = NULL;


boost::property_tree::ptree& Config::pt()
{
    return pt_;
}

void Config::sync()
{
    boost::property_tree::write_json(getConfigPath().toStdString(), pt_);
}

QString Config::getConfigDirPath()
{
#if defined(Q_OS_MAC)
#elif defined(Q_OS_WIN)
    // get config path
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QString configPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#else
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#endif
    configPath.append("/ainesmile");
#else
    // get config path
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QString configPath = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#else
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif
    configPath.append("/.config/ainesmile");
#endif
    QDir dir(configPath);
    if (!dir.exists())
    {
        dir.mkpath(configPath);
    }

    return configPath;
}

QString Config::getConfigPath()
{
    QString configPath = getConfigDirPath();
    configPath.append("/.ainesmilerc");
    QFile file(configPath);
    if (!file.exists())
    {
        // copy from installed directory
        QString appDirPath = QApplication::applicationDirPath();
        QDir configDir(appDirPath);
#if defined(Q_OS_MAC)
        configDir.cdUp();
        configDir.cd("Resource");
#endif
        QString configFile = configDir.absolutePath();
        configFile.append("/.ainesmilerc");
        file.copy(configFile, configPath);
    }

    return configPath;
}

QString Config::getThemePath()
{
    std::string currentTheme = pt_.get<std::string>("theme");
    QString themePath = getConfigDirPath();
    themePath.append("/themes");
    QDir dir(themePath);
    if (!dir.exists())
    {
        dir.mkpath(themePath);
    }
    themePath.append("/");
    themePath.append(currentTheme.c_str());
    themePath.append(".asTheme");
    QDir themeDir(themePath);
    if (!themeDir.exists())
    {
        // try to copy from application installed directory
        QString appDirPath = QApplication::applicationDirPath();
        QDir configDir(appDirPath);
#if defined(Q_OS_MAC)
        configDir.cdUp();
        configDir.cd("Resource");
#endif
        configDir.cd("themes");
        QString themeDirPath = configDir.absolutePath();
        themeDirPath.append("/");
        themeDirPath.append(currentTheme.c_str());
        themeDirPath.append(".asTheme");
        if (!QDir(themeDirPath).exists())
        {
            // if there's no theme that has this name, use the default theme
            themeDirPath = configDir.absolutePath();
            themeDirPath.append("/Default.asTheme");
        }
        // do copy
        themeDir.mkpath(themePath);
        QStringList files = QDir(themeDirPath).entryList(QDir::Files);
        QStringList::const_iterator constIterator;
        for (constIterator = files.constBegin(); constIterator != files.constEnd(); ++constIterator)
        {
            QString src = themeDirPath + "/" + *constIterator;
            QString dst = themePath + "/" + *constIterator;
            QFile::copy(src, dst);
        }
    }
    return themePath;
}

QString Config::getLanguageMapPath()
{
    QString configPath = getConfigDirPath();
    configPath.append("/langmap.xml");
    QFile file(configPath);
    if (!file.exists())
    {
        // copy from installed directory
        QString appDirPath = QApplication::applicationDirPath();
        QDir configDir(appDirPath);
#if defined(Q_OS_MAC)
        configDir.cdUp();
        configDir.cd("Resource");
#endif
        QString configFile = configDir.absolutePath();
        configFile.append("/langmap.xml");
        file.copy(configFile, configPath);
    }

    return configPath;
}

QString Config::getLanguageDirPath()
{
    QString langDirPath = getConfigDirPath() + "/language";
    QDir langDir(langDirPath);
    if (!langDir.exists())
    {
        // copy from installed directory
        QString appDirPath = QApplication::applicationDirPath();
        QDir configDir(appDirPath);
#if defined(Q_OS_MAC)
        configDir.cdUp();
        configDir.cd("Resource");
#endif
        QString configFile = configDir.absolutePath();
        configFile.append("/language");
        // copy all files from configFile to langDirPath
        langDir.mkpath(langDirPath);
        QStringList files = QDir(configFile).entryList(QDir::Files);
        QStringList::const_iterator constIterator;
        for (constIterator = files.constBegin(); constIterator != files.constEnd(); ++constIterator)
        {
            QString src = configFile + "/" + *constIterator;
            QString dst = langDirPath + "/" + *constIterator;
            QFile::copy(src, dst);
        }
    }
    return langDirPath;
}

QString Config::matchPatternLanguage(const QString &filename)
{
    QString langMapPath = getLanguageMapPath();
    QDomDocument doc;
    QFile file(langMapPath);
    if (!file.open(QIODevice::ReadOnly))
        return "";
    if (!doc.setContent(&file))
    {
        file.close();
        return "";
    }
    file.close();

    QDomElement docElem = doc.documentElement();

    QDomElement langElem = docElem.firstChildElement("language");
    while(!langElem.isNull())
    {
        QString pattern = langElem.attribute("pattern");
        QString suffix = langElem.attribute("suffix");
        if (matchSuffix(filename, suffix) || matchPattern(filename, pattern))
        {
            QString name = langElem.attribute("name");
            return name;
        }
        langElem = langElem.nextSiblingElement("language");
    }
    return "";
}

Config::Config()
{
    boost::property_tree::read_json(getConfigPath().toStdString(), pt_);
}

bool Config::matchPattern(const QString &filename, const QString &pattern)
{
#if defined(Q_OS_WIN)
    QRegExp regex(pattern, Qt::CaseInsensitive);
#else
    QRegExp regex(pattern, Qt::CaseSensitive);
#endif
    QFileInfo fi(filename);
    if (regex.exactMatch(fi.fileName()))
        return true;
    return false;
}

bool Config::matchSuffix(const QString &filename, const QString &suffix)
{
    QStringList suffixes = suffix.split(' ');
    QFileInfo fi(filename);
    Q_FOREACH( QString ext, suffixes)
    {
#if defined(Q_OS_WIN)
        if (QString::compare(ext, fi.suffix(), Qt::CaseInsensitive) == 0)
#else
        if (QString::compare(ext, fi.suffix(), Qt::CaseSensitive) == 0)
#endif
        {
            return true;
        }
    }

    return false;
}

Config *Config::instance()
{
    if (!instance_)
    {
        instance_ = new Config;
    }

    Q_ASSERT(instance_);
    return instance_;
}
