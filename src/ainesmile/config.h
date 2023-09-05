#ifndef CONFIG_H
#define CONFIG_H

#include <boost/property_tree/ptree.hpp>

#include <QString>

class Config
{
private:
    static Config *instance_;

    QString                     m_languageMapPath;
    boost::property_tree::ptree m_ptree;
    bool                        m_ok {false};

    Config();
    bool matchPattern(const QString &filename, const QString &pattern);
    bool matchSuffix(const QString &filename, const QString &suffix);

public:
    static Config               *instance();
    boost::property_tree::ptree &pt();
    bool                         sync();
    bool                         init();
    [[nodiscard]] bool           isOk() const;
    [[nodiscard]] QString        getConfigDirPath();
    [[nodiscard]] QString        getConfigPath();
    [[nodiscard]] QString        getThemePath();
    [[nodiscard]] QString        getLanguageMapPath();
    [[nodiscard]] QString        getLanguageDirPath();
    [[nodiscard]] QString        matchPatternLanguage(const QString &filename);
    [[nodiscard]] QStringList    supportedProgrammingLanguages();
};

#endif // CONFIG_H
