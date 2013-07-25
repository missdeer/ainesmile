#include <QApplication>
#include <QFile>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QMessageBox>
#include <QClipboard>
#include "SciLexer.h"
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

    initScintilla(m_sciControlMaster);
    initScintilla(m_sciControlSlave);
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
        initEditorStyle(m_sciControlMaster);
        initEditorStyle(m_sciControlSlave);
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
            initEditorStyle(m_sciControlMaster);
            initEditorStyle(m_sciControlSlave);
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
    initEditorStyle(m_sciControlMaster);
    initEditorStyle(m_sciControlSlave);
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

void CodeEditPage::initScintilla(ScintillaEdit* sci)
{
    sci->styleResetDefault();
    sci->styleClearAll();
    sci->clearDocumentStyle();
    sci->setHScrollBar(true);
    sci->setVScrollBar(true);
    sci->setXOffset(0);
    sci->setScrollWidth(1);
    sci->setScrollWidthTracking(true);
    sci->setEndAtLastLine(true);
    sci->setWhitespaceFore(true, 0x808080);
    sci->setWhitespaceBack(true, 0xFFFFFF);
    sci->setMouseDownCaptures(true);
    sci->setEOLMode(SC_EOL_LF);
    sci->setViewEOL(false);
    sci->setStyleBits(5);
    sci->setCaretFore(0x0000FF);
    sci->setCaretLineVisible(true);
    sci->setCaretLineBack(0xFFFFD0);
    sci->setCaretLineBackAlpha(256);
    sci->setCaretPeriod(500);
    sci->setCaretWidth(2);
    sci->setCaretSticky(0);
    sci->setSelFore(true, 0xFFFFFF);
    sci->setSelBack(true, 0xC56A31);
    sci->setSelAlpha(256);
    sci->setSelEOLFilled(true);
    sci->setAdditionalSelectionTyping(true);
    sci->setVirtualSpaceOptions(SCVS_RECTANGULARSELECTION);
    sci->setHotspotActiveFore(true, 0x0000FF);
    sci->setHotspotActiveBack(true, 0xFFFFFF);
    sci->setHotspotActiveUnderline(true);
    sci->setHotspotSingleLine(false);
    sci->setControlCharSymbol(0);
    sci->setMarginLeft(3);
    sci->setMarginRight(3);
    sci->setMarginTypeN(0, SC_MARGIN_NUMBER);
    sci->setMarginWidthN(0, 24);
    sci->setMarginMaskN(0, 0);
    sci->setMarginSensitiveN(0, false);
    sci->setMarginTypeN(1, SC_MARGIN_SYMBOL);
    sci->setMarginWidthN(1, 14);
    sci->setMarginMaskN(1, 33554431);
    sci->setMarginSensitiveN(1, true);
    sci->setMarginTypeN(2, SC_MARGIN_SYMBOL);
    sci->setMarginWidthN(2, 14);
    sci->setMarginMaskN(2, SC_MASK_FOLDERS);// -33554432)
    sci->setMarginSensitiveN(2, true);

    sci->setFoldMarginColour(true, 0xCDCDCD);
    sci->setFoldMarginHiColour(true, 0xFFFFFF);

    sci->setTabWidth(4);
    sci->setUseTabs(false);
    sci->setIndent(4);
    sci->setTabIndents(false);
    sci->setBackSpaceUnIndents(false);
    sci->setIndentationGuides(0);
    sci->setHighlightGuide(1);
    sci->setPrintMagnification(1);
    sci->setPrintColourMode(0);
    sci->setPrintWrapMode(1);
    
    initFolderStyle( sci );

    sci->setWrapMode(SC_WRAP_NONE);
    sci->setWrapVisualFlags(SC_WRAPVISUALFLAG_NONE);
    sci->setWrapVisualFlagsLocation(SC_WRAPVISUALFLAGLOC_DEFAULT);
    sci->setWrapStartIndent(0);

    sci->setLayoutCache(2);
    sci->linesSplit(0);
    sci->setEdgeMode(0);
    sci->setEdgeColumn(200);
    sci->setEdgeColour(0xC0DCC0);

    sci->usePopUp(false);
    //sci->send(SCI_SETUSEPALETTE, 1, 0);
    sci->setBufferedDraw(true);
    sci->setTwoPhaseDraw(true);
    sci->setCodePage(SC_CP_UTF8);
    sci->setWordChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
    sci->setZoom(1);
    sci->setWhitespaceChars(NULL);
    sci->setMouseDwellTime(2500);

    sci->setSavePoint();
    sci->setFontQuality(SC_EFF_QUALITY_LCD_OPTIMIZED);

    //sci:SetEncoding( cfg:GetString("config/encoding") )

    m_lastCopyAvailable = canCopy();
    m_lastPasteAvailable = canPaste();
    m_lastUndoAvailable = canUndo();
    m_lastRedoAvailable = canRedo();

    connect(m_sciControlMaster, SIGNAL(updateUi()), this, SLOT(updateUI()));
    connect(m_sciControlSlave, SIGNAL(updateUi()), this, SLOT(updateUI()));
    connect(m_sciControlMaster, SIGNAL(modified(int,int,int,int,QByteArray,int,int,int)),
            this, SLOT(modified(int,int,int,int,QByteArray,int,int,int)));
}

void CodeEditPage::initFolderStyle(ScintillaEdit *sci)
{
    sci->setFoldFlags(SC_FOLDFLAG_LINEAFTER_CONTRACTED);

    sci->markerDefine(SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS);
    sci->markerDefine(SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS);
    sci->markerDefine(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
    sci->markerDefine(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER);
    sci->markerDefine(SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED);
    sci->markerDefine(SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED);
    sci->markerDefine(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER);

    sci->markerSetFore(SC_MARKNUM_FOLDEROPEN, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDEROPEN, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDER, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDER, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDERSUB, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDERSUB, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDERTAIL, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDERTAIL, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDEREND, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDEREND, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDEROPENMID, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDEROPENMID, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDERMIDTAIL, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDERMIDTAIL, 0x808080);
    sci->setProperty( "fold", "1");
    sci->setProperty( "fold.compact", "1");
    sci->setProperty( "fold.at.else", "1");
    sci->setProperty( "fold.preprocessor", "1");
    sci->setProperty( "fold.view", "1");
    sci->setProperty( "fold.comment", "1");
    sci->setProperty( "fold.html", "1");
    sci->setProperty( "fold.comment.python", "1");
    sci->setProperty( "fold.quotes.python", "1");
}

void CodeEditPage::initEditorStyle(ScintillaEdit *sci)
{
}

