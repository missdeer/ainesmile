#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>
#include "recentfiles.h"

namespace Ui {
class MainWindow;
}

class CodeEditPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void openFiles(const QStringList& files);
    void newDocument();

protected:
    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
private:
    void setActionShortcuts();
    void setRecentFiles();
    void updateUI(CodeEditPage* page);
    void connectSignals(CodeEditPage* page);

private slots:
    void currentPageChanged(int index);
    void closeRequested(int index);
    void currentDocumentChanged();
    void copyAvailableChanged();
    void pasteAvailableChanged();
    void undoAvailableChanged();
    void redoAvailableChanged();
    void recentFileTriggered(const QString & file);
    void recentProjectTriggered(const QString & project);

    void on_actionNewFile_triggered();

    void on_actionToggleFullScreenMode_triggered();

    void on_actionOpenFile_triggered();

    void on_actionExitApp_triggered();

    void on_actionSaveFile_triggered();

    void on_actionSaveAs_triggered();

    void on_actionHelpContent_triggered();

    void on_actionAboutApp_triggered();

    void on_actionDForDSoftwareHome_triggered();

    void on_actionAinesmileProductPage_triggered();

    void on_actionSaveAll_triggered();

    void on_actionCloseAll_triggered();

    void on_actionCloseAllButActiveDocument_triggered();

    void on_actionOpenAllRecentFiles_triggered();

    void on_actionEmptyRecentFilesList_triggered();

    void on_actionAlwaysOnTop_triggered();

    void on_actionCloseAllDocuments_triggered();

    void on_actionClose_triggered();

    void on_actionFindInFiles_triggered();

    void on_actionSearchResultsWindow_triggered();

    void on_actionNextSearchResult_triggered();

    void on_actionPreviousSearchResult_triggered();

    void on_actionRegistration_triggered();

    void on_actionSelectExtensionItem_triggered();

    void on_actionShowExtensionEditor_triggered();

    void on_actionEditCommands_triggered();

    void on_actionEditSnippets_triggered();

    void on_actionReloadExtensions_triggered();

    void on_actionShowWhiteSpaceAndTAB_triggered();

    void on_actionShowEndOfLine_triggered();

    void on_actionShowAllCharacters_triggered();

    void on_actionShowIndentGuide_triggered();

    void on_actionShowWrapSymbol_triggered();

    void on_actionPreferences_triggered();

    void on_actionExternalTools_triggered();

    void on_actionPluginManager_triggered();

    void on_actionWindowsList_triggered();

    void on_actionOpenProject_triggered();

    void on_actionCloseProject_triggered();

    void on_actionNewProject_triggered();

    void on_actionEmptyRecentProjectsList_triggered();

private:
    Ui::MainWindow *ui;
    QSignalMapper* recentFileSignalMapper_;
    QSignalMapper* recentProjectSignalMapper_;
    QList<QAction*> recentFileActions_;
    QList<QAction*> recentProjectActions_;
    RecentFiles rf_;
    bool aboutToQuit_;
};

#endif // MAINWINDOW_H
