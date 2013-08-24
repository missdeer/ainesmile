#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QMouseEvent>
#include <QMenu>
#include <QFileInfo>
#include "recentfiles.h"
#include "codeeditpage.h"

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = 0);
    void setTheOtherSide(TabWidget* tabWidget);
    void setRecentFiles(RecentFiles* recentFiles);
    void setAboutToQuit(bool aboutToQuit);
    
    bool focus();
    void doCloseRequested(int index);
    bool fileExists(const QString& filePath);
    bool fileExists(const QFileInfo& fileInfo);
    int openFile(const QString& filePath);
    int newDocument(const QString& title);
    void saveCurrentFile();
    void saveAsCurrentFile();
    void saveAll();
    void closeAll();
    void closeAllButActive();
    void setShowWhiteSpaceAndTAB(bool enabled);
    void setShowEndOfLine(bool enabled);
    void setShowIndentGuide(bool enabled);
    void setShowWrapSymbol(bool enabled);
    void getFileList(QStringList& fileList);
protected:
    void mousePressEvent(QMouseEvent * event);
signals:
    void exchangeTab();
    void updateRecentFiles();
    void codeEditPageCreated(CodeEditPage* page);
public slots:
    
private:
    QMenu* contentMenu_;
    TabWidget* theOtherSide_;
    RecentFiles* rf_;
    bool aboutToQuit_;
};

#endif // TABWIDGET_H
