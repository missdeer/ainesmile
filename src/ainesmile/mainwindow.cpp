#if defined(WIN32)
#include <Windows.h>
#endif
#include <QFileDialog>
#include <QMdiSubWindow>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMimeData>
#include <QStringList>
#include "config.h"
#include "stupidcheck.h"
#include "codeeditpage.h"
#include "aboutdialog.h"
#include "registerdialog.h"
#include "preferencedialog.h"
#include "windowlistdialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    findReplaceDialog_(NULL),
    recentFileSignalMapper_(new QSignalMapper(this)),
    recentProjectSignalMapper_(new QSignalMapper(this)),
    aboutToQuit_(false)
{
    ui->setupUi(this);
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onCurrentPageChanged(int)));
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(onCloseRequested(int)));
    connect(ui->tabWidget, SIGNAL(exchangeTab()), this, SLOT(onExchangeTab()));
    connect(ui->tabWidgetSlave, SIGNAL(currentChanged(int)), this, SLOT(onCurrentPageChanged(int)));
    connect(ui->tabWidgetSlave, SIGNAL(tabCloseRequested(int)), this, SLOT(onCloseRequested(int)));
    connect(ui->tabWidgetSlave, SIGNAL(exchangeTab()), this, SLOT(onExchangeTab()));

    setActionShortcuts();
    setRecentFiles();
    setMenuItemChecked();
    setAcceptDrops(true);
    hideFeatures();
#if defined(Q_OS_MAC)
    ui->actionRegistration->setMenuRole(QAction::ApplicationSpecificRole);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    ui->actionAboutApp->setMenuRole(QAction::AboutRole);
    ui->actionPreferences->setMenuRole(QAction::PreferencesRole);
    ui->actionExitApp->setMenuRole(QAction::QuitRole);
#endif
#endif
    ui->tabWidgetSlave->hide();
    ui->tabWidget->setFocus();
}

MainWindow::~MainWindow()
{
    Config::instance()->sync();
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    aboutToQuit_ = true;
    // query all pages can close
    int count = ui->tabWidget->count();
    for (int index = count -1; index >=0; index--)
    {
        doCloseRequested(ui->tabWidget, index);
    }

    count = ui->tabWidgetSlave->count();
    for (int index = count -1; index >=0; index--)
    {
        doCloseRequested(ui->tabWidgetSlave, index);
    }

    if (ui->tabWidget->count() + ui->tabWidgetSlave->count() != 0)
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
    aboutToQuit_ = false;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/plain"))
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    event->mimeData()->formats();
    event->mimeData()->text();

    event->acceptProposedAction();
}

TabWidget *MainWindow::getFocusTabWidget()
{
    for (int i = 0; i < ui->tabWidgetSlave->count(); i++)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidget->currentWidget());
        Q_ASSERT(page);
        if (page->focus())
            return ui->tabWidgetSlave;
    }
    return ui->tabWidget;
}

void MainWindow::setActionShortcuts()
{
    ui->actionHelpContent->setShortcut(QKeySequence::HelpContents);
    ui->actionOpenFile->setShortcut(QKeySequence::Open);
    ui->actionClose->setShortcut(QKeySequence::Close);
    ui->actionSaveFile->setShortcut(QKeySequence::Save);
#if !defined(Q_WS_WIN)
    ui->actionExitApp->setShortcut(QKeySequence::Quit);
    ui->actionSaveAs->setShortcut(QKeySequence::SaveAs);
#endif
    ui->actionNewFile->setShortcut(QKeySequence::New);
    ui->actionDelete->setShortcut(QKeySequence::Delete);
    ui->actionCut->setShortcut(QKeySequence::Cut);
    ui->actionCopy->setShortcut(QKeySequence::Copy);
    ui->actionPaste->setShortcut(QKeySequence::Paste);
#if !defined(Q_WS_WIN)
    ui->actionPreferences->setShortcut(QKeySequence::Preferences);
#endif
    ui->actionUndo->setShortcut(QKeySequence::Undo);
    ui->actionRedo->setShortcut(QKeySequence::Redo);
    ui->actionZoomIn->setShortcut(QKeySequence::ZoomIn);
    ui->actionZoomOut->setShortcut(QKeySequence::ZoomOut);
    ui->actionPrint->setShortcut(QKeySequence::Print);
    ui->actionFind->setShortcut(QKeySequence::Find);
//    ui->actionFindNext->setShortcut(QKeySequence::FindNext);
//    ui->actionFindPrevious->setShortcut(QKeySequence::FindPrevious);
    ui->actionReplace->setShortcut(QKeySequence::Replace);
    ui->actionSelectAll->setShortcut(QKeySequence::SelectAll);
}

void MainWindow::setRecentFiles()
{
    recentFileActions_ << ui->actionRecentFile1
                          << ui->actionRecentFile2
                          << ui->actionRecentFile3
                          << ui->actionRecentFile4
                          << ui->actionRecentFile5
                          << ui->actionRecentFile6
                          << ui->actionRecentFile7
                          << ui->actionRecentFile8
                          << ui->actionRecentFile9
                          << ui->actionRecentFile10;
    recentProjectActions_ << ui->actionRecentProject1
                             << ui->actionRecentProject2
                             << ui->actionRecentProject3
                             << ui->actionRecentProject4
                             << ui->actionRecentProject5
                             << ui->actionRecentProject6
                             << ui->actionRecentProject7
                             << ui->actionRecentProject8
                             << ui->actionRecentProject9
                             << ui->actionRecentProject10;

    Q_FOREACH(QAction* action, recentFileActions_)
    {
        connect(action, SIGNAL(triggered()), recentFileSignalMapper_, SLOT(map()));
    }

    Q_FOREACH(QAction* action, recentProjectActions_)
    {
        connect(action, SIGNAL(triggered()), recentProjectSignalMapper_, SLOT(map()));
    }
    updateRecentFilesMenuItems();
}

void MainWindow::setMenuItemChecked()
{
    boost::property_tree::ptree& pt = Config::instance()->pt();
    bool enabled = pt.get<bool>("show.end_of_line", true);
    ui->actionShowEndOfLine->setChecked(enabled);
    enabled = pt.get<bool>("show.indent_guide", true);
    ui->actionShowIndentGuide->setChecked(enabled);
    enabled = pt.get<bool>("show.white_space_and_tab", true);
    ui->actionShowWhiteSpaceAndTAB->setChecked(enabled);
    enabled = pt.get<bool>("show.wrap_symbol", true);
    ui->actionShowWrapSymbol->setChecked(enabled);
}

