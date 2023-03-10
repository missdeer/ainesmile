#include "stdafx.h"

#include "codeeditpage.h"

CodeEditor::CodeEditor(QWidget *parent)
    : QWidget(parent),
      m_webView(new QWidget(parent)),
      m_verticalEditorSplitter(new QSplitter(Qt::Vertical, parent)),
      m_sciControlMaster(new ScintillaEdit(m_verticalEditorSplitter)),
      m_sciControlSlave(new ScintillaEdit(m_verticalEditorSplitter)),
      m_sciFocusView(m_sciControlMaster)
{
    m_verticalEditorSplitter->addWidget(m_sciControlSlave);
    m_verticalEditorSplitter->addWidget(m_sciControlMaster);

    QList<int> sizes;
    sizes << 0 << 0x7FFFF; // << 0;
    m_verticalEditorSplitter->setSizes(sizes);

    QVBoxLayout *m_mainLayout = new QVBoxLayout;
    Q_ASSERT(m_mainLayout);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    // m_mainLayout->setMargin(0);
    m_mainLayout->addWidget(m_verticalEditorSplitter);
    setLayout(m_mainLayout);

    init();

    m_sciControlSlave->set_doc(m_sciControlMaster->get_doc());
    // m_webView->load(QUrl("qrc:/rc/index.html"));

    m_sciControlMaster->setSavePoint();
    m_sciControlSlave->setSavePoint();
}

void CodeEditor::init()
{
    m_sc.initScintilla(m_sciControlMaster);
    m_sc.initScintilla(m_sciControlSlave);

    m_lastCopyAvailable  = canCopy();
    m_lastPasteAvailable = canPaste();
    m_lastUndoAvailable  = canUndo();
    m_lastRedoAvailable  = canRedo();

    connect(m_sciControlMaster, &ScintillaEdit::linesAdded, this, &CodeEditor::linesAdded);
    connect(m_sciControlMaster, &ScintillaEdit::marginClicked, this, &CodeEditor::marginClicked);
    connect(m_sciControlMaster, &ScintillaEdit::modified, this, &CodeEditor::modified);
    connect(m_sciControlMaster, &ScintillaEdit::dwellEnd, this, &CodeEditor::dwellEnd);
    connect(m_sciControlMaster, &ScintillaEdit::updateUi, this, &CodeEditor::updateUI);
    connect(m_sciControlSlave, &ScintillaEdit::linesAdded, this, &CodeEditor::linesAdded);
    connect(m_sciControlSlave, &ScintillaEdit::marginClicked, this, &CodeEditor::marginClicked);
    connect(m_sciControlSlave, &ScintillaEdit::modified, this, &CodeEditor::modified);
    connect(m_sciControlSlave, &ScintillaEdit::dwellEnd, this, &CodeEditor::dwellEnd);
    connect(m_sciControlSlave, &ScintillaEdit::updateUi, this, &CodeEditor::updateUI);
}

ScintillaEdit *CodeEditor::getFocusView()
{
    if (!m_sciControlMaster->focus() && !m_sciControlSlave->focus())
    {
        return m_sciFocusView != nullptr ? m_sciFocusView : (m_sciFocusView = m_sciControlMaster);
    }
    if (m_sciControlSlave->focus())
    {
        m_sciFocusView = m_sciControlSlave;
    }
    else
    {
        m_sciFocusView = m_sciControlMaster;
    }

    return m_sciFocusView;
}

bool CodeEditor::initialDocument()
{
    return !isModified() && m_filePath.isEmpty();
}

void CodeEditor::openFile(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly))
    {
        m_filePath = filePath;
        m_sciControlMaster->setText(file.readAll().data());
        m_sciControlMaster->emptyUndoBuffer();
        file.close();
        emit filePathChanged(m_filePath);
        m_sc.initEditorStyle(m_sciControlMaster, filePath);
        m_sc.initEditorStyle(m_sciControlSlave, filePath);
        m_sciControlMaster->colourise(0, -1);
        m_sciControlSlave->colourise(0, -1);
    }
}

void CodeEditor::saveFile(const QString &filePath)
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
            sptr_t len  = m_sciControlMaster->textLength();
            qint64 size = file.write(m_sciControlMaster->getText(len + 1));
            file.close();

            if (size != len)
            {
                QMessageBox::warning(this, tr("Saving file failed:"), tr("Not all data is saved to file."), QMessageBox::Ok);
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

const QString &CodeEditor::getFilePath() const
{
    return m_filePath;
}

bool CodeEditor::canClose()
{
    return !m_sciControlMaster->modify();
}

bool CodeEditor::canCut()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    return !sci->selectionEmpty();
}

