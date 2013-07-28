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
    // get config path
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QString configPath = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#else
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif
    configPath.append("/.Ainesmile");
    QDir dir(configPath);
    if (!dir.exists())
    {
        dir.mkpath(configPath);
    }

    configPath.append("/.Ainesmilerc");
    boost::property_tree::write_json(configPath.toStdString(), pt_);
}

Config::Config()
{
    // get config path
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QString configPath = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
#else
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif
    configPath.append("/.Ainesmile");
    QDir dir(configPath);
    if (!dir.exists())
    {
        dir.mkpath(configPath);
    }

    configPath.append("/.Ainesmilerc");

    boost::property_tree::read_json(configPath.toStdString(), pt_);
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