void MainWindow::updateRecentFilesMenuItems()
{
    Q_ASSERT(recentFileSignalMapper_);
    Q_ASSERT(recentProjectSignalMapper_);

    QStringList recentFiles = rf_.recentFiles();
    int index=0;
    for (QStringList::ConstIterator it = recentFiles.constBegin();
         recentFiles.constEnd() != it && index < 10;
         ++it, ++index)
    {
        QAction* action = recentFileActions_.at(index);
        QFile file(*it);
        action->setText(file.fileName());
        action->setVisible(true);
        recentFileSignalMapper_->removeMappings(action);
        recentFileSignalMapper_->setMapping(action, file.fileName());
    }
    connect(recentFileSignalMapper_, SIGNAL(mapped(const QString&)),
            this, SLOT(onRecentFileTriggered(const QString&)));
    for (int i = index; i < 10; i++)
    {
        QAction* action = recentFileActions_.at(i);
        action->setVisible(false);
    }

    QStringList recentProjects = rf_.recentProjects();
    index=0;
    for (QStringList::ConstIterator it = recentProjects.constBegin();
         recentProjects.constEnd() != it && index < 10;
         ++it, ++index)
    {
        QAction* action = recentProjectActions_.at(index);
        QFile file(*it);
        action->setText(file.fileName());
        action->setVisible(true);
        recentProjectSignalMapper_->removeMappings(action);
        recentProjectSignalMapper_->setMapping(action, file.fileName());
    }
    connect(recentProjectSignalMapper_, SIGNAL(mapped(const QString&)),
            this, SLOT(onRecentProjectTriggered(const QString&)));
    for (int i = index; i < 10; i++)
    {
        QAction* action = recentProjectActions_.at(i);
        action->setVisible(false);
    }
}

void MainWindow::updateUI(CodeEditPage* page)
{
    ui->actionCut->setEnabled(page->canCut());
    ui->actionCopy->setEnabled(page->canCopy());
    ui->actionPaste->setEnabled(page->canPaste());
    ui->actionUndo->setEnabled(page->canUndo());
    ui->actionRedo->setEnabled(page->canRedo());
}

