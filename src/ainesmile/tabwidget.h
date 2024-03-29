﻿#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QFileInfo>
#include <QMenu>
#include <QMouseEvent>
#include <QTabWidget>

#include "codeeditpage.h"
#include "findreplace.h"
#include "recentfiles.h"

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = nullptr);
    void setTheOtherSide(TabWidget *tabWidget);
    void setRecentFiles(RecentFiles *recentFiles);
    void setAboutToQuit(bool aboutToQuit);

    bool focus();
    void doCloseRequested(int index);
    bool fileExists(const QString &filePath);
    bool fileExists(const QFileInfo &fileInfo);
    int  openFile(const QString &filePath);
    void openFiles(const QStringList &files);
    int  newDocument(const QString &title);
    void saveCurrentFile();
    void saveAsCurrentFile();
    void saveAll();
    void closeAll();
    void closeAllButActive();
    void setShowWhiteSpaceAndTAB(bool enabled);
    void setShowEndOfLine(bool enabled);
    void setShowIndentGuide(bool enabled);
    void setShowWrapSymbol(bool enabled);
    void getFileList(QStringList &fileList);
    int  findTabIndex(QWidget *widget);

    void    getAllEditors(std::vector<CodeEditor *> &docs);
    QString getCurrentFilePath();

protected:
    void mousePressEvent(QMouseEvent *event) override;
signals:
    void exchangeTab();
    void updateRecentFiles();
    void codeEditPageCreated(CodeEditor *page);
public slots:
    void onOpenFilesRequest(const QStringList &file);

private:
    TabWidget   *m_theOtherSide {nullptr};
    RecentFiles *m_recentFiles;
    bool         m_aboutToQuit {false};
};

#endif // TABWIDGET_H
