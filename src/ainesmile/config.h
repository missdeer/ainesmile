#ifndef CONFIG_H
#define CONFIG_H

#include <boost/property_tree/ptree.hpp>

class Config
{
private:
    static Config* instance_;

    boost::property_tree::ptree pt_;
    Config();
public:
    static Config* instance();
    boost::property_tree::ptree& pt();
    void sync();
    QString getConfigDirPath();
    QString getConfigPath();
    QString getThemePath();
};

#endif // CONFIG_H