void MainWindow::connectSignals(CodeEditPage *page)
{
    disconnect(this, SLOT(onCurrentDocumentChanged()));
    connect(page, SIGNAL(modifiedNotification()), this, SLOT(onCurrentDocumentChanged()));
    disconnect(this, SLOT(onCopyAvailableChanged()));
    connect(page, SIGNAL(copyAvailableChanged()), this, SLOT(onCopyAvailableChanged()));
    disconnect(this, SLOT(onPasteAvailableChanged()));
    connect(page, SIGNAL(pasteAvailableChanged()), this, SLOT(onPasteAvailableChanged()));
    disconnect(this, SLOT(onUndoAvailableChanged()));
    connect(page, SIGNAL(undoAvailableChanged()), this, SLOT(onUndoAvailableChanged()));
    disconnect(this, SLOT(onRedoAvailableChanged()));
    connect(page, SIGNAL(redoAvailableChanged()), this, SLOT(onRedoAvailableChanged()));
    disconnect(ui->actionCut, SIGNAL(triggered()), 0, 0);
    connect(ui->actionCut, SIGNAL(triggered()), page, SLOT(cut()));
    disconnect(ui->actionCopy, SIGNAL(triggered()), 0, 0);
    connect(ui->actionCopy, SIGNAL(triggered()), page, SLOT(copy()));
    disconnect(ui->actionPaste, SIGNAL(triggered()), 0, 0);
    connect(ui->actionPaste, SIGNAL(triggered()), page, SLOT(paste()));
    disconnect(ui->actionUndo, SIGNAL(triggered()), 0, 0);
    connect(ui->actionUndo, SIGNAL(triggered()), page, SLOT(undo()));
    disconnect(ui->actionRedo, SIGNAL(triggered()), 0, 0);
    connect(ui->actionRedo, SIGNAL(triggered()), page, SLOT(redo()));
    disconnect(ui->actionPrint, SIGNAL(triggered()), 0, 0);
    connect(ui->actionPrint, SIGNAL(triggered()), page, SLOT(print()));
    disconnect(ui->actionPrintNow, SIGNAL(triggered()), 0, 0);
    connect(ui->actionPrintNow, SIGNAL(triggered()), page, SLOT(printNow()));
    disconnect(ui->actionDelete, SIGNAL(triggered()), 0, 0);
    connect(ui->actionDelete, SIGNAL(triggered()), page, SLOT(deleteCurrent()));
    disconnect(ui->actionSelectAll, SIGNAL(triggered()), 0, 0);
    connect(ui->actionSelectAll, SIGNAL(triggered()), page, SLOT(selectAll()));
    disconnect(ui->actionColumnMode, SIGNAL(triggered()), 0, 0);
    connect(ui->actionColumnMode, SIGNAL(triggered()), page, SLOT(columnMode()));
    disconnect(ui->actionColumnEditor, SIGNAL(triggered()), 0, 0);
    connect(ui->actionColumnEditor, SIGNAL(triggered()), page, SLOT(columnEditor()));
    disconnect(ui->actionCharacterPanel, SIGNAL(triggered()), 0, 0);
    connect(ui->actionCharacterPanel, SIGNAL(triggered()), page, SLOT(characterPanel()));
    disconnect(ui->actionClipboardHistory, SIGNAL(triggered()), 0, 0);
    connect(ui->actionClipboardHistory, SIGNAL(triggered()), page, SLOT(clipboardHistory()));
    disconnect(ui->actionSetReadOnly, SIGNAL(triggered()), 0, 0);
    connect(ui->actionSetReadOnly, SIGNAL(triggered()), page, SLOT(setReadOnly()));
    disconnect(ui->actionClearReadOnlyFlag, SIGNAL(triggered()), 0, 0);
    connect(ui->actionClearReadOnlyFlag, SIGNAL(triggered()), page, SLOT(clearReadOnlyFlag()));
    disconnect(ui->actionCurrentFullFilePathToClipboard, SIGNAL(triggered()), 0, 0);
    connect(ui->actionCurrentFullFilePathToClipboard, SIGNAL(triggered()), page, SLOT(currentFullFilePathToClipboard()));
    disconnect(ui->actionCurrentFileNameToClipboard, SIGNAL(triggered()), 0, 0);
    connect(ui->actionCurrentFileNameToClipboard, SIGNAL(triggered()), page, SLOT(currentFileNameToClipboard()));
    disconnect(ui->actionCurrentDirectoryPathToClipboard, SIGNAL(triggered()), 0, 0);
    connect(ui->actionCurrentDirectoryPathToClipboard, SIGNAL(triggered()), page, SLOT(currentDirectoryPathToClipboard()));
    disconnect(ui->actionIncreaseLineIndent, SIGNAL(triggered()), 0, 0);
    connect(ui->actionIncreaseLineIndent, SIGNAL(triggered()), page, SLOT(increaseLineIndent()));
    disconnect(ui->actionDecreaseLineIndent, SIGNAL(triggered()), 0, 0);
    connect(ui->actionDecreaseLineIndent, SIGNAL(triggered()), page, SLOT(decreaseLineIndent()));
    disconnect(ui->actionUppercase, SIGNAL(triggered()), 0, 0);
    connect(ui->actionUppercase, SIGNAL(triggered()), page, SLOT(upperCase()));
    disconnect(ui->actionLowercase, SIGNAL(triggered()), 0, 0);
    connect(ui->actionLowercase, SIGNAL(triggered()), page, SLOT(lowerCase()));
    disconnect(ui->actionDuplicateCurrentLine, SIGNAL(triggered()), 0, 0);
    connect(ui->actionDuplicateCurrentLine, SIGNAL(triggered()), page, SLOT(duplicateCurrentLine()));
    disconnect(ui->actionSplitLines, SIGNAL(triggered()), 0, 0);
    connect(ui->actionSplitLines, SIGNAL(triggered()), page, SLOT(splitLines()));
    disconnect(ui->actionJoinLines, SIGNAL(triggered()), 0, 0);
    connect(ui->actionJoinLines, SIGNAL(triggered()), page, SLOT(joinLines()));
    disconnect(ui->actionMoveUpCurrentLine, SIGNAL(triggered()), 0, 0);
    connect(ui->actionMoveUpCurrentLine, SIGNAL(triggered()), page, SLOT(moveUpCurrentLine()));
    disconnect(ui->actionMoveDownCurrentLine, SIGNAL(triggered()), 0, 0);
    connect(ui->actionMoveDownCurrentLine, SIGNAL(triggered()), page, SLOT(moveDownCurrentLine()));
    disconnect(ui->actionToggleBlockComment, SIGNAL(triggered()), 0, 0);
    connect(ui->actionToggleBlockComment, SIGNAL(triggered()), page, SLOT(toggleBlockComment()));
    disconnect(ui->actionBlockComment, SIGNAL(triggered()), 0, 0);
    connect(ui->actionBlockComment, SIGNAL(triggered()), page, SLOT(blockComment()));
    disconnect(ui->actionBlockUncomment, SIGNAL(triggered()), 0, 0);
    connect(ui->actionBlockUncomment, SIGNAL(triggered()), page, SLOT(blockUncomment()));
    disconnect(ui->actionStreamComment, SIGNAL(triggered()), 0, 0);
    connect(ui->actionStreamComment, SIGNAL(triggered()), page, SLOT(streamComment()));
    disconnect(ui->actionFunctionCompletion, SIGNAL(triggered()), 0, 0);
    connect(ui->actionFunctionCompletion, SIGNAL(triggered()), page, SLOT(functionCompletion()));
    disconnect(ui->actionWordCompletion, SIGNAL(triggered()), 0, 0);
    connect(ui->actionWordCompletion, SIGNAL(triggered()), page, SLOT(wordCompletion()));
    disconnect(ui->actionFunctionParametersHint, SIGNAL(triggered()), 0, 0);
    connect(ui->actionFunctionParametersHint, SIGNAL(triggered()), page, SLOT(functionParametersHint()));
    disconnect(ui->actionWindowsFormat, SIGNAL(triggered()), 0, 0);
    connect(ui->actionWindowsFormat, SIGNAL(triggered()), page, SLOT(eolWindowsFormat()));
    disconnect(ui->actionUNIXFormat, SIGNAL(triggered()), 0, 0);
    connect(ui->actionUNIXFormat, SIGNAL(triggered()), page, SLOT(eolUNIXFormat()));
    disconnect(ui->actionMacFormat, SIGNAL(triggered()), 0, 0);
    connect(ui->actionMacFormat, SIGNAL(triggered()), page, SLOT(eolMacFormat()));
    disconnect(ui->actionTrimTrailingSpace, SIGNAL(triggered()), 0, 0);
    connect(ui->actionTrimTrailingSpace, SIGNAL(triggered()), page, SLOT(trimTrailingSpace()));
    disconnect(ui->actionTrimLeadingSpace, SIGNAL(triggered()), 0, 0);
    connect(ui->actionTrimLeadingSpace, SIGNAL(triggered()), page, SLOT(trimLeadingSpace()));
    disconnect(ui->actionTrimLeadingAndTrailingSpace, SIGNAL(triggered()), 0, 0);
    connect(ui->actionTrimLeadingAndTrailingSpace, SIGNAL(triggered()), page, SLOT(trimLeadingAndTrailingSpace()));
    disconnect(ui->actionEOLToSpace, SIGNAL(triggered()), 0, 0);
    connect(ui->actionEOLToSpace, SIGNAL(triggered()), page, SLOT(eolToSpace()));
    disconnect(ui->actionRemoveUnnecessaryBlankAndEOL, SIGNAL(triggered()), 0, 0);
    connect(ui->actionRemoveUnnecessaryBlankAndEOL, SIGNAL(triggered()), page, SLOT(removeUnnecessaryBlankAndEOL()));
    disconnect(ui->actionTABToSpace, SIGNAL(triggered()), 0, 0);
    connect(ui->actionTABToSpace, SIGNAL(triggered()), page, SLOT(tabToSpace()));
    disconnect(ui->actionSpaceToTAB, SIGNAL(triggered()), 0, 0);
    connect(ui->actionSpaceToTAB, SIGNAL(triggered()), page, SLOT(spaceToTab()));
    disconnect(ui->actionPasteHTMLContent, SIGNAL(triggered()), 0, 0);
    connect(ui->actionPasteHTMLContent, SIGNAL(triggered()), page, SLOT(pasteHTMLContent()));
    disconnect(ui->actionPasteRTFContent, SIGNAL(triggered()), 0, 0);
    connect(ui->actionPasteRTFContent, SIGNAL(triggered()), page, SLOT(pasteRTFContent()));
    disconnect(ui->actionCopyBinaryContent, SIGNAL(triggered()), 0, 0);
    connect(ui->actionCopyBinaryContent, SIGNAL(triggered()), page, SLOT(copyBinaryContent()));
    disconnect(ui->actionCutBinaryContent, SIGNAL(triggered()), 0, 0);
    connect(ui->actionCutBinaryContent, SIGNAL(triggered()), page, SLOT(cutBinaryContent()));
    disconnect(ui->actionPasteBinaryContent, SIGNAL(triggered()), 0, 0);
    connect(ui->actionPasteBinaryContent, SIGNAL(triggered()), page, SLOT(pasteBinaryContent()));
    disconnect(ui->actionFind, SIGNAL(triggered()), 0, 0);
    connect(ui->actionFind, SIGNAL(triggered()), page, SLOT(find()));
    disconnect(ui->actionFindNext, SIGNAL(triggered()), 0, 0);
    connect(ui->actionFindNext, SIGNAL(triggered()), page, SLOT(findNext()));
    disconnect(ui->actionFindPrevious, SIGNAL(triggered()), 0, 0);
    connect(ui->actionFindPrevious, SIGNAL(triggered()), page, SLOT(findPrevious()));
    disconnect(ui->actionSelectAndFindNext, SIGNAL(triggered()), 0, 0);
    connect(ui->actionSelectAndFindNext, SIGNAL(triggered()), page, SLOT(selectAndFindNext()));
    disconnect(ui->actionSelectAndFindPrevious, SIGNAL(triggered()), 0, 0);
    connect(ui->actionSelectAndFindPrevious, SIGNAL(triggered()), page, SLOT(selectAndFindPrevious()));
    disconnect(ui->actionFindVolatieNext, SIGNAL(triggered()), 0, 0);
    connect(ui->actionFindVolatieNext, SIGNAL(triggered()), page, SLOT(findVolatieNext()));
    disconnect(ui->actionFindVolatiePrevious, SIGNAL(triggered()), 0, 0);
    connect(ui->actionFindVolatiePrevious, SIGNAL(triggered()), page, SLOT(findVolatiePrevious()));
    disconnect(ui->actionReplace, SIGNAL(triggered()), 0, 0);
    connect(ui->actionReplace, SIGNAL(triggered()), page, SLOT(replace()));
    disconnect(ui->actionIncrementalSearch, SIGNAL(triggered()), 0, 0);
    connect(ui->actionIncrementalSearch, SIGNAL(triggered()), page, SLOT(incrementalSearch()));
    disconnect(ui->actionGoTo, SIGNAL(triggered()), 0, 0);
    connect(ui->actionGoTo, SIGNAL(triggered()), page, SLOT(gotoLine()));
    disconnect(ui->actionGoToMatchingBrace, SIGNAL(triggered()), 0, 0);
    connect(ui->actionGoToMatchingBrace, SIGNAL(triggered()), page, SLOT(gotoMatchingBrace()));
    disconnect(ui->actionFindCharactersInRange, SIGNAL(triggered()), 0, 0);
    connect(ui->actionFindCharactersInRange, SIGNAL(triggered()), page, SLOT(findCharactersInRange()));
    disconnect(ui->actionToggleBookmark, SIGNAL(triggered()), 0, 0);
    connect(ui->actionToggleBookmark, SIGNAL(triggered()), page, SLOT(toggleBookmark()));
    disconnect(ui->actionNextBookmark, SIGNAL(triggered()), 0, 0);
    connect(ui->actionNextBookmark, SIGNAL(triggered()), page, SLOT(nextBookmark()));
    disconnect(ui->actionPreviousBookmark, SIGNAL(triggered()), 0, 0);
    connect(ui->actionPreviousBookmark, SIGNAL(triggered()), page, SLOT(previousBookmark()));
    disconnect(ui->actionClearAllBookmarks, SIGNAL(triggered()), 0, 0);
    connect(ui->actionClearAllBookmarks, SIGNAL(triggered()), page, SLOT(clearAllBookmarks()));
    disconnect(ui->actionCutBookmarkedLines, SIGNAL(triggered()), 0, 0);
    connect(ui->actionCutBookmarkedLines, SIGNAL(triggered()), page, SLOT(cutBookmarkLines()));
    disconnect(ui->actionCopyBookmarkedLines, SIGNAL(triggered()), 0, 0);
    connect(ui->actionCopyBookmarkedLines, SIGNAL(triggered()), page, SLOT(copyBookmarkLines()));
    disconnect(ui->actionPasteToReplaceBookmarkedLines, SIGNAL(triggered()), 0, 0);
    connect(ui->actionPasteToReplaceBookmarkedLines, SIGNAL(triggered()), page, SLOT(pasteToReplaceBookmarkedLines()));
    disconnect(ui->actionRemoveBookmarkedLines, SIGNAL(triggered()), 0, 0);
    connect(ui->actionRemoveBookmarkedLines, SIGNAL(triggered()), page, SLOT(removeBookmarkedLines()));
    disconnect(ui->actionRemoveUnmarkedLines, SIGNAL(triggered()), 0, 0);
    connect(ui->actionRemoveUnmarkedLines, SIGNAL(triggered()), page, SLOT(removeUnbookmarkedLines()));
    disconnect(ui->actionInverseBookmark, SIGNAL(triggered()), 0, 0);
    connect(ui->actionInverseBookmark, SIGNAL(triggered()), page, SLOT(inverseBookmark()));
    disconnect(ui->actionStartRecording, SIGNAL(triggered()), 0, 0);
    connect(ui->actionStartRecording, SIGNAL(triggered()), page, SLOT(startRecording()));
    disconnect(ui->actionAbortRecording, SIGNAL(triggered()), 0, 0);
    connect(ui->actionAbortRecording, SIGNAL(triggered()), page, SLOT(abortRecording()));
    disconnect(ui->actionSaveLastRecording, SIGNAL(triggered()), 0, 0);
    connect(ui->actionSaveLastRecording, SIGNAL(triggered()), page, SLOT(saveLastRecording()));
    disconnect(ui->actionReplayLastRecording, SIGNAL(triggered()), 0, 0);
    connect(ui->actionReplayLastRecording, SIGNAL(triggered()), page, SLOT(replayLastRecording()));
    disconnect(ui->actionWordWrap, SIGNAL(triggered()), 0, 0);
    connect(ui->actionWordWrap, SIGNAL(triggered()), page, SLOT(wordWrap()));
    disconnect(ui->actionFocusOnAnotherView, SIGNAL(triggered()), 0, 0);
    connect(ui->actionFocusOnAnotherView, SIGNAL(triggered()), page, SLOT(focusOnAnotherView()));
    disconnect(ui->actionHideLines, SIGNAL(triggered()), 0, 0);
    connect(ui->actionHideLines, SIGNAL(triggered()), page, SLOT(hideLines()));
    disconnect(ui->actionFoldAll, SIGNAL(triggered()), 0, 0);
    connect(ui->actionFoldAll, SIGNAL(triggered()), page, SLOT(foldAll()));
    disconnect(ui->actionUnfoldAll, SIGNAL(triggered()), 0, 0);
    connect(ui->actionUnfoldAll, SIGNAL(triggered()), page, SLOT(unfoldAll()));
    disconnect(ui->actionCollapseCurrentLevel, SIGNAL(triggered()), 0, 0);
    connect(ui->actionCollapseCurrentLevel, SIGNAL(triggered()), page, SLOT(collapseCurrentLevel()));
    disconnect(ui->actionUncollapseCurrentLevel, SIGNAL(triggered()), 0, 0);
    connect(ui->actionUncollapseCurrentLevel, SIGNAL(triggered()), page, SLOT(uncollapseCurrentLevel()));
    disconnect(ui->actionCollapseLevel, SIGNAL(triggered()), 0, 0);
    connect(ui->actionCollapseLevel, SIGNAL(triggered()), page, SLOT(collapseLevel()));
    disconnect(ui->actionUnCollapseLevel, SIGNAL(triggered()), 0, 0);
    connect(ui->actionUnCollapseLevel, SIGNAL(triggered()), page, SLOT(uncollapseLevel()));
    disconnect(ui->actionEncodeInANSI, SIGNAL(triggered()), 0, 0);
    connect(ui->actionEncodeInANSI, SIGNAL(triggered()), page, SLOT(encodeInANSI()));
    disconnect(ui->actionEncodeInUTF8WithoutBOM, SIGNAL(triggered()), 0, 0);
    connect(ui->actionEncodeInUTF8WithoutBOM, SIGNAL(triggered()), page, SLOT(encodeInUTF8WithoutBOM()));
    disconnect(ui->actionEncodeInUTF8, SIGNAL(triggered()), 0, 0);
    connect(ui->actionEncodeInUTF8, SIGNAL(triggered()), page, SLOT(encodeInUTF8()));
    disconnect(ui->actionEncodeInUCS2BigEndian, SIGNAL(triggered()), 0, 0);
    connect(ui->actionEncodeInUCS2BigEndian, SIGNAL(triggered()), page, SLOT(encodeInUCS2BigEndian()));
    disconnect(ui->actionEncodeInUCS2LittleEndian, SIGNAL(triggered()), 0, 0);
    connect(ui->actionEncodeInUCS2LittleEndian, SIGNAL(triggered()), page, SLOT(encodeInUCS2LittleEndian()));
    disconnect(ui->actionConvertToANSI, SIGNAL(triggered()), 0, 0);
    connect(ui->actionConvertToANSI, SIGNAL(triggered()), page, SLOT(convertToANSI()));
    disconnect(ui->actionConvertToUTF8WithoutBOM, SIGNAL(triggered()), 0, 0);
    connect(ui->actionConvertToUTF8WithoutBOM, SIGNAL(triggered()), page, SLOT(convertToUTF8WithoutBOM()));
    disconnect(ui->actionConvertToUTF8, SIGNAL(triggered()), 0, 0);
    connect(ui->actionConvertToUTF8, SIGNAL(triggered()), page, SLOT(convertToUTF8()));
    disconnect(ui->actionConvertToUCS2BigEndian, SIGNAL(triggered()), 0, 0);
    connect(ui->actionConvertToUCS2BigEndian, SIGNAL(triggered()), page, SLOT(convertToUCS2BigEndian()));
    disconnect(ui->actionConvertToUCS2LittleEndian, SIGNAL(triggered()), 0, 0);
    connect(ui->actionConvertToUCS2LittleEndian, SIGNAL(triggered()), page, SLOT(convertToUCS2LittleEndian()));
    disconnect(ui->actionZoomIn, SIGNAL(triggered()), 0, 0);
    connect(ui->actionZoomIn, SIGNAL(triggered()), page, SLOT(zoomIn()));
    disconnect(ui->actionZoomOut, SIGNAL(triggered()), 0, 0);
    connect(ui->actionZoomOut, SIGNAL(triggered()), page, SLOT(zoomOut()));
    disconnect(ui->actionRestoreDefaultZoom, SIGNAL(triggered()), 0, 0);
    connect(ui->actionRestoreDefaultZoom, SIGNAL(triggered()), page, SLOT(restoreDefaultZoom()));
}

