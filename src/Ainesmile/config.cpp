#include <QApplication>
#include <QDir>
#include <QDesktopServices>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QStandardPaths>
#endif
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

QString Config::getConfigPath()
{
#if defined(Q_OS_MAC)
#elif defined(Q_OS_WIN)
    // get config path
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QString configPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#else
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#endif
    configPath.append("/Ainesmile");
#else
    // get config path
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QString configPath = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#else
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif
    configPath.append("/.config/Ainesmile");
#endif
    QDir dir(configPath);
    if (!dir.exists())
    {
        dir.mkpath(configPath);
    }

    configPath.append("/.Ainesmilerc");
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
        file.copy(configFile, configPath);
    }

    return configPath;
}

QString Config::getThemePath()
{
    std::string currentTheme = pt_.get<std::string>("theme");
}

Config::Config()
{
    boost::property_tree::read_json(getConfigPath().toStdString(), pt_);
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