bool CodeEditor::canCopy()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    return !sci->selectionEmpty();
}

bool CodeEditor::canPaste()
{
    return !!m_sciControlMaster->canPaste();
}

bool CodeEditor::canUndo()
{
    return !!m_sciControlMaster->canUndo();
}

bool CodeEditor::canRedo()
{
    return !!m_sciControlMaster->canRedo();
}

bool CodeEditor::isModified()
{
    return m_sciControlMaster->modify();
}

void CodeEditor::grabFocus()
{
    m_sciControlMaster->grabFocus();
}

bool CodeEditor::focus()
{
    return (m_sciControlMaster->focus() || m_sciControlSlave->focus());
}

void CodeEditor::applyEditorStyles()
{
    m_sc.initEditorStyle(m_sciControlMaster, m_filePath);
    m_sc.initEditorStyle(m_sciControlSlave, m_filePath);
}

void CodeEditor::setShowWhiteSpaceAndTAB(bool enabled)
{
    m_sciControlMaster->setViewWS(enabled ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
    m_sciControlSlave->setViewWS(enabled ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
}

void CodeEditor::setShowEndOfLine(bool enabled)
{
    m_sciControlMaster->setViewEOL(enabled);
    m_sciControlSlave->setViewEOL(enabled);
}

void CodeEditor::setShowIndentGuide(bool enabled)
{
    m_sciControlMaster->setIndentationGuides(enabled ? SC_IV_REAL : SC_IV_NONE);
    m_sciControlSlave->setIndentationGuides(enabled ? SC_IV_REAL : SC_IV_NONE);
}

void CodeEditor::setShowWrapSymbol(bool enabled)
{
    m_sciControlMaster->setWrapVisualFlags(enabled ? SC_WRAPVISUALFLAG_END : SC_WRAPVISUALFLAG_NONE);
    m_sciControlSlave->setWrapVisualFlags(enabled ? SC_WRAPVISUALFLAG_END : SC_WRAPVISUALFLAG_NONE);
}

// void CodeEditPage::focusInEvent(QFocusEvent *event)
//{
//     qDebug() << __FUNCTION__;
//     if (event->gotFocus())
//     {
//         focusIn_ = true;
//         emit focusIn();
//     }
//     if (event->lostFocus())
//     {
//         focusIn_ = false;
//     }
//     event->ignore();
// }

void CodeEditor::updateUI(Scintilla::Update /*updated*/)
{
    if (!m_isFocusIn && (m_sciControlMaster->focus() || m_sciControlSlave->focus() || QApplication::focusWidget() == m_sciControlMaster ||
                         QApplication::focusWidget() == m_sciControlSlave || QApplication::focusWidget() == this))
    {
        m_isFocusIn = true;
        emit focusIn();
    }
    else
    {
        m_isFocusIn = false;
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

void CodeEditor::modified(Scintilla::ModificationFlags /*type*/,
                          Scintilla::Position /*position*/,
                          Scintilla::Position /*length*/,
                          Scintilla::Position /*linesAdded*/,
                          const QByteArray & /*text*/,
                          Scintilla::Position /*line*/,
                          Scintilla::FoldLevel /*foldNow*/,
                          Scintilla::FoldLevel /*foldPrev*/)
{
    emit modifiedNotification();
}

void CodeEditor::linesAdded(Scintilla::Position /*linesAdded*/)
{
    auto  *sci        = qobject_cast<ScintillaEdit *>(sender());
    sptr_t line_count = sci->lineCount();
    sptr_t left       = sci->marginLeft() + 2;
    sptr_t right      = sci->marginRight() + 2;
    sptr_t width      = left + right + sci->textWidth(STYLE_LINENUMBER, QString("%1").arg(line_count).toStdString().c_str());
    if (width > sci->marginWidthN(0))
    {
        sci->setMarginWidthN(0, width);
    }
}

void CodeEditor::marginClicked(Scintilla::Position position, Scintilla::KeyMod /*modifiers*/, int margin)
{
    auto *sci = qobject_cast<ScintillaEdit *>(sender());
    if (sci->marginTypeN(margin) == SC_MARGIN_SYMBOL)
    {
        sptr_t line  = sci->lineFromPosition(position);
        sptr_t maskN = sci->marginMaskN(margin);
        qDebug() << "sci->marginTypeN(margin) == SC_MARGIN_SYMBOL" << margin << maskN;
        if ((maskN & 0xFFFFFFFF) == SC_MASK_FOLDERS)
        {
            qDebug() << "maskN == SC_MASK_FOLDERS";
            sptr_t foldLevel = sci->foldLevel(line);
            if (foldLevel & SC_FOLDLEVELHEADERFLAG)
            {
                qDebug() << "sci->toggleFold" << line;
                sci->toggleFold(line);
            }
        }
        else if (maskN == ~SC_MASK_FOLDERS) // breakpoint margin operations
        {
            if (sci->markerGet(line))
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

void CodeEditor::dwellEnd(int /*x*/, int /*y*/)
{
    auto *sci = qobject_cast<ScintillaEdit *>(sender());
    if (sci->autoCActive())
    {
        sci->autoCCancel();
    }
}

void CodeEditor::undo()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->undo();
}

void CodeEditor::redo()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->redo();
}

void CodeEditor::copy()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->copy();
}

void CodeEditor::cut()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->cut();
}

void CodeEditor::paste()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->paste();
}

void CodeEditor::print() {}

void CodeEditor::printNow() {}

void CodeEditor::deleteCurrent()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->deleteBack();
}

void CodeEditor::selectAll()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->selectAll();
}