void MainWindow::doCloseRequested(TabWidget *tabWidget, int index)
{
    CodeEditPage* page = qobject_cast<CodeEditPage*>(tabWidget->widget(index));
    Q_ASSERT(page);
    if (!page->canClose())
    {
        if (QMessageBox::question(this, tr("Confirm"),
                                  tr("This document is not saved, save it now?"),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            // save to file
            QString filePath = page->getFilePath();
            if (!QFile::exists(filePath))
            {
                QFileDialog::Options options;
                QString selectedFilter;
                filePath = QFileDialog::getSaveFileName(this,
                                                        tr("ainesmile Save File To"),
                                                        tr(""),
                                                        tr("All Files (*);;Text Files (*.txt)"),
                                                        &selectedFilter,
                                                        options);
            }

            if (filePath.isEmpty())
                return; // don't close this tab

            page->saveFile(filePath);
            if (rf_.addFile(filePath))
                updateRecentFilesMenuItems();
        }
    }
    else
    {
        if (ui->tabWidget->count() + ui->tabWidgetSlave->count() == 1
                && !page->isModified()
                && !aboutToQuit_
                && page->getFilePath().isEmpty())
        {
            // the only unmodified page, just skip this action
            return;
        }
    }

    tabWidget->removeTab(index);
    page->deleteLater();
    if (tabWidget->count() == 0)
    {
        tabWidget->hide();
    }
}

void MainWindow::onExchangeTab()
{
    TabWidget* tabWidget = qobject_cast<TabWidget*>(sender());
    QWidget* currentWidget = tabWidget->currentWidget();
    int currentIndex = tabWidget->currentIndex();
    QString currentText = tabWidget->tabText(currentIndex);
    QString currentTabTooltip = tabWidget->tabToolTip(currentIndex);
    tabWidget->removeTab(currentIndex);
    TabWidget* targetTabWidget = ui->tabWidgetSlave;
    if (tabWidget == ui->tabWidgetSlave)
    {
        targetTabWidget = ui->tabWidget;
    }
    if (targetTabWidget->isHidden())
    {
        targetTabWidget->setHidden(false);
        QList<int> sizes;
        sizes << ui->widget->width()/2
                 << ui->widget->width() /2;
        ui->splitterMain->setSizes(sizes);
    }
    int targetIndex = targetTabWidget->addTab(currentWidget, currentText);
    targetTabWidget->setTabToolTip(targetIndex, currentTabTooltip);

    if (tabWidget->count() == 0)
    {
        tabWidget->hide();
    }
}

void MainWindow::onIPCMessageReceived(const QString &message, QObject *socket)
{
    Q_UNUSED(socket);
    // split the message
    QStringList files = message.split('\n');
    openFiles(files);
    raise();
    activateWindow();
}

void MainWindow::openFiles(const QStringList &files)
{
    TabWidget* tabWidget = getFocusTabWidget();
    int index = 0;
    foreach(const QString& file, files)
    {
        QFileInfo fileInfo(file);
        if (QFile::exists(fileInfo.absoluteFilePath()))
        {
            // check if the file has been opened already
            bool bExists = false;
            int count = ui->tabWidget->count();
            for (int i = 0; i< count; i++)
            {
                CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidget->widget(i));
                Q_ASSERT(page);
                const QString& pageFileName = page->getFilePath();
                if (!pageFileName.isEmpty())
                {
                    QFileInfo pageFileInfo(pageFileName);
                    if (pageFileInfo == fileInfo)
                    {
                        bExists = true;
                        break;
                    }
                }
            }
            if (!bExists)
            {
                count = ui->tabWidgetSlave->count();
                for (int i = 0; i< count; i++)
                {
                    CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidgetSlave->widget(i));
                    Q_ASSERT(page);
                    const QString& pageFileName = page->getFilePath();
                    if (!pageFileName.isEmpty())
                    {
                        QFileInfo pageFileInfo(pageFileName);
                        if (pageFileInfo == fileInfo)
                        {
                            bExists = true;
                            break;
                        }
                    }
                }
            }

            if (!bExists)
            {
                // create an edit page, open the file
                CodeEditPage* codeeditpage = new CodeEditPage(this);
                index = tabWidget->addTab(codeeditpage, QIcon(), fileInfo.fileName());
                connectSignals(codeeditpage);
                updateUI(codeeditpage);
                codeeditpage->openFile(fileInfo.absoluteFilePath());
                tabWidget->setTabToolTip(index, fileInfo.absoluteFilePath());
                // log into recent file list
                if (rf_.addFile(fileInfo.absoluteFilePath()))
                    updateRecentFilesMenuItems();
            }
        }
    }

    if (!files.isEmpty())
        tabWidget->setCurrentIndex(index);
}

