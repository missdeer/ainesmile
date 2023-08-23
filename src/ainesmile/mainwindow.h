#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QSignalMapper>

#include "recentfiles.h"
#include "tabwidget.h"

namespace Ui
{
    class MainWindow;
}

class CodeEditor;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void openFiles(const QStringList &files);
    void newDocument();

protected:
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    [[nodiscard]] TabWidget *getFocusTabWidget();
    void                     hideFeatures();
    void                     setActionShortcuts();
    void                     setRecentFiles();
    void                     setMenuItemChecked();
    void                     updateUI(CodeEditor *page);
    void                     connectSignals(CodeEditor *page);
    void                     openFile(const QString &file);

private slots:
    void onCodeEditPageCreated(CodeEditor *page);
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
    void onRecentFileTriggered();
    void onActivateTabClicked(int index);
    void onCloseTabClicked(const QList<int> &fileList);
    void onSaveTabClicked(const QList<int> &fileList);

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
    Ui::MainWindow  *ui;
    CodeEditor      *m_lastConnectedCodeEditPage {nullptr};
    QDockWidget     *m_dockFindReplace;
    QDockWidget     *m_dockFindResult;
    QList<QAction *> m_recentFileActions;
    RecentFiles      m_recentFiles;
    bool             m_aboutToQuit;
    bool             m_exchanging;
};

inline MainWindow *g_mainWindow = nullptr;

#endif // MAINWINDOW_H
