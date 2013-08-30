#include <QtCore>
#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QMessageBox>
#include <QClipboard>
#include <boost/lexical_cast.hpp>
#include "ScintillaEdit.h"
#include "gotolinedialog.h"
#include "codeeditpage.h"


CodeEditPage::CodeEditPage(QWidget *parent) :
    QWidget(parent),
    m_splitter(new QSplitter( Qt::Vertical, this)),
    m_sciControlMaster(new ScintillaEdit(m_splitter)),
    m_sciControlSlave(new ScintillaEdit(m_splitter))
  //  ,m_webView(new QWebView(m_splitter))
{
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

    m_sciControlSlave->set_doc(m_sciControlMaster->get_doc());
    //webView->load(QUrl("http://www.dfordsoft.com"));
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
    connect(m_sciControlSlave, SIGNAL(modified(int,int,int,int,QByteArray,int,int,int)),
            this, SLOT(modified(int,int,int,int,QByteArray,int,int,int)));
    connect(m_sciControlMaster, SIGNAL(linesAdded(int)), this, SLOT(linesAdded(int)));
    connect(m_sciControlSlave, SIGNAL(linesAdded(int)), this, SLOT(linesAdded(int)));
    connect(m_sciControlMaster, SIGNAL(marginClicked(int,int,int)), this, SLOT(marginClicked(int,int,int)));
    connect(m_sciControlSlave, SIGNAL(marginClicked(int,int,int)), this, SLOT(marginClicked(int,int,int)));
    connect(m_sciControlMaster, SIGNAL(dwellEnd(int,int)), this, SLOT(dwellEnd(int,int)));
    connect(m_sciControlSlave, SIGNAL(dwellEnd(int,int)), this, SLOT(dwellEnd(int,int)));
}

ScintillaEdit *CodeEditPage::getFocusView()
{
    ScintillaEdit* sci = NULL;
    if (m_sciControlMaster->focus())
        sci = m_sciControlMaster;
    else
        sci = m_sciControlSlave;
    return sci;
}

void CodeEditPage::openFile(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        m_filePath = filePath;
        m_sciControlMaster->setText(file.readAll().data());
        m_sciControlMaster->emptyUndoBuffer();;
        file.close();
        emit filePathChanged(m_filePath);
        m_sc.initEditorStyle(m_sciControlMaster, filePath);
        m_sc.initEditorStyle(m_sciControlSlave, filePath);
        m_sciControlMaster->colourise(0, -1);
        m_sciControlSlave->colourise(0, -1);
    }
}

void CodeEditPage::saveFile(const QString &filePath)
{
    QFileInfo saveFileInfo(filePath);
    QFileInfo fileInfo(m_filePath);
    if (m_filePath.isEmpty() || saveFileInfo != fileInfo)
    {
        m_filePath = filePath;
        emit filePathChanged(m_filePath);

        m_sc.initEditorStyle(m_sciControlMaster, filePath);
        m_sc.initEditorStyle(m_sciControlSlave, filePath);
    }

    if (m_sciControlMaster->modify() || m_filePath.isEmpty() || saveFileInfo != fileInfo)
    {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly))
        {
            sptr_t len = m_sciControlMaster->textLength();
            qint64 size = file.write(m_sciControlMaster->getText(len + 1));
            file.close();

            if (size != len)
            {
                QMessageBox::warning(this, tr("Saving file failed:"),
                                     tr("Not all data is saved to file."));
            }
            else
            {
                m_sciControlMaster->setSavePoint();
                m_sciControlSlave->setSavePoint();
                emit modifiedNotification();
            }
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
    ScintillaEdit* sci = getFocusView();
    return !sci->selectionEmpty();
}

