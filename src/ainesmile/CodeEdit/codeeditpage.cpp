﻿#include "stdafx.h"

#include <QPainter>

#include "codeeditpage.h"
#include "config.h"
#include "scintillaconfig.h"
#include "textutils.h"


CodeEditor::CodeEditor(QWidget *parent) : QWidget(parent), m_sciControlMaster(new ScintillaEdit(m_editorSplitter)), m_sciFocusView(m_sciControlMaster)
{
    auto *m_mainLayout = new QVBoxLayout;
    Q_ASSERT(m_mainLayout);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    // m_mainLayout->setMargin(0);
    m_mainLayout->addWidget(m_sciControlMaster);
    setLayout(m_mainLayout);
    m_sciFocusView = m_sciControlMaster;

    init();

    m_sciControlMaster->setSavePoint();
}

void CodeEditor::init()
{
    initSci(m_sciControlMaster);
}

void CodeEditor::initSci(ScintillaEdit *sci)
{
    ScintillaConfig::initScintilla(sci);

    connect(sci, &ScintillaEdit::linesAdded, this, &CodeEditor::linesAdded);
    connect(sci, &ScintillaEdit::marginClicked, this, &CodeEditor::marginClicked);
    connect(sci, &ScintillaEdit::modified, this, &CodeEditor::modified);
    connect(sci, &ScintillaEdit::dwellEnd, this, &CodeEditor::dwellEnd);
    connect(sci, &ScintillaEdit::updateUi, this, &CodeEditor::updateUI);
    connect(sci, &ScintillaEdit::uriDropped, this, &CodeEditor::uriDropped);
    connect(sci, &ScintillaEdit::focusChanged, this, &CodeEditor::focusChanged);
}

ScintillaEdit *CodeEditor::getFocusView()
{
    return m_sciFocusView;
}

bool CodeEditor::initialDocument()
{
    return !isModified() && m_document.filePath().isEmpty();
}

bool CodeEditor::openFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file %1.").arg(QDir::toNativeSeparators(filePath)), QMessageBox::Ok);
        return false;
    }

    if (!filePath.isEmpty())
    {
        Config *config = Config::instance();
        Q_ASSERT(config);
        m_lexerName = config->matchPatternLanguage(filePath);
    }
    m_document.setFilePath(filePath);
    m_document.setForceEncoding(false);
    loadDataFromFile();
    return true;
}

void CodeEditor::loadDataFromFile()
{
    m_sciControlMaster->setReadOnly(false);
    m_sciControlMaster->cancel();
    m_sciControlMaster->setUndoCollection(false);
    m_sciControlMaster->emptyUndoBuffer();
    m_sciControlMaster->clearAll();
    m_sciControlMaster->setXOffset(0);
    m_sciControlMaster->setModEventMask(SC_MOD_NONE);
    bool ok = m_document.loadFromFile([this](qint64 size, const char *data) { m_sciControlMaster->appendText(size, data); });
    m_sciControlMaster->setModEventMask(SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT);
    m_sciControlMaster->setUndoCollection(true);
    m_sciControlMaster->emptyUndoBuffer();
    m_sciControlMaster->setSavePoint();
    if (!ok)
    {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file %1.").arg(QDir::toNativeSeparators(m_document.filePath())), QMessageBox::Ok);
        return;
    }
    documentChanged();
}

void CodeEditor::documentChanged()
{
    m_sciControlMaster->gotoPos(0);

    m_sciControlMaster->emptyUndoBuffer();
    emit filePathChanged(m_document.filePath());
    applyEditorStyles();
    updateLineNumberMargin(m_sciControlMaster);
    if (m_sciControlSlave)
    {
        m_sciControlSlave->gotoPos(0);
        updateLineNumberMargin(m_sciControlSlave);
    }
}

