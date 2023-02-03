#ifndef CONFIG_H
#define CONFIG_H

#include <boost/property_tree/ptree.hpp>

#include <QString>

class Config
{
private:
    static Config *instance_;

    boost::property_tree::ptree pt_;
    bool                        ok_ {false};

    Config();
    bool matchPattern(const QString &filename, const QString &pattern);
    bool matchSuffix(const QString &filename, const QString &suffix);

public:
    static Config               *instance();
    boost::property_tree::ptree &pt();
    bool                         sync();
    bool                         init();
    [[nodiscard]] bool           isOk() const;
    QString                      getConfigDirPath();
    QString                      getConfigPath();
    QString                      getThemePath();
    QString                      getLanguageMapPath();
    QString                      getLanguageDirPath();
    QString                      matchPatternLanguage(const QString &filename);
};

#endif // CONFIG_H
