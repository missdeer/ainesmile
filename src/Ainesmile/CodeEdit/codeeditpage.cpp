#include <QApplication>
#include <QFile>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QMessageBox>
#include <QClipboard>
#include "ScintillaEdit.h"
#include "gotolinedialog.h"
#include "codeeditpage.h"


CodeEditPage::CodeEditPage(QWidget *parent) :
    QWidget(parent),
    m_splitter(new QSplitter( Qt::Vertical, this)),
    m_sciControlMaster(new ScintillaEdit(this)),
    m_sciControlSlave(new ScintillaEdit(this))
  //  ,m_webView(new QWebView(this))
{
    sptr_t docPtr = m_sciControlMaster->send(SCI_GETDOCPOINTER);
    m_sciControlSlave->send(SCI_SETDOCPOINTER, 0, docPtr);

    //webView->load(QUrl("http://www.dfordsoft.com"));
    m_splitter->addWidget(m_sciControlSlave);
    m_splitter->addWidget(m_sciControlMaster);
 //   m_splitter->addWidget(m_webView);

    QList<int> sizes;
    sizes << 0 << 0x7FFFF;//<< 0;
    m_splitter->setSizes(sizes);

    QVBoxLayout* m_mainLayout = new QVBoxLayout;
    Q_ASSERT(m_mainLayout);
    m_mainLayout->setMargin(0);
    m_mainLayout->addWidget(m_splitter);
    setLayout(m_mainLayout);

    init();
}

void CodeEditPage::init()
{
    m_sc.initScintilla(m_sciControlMaster);
    m_sc.initScintilla(m_sciControlSlave);

    m_lastCopyAvailable = canCopy();
    m_lastPasteAvailable = canPaste();
    m_lastUndoAvailable = canUndo();
    m_lastRedoAvailable = canRedo();

    connect(m_sciControlMaster, SIGNAL(updateUi()), this, SLOT(updateUI()));
    connect(m_sciControlSlave, SIGNAL(updateUi()), this, SLOT(updateUI()));
    connect(m_sciControlMaster, SIGNAL(modified(int,int,int,int,QByteArray,int,int,int)),
            this, SLOT(modified(int,int,int,int,QByteArray,int,int,int)));
}

void CodeEditPage::openFile(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        m_filePath = filePath;
        m_sciControlMaster->send(SCI_SETTEXT, 0, (sptr_t)file.readAll().data());
        m_sciControlMaster->send(SCI_EMPTYUNDOBUFFER, 0, 0);
        file.close();
        emit filePathChanged(m_filePath);
        m_sc.initEditorStyle(m_sciControlMaster);
        m_sc.initEditorStyle(m_sciControlSlave);
    }
}

void CodeEditPage::saveFile(const QString &filePath)
{
    QFileInfo saveFileInfo(filePath);
    QFileInfo fileInfo(m_filePath);
    if (saveFileInfo != fileInfo)
    {
        m_filePath = filePath;
        emit filePathChanged(m_filePath);
        if (saveFileInfo.suffix() != fileInfo.suffix())
        {
            m_sc.initEditorStyle(m_sciControlMaster);
            m_sc.initEditorStyle(m_sciControlSlave);
        }
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qint64 len = m_sciControlMaster->send(SCI_GETTEXTLENGTH);
        char * szText = new char[len + 1];
        m_sciControlMaster->send(SCI_GETTEXT, len + 1, (sptr_t)szText);
        m_sciControlMaster->send(SCI_SETSAVEPOINT);
        qint64 size = file.write(szText, len);
        file.close();
        delete[] szText;

        if (size != len)
        {
            QMessageBox::warning(this, tr("Saving file failed:"),
                                 tr("Not all data is saved to file."));
        }
    }
}

const QString &CodeEditPage::getFilePath() const
{
    return m_filePath;
}

bool CodeEditPage::canClose()
{
    return !m_sciControlMaster->modify();
}

bool CodeEditPage::canCut()
{
    if (m_sciControlMaster->focus())
        return !m_sciControlMaster->selectionEmpty();
    else
        return !m_sciControlSlave->selectionEmpty();
}

bool CodeEditPage::canCopy()
{
    if (m_sciControlMaster->focus())
        return !m_sciControlMaster->selectionEmpty();
    else
        return !m_sciControlSlave->selectionEmpty();
}

bool CodeEditPage::canPaste()
{
    return !!m_sciControlMaster->canPaste();
}

bool CodeEditPage::canUndo()
{
    return !!m_sciControlMaster->canUndo();
}

bool CodeEditPage::canRedo()
{
    return !!m_sciControlMaster->canRedo();
}

bool CodeEditPage::isModified()
{
    return m_sciControlMaster->modify();
}

void CodeEditPage::grabFocus()
{
    m_sciControlMaster->grabFocus();
}

