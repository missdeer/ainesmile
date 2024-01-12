#include "stdafx.h"
#if defined(Q_OS_WIN)
#    include "ShellContextMenu.h"
#endif
#include "tabwidget.h"

TabWidget::TabWidget(QWidget *parent) : QTabWidget(parent)
{
    tabBar()->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
}

void TabWidget::setTheOtherSide(TabWidget *tabWidget)
{
    m_theOtherSide = tabWidget;
}

void TabWidget::setRecentFiles(RecentFiles *recentFiles)
{
    m_recentFiles = recentFiles;
}

void TabWidget::setAboutToQuit(bool aboutToQuit)
{
    m_aboutToQuit = aboutToQuit;
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

            auto *contentMenu = new QMenu(this);
            auto *action       = new QAction(tr("Move to the other side"), this);
            connect(action, &QAction::triggered, this, &TabWidget::exchangeTab);
            contentMenu->addAction(action);

#if defined(Q_OS_WIN)
            auto *page = qobject_cast<CodeEditor *>(currentWidget());
            Q_ASSERT(page);
            QString filePath = page->getFilePath();
            if (!filePath.isEmpty())
            {
                CShellContextMenu scm;
                auto              pos = mapToGlobal(event->pos());
                scm.ShowContextMenu(contentMenu, this, pos, filePath.replace(QChar('/'), QChar('\\')));
                return;
            }
#endif
            contentMenu->popup(mapToGlobal(event->pos()));
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
        auto *page = qobject_cast<CodeEditor *>(currentWidget());
        Q_ASSERT(page);
        if (page->focus())
        {
            return true;
        }
    }
    return false;
}