void MainWindow::newDocument()
{
    static int count = 1;
    CodeEditPage* codeeditpage = new CodeEditPage(this);
    QString title = QString(tr("Untitled%1")).arg(count);
    TabWidget* tabWidget = getFocusTabWidget();
    int index = tabWidget->addTab(codeeditpage, QIcon(), title);
    tabWidget->setCurrentIndex(index);
    count++;
    connectSignals(codeeditpage);
    updateUI(codeeditpage);
    tabWidget->setFocus();
    codeeditpage->setFocus();
    codeeditpage->grabFocus();
}


void MainWindow::onCurrentPageChanged(int index)
{
    if (index == -1)
    {
        if (!aboutToQuit_)
            newDocument();
    }
    else
    {
        // update action UI
        TabWidget* tabWidget = qobject_cast<TabWidget*>(sender());
        CodeEditPage* page = qobject_cast<CodeEditPage*>(tabWidget->widget(index));
        Q_ASSERT(page);
        updateUI(page);
        connectSignals(page);
    }
}

void MainWindow::onCloseRequested(int index)
{
    TabWidget* tabWidget = qobject_cast<TabWidget*>(sender());
    doCloseRequested(tabWidget, index);
}

void MainWindow::onCurrentDocumentChanged()
{
    CodeEditPage* page = qobject_cast<CodeEditPage*>(sender());
    Q_ASSERT(page);
    Q_UNUSED(page);
}

