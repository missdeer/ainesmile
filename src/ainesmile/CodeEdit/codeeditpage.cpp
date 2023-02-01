#include "stdafx.h"

#include "codeeditpage.h"

CodeEditPage::CodeEditPage(QWidget *parent)
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

void CodeEditPage::init()
{
    m_sc.initScintilla(m_sciControlMaster);
    m_sc.initScintilla(m_sciControlSlave);

    m_lastCopyAvailable  = canCopy();
    m_lastPasteAvailable = canPaste();
    m_lastUndoAvailable  = canUndo();
    m_lastRedoAvailable  = canRedo();

    connect(m_sciControlMaster, &ScintillaEdit::linesAdded, this, &CodeEditPage::linesAdded);
    connect(m_sciControlMaster, &ScintillaEdit::marginClicked, this, &CodeEditPage::marginClicked);
    connect(m_sciControlMaster, &ScintillaEdit::modified, this, &CodeEditPage::modified);
    connect(m_sciControlMaster, &ScintillaEdit::dwellEnd, this, &CodeEditPage::dwellEnd);
    connect(m_sciControlMaster, &ScintillaEdit::updateUi, this, &CodeEditPage::updateUI);
    connect(m_sciControlSlave, &ScintillaEdit::linesAdded, this, &CodeEditPage::linesAdded);
    connect(m_sciControlSlave, &ScintillaEdit::marginClicked, this, &CodeEditPage::marginClicked);
    connect(m_sciControlSlave, &ScintillaEdit::modified, this, &CodeEditPage::modified);
    connect(m_sciControlSlave, &ScintillaEdit::dwellEnd, this, &CodeEditPage::dwellEnd);
    connect(m_sciControlSlave, &ScintillaEdit::updateUi, this, &CodeEditPage::updateUI);
}

ScintillaEdit *CodeEditPage::getFocusView()
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

bool CodeEditPage::initialDocument()
{
    return !isModified() && m_filePath.isEmpty();
}

void CodeEditPage::openFile(const QString &filePath)
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
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    return !sci->selectionEmpty();
}

bool CodeEditPage::canCopy()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
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
    m_sciControlMaster->setIndentationGuides(enabled ? SC_IV_REAL : SC_IV_NONE);
    m_sciControlSlave->setIndentationGuides(enabled ? SC_IV_REAL : SC_IV_NONE);
}

void CodeEditPage::setShowWrapSymbol(bool enabled)
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

void CodeEditPage::updateUI(Scintilla::Update /*updated*/)
{
    if (!focusIn_ && (m_sciControlMaster->focus() || m_sciControlSlave->focus() || QApplication::focusWidget() == m_sciControlMaster ||
                      QApplication::focusWidget() == m_sciControlSlave || QApplication::focusWidget() == this))
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

void CodeEditPage::modified(Scintilla::ModificationFlags /*type*/,
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

void CodeEditPage::linesAdded(Scintilla::Position /*linesAdded*/)
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

void CodeEditPage::marginClicked(Scintilla::Position position, Scintilla::KeyMod /*modifiers*/, int margin)
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

void CodeEditPage::dwellEnd(int /*x*/, int /*y*/)
{
    auto *sci = qobject_cast<ScintillaEdit *>(sender());
    if (sci->autoCActive())
    {
        sci->autoCCancel();
    }
}

void CodeEditPage::undo()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->undo();
}

void CodeEditPage::redo()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->redo();
}

void CodeEditPage::copy()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->copy();
}

void CodeEditPage::cut()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->cut();
}

void CodeEditPage::paste()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->paste();
}

void CodeEditPage::print() {}

void CodeEditPage::printNow() {}

void CodeEditPage::deleteCurrent()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->deleteBack();
}

void CodeEditPage::selectAll()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->selectAll();
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
    QFileInfo   fi(m_filePath);
    QClipboard *clipboard = QApplication::clipboard();
    Q_ASSERT(clipboard);
    clipboard->setText(fi.fileName());
}

void CodeEditPage::currentDirectoryPathToClipboard()
{
    QFileInfo   fi(m_filePath);
    QClipboard *clipboard = QApplication::clipboard();
    Q_ASSERT(clipboard);
    clipboard->setText(fi.absolutePath());
}

void CodeEditPage::increaseLineIndent()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->tab();
}

