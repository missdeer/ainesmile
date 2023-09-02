#include "stdafx.h"

#include <QCoreApplication>
#include <QEvent>

#include "mainwindow.h"
#include "codeeditpage.h"
#include "config.h"
#include "encodinglistdialog.h"
#include "findreplace.h"
#include "findreplaceresultmodel.h"
#include "preferencedialog.h"
#include "ui_mainwindow.h"
#include "windowlistdialog.h"

using namespace FindReplace;

const QEvent::Type AppIdleEventType = static_cast<QEvent::Type>(QEvent::registerEventType());

class IdleEvent : public QEvent
{
public:
    IdleEvent() : QEvent(AppIdleEventType) {}
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onCurrentPageChanged);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onCloseRequested);
    connect(ui->tabWidget, &TabWidget::exchangeTab, this, &MainWindow::onExchangeTab);
    connect(ui->tabWidget, &TabWidget::updateRecentFiles, this, &MainWindow::onUpdateRecentFilesMenuItems);
    connect(ui->tabWidget, &TabWidget::codeEditPageCreated, this, &MainWindow::onCodeEditPageCreated);
    connect(ui->tabWidgetSlave, &QTabWidget::currentChanged, this, &MainWindow::onCurrentPageChanged);
    connect(ui->tabWidgetSlave, &QTabWidget::tabCloseRequested, this, &MainWindow::onCloseRequested);
    connect(ui->tabWidgetSlave, &TabWidget::exchangeTab, this, &MainWindow::onExchangeTab);
    connect(ui->tabWidgetSlave, &TabWidget::updateRecentFiles, this, &MainWindow::onUpdateRecentFilesMenuItems);
    connect(ui->tabWidgetSlave, &TabWidget::codeEditPageCreated, this, &MainWindow::onCodeEditPageCreated);
    ui->tabWidget->setTheOtherSide(ui->tabWidgetSlave);
    ui->tabWidget->setRecentFiles(&m_recentFiles);
    ui->tabWidgetSlave->setTheOtherSide(ui->tabWidget);
    ui->tabWidgetSlave->setRecentFiles(&m_recentFiles);

    connect(ui->actionReopenAs, &QAction::triggered, this, &MainWindow::onReopenAsEncoding);
    connect(ui->actionSaveAsEncoding, &QAction::triggered, this, &MainWindow::onSaveAsEncoding);
    connect(ui->actionAutoDetectEncoding, &QAction::toggled, this, &MainWindow::onAutoDetectEncoding);

    setActionShortcuts();
    setRecentFiles();
    setMenuItemChecked();
    setAcceptDrops(true);
    hideFeatures();

    ui->menuView->addSeparator();
    ui->menuView->addAction(ui->toolBar->toggleViewAction());

    m_encodingLabel = new QLabel(QStringLiteral("UTF-8"), this);
    m_encodingLabel->setMaximumWidth(150);
    m_encodingLabel->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_encodingLabel, &QWidget::customContextMenuRequested, this, &MainWindow::onSelectEncodingCustomContextMenuRequested);
    ui->statusBar->addPermanentWidget(m_encodingLabel);

    m_withBOMLabel = new QLabel(QStringLiteral("BOM"), this);
    m_withBOMLabel->setEnabled(false);
    m_withBOMLabel->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_withBOMLabel, &QWidget::customContextMenuRequested, this, &MainWindow::onSelectEncodingCustomContextMenuRequested);
    ui->statusBar->addPermanentWidget(m_withBOMLabel);

    QList<int> sizes;
    sizes << ui->widget->width() / 2 << ui->widget->width() / 2;
    ui->splitterMain->setSizes(sizes);
    ui->tabWidgetSlave->hide();
    ui->tabWidget->setFocus();

    ui->cbScope->setCurrentIndex(1);
    ui->cbScope->setCurrentIndex(0);
    ui->dockFindReplace->close();
    ui->dockFindResult->close();

    ui->menuDockWindows->addAction(ui->dockFindReplace->toggleViewAction());
    ui->menuDockWindows->addAction(ui->dockFindResult->toggleViewAction());

    ui->tvFindReplaceResult->setModel(FindReplaceResultModel::instance());

    readFindHistory();

    readReplaceHistory();

    readFindReplaceFiltersHistory();

    QSettings settings(QStringLiteral("dfordsoft.com"), QStringLiteral("ainesmile"));
    if (settings.contains("geometry"))
    {
        restoreGeometry(settings.value("geometry").toByteArray());
    }
    if (settings.contains("windowState"))
    {
        restoreState(settings.value("windowState").toByteArray());
        restoreDockWidget(ui->dockFindReplace);
        restoreDockWidget(ui->dockFindResult);
    }

    // Post the initial idle event
    QCoreApplication::postEvent(this, new IdleEvent());
}