bool CodeEditPage::canCopy()
{
    ScintillaEdit* sci = getFocusView();
    return !sci->selectionEmpty();
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

bool CodeEditPage::focus()
{
    return (m_sciControlMaster->focus() || m_sciControlSlave->focus());
}

void CodeEditPage::applyEditorStyles()
{
    m_sc.initEditorStyle(m_sciControlMaster, m_filePath);
    m_sc.initEditorStyle(m_sciControlSlave, m_filePath);
}

void CodeEditPage::setShowWhiteSpaceAndTAB(bool enabled)
{
    m_sciControlMaster->setViewWS(enabled ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
    m_sciControlSlave->setViewWS(enabled ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
}

void CodeEditPage::setShowEndOfLine(bool enabled)
{
    m_sciControlMaster->setViewEOL(enabled);
    m_sciControlSlave->setViewEOL(enabled);
}

void CodeEditPage::setShowIndentGuide(bool enabled)
{
    m_sciControlMaster->setIndentationGuides(enabled? SC_IV_REAL : SC_IV_NONE);
    m_sciControlSlave->setIndentationGuides(enabled? SC_IV_REAL : SC_IV_NONE);
}

void CodeEditPage::setShowWrapSymbol(bool enabled)
{
    m_sciControlMaster->setWrapVisualFlags(enabled ? SC_WRAPVISUALFLAG_END : SC_WRAPVISUALFLAG_NONE);
    m_sciControlSlave->setWrapVisualFlags(enabled ? SC_WRAPVISUALFLAG_END : SC_WRAPVISUALFLAG_NONE);
}

//void CodeEditPage::focusInEvent(QFocusEvent *event)
//{
//    qDebug() << __FUNCTION__;
//    if (event->gotFocus())
//    {
//        focusIn_ = true;
//        emit focusIn();
//    }
//    if (event->lostFocus())
//    {
//        focusIn_ = false;
//    }
//    event->ignore();
//}

void CodeEditPage::updateUI()
{
    if (!focusIn_
            && (m_sciControlMaster->focus()
            || m_sciControlSlave->focus()
            || QApplication::focusWidget() == m_sciControlMaster
            || QApplication::focusWidget() == m_sciControlSlave
            || QApplication::focusWidget() == this))
    {
        focusIn_ = true;
        emit focusIn();
    }
    else
    {
        focusIn_ = false;
    }
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

void CodeEditPage::linesAdded(int linesAdded)
{
    ScintillaEdit* sci = qobject_cast<ScintillaEdit*>(sender());
    int line_count = sci->lineCount();
    int left = sci->marginLeft() + 2;
    int right = sci->marginRight() + 2;
    std::string line = boost::lexical_cast<std::string>(line_count);
    int width = left + right + sci->textWidth(STYLE_LINENUMBER, line.c_str());
    sci->setMarginWidthN(0, width);
}

void CodeEditPage::marginClicked(int position, int modifiers, int margin)
{
    ScintillaEdit* sci = qobject_cast<ScintillaEdit*>(sender());
    if (sci->marginTypeN(margin) == SC_MARGIN_SYMBOL)
    {
        int line = sci->lineFromPosition(position);
        int maskN = sci->marginMaskN(margin);
        if (maskN == SC_MASK_FOLDERS)
        {
            int foldLevel = sci->foldLevel(line);
            if (foldLevel & SC_FOLDLEVELHEADERFLAG)
            {
                sci->toggleFold(line);
            }
        }
        else if (maskN == ~SC_MASK_FOLDERS) // breakpoint margin operations
        {
            if(sci->markerGet(line))
            {
                sci->markerDelete(line, 1);
                // remove breakpoint
            }
            else
            {
                sci->markerAdd(line, 1);
                // add breakpoint
            }
        }
    }
}

void CodeEditPage::dwellEnd(int x, int y)
{
    ScintillaEdit* sci = qobject_cast<ScintillaEdit*>(sender());
    if (sci->autoCActive())
    {
        sci->autoCCancel();
    }
}

void CodeEditPage::undo()
{
    ScintillaEdit* sci = getFocusView();
    sci->undo();
}

void CodeEditPage::redo()
{
    ScintillaEdit* sci = getFocusView();
    sci->redo();
}

void CodeEditPage::copy()
{
    ScintillaEdit* sci = getFocusView();
    sci->copy();
}

void CodeEditPage::cut()
{
    ScintillaEdit* sci = getFocusView();
    sci->cut();
}

void CodeEditPage::paste()
{
    ScintillaEdit* sci = getFocusView();
    sci->paste();
}

void CodeEditPage::print()
{

}

void CodeEditPage::printNow()
{

}

void CodeEditPage::deleteCurrent()
{
    ScintillaEdit* sci = getFocusView();
    sci->deleteBack();
}

void CodeEditPage::selectAll()
{
    ScintillaEdit* sci = getFocusView();
    sci->selectAll();
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
    ScintillaEdit* sci = getFocusView();
    sci->tab();
}

void CodeEditPage::decreaseLineIndent()
{
    ScintillaEdit* sci = getFocusView();
    sci->backTab();
}

void CodeEditPage::upperCase()
{
    ScintillaEdit* sci = getFocusView();
    sci->beginUndoAction();
    QByteArray upperText = sci->getSelText().toUpper();
    sci->replaceSel(upperText.data());
    sci->endUndoAction();
}

void CodeEditPage::lowerCase()
{
    ScintillaEdit* sci = getFocusView();
    sci->beginUndoAction();
    QByteArray lowerText = sci->getSelText().toLower();
    sci->replaceSel(lowerText.data());
    sci->endUndoAction();
}

void CodeEditPage::duplicateCurrentLine()
{
    ScintillaEdit* sci = getFocusView();
    sci->beginUndoAction();
    sci->lineDuplicate();
    sci->endUndoAction();
}

void CodeEditPage::splitLines()
{
    ScintillaEdit* sci = getFocusView();
    sci->beginUndoAction();
    sci->targetFromSelection();
    sci->linesSplit(0);
    sci->endUndoAction();
}

void CodeEditPage::joinLines()
{
    ScintillaEdit* sci = getFocusView();
    sci->beginUndoAction();
    sci->targetFromSelection();
    sci->linesJoin();
    sci->endUndoAction();
}

void CodeEditPage::moveUpCurrentLine()
{    
    ScintillaEdit* sci = getFocusView();
    sci->beginUndoAction();
    sci->lineUp();
    sci->endUndoAction();
}

void CodeEditPage::moveDownCurrentLine()
{
    ScintillaEdit* sci = getFocusView();
    sci->beginUndoAction();
    sci->lineDown();
    sci->endUndoAction();
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
    ScintillaEdit* sci = getFocusView();
    sci->beginUndoAction();
    sptr_t line = sci->lineFromPosition(sci->currentPos());
    QByteArray lineText = sci->getLine(line);
    while(lineText.at(lineText.length() - 1) == ' ' || lineText.at(lineText.length() - 1) == '\t')
        lineText.remove(lineText.length() - 1, 1);
    sci->setTargetStart(sci->positionFromLine(line));
    sci->setTargetEnd(sci->positionBefore(sci->positionFromLine(line + 1)));
    sci->replaceTarget(lineText.length(), lineText.data());
    sci->endUndoAction();
}

void CodeEditPage::trimLeadingSpace()
{
    ScintillaEdit* sci = getFocusView();
    sci->beginUndoAction();
    sptr_t line = sci->lineFromPosition(sci->currentPos());
    QByteArray lineText = sci->getLine(line);
    while(lineText.at(0) == ' ' || lineText.at(0) == '\t')
        lineText.remove(0, 1);
    sci->setTargetStart(sci->positionFromLine(line));
    sci->setTargetEnd(sci->positionBefore(sci->positionFromLine(line + 1)));
    sci->replaceTarget(lineText.length(), lineText.data());
    sci->endUndoAction();
}

void CodeEditPage::trimLeadingAndTrailingSpace()
{
    ScintillaEdit* sci = getFocusView();
    sci->beginUndoAction();
    sptr_t line = sci->lineFromPosition(sci->currentPos());
    QByteArray lineText = sci->getLine(line);
    while(lineText.at(0) == ' ' || lineText.at(0) == '\t')
        lineText.remove(0, 1);
    while(lineText.at(lineText.length() - 1) == ' ' || lineText.at(lineText.length() - 1) == '\t')
        lineText.remove(lineText.length() - 1, 1);
    sci->setTargetStart(sci->positionFromLine(line));
    sci->setTargetEnd(sci->positionBefore(sci->positionFromLine(line + 1)));
    sci->replaceTarget(lineText.length(), lineText.data());
    sci->endUndoAction();
}

void CodeEditPage::eolToSpace()
{
    ScintillaEdit* sci = getFocusView();
    sci->beginUndoAction();
    sci->setTargetStart(0);
    sci->setTargetEnd(sci->textLength());
    sci->linesJoin();
    sci->endUndoAction();
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

void CodeEditPage::incrementalSearch()
{

}

void CodeEditPage::gotoLine()
{
    GotoLineDialog dlg(this);
    if (dlg.exec())
    {
        ScintillaEdit* sci = getFocusView();
        sci->gotoLine(dlg.line);
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