void CodeEditPage::applyEditorStyles()
{
    m_sc.initEditorStyle(m_sciControlMaster);
    m_sc.initEditorStyle(m_sciControlSlave);
}

void CodeEditPage::updateUI()
{
    if (m_lastCopyAvailable != canCopy())
    {
        m_lastCopyAvailable = !m_lastCopyAvailable;
        emit copyAvailableChanged();
    }

    if (m_lastPasteAvailable != canPaste())
    {
        m_lastPasteAvailable = !m_lastPasteAvailable;
        emit pasteAvailableChanged();
    }

    if (m_lastRedoAvailable != canRedo())
    {
        m_lastRedoAvailable = !m_lastRedoAvailable;
        emit redoAvailableChanged();
    }

    if (m_lastUndoAvailable != canUndo())
    {
        m_lastUndoAvailable = !m_lastUndoAvailable;
        emit undoAvailableChanged();
    }
}

void CodeEditPage::modified(int type, int position, int length, int linesAdded, const QByteArray &text, int line, int foldNow, int foldPrev)
{
    emit modifiedNotification();
}

void CodeEditPage::undo()
{
    if (m_sciControlMaster->focus())
        m_sciControlMaster->undo();
    else
        m_sciControlSlave->undo();
}

void CodeEditPage::redo()
{
    if (m_sciControlMaster->focus())
        m_sciControlMaster->redo();
    else
        m_sciControlSlave->redo();
}

void CodeEditPage::copy()
{
    if (m_sciControlMaster->focus())
        m_sciControlMaster->copy();
    else
        m_sciControlSlave->copy();
}

void CodeEditPage::cut()
{
    if (m_sciControlMaster->focus())
        m_sciControlMaster->cut();
    else
        m_sciControlSlave->cut();
}

void CodeEditPage::paste()
{
    if (m_sciControlMaster->focus())
        m_sciControlMaster->paste();
    else
        m_sciControlSlave->paste();
}

void CodeEditPage::reloadFromDisk()
{

}

void CodeEditPage::print()
{

}

void CodeEditPage::printNow()
{

}

void CodeEditPage::deleteCurrent()
{

}

void CodeEditPage::selectAll()
{
    if (m_sciControlMaster->focus())
        m_sciControlMaster->selectAll();
    else
        m_sciControlSlave->selectAll();
}

void CodeEditPage::columnMode()
{

}

void CodeEditPage::columnEditor()
{

}

void CodeEditPage::characterPanel()
{

}

void CodeEditPage::clipboardHistory()
{

}

void CodeEditPage::setReadOnly()
{
    m_sciControlMaster->setReadOnly(true);
    m_sciControlSlave->setReadOnly(true);
}

void CodeEditPage::clearReadOnlyFlag()
{
    m_sciControlMaster->setReadOnly(false);
    m_sciControlSlave->setReadOnly(false);
}

void CodeEditPage::currentFullFilePathToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(m_filePath);
}

void CodeEditPage::currentFileNameToClipboard()
{
    QFileInfo fi(m_filePath);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(fi.fileName());
}

void CodeEditPage::currentDirectoryPathToClipboard()
{
    QFileInfo fi(m_filePath);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(fi.absolutePath());
}

void CodeEditPage::increaseLineIndent()
{

}

void CodeEditPage::decreaseLineIndent()
{

}

void CodeEditPage::upperCase()
{

}

void CodeEditPage::lowerCase()
{

}

void CodeEditPage::duplicateCurrentLine()
{

}

void CodeEditPage::splitLines()
{

}

void CodeEditPage::joinLines()
{

}

void CodeEditPage::moveUpCurrentLine()
{

}

void CodeEditPage::moveDownCurrentLine()
{

}

void CodeEditPage::toggleBlockComment()
{

}

void CodeEditPage::blockComment()
{

}

void CodeEditPage::blockUncomment()
{

}

void CodeEditPage::streamComment()
{

}

void CodeEditPage::functionCompletion()
{

}

void CodeEditPage::wordCompletion()
{

}

void CodeEditPage::functionParametersHint()
{

}

void CodeEditPage::eolWindowsFormat()
{
    m_sciControlMaster->setEOLMode(SC_EOL_CRLF);
    m_sciControlMaster->convertEOLs(SC_EOL_CRLF);
}

void CodeEditPage::eolUNIXFormat()
{
    m_sciControlMaster->setEOLMode(SC_EOL_LF);
    m_sciControlMaster->convertEOLs(SC_EOL_LF);
}

void CodeEditPage::eolMacFormat()
{
    m_sciControlMaster->setEOLMode(SC_EOL_CR);
    m_sciControlMaster->convertEOLs(SC_EOL_CR);
}

void CodeEditPage::trimTrailingSpace()
{

}