void MainWindow::readFindHistory()
{
    auto &ptree = Config::instance()->pt();
    try
    {
        auto texts = ptree.get_child("find.texts");
        for (auto &val : texts)
        {
            ui->cbFind->addItem(QString::fromStdString(val.second.data()));
        }
        ui->cbFind->lineEdit()->clear();
    }
    catch (...)
    {
    }
}

void MainWindow::readReplaceHistory()
{
    auto &ptree = Config::instance()->pt();
    try
    {
        auto texts = ptree.get_child("replace.texts");
        for (auto &val : texts)
        {
            ui->cbReplace->addItem(QString::fromStdString(val.second.data()));
        }
        ui->cbReplace->lineEdit()->clear();
    }
    catch (...)
    {
    }
}

void MainWindow::readFindReplaceFiltersHistory()
{
    auto &ptree = Config::instance()->pt();
    try
    {
        auto texts = ptree.get_child("filters.texts");
        for (auto &val : texts)
        {
            ui->cbFilters->addItem(QString::fromStdString(val.second.data()));
        }
        ui->cbFilters->lineEdit()->clear();
    }
    catch (...)
    {
    }
}

MainWindow::~MainWindow()
{
    Config::instance()->sync();
    delete ui;
}

void MainWindow::onIdle()
{
    auto *page = getFocusCodeEditor();
    if (page)
    {
        updateUI(page);
        auto filePath = page->getFilePath();
        if (!filePath.isEmpty())
        {
            ui->actionReopenAs->setEnabled(true);
            ui->actionSaveAsEncoding->setEnabled(true);
            m_encodingLabel->setText(page->encoding());
            m_withBOMLabel->setEnabled(page->hasBOM());
            return;
        }
    }
    ui->actionReopenAs->setEnabled(false);
    ui->actionSaveAsEncoding->setEnabled(true);
    m_encodingLabel->setText(QStringLiteral("UTF-8"));
    m_withBOMLabel->setEnabled(false);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_aboutToQuit = true;
    ui->tabWidget->setAboutToQuit(m_aboutToQuit);
    ui->tabWidgetSlave->setAboutToQuit(m_aboutToQuit);

    ui->tabWidget->closeAll();
    ui->tabWidgetSlave->closeAll();

    if (ui->tabWidget->count() + ui->tabWidgetSlave->count() != 0)
    {
        event->ignore();
    }
    else
    {
        QSettings settings(QStringLiteral("dfordsoft.com"), QStringLiteral("ainesmile"));
        settings.setValue("geometry", saveGeometry());
        settings.setValue("windowState", saveState());
        settings.sync();
        event->accept();
    }
    m_aboutToQuit = false;
    ui->tabWidget->setAboutToQuit(m_aboutToQuit);
    ui->tabWidgetSlave->setAboutToQuit(m_aboutToQuit);

    QMainWindow::closeEvent(event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event && event->mimeData() && event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const auto *mime = event->mimeData();
    if (mime && mime->hasUrls())
    {
        auto urls = mime->urls();
        for (auto &url : urls)
        {
            openFile(url.toLocalFile());
        }
    }
}

TabWidget *MainWindow::getFocusTabWidget()
{
    if (ui->tabWidgetSlave->focus())
    {
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
    ui->actionSelectAll->setShortcut(QKeySequence::SelectAll);
}

void MainWindow::setRecentFiles()
{
    m_recentFileActions << ui->actionRecentFile1 << ui->actionRecentFile2 << ui->actionRecentFile3 << ui->actionRecentFile4 << ui->actionRecentFile5
                        << ui->actionRecentFile6 << ui->actionRecentFile7 << ui->actionRecentFile8 << ui->actionRecentFile9 << ui->actionRecentFile10;

    for (QAction *action : m_recentFileActions)
    {
        connect(action, &QAction::triggered, this, &MainWindow::onRecentFileTriggered);
    }

    onUpdateRecentFilesMenuItems();
}

void MainWindow::setMenuItemChecked()
{
    auto &ptree   = Config::instance()->pt();
    bool  enabled = ptree.get<bool>("show.end_of_line", false);
    ui->actionShowEndOfLine->setChecked(enabled);
    enabled = ptree.get<bool>("show.indent_guide", false);
    ui->actionShowIndentGuide->setChecked(enabled);
    enabled = ptree.get<bool>("show.white_space_and_tab", false);
    ui->actionShowWhiteSpaceAndTAB->setChecked(enabled);
    enabled = ptree.get<bool>("show.wrap_symbol", false);
    ui->actionShowWrapSymbol->setChecked(enabled);
    enabled = ptree.get<bool>("encoding.auto_detect", false);
    ui->actionAutoDetectEncoding->setChecked(enabled);
}

void MainWindow::onUpdateRecentFilesMenuItems()
{
    const int   maxRecentCount = 10;
    QStringList recentFiles    = m_recentFiles.recentFiles();
    for (int index = 0; index < recentFiles.length() && index < maxRecentCount; ++index)
    {
        const QString &filePath = recentFiles.at(index);
        QAction       *action   = m_recentFileActions.at(index);
        action->setText(filePath);
        action->setVisible(true);
    }
    for (int index = recentFiles.length(); index < maxRecentCount; index++)
    {
        QAction *action = m_recentFileActions.at(index);
        action->setVisible(false);
    }
}

void MainWindow::updateUI(CodeEditor *page)
{
    ui->actionSaveFile->setEnabled(page->isModified());
    ui->actionCut->setEnabled(page->canCut());
    ui->actionCopy->setEnabled(page->canCopy());
    ui->actionPaste->setEnabled(page->canPaste());
    ui->actionUndo->setEnabled(page->canUndo());
    ui->actionRedo->setEnabled(page->canRedo());
}

void MainWindow::onReopenAsEncoding()
{
    EncodingListDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        auto *page = getFocusCodeEditor();
        if (page)
        {
            page->reopenAsEncoding(dlg.selectedEncoding(), dlg.withBOM());
        }
    }
}

void MainWindow::onSaveAsEncoding()
{
    EncodingListDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        auto *page = getFocusCodeEditor();
        if (page)
        {
            page->saveAsEncoding(dlg.selectedEncoding(), dlg.withBOM());
        }
    }
}