void CodeEditPage::decreaseLineIndent()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->backTab();
}

void CodeEditPage::upperCase()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    QByteArray upperText = sci->getSelText().toUpper();
    sci->replaceSel(upperText.data());
    sci->endUndoAction();
}

void CodeEditPage::lowerCase()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    QByteArray lowerText = sci->getSelText().toLower();
    sci->replaceSel(lowerText.data());
    sci->endUndoAction();
}

void CodeEditPage::duplicateCurrentLine()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    sci->lineDuplicate();
    sci->endUndoAction();
}

void CodeEditPage::splitLines()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    sci->targetFromSelection();
    sci->linesSplit(0);
    sci->endUndoAction();
}

void CodeEditPage::joinLines()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    sci->targetFromSelection();
    sci->linesJoin();
    sci->endUndoAction();
}

void CodeEditPage::moveUpCurrentLine()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    sci->lineUp();
    sci->endUndoAction();
}

void CodeEditPage::moveDownCurrentLine()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    sci->lineDown();
    sci->endUndoAction();
}

void CodeEditPage::eolWindowsFormat()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->setEOLMode(SC_EOL_CRLF);
    sci->convertEOLs(SC_EOL_CRLF);
}

void CodeEditPage::eolUNIXFormat()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->setEOLMode(SC_EOL_LF);
    sci->convertEOLs(SC_EOL_LF);
}

void CodeEditPage::eolMacFormat()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->setEOLMode(SC_EOL_CR);
    sci->convertEOLs(SC_EOL_CR);
}

void CodeEditPage::trimTrailingSpace()
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

void CodeEditPage::trimLeadingSpace()
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

void CodeEditPage::trimLeadingAndTrailingSpace()
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

void CodeEditPage::eolToSpace()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    sci->setTargetStart(0);
    sci->setTargetEnd(sci->textLength());
    sci->linesJoin();
    sci->endUndoAction();
}

void CodeEditPage::gotoLine()
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

void CodeEditPage::gotoMatchingBrace() {}

void CodeEditPage::toggleBookmark() {}

void CodeEditPage::nextBookmark() {}

void CodeEditPage::previousBookmark() {}

void CodeEditPage::clearAllBookmarks() {}

void CodeEditPage::cutBookmarkLines() {}

void CodeEditPage::copyBookmarkLines() {}

void CodeEditPage::pasteToReplaceBookmarkedLines() {}

void CodeEditPage::removeBookmarkedLines() {}

void CodeEditPage::removeUnbookmarkedLines() {}

void CodeEditPage::inverseBookmark() {}

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
    {
        m_sciControlSlave->grabFocus();
    }
    else
    {
        m_sciControlMaster->grabFocus();
    }
}

void CodeEditPage::encodeInANSI() {}

void CodeEditPage::encodeInUTF8WithoutBOM()
{
    m_sciControlMaster->setCodePage(SC_CP_UTF8);
    m_sciControlSlave->setCodePage(SC_CP_UTF8);
}

void CodeEditPage::encodeInUTF8()
{
    m_sciControlMaster->setCodePage(SC_CP_UTF8);
    m_sciControlSlave->setCodePage(SC_CP_UTF8);
}

void CodeEditPage::encodeInUCS2BigEndian() {}

void CodeEditPage::encodeInUCS2LittleEndian() {}

void CodeEditPage::convertToANSI() {}

void CodeEditPage::convertToUTF8WithoutBOM() {}

void CodeEditPage::convertToUTF8() {}

void CodeEditPage::convertToUCS2BigEndian() {}

void CodeEditPage::convertToUCS2LittleEndian() {}

void CodeEditPage::zoomIn()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->zoomIn();
}

void CodeEditPage::zoomOut()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->zoomOut();
}

void CodeEditPage::restoreDefaultZoom()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->setZoom(1);
}

bool CodeEditPage::getShowWhiteSpaceAndTAB()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    return sci->viewWS() == SCWS_VISIBLEALWAYS;
}

bool CodeEditPage::getShowEndOfLine()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    return sci->viewEOL();
}

bool CodeEditPage::getShowIndentGuide()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    return sci->indentationGuides() == SC_IV_REAL;
}

bool CodeEditPage::getShowWrapSymbol()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    return sci->wrapVisualFlags() == SC_WRAPVISUALFLAG_END;
}