void CodeEditor::saveFileAsEncoding(const QString &filePath, const QString &encoding, BOM bom)
{
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        BOOST_SCOPE_EXIT(&file)
        {
            file.close();
        }
        BOOST_SCOPE_EXIT_END

        sptr_t textLength = m_sciControlMaster->textLength();
        auto   data       = m_sciControlMaster->getText(textLength + 1);
        //  check bom & encoding
        auto bytes = EncodingUtils::generateBOM(bom);
        if (!bytes.isEmpty())
        {
            file.write(bytes);
        }
        if (encoding.toUpper() != QByteArrayLiteral("UTF-8") && encoding.toLower() != QByteArrayLiteral("ASCII") &&
            encoding.toLower() != QByteArrayLiteral("ANSI"))
        {
            UErrorCode errorCode = U_ZERO_ERROR;

            UConverter *sourceConv = ucnv_open("UTF-8", &errorCode);
            if (U_FAILURE(errorCode))
            {
                const char *errorName = u_errorName(errorCode);
                QMessageBox::critical(this, tr("Error"), tr("creating converter for UTF-8 failed: %1").arg(errorName), QMessageBox::Ok);
                return;
            }
            BOOST_SCOPE_EXIT(sourceConv)
            {
                ucnv_close(sourceConv);
            }
            BOOST_SCOPE_EXIT_END

            UConverter *targetConv = ucnv_open(encoding.toStdString().c_str(), &errorCode);
            if (U_FAILURE(errorCode))
            {
                const char *errorName = u_errorName(errorCode);
                QMessageBox::critical(this, tr("Error"), tr("creating converter for %1 failed: %2").arg(encoding).arg(errorName), QMessageBox::Ok);
                return;
            }
            BOOST_SCOPE_EXIT(targetConv)
            {
                ucnv_close(targetConv);
            }
            BOOST_SCOPE_EXIT_END

            const char *source      = data.constData();
            const char *sourceStart = source;
            const char *sourceLimit = source + textLength;

            const qint64      targetBufferSize = textLength * 2;
            std::vector<char> targetBuffer;
            targetBuffer.resize(targetBufferSize);
            char *target      = targetBuffer.data();
            char *targetStart = target;
            char *targetLimit = target + targetBufferSize;
            // convert encoding
            ucnv_convertEx(
                targetConv, sourceConv, &target, targetLimit, &source, sourceLimit, nullptr, nullptr, nullptr, nullptr, true, true, &errorCode);

            if (errorCode == U_BUFFER_OVERFLOW_ERROR)
            {
                errorCode = U_ZERO_ERROR;
            }
            else if (U_FAILURE(errorCode))
            {
                // Handle conversion error
                const char *errorName = u_errorName(errorCode);
                QMessageBox::critical(this, tr("Error"), tr("converting from UTF-8 to %1 failed: %2").arg(encoding, errorName), QMessageBox::Ok);
                return;
            }
            ptrdiff_t bytesGenerated = target - targetStart;

            file.write(targetStart, bytesGenerated);
        }
        else
        {
            file.write(data);
        }

        m_sciControlMaster->setSavePoint();
        emit modifiedNotification();
        return;
    }
    QMessageBox::warning(this, tr("Error"), tr("Cannot write to file %1.").arg(QDir::toNativeSeparators(filePath)), QMessageBox::Ok);
}

void CodeEditor::saveFile(const QString &filePath)
{
    if (!m_document.filePath().isEmpty())
    {
        Config *config = Config::instance();
        Q_ASSERT(config);
        m_lexerName = config->matchPatternLanguage(m_document.filePath());
    }

    QFileInfo saveFileInfo(filePath);
    QFileInfo fileInfo(m_document.filePath());
    if (m_document.filePath().isEmpty() || saveFileInfo != fileInfo)
    {
        m_document.setFilePath(filePath);
        emit filePathChanged(m_document.filePath());

        applyEditorStyles();
    }

    if (m_sciControlMaster->modify() || m_document.filePath().isEmpty() || saveFileInfo != fileInfo)
    {
        sptr_t textLength = m_sciControlMaster->textLength();
        auto   data       = m_sciControlMaster->getText(textLength + 1);
        m_document.saveToFile(data);
    }
}

QString CodeEditor::getFilePath() const
{
    return m_document.filePath();
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
    return (m_sciControlMaster->focus() || (m_sciControlSlave && m_sciControlSlave->focus()));
}