void MainWindow::onAutoDetectEncoding(bool toggled)
{
    auto &ptree = Config::instance()->pt();
    ptree.put("encoding.auto_detect", toggled);
    Config::instance()->sync();
}

void MainWindow::connectSignals(CodeEditor *page)
{
    if (m_lastConnectedCodeEditPage)
    {
        disconnect(m_lastConnectedCodeEditPage, &CodeEditor::modifiedNotification, this, &MainWindow::onCurrentDocumentChanged);
    }
    connect(page, &CodeEditor::modifiedNotification, this, &MainWindow::onCurrentDocumentChanged);

    disconnect(ui->actionCut, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionCut, &QAction::triggered, page, &CodeEditor::cut);
    disconnect(ui->actionCopy, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionCopy, &QAction::triggered, page, &CodeEditor::copy);
    disconnect(ui->actionPaste, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionPaste, &QAction::triggered, page, &CodeEditor::paste);
    disconnect(ui->actionUndo, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionUndo, &QAction::triggered, page, &CodeEditor::undo);
    disconnect(ui->actionRedo, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionRedo, &QAction::triggered, page, &CodeEditor::redo);
    disconnect(ui->actionPrint, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionPrint, &QAction::triggered, page, &CodeEditor::print);
    disconnect(ui->actionPrintNow, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionPrintNow, &QAction::triggered, page, &CodeEditor::printNow);
    disconnect(ui->actionDelete, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionDelete, &QAction::triggered, page, &CodeEditor::deleteCurrent);
    disconnect(ui->actionSelectAll, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionSelectAll, &QAction::triggered, page, &CodeEditor::selectAll);
    disconnect(ui->actionSetReadOnly, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionSetReadOnly, &QAction::triggered, page, &CodeEditor::setReadOnly);
    disconnect(ui->actionClearReadOnlyFlag, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionClearReadOnlyFlag, &QAction::triggered, page, &CodeEditor::clearReadOnlyFlag);
    disconnect(ui->actionWindowsFormat, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionWindowsFormat, &QAction::triggered, page, &CodeEditor::eolWindowsFormat);
    disconnect(ui->actionUNIXFormat, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionUNIXFormat, &QAction::triggered, page, &CodeEditor::eolUNIXFormat);
    disconnect(ui->actionMacFormat, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionMacFormat, &QAction::triggered, page, &CodeEditor::eolMacFormat);
    disconnect(ui->actionGoTo, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionGoTo, &QAction::triggered, page, &CodeEditor::gotoLine);
    disconnect(ui->actionGoToMatchingBrace, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionGoToMatchingBrace, &QAction::triggered, page, &CodeEditor::gotoMatchingBrace);
    disconnect(ui->actionToggleBookmark, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionToggleBookmark, &QAction::triggered, page, &CodeEditor::toggleBookmark);
    disconnect(ui->actionNextBookmark, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionNextBookmark, &QAction::triggered, page, &CodeEditor::nextBookmark);
    disconnect(ui->actionPreviousBookmark, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionPreviousBookmark, &QAction::triggered, page, &CodeEditor::previousBookmark);
    disconnect(ui->actionClearAllBookmarks, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionClearAllBookmarks, &QAction::triggered, page, &CodeEditor::clearAllBookmarks);
    disconnect(ui->actionCutBookmarkedLines, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionCutBookmarkedLines, &QAction::triggered, page, &CodeEditor::cutBookmarkLines);
    disconnect(ui->actionCopyBookmarkedLines, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionCopyBookmarkedLines, &QAction::triggered, page, &CodeEditor::copyBookmarkLines);
    disconnect(ui->actionPasteToReplaceBookmarkedLines, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionPasteToReplaceBookmarkedLines, &QAction::triggered, page, &CodeEditor::pasteToReplaceBookmarkedLines);
    disconnect(ui->actionRemoveBookmarkedLines, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionRemoveBookmarkedLines, &QAction::triggered, page, &CodeEditor::removeBookmarkedLines);
    disconnect(ui->actionRemoveUnmarkedLines, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionRemoveUnmarkedLines, &QAction::triggered, page, &CodeEditor::removeUnbookmarkedLines);
    disconnect(ui->actionInverseBookmark, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionInverseBookmark, &QAction::triggered, page, &CodeEditor::inverseBookmark);
    disconnect(ui->actionWordWrap, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionWordWrap, &QAction::triggered, page, &CodeEditor::wordWrap);
    disconnect(ui->actionZoomIn, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionZoomIn, &QAction::triggered, page, &CodeEditor::zoomIn);
    disconnect(ui->actionZoomOut, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionZoomOut, &QAction::triggered, page, &CodeEditor::zoomOut);
    disconnect(ui->actionRestoreDefaultZoom, &QAction::triggered, nullptr, nullptr);
    connect(ui->actionRestoreDefaultZoom, &QAction::triggered, page, &CodeEditor::restoreDefaultZoom);

    m_lastConnectedCodeEditPage = page;
}

