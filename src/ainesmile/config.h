#ifndef CONFIG_H
#define CONFIG_H

#include <boost/property_tree/ptree.hpp>

class Config
{
private:
    static Config* instance_;

    boost::property_tree::ptree pt_;
    Config();
    bool matchPattern(const QString& filename, const QString& pattern);
    bool matchSuffix(const QString& filename, const QString& suffix);
public:
    static Config* instance();
    boost::property_tree::ptree& pt();
    void sync();
    QString getConfigDirPath();
    QString getConfigPath();
    QString getThemePath();
    QString getLanguageMapPath();
    QString getLanguageDirPath();
    QString matchPatternLanguage(const QString& filename);
};

#endif // CONFIG_H
