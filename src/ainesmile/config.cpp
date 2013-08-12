#include <QApplication>
#include <QDir>
#include <QDesktopServices>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QStandardPaths>
#endif
#include <QFileInfo>
#include <QtXml>
#include <QRegExp>
#include <QMessageBox>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "config.h"

Config* Config::instance_ = NULL;


boost::property_tree::ptree& Config::pt()
{
    return pt_;
}

Config::Config()
{
    boost::property_tree::read_json(getConfigPath().toStdString(), pt_);
}

void Config::sync()
{
    boost::property_tree::write_json(getConfigPath().toStdString(), pt_);
}

QString Config::getDataDirPath()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QString configPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#else
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#endif
    QDir dir(configPath);
    if (!dir.exists())
    {
        dir.mkpath(configPath);
    }

    return configPath;
}

QString Config::getConfigDirPath()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QString configPath = QDesktopServices::storageLocation(QDesktopServices::ConfigLocation);
#else
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
#endif
    configPath.append("/ainesmile");
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
        if (!QFile(configFile).exists())
        {
            QDir dir(appDirPath);
            dir.cdUp();
            dir.cdUp();
#if defined(Q_OS_WIN)
            dir.cdUp();
#endif
#if defined(Q_OS_MAC)
            dir.cdUp();
            dir.cdUp();
#endif
            dir.cd("resource");
            configFile = configDir.absolutePath();
            configFile.append("/.ainesmilerc");
            Q_ASSERT(QFile::exists(configFile));
        }

        file.copy(configFile, configPath);
    }

    return configPath;
}

QString Config::getThemePath()
{
    std::string currentTheme = pt_.get<std::string>("theme", "Default");
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

        if (!QDir(themeDirPath).exists())
        {
            QDir dir(appDirPath);
            dir.cdUp();
            dir.cdUp();
#if defined(Q_OS_WIN)
            dir.cdUp();
#endif
#if defined(Q_OS_MAC)
            dir.cdUp();
            dir.cdUp();
#endif
            dir.cd("resource");
            themeDirPath = dir.absolutePath();
            Q_ASSERT(dir.exists());
        }

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
        if (!QDir(configFile).exists())
        {
            QDir dir(appDirPath);
            dir.cdUp();
            dir.cdUp();
#if defined(Q_OS_WIN)
            dir.cdUp();
#endif
#if defined(Q_OS_MAC)
            dir.cdUp();
            dir.cdUp();
#endif
            dir.cd("resource");
            configFile = dir.absolutePath();
            Q_ASSERT(dir.exists());
        }
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

void Config::saveLicenseInfo(const QString &username, const QString &licenseCode)
{
    boost::property_tree::ptree pt;
    pt.put("license/username", username.toStdString());
    pt.put("license/code", licenseCode.toStdString());
    QString licensePath = getConfigDirPath() + "/ainesmile.lic";
    boost::property_tree::write_json(licensePath.toStdString(), pt);
}

void Config::loadLicenseInfo(QString &username, QString &licenseCode)
{
    QString licensePath = getConfigDirPath() + "/ainesmile.lic";
    if (QFile::exists(licensePath))
    {
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(licensePath.toStdString(), pt);
        username = pt.get<std::string>("license/username", "").c_str();
        licenseCode = pt.get<std::string>("license/code", "").c_str();
    }
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