void MainWindow::onCodeEditPageCreated(CodeEditor *page)
{
    connect(page, &CodeEditor::focusIn, this, &MainWindow::onCodeEditPageFocusIn);
}

void MainWindow::onCodeEditPageFocusIn()
{
    auto *page = qobject_cast<CodeEditor *>(sender());
    Q_ASSERT(page);
    connectSignals(page);
}

void MainWindow::onExchangeTab()
{
    auto *tabWidget         = qobject_cast<TabWidget *>(sender());
    auto *currentWidget     = tabWidget->currentWidget();
    int   currentIndex      = tabWidget->currentIndex();
    auto  currentText       = tabWidget->tabText(currentIndex);
    auto  currentTabTooltip = tabWidget->tabToolTip(currentIndex);
    m_exchanging            = true;
    tabWidget->removeTab(currentIndex);
    TabWidget *targetTabWidget = ui->tabWidgetSlave;
    if (tabWidget == ui->tabWidgetSlave)
    {
        targetTabWidget = ui->tabWidget;
    }
    if (targetTabWidget->isHidden())
    {
        targetTabWidget->setHidden(false);
    }
    int targetIndex = targetTabWidget->addTab(currentWidget, currentText);
    targetTabWidget->setTabToolTip(targetIndex, currentTabTooltip);
    targetTabWidget->setCurrentIndex(targetIndex);
    targetTabWidget->setFocus();
    auto *codeeditpage = qobject_cast<CodeEditor *>(currentWidget);
    codeeditpage->setFocus();
    codeeditpage->grabFocus();

    if (tabWidget->count() == 0)
    {
        tabWidget->hide();
    }

    m_exchanging = false;
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
    auto *tabWidget = getFocusTabWidget();
    tabWidget->openFiles(files);
}

