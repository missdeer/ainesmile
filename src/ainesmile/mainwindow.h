#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>
#include <QList>
#include "tabwidget.h"
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

    TabWidget *getFocusTabWidget();
    void hideFeatures();
    void setActionShortcuts();
    void setRecentFiles();
    void setMenuItemChecked();
    void updateUI(CodeEditPage* page);
    void connectSignals(CodeEditPage* page);

private slots:
    void onCodeEditPageCreated(CodeEditPage* page);
    void onUpdateRecentFilesMenuItems();
    void onCodeEditPageFocusIn();
    void onExchangeTab();
    void onIPCMessageReceived(const QString &message, QObject *socket);
    void onCurrentPageChanged(int index);
    void onCloseRequested(int index);
    void onCurrentDocumentChanged();
    void onCopyAvailableChanged();
    void onPasteAvailableChanged();
    void onUndoAvailableChanged();
    void onRedoAvailableChanged();
    void onRecentFileTriggered(const QString & file);
    void onActivateTabClicked(int index);
    void onCloseTabClicked(const QList<int> & fileList);
    void onSaveTabClicked(const QList<int> & fileList);

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

    void on_actionShowWhiteSpaceAndTAB_triggered();

    void on_actionShowEndOfLine_triggered();

    void on_actionShowIndentGuide_triggered();

    void on_actionShowWrapSymbol_triggered();

    void on_actionPreferences_triggered();

    void on_actionWindowsList_triggered();

    void on_actionFind_triggered();

    void on_cbScope_currentIndexChanged(int index);

    void on_btnSelectDirectory_clicked();

    void on_btnFind_clicked();

    void on_btnFindInFiles_clicked();

    void on_btnReplace_clicked();

    void on_btnReplaceAll_clicked();

    void on_btnReplaceInFiles_clicked();

private:
    Ui::MainWindow *ui;
    QDockWidget *dockFindReplace_;
    QDockWidget *dockFindResult_;
    QSignalMapper* recentFileSignalMapper_;
    QList<QAction*> recentFileActions_;
    RecentFiles rf_;
    bool aboutToQuit_;
    bool exchanging_;
};

extern MainWindow* g_mainWindow;

#endif // MAINWINDOW_H
