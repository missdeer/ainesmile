#include <QFileDialog>
#include <QMdiSubWindow>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QCloseEvent>
#include "codeeditpage.h"
#include "aboutdialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_aboutToQuit(false)
{
    ui->setupUi(this);
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentPageChanged(int)));
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeRequested(int)));

    setActionShortcuts();

    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_aboutToQuit = true;
    // query all pages can close
    int count = ui->tabWidget->count();
    for (int index = 0; index< count; index++)
    {
        closeRequested(index);
    }

    if (ui->tabWidget->count() != 0)
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
    m_aboutToQuit = false;
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
    disconnect(this, SLOT(currentDocumentChanged()));
    connect(page, SIGNAL(modifiedNotification()), this, SLOT(currentDocumentChanged()));
    disconnect(this, SLOT(copyAvailableChanged()));
    connect(page, SIGNAL(copyAvailableChanged()), this, SLOT(copyAvailableChanged()));
    disconnect(this, SLOT(pasteAvailableChanged()));
    connect(page, SIGNAL(pasteAvailableChanged()), this, SLOT(pasteAvailableChanged()));
    disconnect(this, SLOT(undoAvailableChanged()));
    connect(page, SIGNAL(undoAvailableChanged()), this, SLOT(undoAvailableChanged()));
    disconnect(this, SLOT(redoAvailableChanged()));
    connect(page, SIGNAL(redoAvailableChanged()), this, SLOT(redoAvailableChanged()));
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
}

void MainWindow::openFiles(const QStringList &files)
{
    int index = 0;
    foreach(const QString& file, files)
    {
        if (QFile::exists(file))
        {
            // check if the file has been opened already
            bool bExists = false;
            int count = ui->tabWidget->count();
            for (int i = 0; i< count; i++)
            {
                CodeEditPage* page = dynamic_cast<CodeEditPage*>(ui->tabWidget->widget(i));
                Q_ASSERT(page);
                const QString& pageFileName = page->getFilePath();
                if (!pageFileName.isEmpty())
                {
                    QFileInfo fileInfo(file);
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
                // create an edit page, open the file
                CodeEditPage* codeeditpage = new CodeEditPage(this);
                index = ui->tabWidget->addTab(codeeditpage, QIcon(),QFileInfo(file).fileName());
                connectSignals(codeeditpage);
                updateUI(codeeditpage);
                codeeditpage->openFile(file);
                ui->tabWidget->setTabToolTip(index, file);
            }
        }
    }

    if (!files.isEmpty())
        ui->tabWidget->setCurrentIndex(index);
}

void MainWindow::newDocument()
{
    static int count = 1;
    CodeEditPage* codeeditpage = new CodeEditPage(this);
    QString title = QString(tr("Untitled%1")).arg(count);
    int index = ui->tabWidget->addTab(codeeditpage, QIcon(), title);
    ui->tabWidget->setCurrentIndex(index);
    count++;
    connectSignals(codeeditpage);
    updateUI(codeeditpage);
    ui->tabWidget->setFocus();
    codeeditpage->setFocus();
}


void MainWindow::currentPageChanged(int index)
{
    if (index == -1)
    {
        if (!m_aboutToQuit)
            newDocument();
    }
    else
    {
        // update action UI
        CodeEditPage* page = dynamic_cast<CodeEditPage*>(ui->tabWidget->widget(index));
        Q_ASSERT(page);
        connectSignals(page);
    }
}

void MainWindow::closeRequested(int index)
{
    CodeEditPage* page = dynamic_cast<CodeEditPage*>(ui->tabWidget->widget(index));
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
                                                        tr("Ainesmile Save File To"),
                                                        tr(""),
                                                        tr("All Files (*);;Text Files (*.txt)"),
                                                        &selectedFilter,
                                                        options);
            }

            if (filePath.isEmpty())
                return; // don't close this tab

            page->saveFile(filePath);
        }
    }

    ui->tabWidget->removeTab(index);
    page->deleteLater();
}

void MainWindow::currentDocumentChanged()
{
    CodeEditPage* page = qobject_cast<CodeEditPage*>(sender());
    Q_ASSERT(page);
    Q_UNUSED(page);
}

void MainWindow::copyAvailableChanged()
{
    CodeEditPage* page = qobject_cast<CodeEditPage*>(sender());
    Q_ASSERT(page);
    ui->actionCopy->setEnabled(page->canCopy());
    ui->actionCut->setEnabled(page->canCut());
}

void MainWindow::pasteAvailableChanged()
{
    CodeEditPage* page = qobject_cast<CodeEditPage*>(sender());
    Q_ASSERT(page);
    ui->actionPaste->setEnabled(page->canPaste());
}

void MainWindow::undoAvailableChanged()
{
    CodeEditPage* page = qobject_cast<CodeEditPage*>(sender());
    Q_ASSERT(page);
    ui->actionUndo->setEnabled(page->canUndo());
}

void MainWindow::redoAvailableChanged()
{
    CodeEditPage* page = qobject_cast<CodeEditPage*>(sender());
    Q_ASSERT(page);
    ui->actionRedo->setEnabled(page->canRedo());
}

void MainWindow::on_actionNewFile_triggered()
{
    newDocument();
}

void MainWindow::on_actionToggleFullScreenMode_triggered()
{
    static bool isMaximized = false;
    isMaximized = this->isMaximized();
    if (this->isFullScreen()) {
        if (isMaximized)
            this->showMaximized();
        else
            this->showNormal();
    }
    else
        this->showFullScreen();
}

void MainWindow::on_actionOpenFile_triggered()
{
    QFileDialog::Options options;
    QString selectedFilter;
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                    tr("Ainesmile Open Files"),
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
    CodeEditPage* page = dynamic_cast<CodeEditPage*>(ui->tabWidget->currentWidget());
    Q_ASSERT(page);
    QString filePath = page->getFilePath();
    bool resetTabText = false;
    if (!QFile::exists(filePath))
    {
        QFileDialog::Options options;
        QString selectedFilter;
        filePath = QFileDialog::getSaveFileName(this,
                                     tr("Ainesmile Save File To"),
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

void MainWindow::on_actionSaveAs_triggered()
{
    CodeEditPage* page = dynamic_cast<CodeEditPage*>(ui->tabWidget->currentWidget());
    Q_ASSERT(page);

    QFileDialog::Options options;
    QString selectedFilter;
    QString filePath = QFileDialog::getSaveFileName(this,
                                            tr("Ainesmile Save File To"),
                                            tr(""),
                                            tr("All Files (*);;Text Files (*.txt)"),
                                            &selectedFilter,
                                            options);
    if (!filePath.isEmpty())
    {
        page->saveFile(filePath);

        int index = ui->tabWidget->currentIndex();
        ui->tabWidget->setTabText(index, QFileInfo(filePath).fileName());
        ui->tabWidget->setTabToolTip(index, filePath);
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
    AboutDialog dlg;
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

}

void MainWindow::on_actionCloseAll_triggered()
{

}

void MainWindow::on_actionCloseAllButActiveDocument_triggered()
{

}

void MainWindow::on_actionOpenAllRecentFiles_triggered()
{

}

void MainWindow::on_actionEmptyRecentFilesList_triggered()
{

}

void MainWindow::on_actionAlwaysOnTop_triggered()
{

}

void MainWindow::on_actionCloseAllDocuments_triggered()
{

}
