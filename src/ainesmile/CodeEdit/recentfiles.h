#ifndef RECENTFILES_H
#define RECENTFILES_H

#include <boost/property_tree/ptree.hpp>

#include <QObject>
#include <QStringList>

class RecentFiles : public QObject
{
    Q_OBJECT
signals:
    void addRecentFile(const QString &file);

private:
    QStringList        m_files;
    void               init();
    [[nodiscard]] bool exists(const QStringList &container, const QString &file);

public:
    RecentFiles();
    ~RecentFiles() override;
    void                       replaceFile(const QString &originalFile, const QString &newFile);
    bool                       addFile(const QString &file);
    void                       clearFiles();
    [[nodiscard]] QStringList &recentFiles();
    void                       sync();
};

#endif // RECENTFILES_H
