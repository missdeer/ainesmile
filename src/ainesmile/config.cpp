#include "stdafx.h"

#include <QRegularExpression>

#include "config.h"

Config *Config::instance_ = NULL;

boost::property_tree::ptree &Config::pt()
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
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir dir(configPath);
    if (!dir.exists())
    {
        dir.mkpath(configPath);
    }

    return configPath;
}

QString Config::getConfigDirPath()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);

#if !defined(Q_OS_WIN)
    configPath.append("/dfordsoft.com/ainesmile");
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
        QDir    configDir(appDirPath);
#if defined(Q_OS_MAC)
        configDir.cdUp();
        configDir.cd("Resources");
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

    qDebug() << "config path: " << configPath;
    return configPath;
}

QString Config::getThemePath()
{
    std::string currentTheme = pt_.get<std::string>("theme", "Default");
    QString     themePath    = getConfigDirPath();
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
    if (!themeDir.exists() || themeDir.entryList(QDir::Files).isEmpty())
    {
        // try to copy from application installed directory
        QString appDirPath = QApplication::applicationDirPath();
        QDir    configDir(appDirPath);
#if defined(Q_OS_MAC)
        configDir.cdUp();
        configDir.cd("Resources");
#endif

        QString themeDirPath;
        if (!configDir.cd("themes") || configDir.entryList(QDir::Files).isEmpty())
        {
            QDir dir(appDirPath);
            dir.cdUp();
            dir.cdUp();
#if defined(Q_OS_LINUX)
#elif defined(Q_OS_MAC)
            dir.cdUp();
            dir.cdUp();
#else
            dir.cdUp();
#endif
            dir.cd("resource/themes");
            themeDirPath = dir.absolutePath();
            Q_ASSERT(dir.exists());
        }
        else
        {
            themeDirPath = configDir.absolutePath();
        }

        themeDirPath.append("/");
        themeDirPath.append(currentTheme.c_str());
        themeDirPath.append(".asTheme");
        if (!QDir(themeDirPath).exists() || QDir(themeDirPath).entryList(QDir::Files).isEmpty())
        {
            // if there's no theme that has this name, use the default theme
            themeDirPath = configDir.absolutePath();
            themeDirPath.append("/Default.asTheme");
        }
        // do copy
        themeDir.mkpath(themePath);
        QStringList files = QDir(themeDirPath).entryList(QDir::Files);
        std::for_each(files.begin(), files.end(), [&](const QString &f) { QFile::copy(themeDirPath + "/" + f, themePath + "/" + f); });
    }
    qDebug() << "theme path: " << themePath;
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
        QDir    configDir(appDirPath);
#if defined(Q_OS_MAC)
        configDir.cdUp();
        configDir.cd("Resources");
#endif
        QString configFile = configDir.absolutePath();
        configFile.append("/langmap.xml");
        if (!QFile::exists(configFile))
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
            Q_ASSERT(dir.exists());
            configFile = dir.absolutePath();
            configFile.append("/langmap.xml");
            Q_ASSERT(QFile::exists(configFile));
        }
        qDebug() << "copy langmap from " << configFile << " to " << configPath;
        file.copy(configFile, configPath);
    }
    qDebug() << "langmap: " << configPath;
    return configPath;
}

QString Config::getLanguageDirPath()
{
    QString langDirPath = getConfigDirPath() + "/language";
    QDir    langDir(langDirPath);
    if (!langDir.exists() || langDir.entryList(QDir::Files).isEmpty())
    {
        // copy from installed directory
        QString appDirPath = QApplication::applicationDirPath();
        QDir    configDir(appDirPath);
#if defined(Q_OS_MAC)
        configDir.cdUp();
        configDir.cd("Resources");
#endif
        QString configFile = configDir.absolutePath();
        configFile.append("/language");
        QDir langDirOrig(configFile);
        if (!langDirOrig.exists() || langDirOrig.entryList(QDir::Files).isEmpty())
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
            dir.cd("resource/language");
            configFile = dir.absolutePath();
        }
        // copy all files from configFile to langDirPath
        langDir.mkpath(langDirPath);
        QStringList files = QDir(configFile).entryList(QDir::Files);
        std::for_each(files.begin(), files.end(), [&](const QString &f) { QFile::copy(configFile + "/" + f, langDirPath + "/" + f); });
    }
    qDebug() << "langDir: " << langDirPath;
    return langDirPath;
}

QString Config::matchPatternLanguage(const QString &filename)
{
    QString      langMapPath = getLanguageMapPath();
    QDomDocument doc;
    QFile        file(langMapPath);
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
    while (!langElem.isNull())
    {
        QString pattern = langElem.attribute("pattern");
        QString suffix  = langElem.attribute("suffix");
        if (matchSuffix(filename, suffix) || matchPattern(filename, pattern))
        {
            QString name = langElem.attribute("name");
            return name;
        }
        langElem = langElem.nextSiblingElement("language");
    }
    return "";
}

bool Config::matchPattern(const QString &filename, const QString &pattern)
{
    QRegularExpression regex(pattern);
#if defined(Q_OS_WIN)
    regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
#else
    regex.setPatternOptions(QRegularExpression::CaseSensitiveOption);
#endif
    QFileInfo fi(filename);
    auto      match = regex.match(fi.fileName());
    if (match.hasMatch())
        return true;
    return false;
}

bool Config::matchSuffix(const QString &filename, const QString &suffix)
{
    QStringList suffixes = suffix.split(' ');
    QFileInfo   fi(filename);
    auto        it = std::find_if(suffixes.begin(), suffixes.end(), [&fi](const QString &ext) {
#if defined(Q_OS_WIN)
        return (QString::compare(ext, fi.suffix(), Qt::CaseInsensitive) == 0);
#else
        return (QString::compare(ext, fi.suffix(), Qt::CaseSensitive) == 0);
#endif
    });
    return suffixes.end() != it;
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