void CodeEditor::applyEditorStyles()
{
    ScintillaConfig::initEditorStyle(m_sciControlMaster, m_lexerName);
    // m_sciControlMaster->colourise(0, -1);
    if (m_sciControlSlave)
    {
        ScintillaConfig::initEditorStyle(m_sciControlSlave, m_lexerName);
    }
}

void CodeEditor::setShowWhiteSpaceAndTAB(bool enabled)
{
    m_sciControlMaster->setViewWS(enabled ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
    if (m_sciControlSlave)
    {
        m_sciControlSlave->setViewWS(enabled ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
    }
}

void CodeEditor::setShowEndOfLine(bool enabled)
{
    m_sciControlMaster->setViewEOL(enabled);
    if (m_sciControlSlave)
    {
        m_sciControlSlave->setViewEOL(enabled);
    }
}

void CodeEditor::setShowIndentGuide(bool enabled)
{
    m_sciControlMaster->setIndentationGuides(enabled ? SC_IV_REAL : SC_IV_NONE);
    if (m_sciControlSlave)
    {
        m_sciControlSlave->setIndentationGuides(enabled ? SC_IV_REAL : SC_IV_NONE);
    }
}

void CodeEditor::setShowWrapSymbol(bool enabled)
{
    m_sciControlMaster->setWrapVisualFlags(enabled ? SC_WRAPVISUALFLAG_END : SC_WRAPVISUALFLAG_NONE);
    if (m_sciControlSlave)
    {
        m_sciControlSlave->setWrapVisualFlags(enabled ? SC_WRAPVISUALFLAG_END : SC_WRAPVISUALFLAG_NONE);
    }
}

void CodeEditor::updateUI(Scintilla::Update updated)
{
    if (updated == Scintilla::Update::Content || updated == Scintilla::Update::Selection)
    {
        // smart highlight
        auto *editor = qobject_cast<ScintillaEdit *>(sender());
        Q_ASSERT(editor);
        const int smartIndicator = ScintillaConfig::smartHighlightIndicator();
        editor->setIndicatorCurrent(smartIndicator);
        editor->indicatorClearRange(0, editor->length());

        if (editor->selectionEmpty())
        {
            return;
        }

        const sptr_t mainSelection  = editor->mainSelection();
        const sptr_t selectionStart = editor->selectionNStart(mainSelection);
        const sptr_t selectionEnd   = editor->selectionNEnd(mainSelection);

        // Make sure the current selection is valid
        if (selectionStart == selectionEnd)
        {
            return;
        }

        const sptr_t curPos    = editor->currentPos();
        const sptr_t wordStart = editor->wordStartPosition(curPos, true);
        const sptr_t wordEnd   = editor->wordEndPosition(wordStart, true);

        // Make sure the selection is on word boundaries
        if (wordStart == wordEnd || wordStart != selectionStart || wordEnd != selectionEnd)
        {
            return;
        }

        const QByteArray selText = editor->get_text_range(selectionStart, selectionEnd);

        // TODO: Handle large files. By default Notepad++ only monitors the text on screen. However,
        // that will not work when using a highlighted scroll bar. Testing with small files seems to
        // have minimal impact. For large files, Qt can have a timer set to 0 to do heavier processing.
        // Using threads seems to be a bit overkill and too burdensome to do it properly.

        // const int startLine = editor->firstVisibleLine();
        // const int linesOnScreen = editor->linesOnScreen();
        // const int startPos = editor->positionFromLine(startLine);
        // const int endPos = editor->lineEndPosition(startLine + linesOnScreen);

        // TODO: skip hidden or folded lines?

        Sci_TextToFind ttf {{0, static_cast<Sci_PositionCR>(editor->length())}, selText.constData(), {-1, -1}};
        const int      flags = SCFIND_MATCHCASE | SCFIND_WHOLEWORD;

        while (editor->send(SCI_FINDTEXT, flags, (sptr_t)&ttf) != -1)
        {
            editor->indicatorFillRange(ttf.chrgText.cpMin, ttf.chrgText.cpMax - ttf.chrgText.cpMin);
            ttf.chrg.cpMin = ttf.chrgText.cpMax;
        }
    }
}

void CodeEditor::uriDropped(const QString &uri)
{
    emit openFilesRequest({QUrl::fromUserInput(uri).toLocalFile()});
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
    auto *sci = qobject_cast<ScintillaEdit *>(sender());
    updateLineNumberMargin(sci);
}

void CodeEditor::updateLineNumberMargin(ScintillaEdit *sci)
{
    sptr_t line_count = sci->lineCount();
    sptr_t left       = sci->marginLeft() + 2;
    sptr_t right      = sci->marginRight() + 2;
    sptr_t width      = left + right + sci->textWidth(STYLE_LINENUMBER, QStringLiteral("%1").arg(line_count).toStdString().c_str());
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
            toggleBookmarkAtLine(sci, line);
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

void CodeEditor::print(QPainter &painter) {}

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
}

void CodeEditor::clearReadOnlyFlag()
{
    m_sciControlMaster->setReadOnly(false);
}

void CodeEditor::currentFullFilePathToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(m_document.filePath());
}

void CodeEditor::currentFileNameToClipboard()
{
    QFileInfo   fi(m_document.filePath());
    QClipboard *clipboard = QApplication::clipboard();
    Q_ASSERT(clipboard);
    clipboard->setText(fi.fileName());
}

void CodeEditor::currentDirectoryPathToClipboard()
{
    QFileInfo   fi(m_document.filePath());
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

void CodeEditor::gotoMatchingBrace()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
}

void CodeEditor::toggleBookmark()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sptr_t line = sci->lineFromPosition(sci->currentPos());
    toggleBookmarkAtLine(sci, line);
}

