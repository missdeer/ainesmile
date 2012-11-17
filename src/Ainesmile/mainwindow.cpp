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
    ui(new Ui::MainWindow)
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
    // query all pages can close
//    if (maybeSave()) {
        event->accept();
//    } else {
//        event->ignore();
        //    }
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
            for (int index = 0; index< count; index++)
            {
                CodeEditPage* page = dynamic_cast<CodeEditPage*>(ui->tabWidget->widget(index));
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
}


void MainWindow::currentPageChanged(int index)
{
    if (index == -1)
    {
        newDocument();
    }
    else
    {
        // update action UI

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

            if (!filePath.isEmpty())
                page->saveFile(filePath);
            else
                return; // don't close this tab
        }
        else
            return;
    }

    ui->tabWidget->removeTab(index);
    page->deleteLater();
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
    qApp->quit();
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