void CodeEditPage::trimLeadingSpace()
{

}

void CodeEditPage::trimLeadingAndTrailingSpace()
{

}

void CodeEditPage::eolToSpace()
{

}

void CodeEditPage::removeUnnecessaryBlankAndEOL()
{

}

void CodeEditPage::tabToSpace()
{

}

void CodeEditPage::spaceToTab()
{

}

void CodeEditPage::pasteHTMLContent()
{

}

void CodeEditPage::pasteRTFContent()
{

}

void CodeEditPage::copyBinaryContent()
{

}

void CodeEditPage::cutBinaryContent()
{

}

void CodeEditPage::pasteBinaryContent()
{

}

void CodeEditPage::find()
{

}

void CodeEditPage::findNext()
{

}

void CodeEditPage::findPrevious()
{

}

void CodeEditPage::selectAndFindNext()
{

}

void CodeEditPage::selectAndFindPrevious()
{

}

void CodeEditPage::findVolatieNext()
{

}

void CodeEditPage::findVolatiePrevious()
{

}

void CodeEditPage::replace()
{

}

void CodeEditPage::incrementalSearch()
{

}

void CodeEditPage::gotoLine()
{
    GotoLineDialog dlg(this);
    if (dlg.exec())
    {
        if (m_sciControlMaster->focus())
            m_sciControlMaster->gotoLine(dlg.line);
        else
            m_sciControlSlave->gotoLine(dlg.line);
    }
}

void CodeEditPage::gotoMatchingBrace()
{

}

void CodeEditPage::findCharactersInRange()
{

}

void CodeEditPage::toggleBookmark()
{

}

void CodeEditPage::nextBookmark()
{

}

void CodeEditPage::previousBookmark()
{

}

void CodeEditPage::clearAllBookmarks()
{

}

void CodeEditPage::cutBookmarkLines()
{

}

void CodeEditPage::copyBookmarkLines()
{

}

void CodeEditPage::pasteToReplaceBookmarkedLines()
{

}

void CodeEditPage::removeBookmarkedLines()
{

}

void CodeEditPage::removeUnbookmarkedLines()
{

}

void CodeEditPage::inverseBookmark()
{

}

void CodeEditPage::startRecording()
{

}

void CodeEditPage::abortRecording()
{

}

void CodeEditPage::saveLastRecording()
{

}

void CodeEditPage::replayLastRecording()
{

}

void CodeEditPage::wordWrap()
{
    if (m_sciControlMaster->wrapMode() == SC_WRAP_NONE)
    {
        m_sciControlMaster->setWrapMode(SC_WRAP_WORD);
        m_sciControlSlave->setWrapMode(SC_WRAP_WORD);
    }
    else
    {
        m_sciControlMaster->setWrapMode(SC_WRAP_NONE);
        m_sciControlSlave->setWrapMode(SC_WRAP_NONE);
    }
}

void CodeEditPage::focusOnAnotherView()
{
    if (m_sciControlMaster->focus())
        m_sciControlSlave->grabFocus();
    else
        m_sciControlMaster->grabFocus();
}

void CodeEditPage::hideLines()
{

}

void CodeEditPage::foldAll()
{

}

void CodeEditPage::unfoldAll()
{

}

void CodeEditPage::collapseCurrentLevel()
{

}

void CodeEditPage::uncollapseCurrentLevel()
{

}

void CodeEditPage::collapseLevel()
{

}

void CodeEditPage::uncollapseLevel()
{

}

void CodeEditPage::textDirectionRTL()
{

}

void CodeEditPage::textDirectionLTR()
{

}

void CodeEditPage::encodeInANSI()
{

}

void CodeEditPage::encodeInUTF8WithoutBOM()
{

}

void CodeEditPage::encodeInUTF8()
{
    m_sciControlMaster->setCodePage(SC_CP_UTF8);
    m_sciControlSlave->setCodePage(SC_CP_UTF8);
}

void CodeEditPage::encodeInUCS2BigEndian()
{

}

void CodeEditPage::encodeInUCS2LittleEndian()
{

}

void CodeEditPage::convertToANSI()
{

}

void CodeEditPage::convertToUTF8WithoutBOM()
{

}

void CodeEditPage::convertToUTF8()
{
}

void CodeEditPage::convertToUCS2BigEndian()
{

}

void CodeEditPage::convertToUCS2LittleEndian()
{

}

void CodeEditPage::zoomIn()
{
    m_sciControlMaster->zoomIn();
    m_sciControlSlave->zoomIn();
}

void CodeEditPage::zoomOut()
{
    m_sciControlMaster->zoomOut();
    m_sciControlSlave->zoomOut();
}

void CodeEditPage::restoreDefaultZoom()
{
    m_sciControlMaster->setZoom(1);
    m_sciControlSlave->setZoom(1);
}