void MainWindow::onCopyAvailableChanged()
{
    CodeEditPage* page = qobject_cast<CodeEditPage*>(sender());
    Q_ASSERT(page);
    ui->actionCopy->setEnabled(page->canCopy());
    ui->actionCut->setEnabled(page->canCut());
}

void MainWindow::onPasteAvailableChanged()
{
    CodeEditPage* page = qobject_cast<CodeEditPage*>(sender());
    Q_ASSERT(page);
    ui->actionPaste->setEnabled(page->canPaste());
}

void MainWindow::onUndoAvailableChanged()
{
    CodeEditPage* page = qobject_cast<CodeEditPage*>(sender());
    Q_ASSERT(page);
    ui->actionUndo->setEnabled(page->canUndo());
}

void MainWindow::onRedoAvailableChanged()
{
    CodeEditPage* page = qobject_cast<CodeEditPage*>(sender());
    Q_ASSERT(page);
    ui->actionRedo->setEnabled(page->canRedo());
}

void MainWindow::onRecentFileTriggered(const QString & file)
{
    // open the file
    if (QFile::exists(file))
    {
        // check if the file has been opened already
        bool bExists = false;
        int count = ui->tabWidget->count();
        for (int i = 0; i< count; i++)
        {
            CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidget->widget(i));
            Q_ASSERT(page);
            const QString& pageFileName = page->getFilePath();
            if (!pageFileName.isEmpty())
            {
                QFileInfo fileInfo(file);
                QFileInfo pageFileInfo(pageFileName);
                if (pageFileInfo == fileInfo)
                {
                    bExists = true;
                    ui->tabWidget->setCurrentIndex(i);
                    break;
                }
            }
        }
        if (!bExists)
        {
            count = ui->tabWidgetSlave->count();
            for (int i = 0; i< count; i++)
            {
                CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidgetSlave->widget(i));
                Q_ASSERT(page);
                const QString& pageFileName = page->getFilePath();
                if (!pageFileName.isEmpty())
                {
                    QFileInfo fileInfo(file);
                    QFileInfo pageFileInfo(pageFileName);
                    if (pageFileInfo == fileInfo)
                    {
                        bExists = true;
                        ui->tabWidgetSlave->setCurrentIndex(i);
                        break;
                    }
                }
            }
        }

        if (!bExists)
        {
            // create an edit page, open the file
            TabWidget* tabWidget = getFocusTabWidget();
            CodeEditPage* codeeditpage = new CodeEditPage(this);
            int index = tabWidget->addTab(codeeditpage, QIcon(),QFileInfo(file).fileName());
            connectSignals(codeeditpage);
            updateUI(codeeditpage);
            codeeditpage->openFile(file);
            tabWidget->setTabToolTip(index, file);
            tabWidget->setCurrentIndex(index);
        }
    }
}

void MainWindow::onRecentProjectTriggered(const QString & project)
{

}

void MainWindow::onActivateTabClicked(int index)
{
//    Q_ASSERT(index < ui->tabWidget->count());
//    ui->tabWidget->setCurrentIndex(index);
}

void MainWindow::onCloseTabClicked(const QList<int>& fileList)
{
//    for (int i = fileList.size() -1; i >= 0; i--)
//    {
//        int index = fileList.at(i);
//        ui->tabWidget->setCurrentIndex(index);
//        onCloseRequested(index);
//    }
}