void MainWindow::newDocument()
{
    static int count     = 1;
    QString    title     = tr("Untitled%1").arg(count++);
    auto      *tabWidget = getFocusTabWidget();
    tabWidget->newDocument(title);
}

void MainWindow::onCurrentPageChanged(int index)
{
    if (index == -1)
    {
        if (!m_aboutToQuit && !m_exchanging && (ui->tabWidget->count() + ui->tabWidgetSlave->count() == 0))
        {
            newDocument();
        }
    }
    else
    {
        // update action UI
        auto *tabWidget = qobject_cast<TabWidget *>(sender());
        auto *page      = qobject_cast<CodeEditor *>(tabWidget->widget(index));
        Q_ASSERT(page);
        connectSignals(page);
    }
}

void MainWindow::onCloseRequested(int index)
{
    auto *tabWidget = qobject_cast<TabWidget *>(sender());
    tabWidget->doCloseRequested(index);
}

void MainWindow::onCurrentDocumentChanged()
{
    auto *page = qobject_cast<CodeEditor *>(sender());
    Q_ASSERT(page);
    Q_UNUSED(page);
    ui->actionSaveFile->setEnabled(page->isModified());
    // change the tab title
    TabWidget *targetTabWidget = ui->tabWidget;
    int        index           = targetTabWidget->findTabIndex(page);
    if (index == -1)
    {
        targetTabWidget = ui->tabWidgetSlave;
        index           = targetTabWidget->findTabIndex(page);
    }

    if (index >= 0)
    {
        QString text = targetTabWidget->tabText(index);
        if (page->isModified())
        {
            if (text.at(0) != QChar('*'))
            {
                targetTabWidget->setTabText(index, "*" + text);
            }
        }
        else if (text.at(0) == QChar('*'))
        {
            text.remove(0, 1);
            targetTabWidget->setTabText(index, text);
        }
    }
}

void MainWindow::onRecentFileTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    Q_ASSERT(action);

    QString file = action->text();
    openFile(file);
}

void MainWindow::openFile(const QString &file)
{
    // open the file
    if (QFile::exists(file))
    {
        // check if the file has been opened already
        QFileInfo fileInfo(file);
        if (!ui->tabWidget->fileExists(fileInfo) && !ui->tabWidgetSlave->fileExists(fileInfo))
        {
            // create an edit page, open the file
            TabWidget *tabWidget = getFocusTabWidget();
            tabWidget->openFile(fileInfo.absoluteFilePath());
        }
    }
}

void MainWindow::onActivateTabClicked(int index)
{
    if (index >= ui->tabWidget->count())
    {
        ui->tabWidgetSlave->setCurrentIndex(index - ui->tabWidget->count());
    }
    else
    {
        ui->tabWidget->setCurrentIndex(index);
    }
}

void MainWindow::onCloseTabClicked(const QList<int> &fileList)
{
    for (auto i = fileList.size() - 1; i >= 0; i--)
    {
        int index = fileList.at(i);
        if (index >= ui->tabWidget->count())
        {
            index -= ui->tabWidget->count();
            ui->tabWidgetSlave->doCloseRequested(index);
        }
        else
        {
            ui->tabWidget->doCloseRequested(index);
        }
    }
}

void MainWindow::onSaveTabClicked(const QList<int> &fileList)
{
    for (auto i = fileList.size() - 1; i >= 0; i--)
    {
        int index = fileList.at(i);
        if (index >= ui->tabWidget->count())
        {
            index -= ui->tabWidget->count();
            ui->tabWidgetSlave->setCurrentIndex(index);
            ui->tabWidgetSlave->saveCurrentFile();
        }
        else
        {
            ui->tabWidget->setCurrentIndex(index);
            ui->tabWidget->saveCurrentFile();
        }
    }
}

void MainWindow::on_actionNewFile_triggered()
{
    newDocument();
}

void MainWindow::on_actionToggleFullScreenMode_triggered()
{
    static bool bMaximized = false;
    if (isFullScreen())
    {
        if (bMaximized)
        {
            showMaximized();
        }
        else
        {
            showNormal();
        }
    }
    else
    {
        bMaximized = isMaximized();
        showFullScreen();
    }
}

void MainWindow::on_actionOpenFile_triggered()
{
    QFileDialog::Options options;
    QString              selectedFilter;
    QStringList          fileNames =
        QFileDialog::getOpenFileNames(this, tr("ainesmile Open Files"), {}, tr("All Files (*);;Text Files (*.txt)"), &selectedFilter, options);
    openFiles(fileNames);
}