void CodeEditor::setReadOnly()
{
    m_sciControlMaster->setReadOnly(true);
    m_sciControlSlave->setReadOnly(true);
}

void CodeEditor::clearReadOnlyFlag()
{
    m_sciControlMaster->setReadOnly(false);
    m_sciControlSlave->setReadOnly(false);
}

void CodeEditor::currentFullFilePathToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(m_filePath);
}

void CodeEditor::currentFileNameToClipboard()
{
    QFileInfo   fi(m_filePath);
    QClipboard *clipboard = QApplication::clipboard();
    Q_ASSERT(clipboard);
    clipboard->setText(fi.fileName());
}

void CodeEditor::currentDirectoryPathToClipboard()
{
    QFileInfo   fi(m_filePath);
    QClipboard *clipboard = QApplication::clipboard();
    Q_ASSERT(clipboard);
    clipboard->setText(fi.absolutePath());
}

void CodeEditor::increaseLineIndent()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->tab();
}

void CodeEditor::decreaseLineIndent()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->backTab();
}

void CodeEditor::upperCase()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    QByteArray upperText = sci->getSelText().toUpper();
    sci->replaceSel(upperText.data());
    sci->endUndoAction();
}

void CodeEditor::lowerCase()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    QByteArray lowerText = sci->getSelText().toLower();
    sci->replaceSel(lowerText.data());
    sci->endUndoAction();
}

void CodeEditor::duplicateCurrentLine()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    sci->lineDuplicate();
    sci->endUndoAction();
}

void CodeEditor::splitLines()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    sci->targetFromSelection();
    sci->linesSplit(0);
    sci->endUndoAction();
}

void CodeEditor::joinLines()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    sci->targetFromSelection();
    sci->linesJoin();
    sci->endUndoAction();
}

void CodeEditor::moveUpCurrentLine()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    sci->lineUp();
    sci->endUndoAction();
}

void CodeEditor::moveDownCurrentLine()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    sci->lineDown();
    sci->endUndoAction();
}

void CodeEditor::eolWindowsFormat()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->setEOLMode(SC_EOL_CRLF);
    sci->convertEOLs(SC_EOL_CRLF);
}

void CodeEditor::eolUNIXFormat()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->setEOLMode(SC_EOL_LF);
    sci->convertEOLs(SC_EOL_LF);
}

void CodeEditor::eolMacFormat()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->setEOLMode(SC_EOL_CR);
    sci->convertEOLs(SC_EOL_CR);
}

void CodeEditor::trimTrailingSpace()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    sptr_t     line     = sci->lineFromPosition(sci->currentPos());
    QByteArray lineText = sci->getLine(line);
    while (lineText.at(lineText.length() - 1) == ' ' || lineText.at(lineText.length() - 1) == '\t')
    {
        lineText.remove(lineText.length() - 1, 1);
    }
    sci->setTargetStart(sci->positionFromLine(line));
    sci->setTargetEnd(sci->positionBefore(sci->positionFromLine(line + 1)));
    sci->replaceTarget(lineText.length(), lineText.data());
    sci->endUndoAction();
}