void MainWindow::onSaveTabClicked(const QList<int>& fileList)
{
    for (int i = fileList.size() -1; i >= 0; i--)
    {
        int index = fileList.at(i);
        CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidget->widget(index));
        Q_ASSERT(page);
        QString filePath = page->getFilePath();

        // do save
        bool resetTabText = false;
        if (!QFile::exists(filePath))
        {
            QFileDialog::Options options;
            QString selectedFilter;
            filePath = QFileDialog::getSaveFileName(this,
                                                    tr("ainesmile Save File To"),
                                                    tr(""),
                                                    tr("All Files (*);;Text Files (*.txt)"),
                                                    &selectedFilter,
                                                    options);
            if (!filePath.isEmpty())
                resetTabText = true;
        }

        if (!filePath.isEmpty())
        {
            page->saveFile(filePath);

            if (rf_.addFile(filePath))
                updateRecentFilesMenuItems();
        }

        if (resetTabText)
        {
            ui->tabWidget->setTabText(index, QFileInfo(filePath).fileName());
            ui->tabWidget->setTabToolTip(index, filePath);
        }
    }
}

void MainWindow::on_actionNewFile_triggered()
{
    newDocument();
}

void MainWindow::on_actionToggleFullScreenMode_triggered()
{
    static bool isMaximized = false;
    isMaximized = isMaximized();
    if (isFullScreen()) {
        if (isMaximized)
            showMaximized();
        else
            showNormal();
    }
    else
        showFullScreen();
}

void MainWindow::on_actionOpenFile_triggered()
{
    QFileDialog::Options options;
    QString selectedFilter;
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                    tr("ainesmile Open Files"),
                                                    tr(""),
                                                    tr("All Files (*);;Text Files (*.txt)"),
                                                    &selectedFilter,
                                                    options);
    openFiles(fileNames);
}

void MainWindow::on_actionExitApp_triggered()
{
    close();
}

void MainWindow::on_actionSaveFile_triggered()
{
    CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidget->currentWidget());
    Q_ASSERT(page);
    QString filePath = page->getFilePath();
    bool resetTabText = false;
    if (!QFile::exists(filePath))
    {
        QFileDialog::Options options;
        QString selectedFilter;
        filePath = QFileDialog::getSaveFileName(this,
                                     tr("ainesmile Save File To"),
                                     tr(""),
                                     tr("All Files (*);;Text Files (*.txt)"),
                                     &selectedFilter,
                                     options);
        if (!filePath.isEmpty())
            resetTabText = true;
    }

    if (!filePath.isEmpty())
        page->saveFile(filePath);

    if (resetTabText)
    {
        TabWidget* tabWidget = getFocusTabWidget();
        int index = tabWidget->currentIndex();
        tabWidget->setTabText(index, QFileInfo(filePath).fileName());
        tabWidget->setTabToolTip(index, filePath);
    }
}

void MainWindow::on_actionSaveAs_triggered()
{
    CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidget->currentWidget());
    Q_ASSERT(page);

    QFileDialog::Options options;
    QString selectedFilter;
    QString filePath = QFileDialog::getSaveFileName(this,
                                            tr("ainesmile Save File To"),
                                            tr(""),
                                            tr("All Files (*);;Text Files (*.txt)"),
                                            &selectedFilter,
                                            options);
    if (!filePath.isEmpty())
    {
        page->saveFile(filePath);

        TabWidget* tabWidget = getFocusTabWidget();
        int index = tabWidget->currentIndex();
        tabWidget->setTabText(index, QFileInfo(filePath).fileName());
        tabWidget->setTabToolTip(index, filePath);

        if (rf_.addFile(filePath))
            updateRecentFilesMenuItems();
    }
}

void MainWindow::on_actionHelpContent_triggered()
{
    QDir dir = QApplication::applicationDirPath();
#if defined(Q_WS_MAC)
    dir.cdUp();
    dir.cd("Docs");
#endif
    QString helpFilePath = dir.absolutePath();
    helpFilePath.append("/manual.pdf");
    QDesktopServices::openUrl(QUrl("file://" + helpFilePath));
}

void MainWindow::on_actionAboutApp_triggered()
{
    AboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::on_actionDForDSoftwareHome_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.dfordsoft.com"));
}

void MainWindow::on_actionAinesmileProductPage_triggered()
{
    QDesktopServices::openUrl(QUrl("http://www.dfordsoft.com/ainesmile/"));
}

void MainWindow::on_actionSaveAll_triggered()
{
    int count = ui->tabWidget->count();
    for (int index = count -1; index >=0; index--)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidget->widget(index));
        Q_ASSERT(page);
        QString filePath = page->getFilePath();
        bool resetTabText = false;
        if (!QFile::exists(filePath))
        {
            QFileDialog::Options options;
            QString selectedFilter;
            filePath = QFileDialog::getSaveFileName(this,
                                         tr("ainesmile Save File To"),
                                         tr(""),
                                         tr("All Files (*);;Text Files (*.txt)"),
                                         &selectedFilter,
                                         options);
            if (!filePath.isEmpty())
                resetTabText = true;
        }

        if (!filePath.isEmpty())
            page->saveFile(filePath);

        if (resetTabText)
        {
            int index = ui->tabWidget->currentIndex();
            ui->tabWidget->setTabText(index, QFileInfo(filePath).fileName());
            ui->tabWidget->setTabToolTip(index, filePath);
        }
    }
}

void MainWindow::on_actionCloseAll_triggered()
{
    int count = ui->tabWidget->count();
    for (int index = count -1; index >=0; index--)
    {
        doCloseRequested(ui->tabWidget, index);
    }

    count = ui->tabWidgetSlave->count();
    for (int index = count -1; index >=0; index--)
    {
        doCloseRequested(ui->tabWidgetSlave, index);
    }
}

void MainWindow::on_actionCloseAllButActiveDocument_triggered()
{
    int currentIndex = ui->tabWidget->currentIndex();
    int count = ui->tabWidget->count();
    for (int index = count -1; index > currentIndex; index--)
    {
        doCloseRequested(ui->tabWidget, index);
    }
    while (ui->tabWidget->count() > 1)
    {
        doCloseRequested(ui->tabWidget, 0);
    }
}

void MainWindow::on_actionOpenAllRecentFiles_triggered()
{
    QStringList& files = rf_.recentFiles();
    Q_FOREACH(const QString& file, files)
    {
        onRecentFileTriggered(file);
    }
}

void MainWindow::on_actionEmptyRecentFilesList_triggered()
{
    rf_.clearFiles();
    Q_FOREACH(QAction* action, recentFileActions_)
    {
        action->setVisible(false);
    }
}