void MainWindow::on_actionExitApp_triggered()
{
    close();
}

void MainWindow::on_actionSaveFile_triggered()
{
    TabWidget *tabWidget = getFocusTabWidget();
    tabWidget->saveCurrentFile();
}

void MainWindow::on_actionSaveAs_triggered()
{
    TabWidget *tabWidget = getFocusTabWidget();
    tabWidget->saveAsCurrentFile();
}

void MainWindow::on_actionHelpContent_triggered()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://bitbucket.org/missdeer/ainesmile/wiki/Home")));
}

void MainWindow::on_actionAboutApp_triggered()
{
    QString date;
    QFile   fileDate(QStringLiteral(":/DATE"));
    if (fileDate.open(QIODevice::ReadOnly))
    {
        QByteArray d = fileDate.readAll();
#if defined(Q_OS_MAC)
        date = QString::fromUtf8(d);
#else
        date = QString::fromLocal8Bit(d);
#endif
        date = date.replace(QLatin1String("\n"), QLatin1String(" "));
        fileDate.close();
    }

    QString revision;
    QFile   fileRevision(QStringLiteral(":/REVISION"));
    if (fileRevision.open(QIODevice::ReadOnly))
    {
        QByteArray r = fileRevision.readAll();
#if defined(Q_OS_MAC)
        revision = QString::fromUtf8(r);
#else
        revision = QString::fromLocal8Bit(r);
#endif
        fileRevision.close();
    }

    QString text = tr("Build at %1\nRevision: %2\nKeep It Simple & Stupid!").arg(date, revision);

    QMessageBox::about(this, tr("About ainesmile"), text);
}

void MainWindow::on_actionDForDSoftwareHome_triggered()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("http://www.dfordsoft.com")));
}

void MainWindow::on_actionAinesmileProductPage_triggered()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("http://www.dfordsoft.com/ainesmile/")));
}

void MainWindow::on_actionSaveAll_triggered()
{
    ui->tabWidget->saveAll();
    ui->tabWidgetSlave->saveAll();
}

void MainWindow::on_actionCloseAll_triggered()
{
    ui->tabWidget->closeAll();
    ui->tabWidgetSlave->closeAll();
}

void MainWindow::on_actionCloseAllButActiveDocument_triggered()
{
    TabWidget *tabWidget = getFocusTabWidget();
    if (ui->tabWidget == tabWidget)
    {
        ui->tabWidget->closeAllButActive();
        ui->tabWidgetSlave->closeAll();
    }
    else
    {
        ui->tabWidgetSlave->closeAllButActive();
        ui->tabWidget->closeAll();
    }
}

void MainWindow::on_actionOpenAllRecentFiles_triggered()
{
    QStringList &files = m_recentFiles.recentFiles();

    std::for_each(files.begin(), files.end(), [this](auto &&PH1) { openFile(std::forward<decltype(PH1)>(PH1)); });
}

void MainWindow::on_actionEmptyRecentFilesList_triggered()
{
    m_recentFiles.clearFiles();

    std::for_each(m_recentFileActions.begin(), m_recentFileActions.end(), [](auto &&PH1) { PH1->setVisible(false); });
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
    ui->tabWidget->closeAll();
    ui->tabWidgetSlave->closeAll();
}

void MainWindow::on_actionClose_triggered()
{
    TabWidget *tabWidget    = getFocusTabWidget();
    int        currentIndex = tabWidget->currentIndex();
    tabWidget->doCloseRequested(currentIndex);
}

void MainWindow::on_actionShowWhiteSpaceAndTAB_triggered()
{
    bool  enabled = ui->actionShowWhiteSpaceAndTAB->isChecked();
    auto &ptree   = Config::instance()->pt();
    ptree.put("show.white_space_and_tab", enabled);

    ui->tabWidget->setShowWhiteSpaceAndTAB(enabled);
    ui->tabWidgetSlave->setShowWhiteSpaceAndTAB(enabled);
}

void MainWindow::on_actionShowEndOfLine_triggered()
{
    bool  enabled = ui->actionShowEndOfLine->isChecked();
    auto &ptree   = Config::instance()->pt();
    ptree.put("show.end_of_line", enabled);

    ui->tabWidget->setShowEndOfLine(enabled);
    ui->tabWidgetSlave->setShowEndOfLine(enabled);
}