void TabWidget::doCloseRequested(int index)
{
    auto *page = qobject_cast<CodeEditor *>(widget(index));
    Q_ASSERT(page);
    if (!page->canClose())
    {
        if (QMessageBox::question(this, tr("Confirm"), tr("This document is not saved, save it now?"), QMessageBox::Yes | QMessageBox::No) ==
            QMessageBox::Yes)
        {
            // save to file
            QString filePath = page->getFilePath();
            if (!QFile::exists(filePath))
            {
                QFileDialog::Options options;
                QString              selectedFilter;
                filePath = QFileDialog::getSaveFileName(
                    this, tr("ainesmile Save File To"), tabText(index), tr("All Files (*);;Text Files (*.txt)"), &selectedFilter, options);
            }

            if (filePath.isEmpty())
            {
                return; // don't close this tab
            }

            page->saveFile(filePath);
            if (m_recentFiles->addFile(filePath))
            {
                emit updateRecentFiles();
            }
        }
    }
    else
    {
        if (count() + m_theOtherSide->count() == 1 && !page->isModified() && !m_aboutToQuit && page->getFilePath().isEmpty())
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
    for (int i = 0; i < count(); i++)
    {
        auto *page = qobject_cast<CodeEditor *>(widget(i));
        Q_ASSERT(page);
        const QString &pageFileName = page->getFilePath();
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

void TabWidget::onOpenFilesRequest(const QStringList &files)
{
    openFiles(files);
}

int TabWidget::openFile(const QString &filePath)
{
    auto *codeeditpage = new CodeEditor(this);
    connect(codeeditpage, &CodeEditor::openFilesRequest, this, &TabWidget::onOpenFilesRequest);
    int  index = addTab(codeeditpage, QIcon(), QFileInfo(filePath).fileName());
    emit codeEditPageCreated(codeeditpage);
    codeeditpage->openFile(filePath);
    setTabToolTip(index, filePath);
    setFocus();
    codeeditpage->setFocus();
    codeeditpage->grabFocus();
    // log into recent file list
    if (m_recentFiles->addFile(filePath))
    {
        emit updateRecentFiles();
    }
    if (isHidden())
    {
        setHidden(false);
    }

    if (count() == 2)
    {
        QWidget *w    = widget(0);
        auto    *page = qobject_cast<CodeEditor *>(w);
        Q_ASSERT(page);
        if (page->initialDocument())
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
    for (const auto &file : files)
    {
        QFileInfo fileInfo(file);
        if (QFile::exists(fileInfo.absoluteFilePath()))
        {
            // check if the file has been opened already
            if (!fileExists(fileInfo) && !m_theOtherSide->fileExists(fileInfo))
            {
                // create an edit page, open the file
                index = openFile(fileInfo.absoluteFilePath());
            }
        }
    }

    if (!files.isEmpty())
    {
        setCurrentIndex(index);
    }
}

int TabWidget::newDocument(const QString &title)
{
    auto *codeeditpage = new CodeEditor(this);
    connect(codeeditpage, &CodeEditor::openFilesRequest, this, &TabWidget::onOpenFilesRequest);
    int index = addTab(codeeditpage, QIcon(), title);
    setCurrentIndex(index);
    emit codeEditPageCreated(codeeditpage);
    setFocus();
    codeeditpage->setFocus();
    codeeditpage->grabFocus();
    if (isHidden())
    {
        setHidden(false);
    }
    return index;
}

QString TabWidget::getCurrentFilePath()
{
    auto *page = qobject_cast<CodeEditor *>(currentWidget());
    Q_ASSERT(page);
    return page->getFilePath();
}

void TabWidget::saveCurrentFile()
{
    auto *page = qobject_cast<CodeEditor *>(currentWidget());
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
    auto *page = qobject_cast<CodeEditor *>(currentWidget());
    Q_ASSERT(page);

    QFileDialog::Options options;
    QString              selectedFilter;
    QString              filePath = QFileDialog::getSaveFileName(
        this, tr("ainesmile Save File To"), tabText(currentIndex()), tr("All Files (*);;Text Files (*.txt)"), &selectedFilter, options);
    if (!filePath.isEmpty())
    {
        QString originalFilePath = page->getFilePath();
        page->saveFile(filePath);

        int index = currentIndex();
        setTabText(index, QFileInfo(filePath).fileName());
        setTabToolTip(index, filePath);

        m_recentFiles->replaceFile(originalFilePath, filePath);
        emit updateRecentFiles();
    }
}

void TabWidget::saveAll()
{
    for (int index = count() - 1; index >= 0; index--)
    {
        auto *page = qobject_cast<CodeEditor *>(widget(index));
        Q_ASSERT(page);
        QString filePath     = page->getFilePath();
        bool    resetTabText = false;
        if (!QFile::exists(filePath))
        {
            QFileDialog::Options options;
            QString              selectedFilter;
            filePath = QFileDialog::getSaveFileName(
                this, tr("ainesmile Save File To"), tabText(index), tr("All Files (*);;Text Files (*.txt)"), &selectedFilter, options);
            if (!filePath.isEmpty())
            {
                resetTabText = true;
            }
        }

        if (!filePath.isEmpty())
        {
            page->saveFile(filePath);
        }

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
    for (int index = count() - 1; index >= 0; index--)
    {
        doCloseRequested(index);
    }
}

void TabWidget::closeAllButActive()
{
    for (int index = count() - 1; index > currentIndex(); index--)
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
    for (int index = count() - 1; index >= 0; index--)
    {
        auto *page = qobject_cast<CodeEditor *>(widget(index));
        Q_ASSERT(page);

        page->setShowWhiteSpaceAndTAB(enabled);
    }
}

void TabWidget::setShowEndOfLine(bool enabled)
{
    for (int index = count() - 1; index >= 0; index--)
    {
        auto *page = qobject_cast<CodeEditor *>(widget(index));
        Q_ASSERT(page);

        page->setShowEndOfLine(enabled);
    }
}

void TabWidget::setShowIndentGuide(bool enabled)
{
    for (int index = count() - 1; index >= 0; index--)
    {
        auto *page = qobject_cast<CodeEditor *>(widget(index));
        Q_ASSERT(page);

        page->setShowIndentGuide(enabled);
    }
}

void TabWidget::setShowWrapSymbol(bool enabled)
{
    for (int index = count() - 1; index >= 0; index--)
    {
        auto *page = qobject_cast<CodeEditor *>(widget(index));
        Q_ASSERT(page);

        page->setShowWrapSymbol(enabled);
    }
}

void TabWidget::getFileList(QStringList &fileList)
{
    for (int i = 0; i < count(); i++)
    {
        auto *page = qobject_cast<CodeEditor *>(widget(i));
        Q_ASSERT(page);
        if (!page->getFilePath().isEmpty())
        {
            fileList << page->getFilePath();
        }
        else
        {
            fileList << tabText(i);
        }
    }
}

int TabWidget::findTabIndex(QWidget *w)
{
    for (int i = 0; i < count(); i++)
    {
        auto *page = qobject_cast<CodeEditor *>(widget(i));
        Q_ASSERT(page);
        if (page == w)
        {
            return i;
        }
    }
    return -1;
}

void TabWidget::getAllEditors(std::vector<CodeEditor *> &docs)
{
    for (int i = 0; i < count(); i++)
    {
        auto *page = qobject_cast<CodeEditor *>(widget(i));
        Q_ASSERT(page);
        docs.push_back(page);
    }
}
