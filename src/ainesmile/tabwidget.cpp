#include <QMenu>
#include <QTabBar>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include "tabwidget.h"

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent),
    contentMenu_(new QMenu(this))
{
    QAction* action = new QAction(tr("Move to the other side"), this);
    connect(action, SIGNAL(triggered()), this, SIGNAL(exchangeTab()));
    contentMenu_->addAction(action);

    tabBar()->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
}

bool TabWidget::focus()
{
    for (int i = 0; i < count(); i++)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(currentWidget());
        Q_ASSERT(page);
        if (page->focus())
            return true;
    }
    return false;
}

void TabWidget::doCloseRequested(int index)
{
    CodeEditPage* page = qobject_cast<CodeEditPage*>(widget(index));
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
//            if (rf_.addFile(filePath))
//                updateRecentFilesMenuItems();
        }
    }
    else
    {
//        if (ui->tabWidget->count() + ui->tabWidgetSlave->count() == 1
//                && !page->isModified()
//                && !aboutToQuit_
//                && page->getFilePath().isEmpty())
//        {
//            // the only unmodified page, just skip this action
//            return;
//        }
    }

    removeTab(index);
    page->deleteLater();
    if (count() == 0)
    {
        hide();
    }
}

bool TabWidget::fileExists(const QString &filePath)
{
    return fileExists(QFileInfo(filePath));
}

bool TabWidget::fileExists(const QFileInfo &fileInfo)
{
    for (int i = 0; i< count(); i++)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(widget(i));
        Q_ASSERT(page);
        const QString& pageFileName = page->getFilePath();
        if (!pageFileName.isEmpty())
        {
            QFileInfo pageFileInfo(pageFileName);
            if (pageFileInfo == fileInfo)
            {
                return true;
            }
        }
    }
    return false;
}

int TabWidget::openFile(const QString &filePath)
{
    CodeEditPage* codeeditpage = new CodeEditPage(this);
    connect(codeeditpage, SIGNAL(focusIn()), this, SLOT(onCodeEditPageFocusIn()));
    int index = addTab(codeeditpage, QIcon(), QFileInfo(filePath).fileName());
    connectSignals(codeeditpage);
    updateUI(codeeditpage);
    codeeditpage->openFile(filePath);
    setTabToolTip(index, filePath);
    setFocus();
    codeeditpage->setFocus();
    codeeditpage->grabFocus();
    // log into recent file list
//    if (rf_.addFile(fileInfo.absoluteFilePath()))
//        updateRecentFilesMenuItems();
    if (isHidden())
        setHidden(false);
    return index;
}

int TabWidget::newDocument(const QString& title)
{
    CodeEditPage* codeeditpage = new CodeEditPage(this);
    connect(codeeditpage, SIGNAL(focusIn()), this, SLOT(onCodeEditPageFocusIn()));
    int index = addTab(codeeditpage, QIcon(), title);
    setCurrentIndex(index);
    count++;
    connectSignals(codeeditpage);
    updateUI(codeeditpage);
    setFocus();
    codeeditpage->setFocus();
    codeeditpage->grabFocus();
    if (isHidden())
        setHidden(false);
    return index;
}

void TabWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        event->accept();
        int index = tabBar()->tabAt(event->pos());
        if (index != -1)
        {
            setCurrentIndex(index);
            contentMenu_->popup(mapToGlobal(event->pos()));
        }
    }
    else
    {
        event->ignore();
    }
}