void MainWindow::on_actionShowIndentGuide_triggered()
{
    bool  enabled = ui->actionShowIndentGuide->isChecked();
    auto &ptree   = Config::instance()->pt();
    ptree.put("show.indent_guide", enabled);

    ui->tabWidget->setShowIndentGuide(enabled);
    ui->tabWidgetSlave->setShowIndentGuide(enabled);
}

void MainWindow::on_actionShowWrapSymbol_triggered()
{
    bool                         enabled = ui->actionShowWrapSymbol->isChecked();
    boost::property_tree::ptree &pt      = Config::instance()->pt();
    pt.put("show.wrap_symbol", enabled);

    ui->tabWidget->setShowWrapSymbol(enabled);
    ui->tabWidgetSlave->setShowWrapSymbol(enabled);
}

void MainWindow::on_actionPreferences_triggered()
{
    PreferenceDialog dlg(this);
    dlg.exec();
}

void MainWindow::on_actionWindowsList_triggered()
{
    QStringList fileList;
    ui->tabWidget->getFileList(fileList);
    ui->tabWidgetSlave->getFileList(fileList);
    WindowListDialog dlg(this);
    dlg.setFileList(fileList);
    connect(&dlg, &WindowListDialog::activateTab, this, &MainWindow::onActivateTabClicked);
    connect(&dlg, &WindowListDialog::closeTab, this, &MainWindow::onCloseTabClicked);
    connect(&dlg, &WindowListDialog::saveTab, this, &MainWindow::onSaveTabClicked);
    dlg.exec();
}

void MainWindow::on_actionFind_triggered()
{
    if (!ui->dockFindReplace->isVisible())
    {
        ui->dockFindReplace->setVisible(true);
    }
}

void MainWindow::hideFeatures()
{
#if defined(Q_OS_MAC)
    ui->actionAlwaysOnTop->setVisible(false);
    ui->actionToggleFullScreenMode->setVisible(false);
#endif
}

void MainWindow::on_cbScope_currentIndexChanged(int index)
{
    if (index == 0 || index == 1)
    {
        // document
        ui->lbDirectory->setVisible(false);
        ui->edtDirectory->setVisible(false);
        ui->btnSelectDirectory->setVisible(false);
        ui->lbFilters->setVisible(false);
        ui->cbFilters->setVisible(false);
    }
    else
    {
        // directory
        ui->lbDirectory->setVisible(true);
        ui->edtDirectory->setVisible(true);
        ui->btnSelectDirectory->setVisible(true);
        ui->lbFilters->setVisible(true);
        ui->cbFilters->setVisible(true);
    }
}