void CodeEditor::nextBookmark()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sptr_t line = sci->lineFromPosition(sci->currentPos()) + 1;

    const int mask           = ScintillaConfig::bookmarkMask();
    sptr_t    nextMarkedLine = sci->markerNext(line, mask);

    if (nextMarkedLine == -1)
    {
        sci->gotoLine(sci->markerNext(0, mask));
    }
    else
    {
        sci->gotoLine(nextMarkedLine);
    }
}

void CodeEditor::previousBookmark()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sptr_t line = sci->lineFromPosition(sci->currentPos()) - 1;

    const int mask           = ScintillaConfig::bookmarkMask();
    sptr_t    prevMarkedLine = sci->markerPrevious(line, mask);

    if (prevMarkedLine == -1)
    {
        sci->gotoLine(sci->markerPrevious(sci->lineCount(), mask));
    }
    else
    {
        sci->gotoLine(prevMarkedLine);
    }
}

void CodeEditor::clearAllBookmarks()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    const int marker = ScintillaConfig::bookmarkMarker();
    sci->markerDeleteAll(marker);
}

void CodeEditor::cutBookmarkLines()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    copyBookmarkLines();
    removeBookmarkedLines();
}

void CodeEditor::copyBookmarkLines()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    QByteArray text;
    auto       lines = bookmarkedLines(sci);
    for (auto line : lines)
    {
        sptr_t lineLen   = sci->lineLength(line);
        sptr_t lineBegin = sci->positionFromLine(line);
        sci->setTargetRange(lineBegin, lineBegin + lineLen);
        text.append(sci->targetText());
    }
    QApplication::clipboard()->setText(QString(text));
}

void CodeEditor::pasteToReplaceBookmarkedLines()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    const int marker                = ScintillaConfig::bookmarkMarker();
    auto      bookmarkedLinesNumber = bookmarkedLines(sci);
    auto      text                  = QApplication::clipboard()->text();
    auto      linesText             = text.split('\n');
    int       lineIndex             = 0;

    sci->beginUndoAction();
    for (auto bookmarkedLineNumber : bookmarkedLinesNumber)
    {
        if (lineIndex >= linesText.size() || lineIndex >= bookmarkedLinesNumber.size())
        {
            break;
        }
        auto linetext = linesText.at(lineIndex).toUtf8();
        lineIndex++;
        // do replace, skip \n at the end of the line
        sptr_t lineLen   = sci->lineLength(bookmarkedLineNumber) - 1;
        sptr_t lineBegin = sci->positionFromLine(bookmarkedLineNumber);
        sci->setTargetRange(lineBegin, lineBegin + lineLen);
        sci->replaceTarget(linetext.length(), linetext.constData());
        sci->markerAdd(bookmarkedLineNumber, marker);
    }
    sci->endUndoAction();
}