void CodeEditor::trimLeadingSpace()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    sptr_t     line     = sci->lineFromPosition(sci->currentPos());
    QByteArray lineText = sci->getLine(line);
    while (lineText.at(0) == ' ' || lineText.at(0) == '\t')
    {
        lineText.remove(0, 1);
    }
    sci->setTargetStart(sci->positionFromLine(line));
    sci->setTargetEnd(sci->positionBefore(sci->positionFromLine(line + 1)));
    sci->replaceTarget(lineText.length(), lineText.data());
    sci->endUndoAction();
}

void CodeEditor::trimLeadingAndTrailingSpace()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    sptr_t     line     = sci->lineFromPosition(sci->currentPos());
    QByteArray lineText = sci->getLine(line);
    while (lineText.at(0) == ' ' || lineText.at(0) == '\t')
    {
        lineText.remove(0, 1);
    }
    while (lineText.at(lineText.length() - 1) == ' ' || lineText.at(lineText.length() - 1) == '\t')
    {
        lineText.remove(lineText.length() - 1, 1);
    }
    sci->setTargetStart(sci->positionFromLine(line));
    sci->setTargetEnd(sci->positionBefore(sci->positionFromLine(line + 1)));
    sci->replaceTarget(lineText.length(), lineText.data());
    sci->endUndoAction();
}

void CodeEditor::eolToSpace()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    sci->setTargetStart(0);
    sci->setTargetEnd(sci->textLength());
    sci->linesJoin();
    sci->endUndoAction();
}

void CodeEditor::gotoLine()
{
    bool  ok  = false;
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sptr_t lineCount = sci->lineCount();
    int    line      = QInputDialog::getInt(this, tr("Goto line"), tr("Input line number:(1 - %2)").arg(lineCount), 1, 1, lineCount, 1, &ok);
    if (ok)
    {
        sci->gotoLine(line - 1);
    }
}

void CodeEditor::gotoMatchingBrace() {}

void CodeEditor::toggleBookmark() {}

void CodeEditor::nextBookmark() {}

void CodeEditor::previousBookmark() {}

void CodeEditor::clearAllBookmarks() {}

void CodeEditor::cutBookmarkLines() {}

void CodeEditor::copyBookmarkLines() {}

void CodeEditor::pasteToReplaceBookmarkedLines() {}

void CodeEditor::removeBookmarkedLines() {}

void CodeEditor::removeUnbookmarkedLines() {}

void CodeEditor::inverseBookmark() {}

void CodeEditor::wordWrap()
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

void CodeEditor::focusOnAnotherView()
{
    if (m_sciControlMaster->focus())
    {
        m_sciControlSlave->grabFocus();
    }
    else
    {
        m_sciControlMaster->grabFocus();
    }
}

void CodeEditor::encodeInANSI() {}

void CodeEditor::encodeInUTF8WithoutBOM()
{
    m_sciControlMaster->setCodePage(SC_CP_UTF8);
    m_sciControlSlave->setCodePage(SC_CP_UTF8);
}

void CodeEditor::encodeInUTF8()
{
    m_sciControlMaster->setCodePage(SC_CP_UTF8);
    m_sciControlSlave->setCodePage(SC_CP_UTF8);
}

void CodeEditor::encodeInUCS2BigEndian() {}

void CodeEditor::encodeInUCS2LittleEndian() {}

void CodeEditor::convertToANSI() {}

void CodeEditor::convertToUTF8WithoutBOM() {}

void CodeEditor::convertToUTF8() {}

void CodeEditor::convertToUCS2BigEndian() {}

void CodeEditor::convertToUCS2LittleEndian() {}

void CodeEditor::zoomIn()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->zoomIn();
}

void CodeEditor::zoomOut()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->zoomOut();
}

void CodeEditor::restoreDefaultZoom()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->setZoom(1);
}

bool CodeEditor::getShowWhiteSpaceAndTAB()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    return sci->viewWS() == SCWS_VISIBLEALWAYS;
}

bool CodeEditor::getShowEndOfLine()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    return sci->viewEOL();
}

bool CodeEditor::getShowIndentGuide()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    return sci->indentationGuides() == SC_IV_REAL;
}

bool CodeEditor::getShowWrapSymbol()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    return sci->wrapVisualFlags() == SC_WRAPVISUALFLAG_END;
}
