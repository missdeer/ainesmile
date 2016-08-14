#include "stdafx.h"
#include "tabwidget.h"

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent),
    contentMenu_(new QMenu(this)),
    theOtherSide_(NULL),
    aboutToQuit_(false)
{
    QAction* action = new QAction(tr("Move to the other side"), this);
    connect(action, SIGNAL(triggered()), this, SIGNAL(exchangeTab()));
    contentMenu_->addAction(action);

    tabBar()->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
}

void TabWidget::setTheOtherSide(TabWidget *tabWidget)
{
    theOtherSide_ = tabWidget;
}

void TabWidget::setRecentFiles(RecentFiles *recentFiles)
{
    rf_ = recentFiles;
}

void TabWidget::setAboutToQuit(bool aboutToQuit)
{
    aboutToQuit_ = aboutToQuit;
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
                                                        tabText(index),
                                                        tr("All Files (*);;Text Files (*.txt)"),
                                                        &selectedFilter,
                                                        options);
            }

            if (filePath.isEmpty())
                return; // don't close this tab

            page->saveFile(filePath);
            if (rf_->addFile(filePath))
                emit updateRecentFiles();
        }
    }
    else
    {
        if (count() + theOtherSide_->count() == 1
                && !page->isModified()
                && !aboutToQuit_
                && page->getFilePath().isEmpty())
        {
            // the only unmodified page, just skip this action
            return;
        }
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
    int index = addTab(codeeditpage, QIcon(), QFileInfo(filePath).fileName());
    emit codeEditPageCreated(codeeditpage);
    codeeditpage->openFile(filePath);
    setTabToolTip(index, filePath);
    setFocus();
    codeeditpage->setFocus();
    codeeditpage->grabFocus();
    // log into recent file list
    if (rf_->addFile(filePath))
        emit updateRecentFiles();
    if (isHidden())
        setHidden(false);

    if (count() == 2)
    {
        QWidget* w = widget(0);
        CodeEditPage * p = qobject_cast<CodeEditPage*>(w);
        if (p->initialDocument())
        {
            doCloseRequested(0);
            return 0;
        }
    }
    return index;
}

void TabWidget::openFiles(const QStringList &files)
{
    int index = 0;
    foreach(const QString& file, files)
    {
        QFileInfo fileInfo(file);
        if (QFile::exists(fileInfo.absoluteFilePath()))
        {
            // check if the file has been opened already
            if (!fileExists(fileInfo) && !theOtherSide_->fileExists(fileInfo))
            {
                // create an edit page, open the file
                index = openFile(fileInfo.absoluteFilePath());
            }
        }
    }

    if (!files.isEmpty())
        setCurrentIndex(index);
}

int TabWidget::newDocument(const QString& title)
{
    CodeEditPage* codeeditpage = new CodeEditPage(this);
    int index = addTab(codeeditpage, QIcon(), title);
    setCurrentIndex(index);
    emit codeEditPageCreated(codeeditpage);
    setFocus();
    codeeditpage->setFocus();
    codeeditpage->grabFocus();
    if (isHidden())
        setHidden(false);
    return index;
}

void TabWidget::saveCurrentFile()
{
    CodeEditPage* page = qobject_cast<CodeEditPage*>(currentWidget());
    Q_ASSERT(page);
    QString filePath = page->getFilePath();
    if (filePath.isEmpty())
    {
        saveAsCurrentFile();
    }
    else
    {
        page->saveFile(filePath);
    }
}

void TabWidget::saveAsCurrentFile()
{
    CodeEditPage* page = qobject_cast<CodeEditPage*>(currentWidget());
    Q_ASSERT(page);

    QFileDialog::Options options;
    QString selectedFilter;
    QString filePath = QFileDialog::getSaveFileName(this,
                                            tr("ainesmile Save File To"),
                                            tabText(currentIndex()),
                                            tr("All Files (*);;Text Files (*.txt)"),
                                            &selectedFilter,
                                            options);
    if (!filePath.isEmpty())
    {
        QString originalFilePath = page->getFilePath();
        page->saveFile(filePath);

        int index = currentIndex();
        setTabText(index, QFileInfo(filePath).fileName());
        setTabToolTip(index, filePath);

        rf_->replaceFile(originalFilePath, filePath);
        emit updateRecentFiles();
    }
}

void TabWidget::saveAll()
{
    for (int index = count() -1; index >=0; index--)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(widget(index));
        Q_ASSERT(page);
        QString filePath = page->getFilePath();
        bool resetTabText = false;
        if (!QFile::exists(filePath))
        {
            QFileDialog::Options options;
            QString selectedFilter;
            filePath = QFileDialog::getSaveFileName(this,
                                         tr("ainesmile Save File To"),
                                         tabText(index),
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
            int index = currentIndex();
            setTabText(index, QFileInfo(filePath).fileName());
            setTabToolTip(index, filePath);
        }
    }
}

void TabWidget::closeAll()
{
    for (int index = count() -1; index >=0; index--)
    {
        doCloseRequested(index);
    }
}

void TabWidget::closeAllButActive()
{
    for (int index = count() -1; index > currentIndex(); index--)
    {
        doCloseRequested(index);
    }
    while (count() > 1)
    {
        doCloseRequested(0);
    }
}

void TabWidget::setShowWhiteSpaceAndTAB(bool enabled)
{
    for (int index = count() -1; index >=0; index--)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(widget(index));
        Q_ASSERT(page);

        page->setShowWhiteSpaceAndTAB(enabled);
    }
}

void TabWidget::setShowEndOfLine(bool enabled)
{
    for (int index = count() -1; index >=0; index--)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(widget(index));
        Q_ASSERT(page);

        page->setShowEndOfLine(enabled);
    }
}

void TabWidget::setShowIndentGuide(bool enabled)
{
    for (int index = count() -1; index >=0; index--)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(widget(index));
        Q_ASSERT(page);

        page->setShowIndentGuide(enabled);
    }
}

void TabWidget::setShowWrapSymbol(bool enabled)
{
    for (int index = count() -1; index >=0; index--)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(widget(index));
        Q_ASSERT(page);

        page->setShowWrapSymbol(enabled);
    }
}

void TabWidget::getFileList(QStringList &fileList)
{
    for (int i = 0; i < count(); i++)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(widget(i));
        Q_ASSERT(page);
        if (!page->getFilePath().isEmpty())
            fileList << page->getFilePath();
        else
            fileList << tabText(i);
    }
}

int TabWidget::findTabIndex(QWidget *w)
{
    for (int i = 0; i < count(); i++)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(widget(i));
        Q_ASSERT(page);
        if (page == w)
            return i;
    }
    return -1;
}

void TabWidget::find(FindReplace::FindReplaceOption &fro)
{
    QWidget* w = currentWidget();
    if (w)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(w);
        ScintillaEdit* sci = page->getFocusView();
        FindReplace::findInDocument(sci, fro);
    }
}

void TabWidget::findInFiles(FindReplace::FindReplaceOption &fro)
{

}

void TabWidget::replace(FindReplace::FindReplaceOption &fro)
{
    QWidget* w = currentWidget();
    if (w)
    {
        CodeEditPage* page = qobject_cast<CodeEditPage*>(w);
        ScintillaEdit* sci = page->getFocusView();
        FindReplace::replaceInDocument(sci, fro);
    }
}

void TabWidget::replaceAll(FindReplace::FindReplaceOption &fro)
{

}

void TabWidget::replaceInFiles(FindReplace::FindReplaceOption &fro)
{

}