void MainWindow::on_actionAlwaysOnTop_triggered()
{
#if defined(Q_OS_WIN)
    if (ui->actionAlwaysOnTop->isChecked())
    {
        ::SetWindowPos((HWND)winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    else
    {
        ::SetWindowPos((HWND)winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
#else
    Qt::WindowFlags flags = this->windowFlags();
    if (ui->actionAlwaysOnTop->isChecked())
    {
        setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        show();
    }
    else
    {
        setWindowFlags(flags ^ (Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
        show();
    }
#endif
}

void MainWindow::on_actionCloseAllDocuments_triggered()
{
    int count = ui->tabWidget->count();
    for (int index = count -1; index >=0; index--)
    {
        onCloseRequested(index);
    }
    count = ui->tabWidgetSlave->count();
    for (int index = count -1; index >=0; index--)
    {
        onCloseRequested(index);
    }
}

void MainWindow::on_actionClose_triggered()
{
    int currentIndex = ui->tabWidget->currentIndex();
    onCloseRequested(currentIndex);
}

void MainWindow::on_actionFindInFiles_triggered()
{
    if (!findReplaceDialog_)
    {
        findReplaceDialog_ = new FindReplaceDialog(this);
    }
    findReplaceDialog_->show();
    findReplaceDialog_->raise();
    findReplaceDialog_->activateWindow();
}

void MainWindow::on_actionSearchResultsWindow_triggered()
{

}

void MainWindow::on_actionNextSearchResult_triggered()
{

}

void MainWindow::on_actionPreviousSearchResult_triggered()
{

}

void MainWindow::on_actionRegistration_triggered()
{
    RegisterDialog dlg(this);
    dlg.exec();
}

void MainWindow::on_actionSelectExtensionItem_triggered()
{

}

void MainWindow::on_actionShowExtensionEditor_triggered()
{

}

void MainWindow::on_actionEditCommands_triggered()
{

}

void MainWindow::on_actionEditSnippets_triggered()
{

}

void MainWindow::on_actionReloadExtensions_triggered()
{

}

void MainWindow::on_actionShowWhiteSpaceAndTAB_triggered()
{
    bool enabled = ui->actionShowWhiteSpaceAndTAB->isChecked();
    boost::property_tree::ptree& pt = Config::instance()->pt();
    pt.put("show.white_space_and_tab", enabled);

    int count = ui->tabWidget->count();
    for (int index = count -1; index >=0; index--)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidget->widget(index));
        Q_ASSERT(page);

        page->setShowWhiteSpaceAndTAB(enabled);
    }
    count = ui->tabWidgetSlave->count();
    for (int index = count -1; index >=0; index--)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidgetSlave->widget(index));
        Q_ASSERT(page);

        page->setShowWhiteSpaceAndTAB(enabled);
    }
}

void MainWindow::on_actionShowEndOfLine_triggered()
{
    bool enabled = ui->actionShowEndOfLine->isChecked();
    boost::property_tree::ptree& pt = Config::instance()->pt();
    pt.put("show.end_of_line", enabled);

    int count = ui->tabWidget->count();
    for (int index = count -1; index >=0; index--)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidget->widget(index));
        Q_ASSERT(page);

        page->setShowEndOfLine(enabled);
    }
    count = ui->tabWidgetSlave->count();
    for (int index = count -1; index >=0; index--)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidgetSlave->widget(index));
        Q_ASSERT(page);

        page->setShowEndOfLine(enabled);
    }
}

void MainWindow::on_actionShowIndentGuide_triggered()
{
    bool enabled = ui->actionShowIndentGuide->isChecked();
    boost::property_tree::ptree& pt = Config::instance()->pt();
    pt.put("show.indent_guide", enabled);

    int count = ui->tabWidget->count();
    for (int index = count -1; index >=0; index--)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidget->widget(index));
        Q_ASSERT(page);

        page->setShowIndentGuide(enabled);
    }
    count = ui->tabWidgetSlave->count();
    for (int index = count -1; index >=0; index--)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidgetSlave->widget(index));
        Q_ASSERT(page);

        page->setShowIndentGuide(enabled);
    }
}

void MainWindow::on_actionShowWrapSymbol_triggered()
{
    bool enabled = ui->actionShowWrapSymbol->isChecked();
    boost::property_tree::ptree& pt = Config::instance()->pt();
    pt.put("show.wrap_symbol", enabled);

    int count = ui->tabWidget->count();
    for (int index = count -1; index >=0; index--)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidget->widget(index));
        Q_ASSERT(page);

        page->setShowWrapSymbol(enabled);
    }
    count = ui->tabWidgetSlave->count();
    for (int index = count -1; index >=0; index--)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidgetSlave->widget(index));
        Q_ASSERT(page);

        page->setShowWrapSymbol(enabled);
    }
}

void MainWindow::on_actionPreferences_triggered()
{
    PreferenceDialog dlg(this);
    dlg.exec();
}

void MainWindow::on_actionExternalTools_triggered()
{

}

void MainWindow::on_actionPluginManager_triggered()
{

}

void MainWindow::on_actionWindowsList_triggered()
{
    WindowListDialog dlg(this);
    QStringList fileList;
    for (int i = 0; i < ui->tabWidget->count(); i++)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidget->widget(i));
        Q_ASSERT(page);
        if (!page->getFilePath().isEmpty())
            fileList << page->getFilePath();
        else
            fileList << ui->tabWidget->tabText(i);
    }
    for (int i = 0; i < ui->tabWidgetSlave->count(); i++)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(ui->tabWidgetSlave->widget(i));
        Q_ASSERT(page);
        if (!page->getFilePath().isEmpty())
            fileList << page->getFilePath();
        else
            fileList << ui->tabWidgetSlave->tabText(i);
    }
    dlg.setFileList(fileList);
    connect(&dlg, SIGNAL(activateTab(int)), this, SLOT(onActivateTabClicked(int)));
    connect(&dlg, SIGNAL(closeTab(const QList<int> &)), this, SLOT(onCloseTabClicked(const QList<int> &)));
    connect(&dlg, SIGNAL(saveTab(const QList<int> &)), this, SLOT(onSaveTabClicked(const QList<int> &)));
    dlg.exec();
}

void MainWindow::on_actionOpenProject_triggered()
{

}

void MainWindow::on_actionCloseProject_triggered()
{

}

void MainWindow::on_actionNewProject_triggered()
{

}

void MainWindow::on_actionEmptyRecentProjectsList_triggered()
{
    rf_.clearProjects();
    Q_FOREACH(QAction* action, recentProjectActions_)
    {
        action->setVisible(false);
    }
}

void MainWindow::hideFeatures()
{
#if defined(Q_OS_MAC)
    ui->actionAlwaysOnTop->setVisible(false);
#endif

    StupidCheck sc;
    if (!sc.isDeluxe())
    {
        // hide the deluxe features
    }

    if (sc.isStandard())
    {
        // hide the professional features
        ui->menuPlugins->setVisible(false);
        ui->actionPluginManager->setVisible(false);
        ui->actionPlugins->setVisible(false);
        ui->actionShowExtensionEditor->setVisible(false);
        ui->actionEditCommands->setVisible(false);
        ui->actionEditSnippets->setVisible(false);
        ui->actionReloadExtensions->setVisible(false);
        ui->actionSelectExtensionItem->setVisible(false);
    }
}