void CodeEditor::removeBookmarkedLines()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    const int marker = ScintillaConfig::bookmarkMarker();
    auto      lines  = bookmarkedLines(sci);
    for (auto iter = lines.crbegin(); iter != lines.crend(); ++iter)
    {
        sptr_t line = *iter;
        sci->markerDelete(line, marker);
        deleteLine(sci, line);
    }
    sci->endUndoAction();
}

void CodeEditor::removeUnbookmarkedLines()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    sci->beginUndoAction();
    auto   lines     = bookmarkedLines(sci);
    sptr_t lineCount = sci->lineCount();
    for (sptr_t line = lineCount - 1; line >= 0; --line)
    {
        if (std::find(lines.begin(), lines.end(), line) == lines.end())
        {
            deleteLine(sci, line);
        }
    }
    sci->endUndoAction();
}

void CodeEditor::inverseBookmark()
{
    auto *sci = getFocusView();
    Q_ASSERT(sci);
    intptr_t lastLine = sci->lineCount();

    sci->beginUndoAction();
    for (int line = 0; line < lastLine; ++line)
    {
        toggleBookmarkAtLine(sci, line);
    }
    sci->endUndoAction();
}

void CodeEditor::wordWrap()
{
    if (m_sciControlMaster->wrapMode() == SC_WRAP_NONE)
    {
        m_sciControlMaster->setWrapMode(SC_WRAP_WORD);
        if (m_sciControlSlave)
        {
            m_sciControlSlave->setWrapMode(SC_WRAP_WORD);
        }
    }
    else
    {
        m_sciControlMaster->setWrapMode(SC_WRAP_NONE);
        if (m_sciControlSlave)
        {
            m_sciControlSlave->setWrapMode(SC_WRAP_NONE);
        }
    }
}

void CodeEditor::focusOnAnotherView()
{
    if (m_sciControlMaster->focus() && m_sciControlSlave)
    {
        m_sciControlSlave->grabFocus();
    }
    else
    {
        m_sciControlMaster->grabFocus();
    }
}

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

void CodeEditor::reopenAsEncoding(const QString &encoding, bool withBOM)
{
    m_document.setEncoding(encoding);
    m_document.setBOM(BOM::None);
    if (withBOM)
    {
        static std::map<QString, BOM> encodingNameBOMMap = {
            {QStringLiteral("UTF-8"), BOM::UTF8},
            {QStringLiteral("UTF-16LE"), BOM::UTF16LE},
            {QStringLiteral("UTF-16BE"), BOM::UTF16BE},
            {QStringLiteral("UTF-32LE"), BOM::UTF32LE},
            {QStringLiteral("UTF-32BE"), BOM::UTF32BE},
            {QStringLiteral("GB18030"), BOM::GB18030},
        };
        auto iter = encodingNameBOMMap.find(encoding.toUpper());
        if (encodingNameBOMMap.end() != iter)
        {
            m_document.setBOM(iter->second);
        }
    }

    m_document.setForceEncoding(true);
    loadDataFromFile();
}

void CodeEditor::saveAsEncoding(const QString &encoding, bool withBOM)
{
    m_document.setEncoding(encoding);
    m_document.setBOM(BOM::None);
    if (withBOM)
    {
        static std::map<QString, BOM> encodingNameBOMMap = {
            {QStringLiteral("UTF-8"), BOM::UTF8},
            {QStringLiteral("UTF-16LE"), BOM::UTF16LE},
            {QStringLiteral("UTF-16BE"), BOM::UTF16BE},
            {QStringLiteral("UTF-32LE"), BOM::UTF32LE},
            {QStringLiteral("UTF-32BE"), BOM::UTF32BE},
            {QStringLiteral("GB18030"), BOM::GB18030},
        };
        auto iter = encodingNameBOMMap.find(encoding.toUpper());
        if (encodingNameBOMMap.end() != iter)
        {
            m_document.setBOM(iter->second);
        }
    }
    sptr_t textLength = m_sciControlMaster->textLength();
    auto   data       = m_sciControlMaster->getText(textLength + 1);
    m_document.saveToFile(data);
}

