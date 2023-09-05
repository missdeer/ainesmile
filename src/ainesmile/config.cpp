#include "stdafx.h"

#include <boost/scope_exit.hpp>

#include <QRegularExpression>
#include <QtCore/qcontainerfwd.h>
#include <QtCore/qstringliteral.h>

#include "config.h"

Config *Config::instance_ = nullptr;

boost::property_tree::ptree &Config::pt()
{
    return m_ptree;
}

Config::Config()
{
    init();
}

bool Config::sync()
{
    m_ok = true;
    try
    {
        boost::property_tree::write_json(getConfigPath().toStdString(), m_ptree);
    }
    catch (boost::property_tree::ptree_error &)
    {
        m_ok = false;
    }
    return m_ok;
}

bool Config::init()
{
    m_ok = true;
    try
    {
        boost::property_tree::read_json(getConfigPath().toStdString(), m_ptree);
    }
    catch (boost::property_tree::ptree_error &)
    {
        m_ok = false;
    }
    return m_ok;
}

bool Config::isOk() const
{
    return m_ok;
}

QString Config::getConfigDirPath()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir    dir(configPath);
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
            dir.cd(QStringLiteral("resource"));
            configFile = configDir.absolutePath();
            configFile.append("/.ainesmilerc");
            Q_ASSERT(QFile::exists(configFile));
        }

        QFile::copy(configFile, configPath);
    }

    qDebug() << "config path: " << configPath;
    return configPath;
}

QString Config::getThemePath()
{
    std::string currentTheme = m_ptree.get<std::string>("theme", "Default");
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
        configDir.cd(QStringLiteral("Resources"));
#endif

        QString themeDirPath;
        if (!configDir.cd(QStringLiteral("themes")) || configDir.entryList(QDir::Files).isEmpty())
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
            dir.cd(QStringLiteral("resource/themes"));
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
    if (!m_languageMapPath.isEmpty())
    {
        return m_languageMapPath;
    }

    QString dstLangMapFile = getConfigDirPath();
    dstLangMapFile.append("/langmap.xml");
    QFile file(dstLangMapFile);
    if (!file.exists())
    {
        // copy from installed directory
        QString appDirPath = QApplication::applicationDirPath();
        QDir    configDir(appDirPath);
#if defined(Q_OS_MAC)
        configDir.cdUp();
        configDir.cd(QStringLiteral("Resources"));
#endif
        QString srcLangMapFile = configDir.absoluteFilePath(QStringLiteral("langmap.xml"));
        if (!QFile::exists(srcLangMapFile))
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
            dir.cd(QStringLiteral("resource"));
            Q_ASSERT(dir.exists());
            srcLangMapFile = dir.absoluteFilePath(QStringLiteral("langmap.xml"));
            Q_ASSERT(QFile::exists(srcLangMapFile));
        }
        qDebug() << "copy langmap from " << srcLangMapFile << " to " << dstLangMapFile;
        QFile::copy(srcLangMapFile, dstLangMapFile);
    }
    qDebug() << "langmap: " << dstLangMapFile;
    m_languageMapPath = dstLangMapFile;
    return dstLangMapFile;
}

QString Config::getLanguageDirPath()
{
    QString dstLangDirPath = getConfigDirPath() + "/language";
    QDir    dstLangDir(dstLangDirPath);
    if (!dstLangDir.exists() || dstLangDir.entryList(QDir::Files).isEmpty())
    {
        // copy from installed directory
        QString appDirPath = QApplication::applicationDirPath();
        QDir    configDir(appDirPath);
#if defined(Q_OS_MAC)
        configDir.cdUp();
        configDir.cd(QStringLiteral("Resources"));
#endif
        QString srcLangDirPath = configDir.absoluteFilePath(QStringLiteral("language"));
        QDir    langDirOrig(srcLangDirPath);
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
            dir.cd(QStringLiteral("resource/language"));
            srcLangDirPath = dir.absolutePath();
        }
        // copy all files from configFile to langDirPath
        dstLangDir.mkpath(dstLangDirPath);
        QStringList files = QDir(srcLangDirPath).entryList(QDir::Files);
        std::for_each(files.begin(), files.end(), [&srcLangDirPath, &dstLangDirPath](const QString &fileName) {
            QFile::copy(srcLangDirPath + "/" + fileName, dstLangDirPath + "/" + fileName);
        });
    }
    qDebug() << "langDir: " << dstLangDirPath;
    return dstLangDirPath;
}

QString Config::matchPatternLanguage(const QString &filename)
{
    QString      defaultLanguage = QStringLiteral("normal");
    QString      langMapPath     = getLanguageMapPath();
    QDomDocument doc;
    QFile        file(langMapPath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return defaultLanguage;
    }
    BOOST_SCOPE_EXIT(&file)
    {
        file.close();
    }
    BOOST_SCOPE_EXIT_END

    QString errorMsg;
    int     errorLine   = 0;
    int     errorColumn = 0;
    if (!doc.setContent(&file, &errorMsg, &errorLine, &errorColumn))
    {
        return defaultLanguage;
    }

    QDomElement docElem = doc.documentElement();

    QDomElement langElem = docElem.firstChildElement(QStringLiteral("language"));
    while (!langElem.isNull())
    {
        QString pattern = langElem.attribute(QStringLiteral("pattern"));
        QString suffix  = langElem.attribute(QStringLiteral("suffix"));
        if (matchSuffix(filename, suffix) || matchPattern(filename, pattern))
        {
            QString name = langElem.attribute(QStringLiteral("name"));
            return name;
        }
        langElem = langElem.nextSiblingElement(QStringLiteral("language"));
    }
    return defaultLanguage;
}

bool Config::matchPattern(const QString &filename, const QString &pattern)
{
    if (pattern.isEmpty())
    {
        return false;
    }
    QRegularExpression regex(pattern);
#if defined(Q_OS_WIN)
    regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
#endif
    QFileInfo fi(filename);
    auto      match = regex.match(fi.fileName());
    return match.hasMatch();
}

bool Config::matchSuffix(const QString &filename, const QString &suffix)
{
    if (suffix.isEmpty())
    {
        return false;
    }
    QStringList suffixes = suffix.split(' ');
    QFileInfo   fi(filename);
    auto        iter = std::find_if(suffixes.begin(), suffixes.end(), [&fi](const QString &ext) {
#if defined(Q_OS_WIN)
        return (QString::compare(ext, fi.suffix(), Qt::CaseInsensitive) == 0);
#else
        return (QString::compare(ext, fi.suffix(), Qt::CaseSensitive) == 0);
#endif
    });
    return suffixes.end() != iter;
}

QStringList Config::supportedProgrammingLanguages()
{
    static QStringList languagesList;
    if (languagesList.isEmpty())
    {
        QString      langMapPath = getLanguageMapPath();
        QDomDocument doc;
        QFile        file(langMapPath);
        if (!file.open(QIODevice::ReadOnly))
        {
            return languagesList;
        }
        BOOST_SCOPE_EXIT(&file)
        {
            file.close();
        }
        BOOST_SCOPE_EXIT_END
        if (!doc.setContent(&file))
        {
            return languagesList;
        }

        QDomElement docElem = doc.documentElement();

        QDomElement langElem = docElem.firstChildElement(QStringLiteral("language"));
        while (!langElem.isNull())
        {
            QString name = langElem.attribute(QStringLiteral("name"));
            languagesList.append(name);
            langElem = langElem.nextSiblingElement(QStringLiteral("language"));
        }
    }

    return languagesList;
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