void MainWindow::on_btnSelectDirectory_clicked()
{
    const QString &dir = QFileDialog::getExistingDirectory(
        this, tr("Select Directory"), QLatin1String(""), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
    {
        ui->edtDirectory->setText(dir);
    }
}

void MainWindow::on_btnFind_clicked()
{
    const QString &strToFind = ui->cbFind->lineEdit()->text();
    if (strToFind.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please input text to search."), QMessageBox::Ok);
        return;
    }
    updateFindList();

    FindReplace::FindReplaceOption fro {
        ui->cbMatchCase->isChecked(),
        ui->cbMatchWholeWord->isChecked(),
        ui->cbSearchUp->isChecked(),
        ui->cbRegexp->isChecked(),
        static_cast<FindReplace::FindScope>(ui->cbScope->currentIndex()),
        ui->cbFind->lineEdit()->text(),
        ui->cbReplace->lineEdit()->text(),
        ui->edtDirectory->text(),
        ui->cbFilters->lineEdit()->text(),
    };
    auto *tabWidget = getFocusTabWidget();
    tabWidget->find(fro);
}

void MainWindow::on_btnReplace_clicked()
{
    const QString &strToFind = ui->cbFind->lineEdit()->text();
    if (strToFind.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please input text to search."), QMessageBox::Ok);
        return;
    }
    updateFindList();
    updateReplaceList();
    updateFindReplaceFilterList();

    FindReplace::FindReplaceOption fro {
        ui->cbMatchCase->isChecked(),
        ui->cbMatchWholeWord->isChecked(),
        ui->cbSearchUp->isChecked(),
        ui->cbRegexp->isChecked(),
        static_cast<FindReplace::FindScope>(ui->cbScope->currentIndex()),
        ui->cbFind->lineEdit()->text(),
        ui->cbReplace->lineEdit()->text(),
        ui->edtDirectory->text(),
        ui->cbFilters->lineEdit()->text(),
    };
    auto *tabWidget = getFocusTabWidget();
    tabWidget->replace(fro);
}

void MainWindow::on_btnReplaceAll_clicked()
{
    const QString &strToFind = ui->cbFind->lineEdit()->text();
    if (strToFind.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please input text to search."), QMessageBox::Ok);
        return;
    }
    updateFindList();
    updateReplaceList();
    updateFindReplaceFilterList();

    FindReplace::FindReplaceOption fro {
        ui->cbMatchCase->isChecked(),
        ui->cbMatchWholeWord->isChecked(),
        ui->cbSearchUp->isChecked(),
        ui->cbRegexp->isChecked(),
        static_cast<FindReplace::FindScope>(ui->cbScope->currentIndex()),
        ui->cbFind->lineEdit()->text(),
        ui->cbReplace->lineEdit()->text(),
        ui->edtDirectory->text(),
        ui->cbFilters->lineEdit()->text(),
    };

    switch (ui->cbScope->currentIndex())
    {
    case FindReplace::FindScope::FS_DOCUMENT:
        getFocusTabWidget()->replaceAll(fro);
        break;
    case FindReplace::FindScope::FS_ALLOPENED_DOCUMENT: {
        QList<ScintillaEdit *> docs;
        ui->tabWidget->getAllEditors(docs);
        ui->tabWidgetSlave->getAllEditors(docs);
        FindReplace::replaceAllInDocuments(docs, fro);
    }
    break;
    }
}

void MainWindow::updateFindList()
{
    auto strToFind = ui->cbFind->lineEdit()->text().toStdString();
    if (!strToFind.empty())
    {
        auto &ptree = Config::instance()->pt();
        try
        {
            auto findTexts = ptree.get_child("find.texts");
            auto iter = std::find_if(findTexts.begin(), findTexts.end(), [&strToFind](const auto &val) { return val.second.data() == strToFind; });
            if (findTexts.end() != iter)
            {
                return;
            }
        }
        catch (...)
        {
        }
        ptree.add("find.texts.text", strToFind);
        ui->cbFind->addItem(QString::fromStdString(strToFind));
    }
}

void MainWindow::updateReplaceList()
{
    auto strReplace = ui->cbReplace->lineEdit()->text().toStdString();
    if (!strReplace.empty())
    {
        auto &ptree        = Config::instance()->pt();
        auto  replaceTexts = ptree.get_child("replace.texts");
        auto  iter =
            std::find_if(replaceTexts.begin(), replaceTexts.end(), [&strReplace](const auto &val) { return val.second.data() == strReplace; });
        if (replaceTexts.end() == iter)
        {
            ptree.add("replace.texts.text", strReplace);
            ui->cbReplace->addItem(QString::fromStdString(strReplace));
        }
    }
}
void MainWindow::updateFindReplaceFilterList()
{
    auto strFilters = ui->cbFilters->lineEdit()->text().toStdString();
    if (!strFilters.empty())
    {
        auto &ptree       = Config::instance()->pt();
        auto  filterTexts = ptree.get_child("filters.texts");
        auto  iter = std::find_if(filterTexts.begin(), filterTexts.end(), [&strFilters](const auto &val) { return val.second.data() == strFilters; });
        if (filterTexts.end() == iter)
        {
            ptree.add("filters.texts.text", strFilters);
            ui->cbFilters->addItem(QString::fromStdString(strFilters));
        }
    }
}

CodeEditor *MainWindow::getFocusCodeEditor()
{
    auto *tabWidget = getFocusTabWidget();
    if (!tabWidget)
    {
        return nullptr;
    }
    auto *widget = tabWidget->currentWidget();
    if (!widget)
    {
        return nullptr;
    }
    return qobject_cast<CodeEditor *>(widget);
}

void MainWindow::onSelectEncodingCustomContextMenuRequested(QPoint pos)
{
    QWidget *widget = qobject_cast<QWidget *>(sender());
    Q_ASSERT(widget);
    auto  globalPos = widget->mapToGlobal(pos);
    QMenu menu;
    menu.addAction(ui->actionReopenAs);
    menu.addAction(ui->actionSaveAsEncoding);
    menu.exec(globalPos);
}

void MainWindow::customEvent(QEvent *event)
{
    if (event->type() == AppIdleEventType)
    {
        onIdle();
        QCoreApplication::postEvent(this, new IdleEvent()); // repost the event to keep it running when idle
    }
}