QString CodeEditor::encoding() const
{
    return m_document.encoding();
}

bool CodeEditor::hasBOM()
{
    return m_document.hasBOM();
}

void CodeEditor::focusChanged(bool focused)
{
    if (focused)
    {
        m_sciFocusView = qobject_cast<ScintillaEdit *>(sender());
        Q_ASSERT(m_sciFocusView);

        emit focusIn();
    }
    else
    {
        if (!m_sciControlSlave)
        {
            m_sciFocusView = m_sciControlMaster;
        }
    }
}

QString CodeEditor::lexerName() const
{
    return m_lexerName;
}

void CodeEditor::setLexerName(const QString &lexerName)
{
    m_lexerName = lexerName;
    applyEditorStyles();
}

bool CodeEditor::isWordWrap() const
{
    return m_sciControlMaster->wrapMode() != SC_WRAP_NONE;
}

void CodeEditor::toggleBookmarkAtLine(ScintillaEdit *sci, int line)
{
    Q_ASSERT(sci);
    const int marker = ScintillaConfig::bookmarkMarker();
    const int mask   = ScintillaConfig::bookmarkMask();
    if (sci->markerGet(line) & mask)
    {
        // The marker can be set multiple times, so keep deleting it till it is no longer set
        while (sci->markerGet(line) & (ScintillaConfig::bookmarkMask()))
        {
            sci->markerDelete(line, marker);
        }
    }
    else
    {
        sci->markerAdd(line, marker);
    }
}

std::vector<sptr_t> CodeEditor::bookmarkedLines(ScintillaEdit *sci)
{
    Q_ASSERT(sci);
    std::vector<sptr_t> res;
    const int           mask           = ScintillaConfig::bookmarkMask();
    sptr_t              nextMarkedLine = sci->markerNext(0, mask);
    while (nextMarkedLine != -1)
    {
        res.push_back(nextMarkedLine);
        nextMarkedLine = sci->markerNext(nextMarkedLine + 1, mask);
    }
    return res;
}

void CodeEditor::deleteLine(ScintillaEdit *sci, sptr_t line)
{
    Q_ASSERT(line);
    sptr_t lineLen   = sci->lineLength(line);
    sptr_t lineBegin = sci->positionFromLine(line);
    sci->setTargetRange(lineBegin, lineBegin + lineLen);
    sci->replaceTarget(0, "");
}

bool CodeEditor::isSplittered() const
{
    return m_editorSplitter != nullptr;
}

void CodeEditor::splitEditor(Qt::Orientation orientation, int size)
{
    Q_ASSERT(!m_editorSplitter);
    if (m_editorSplitter == nullptr)
    {
        m_editorSplitter  = new QSplitter(orientation, this);
        m_sciControlSlave = new ScintillaEdit(m_editorSplitter);
        Q_ASSERT(m_sciControlSlave);
        m_sciControlMaster->setParent(m_editorSplitter);
        m_editorSplitter->addWidget(m_sciControlMaster);
        m_editorSplitter->addWidget(m_sciControlSlave);
        layout()->addWidget(m_editorSplitter);

        QList<int> sizes {size, size};
        m_editorSplitter->setSizes(sizes);

        m_sciControlSlave->set_doc(m_sciControlMaster->get_doc());
        initSci(m_sciControlSlave);
    }
}

void CodeEditor::splitEditorRight()
{
    splitEditor(Qt::Horizontal, width() / 2);
}

void CodeEditor::splitEditorDown()
{
    splitEditor(Qt::Vertical, height() / 2);
}

void CodeEditor::cancelSplit()
{
    Q_ASSERT(m_editorSplitter);
    if (m_editorSplitter != nullptr)
    {
        m_sciFocusView = m_sciControlMaster;
        delete m_sciControlSlave;
        m_sciControlSlave = nullptr;
        m_sciControlMaster->setParent(this);
        layout()->addWidget(m_sciControlMaster);
        delete m_editorSplitter;
        m_editorSplitter = nullptr;
    }
}