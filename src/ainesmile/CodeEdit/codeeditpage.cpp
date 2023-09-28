#include "stdafx.h"

#include "codeeditpage.h"
#include "config.h"
#include "scintillaconfig.h"
#include "textutils.h"

CodeEditor::CodeEditor(QWidget *parent)
    : QWidget(parent),
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

    auto *m_mainLayout = new QVBoxLayout;
    Q_ASSERT(m_mainLayout);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    // m_mainLayout->setMargin(0);
    m_mainLayout->addWidget(m_verticalEditorSplitter);
    setLayout(m_mainLayout);

    init();

    m_sciControlSlave->set_doc(m_sciControlMaster->get_doc());

    m_sciControlMaster->setSavePoint();
    m_sciControlSlave->setSavePoint();
}

void CodeEditor::init()
{
    ScintillaConfig::initScintilla(m_sciControlMaster);
    ScintillaConfig::initScintilla(m_sciControlSlave);

    connect(m_sciControlMaster, &ScintillaEdit::linesAdded, this, &CodeEditor::linesAdded);
    connect(m_sciControlMaster, &ScintillaEdit::marginClicked, this, &CodeEditor::marginClicked);
    connect(m_sciControlMaster, &ScintillaEdit::modified, this, &CodeEditor::modified);
    connect(m_sciControlMaster, &ScintillaEdit::dwellEnd, this, &CodeEditor::dwellEnd);
    connect(m_sciControlMaster, &ScintillaEdit::updateUi, this, &CodeEditor::updateUI);
    connect(m_sciControlMaster, &ScintillaEdit::uriDropped, this, &CodeEditor::uriDropped);
    connect(m_sciControlMaster, &ScintillaEdit::focusChanged, this, &CodeEditor::focusChanged);
    connect(m_sciControlSlave, &ScintillaEdit::linesAdded, this, &CodeEditor::linesAdded);
    connect(m_sciControlSlave, &ScintillaEdit::marginClicked, this, &CodeEditor::marginClicked);
    connect(m_sciControlSlave, &ScintillaEdit::modified, this, &CodeEditor::modified);
    connect(m_sciControlSlave, &ScintillaEdit::dwellEnd, this, &CodeEditor::dwellEnd);
    connect(m_sciControlSlave, &ScintillaEdit::updateUi, this, &CodeEditor::updateUI);
    connect(m_sciControlSlave, &ScintillaEdit::uriDropped, this, &CodeEditor::uriDropped);
    connect(m_sciControlSlave, &ScintillaEdit::focusChanged, this, &CodeEditor::focusChanged);
}

ScintillaEdit *CodeEditor::getFocusView()
{
    return m_sciFocusView;
}

bool CodeEditor::initialDocument()
{
    return !isModified() && m_filePath.isEmpty();
}

void CodeEditor::openFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    m_filePath = filePath;
    if (!m_filePath.isEmpty())
    {
        Config *config = Config::instance();
        Q_ASSERT(config);
        m_lexerName = config->matchPatternLanguage(m_filePath);
    }

    auto &ptree              = Config::instance()->pt();
    bool  autoDetectEncoding = ptree.get<bool>("encoding.auto_detect", false);

    m_bom                                 = BOM::None;
    const qint64                headerLen = 4;
    std::array<char, headerLen> header {};
    qint64                      cbRead          = file.read(header.data(), headerLen);
    bool                        charsetDetected = false;
    auto [bom, length]                          = EncodingUtils::checkBOM(QByteArray::fromRawData(header.data(), cbRead));

    file.seek(length);
    auto data = file.readAll();
    if (bom == BOM::UTF8)
    {
        m_encoding = QByteArrayLiteral("UTF-8");
        m_bom      = bom;
        loadRawFile(data);
        return;
    }
    if (bom != BOM::None)
    {
        auto encoding = EncodingUtils::encodingNameForBOM(bom);
        loadFileAsEncoding(data, encoding);
        m_encoding      = encoding;
        m_bom           = bom;
        charsetDetected = true;
        return;
    }

    if (autoDetectEncoding)
    {
        if (!charsetDetected)
        {
            m_bom = BOM::None;
            file.seek(0);
            QString encoding = EncodingUtils::fileEncodingDetect(data);
            if (!encoding.isEmpty() && encoding.toUpper() != QByteArrayLiteral("UTF-8"))
            {
                loadFileAsEncoding(data, encoding);
                m_encoding      = encoding.toUtf8();
                charsetDetected = true;
                return;
            }
        }
    }
    if (!charsetDetected)
    {
        m_encoding = QByteArrayLiteral("UTF-8");
        loadRawFile(data);
        return;
    }
}

void CodeEditor::loadFileAsEncoding(const QByteArray &data, const QString &encoding)
{
    UErrorCode errorCode = U_ZERO_ERROR;

    UConverter *sourceConv = ucnv_open(encoding.toStdString().c_str(), &errorCode);
    if (U_FAILURE(errorCode))
    {
        const char *errorName = u_errorName(errorCode);
        QMessageBox::critical(this, tr("Error"), tr("creating converter for %1 failed: %2").arg(encoding).arg(errorName), QMessageBox::Ok);
        return;
    }
    BOOST_SCOPE_EXIT(sourceConv)
    {
        ucnv_close(sourceConv);
    }
    BOOST_SCOPE_EXIT_END

    UConverter *targetConv = ucnv_open("UTF-8", &errorCode);
    if (U_FAILURE(errorCode))
    {
        const char *errorName = u_errorName(errorCode);
        QMessageBox::critical(this, tr("Error"), tr("creating converter for UTF-8 failed: %1").arg(errorName), QMessageBox::Ok);
        return;
    }
    BOOST_SCOPE_EXIT(targetConv)
    {
        ucnv_close(targetConv);
    }
    BOOST_SCOPE_EXIT_END

    const qint64 sourceSize  = data.length();
    const char  *source      = data.constData();
    const char  *sourceStart = data.constData();
    const char  *sourceLimit = source + sourceSize;

    const qint64      targetBufferSize = sourceSize * 2;
    std::vector<char> targetBuffer;
    targetBuffer.resize(targetBufferSize);
    char *target      = targetBuffer.data();
    char *targetStart = target;
    char *targetLimit = target + targetBufferSize;
    // convert encoding
    ucnv_convertEx(targetConv, sourceConv, &target, targetLimit, &source, sourceLimit, nullptr, nullptr, nullptr, nullptr, true, true, &errorCode);

    if (errorCode == U_BUFFER_OVERFLOW_ERROR)
    {
        errorCode = U_ZERO_ERROR;
    }
    else if (U_FAILURE(errorCode))
    {
        const char *errorName = u_errorName(errorCode);
        QMessageBox::critical(this, tr("Error"), tr("converting from %1 to UTF-8 failed: %2").arg(encoding).arg(errorName), QMessageBox::Ok);
        return;
    }
    ptrdiff_t bytesConsumed  = source - sourceStart;
    ptrdiff_t bytesGenerated = target - targetStart;
    // display text
    m_sciControlMaster->clearAll();
    int lineCount = TextUtils::getLineCount(targetStart, bytesGenerated);
    m_sciControlMaster->allocateLines(lineCount);
    m_sciControlMaster->appendText(bytesGenerated, (const char *)targetStart);

    documentChanged();
}

void CodeEditor::loadRawFile(const QByteArray &data)
{
    m_sciControlMaster->clearAll();
    int lineCount = TextUtils::getLineCount(data);
    m_sciControlMaster->allocateLines(lineCount);
    m_sciControlMaster->appendText(data.length(), data.constData());

    documentChanged();
}

void CodeEditor::documentChanged()
{
    m_sciControlMaster->gotoPos(0);
    m_sciControlSlave->gotoPos(0);

    m_sciControlMaster->emptyUndoBuffer();
    m_sciControlSlave->emptyUndoBuffer();
    emit filePathChanged(m_filePath);
    applyEditorStyles();
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
        //  check bom & encoding
        auto bytes = EncodingUtils::generateBOM(bom);
        if (!bytes.isEmpty())
        {
            file.write(bytes);
        }
        auto data = m_sciControlMaster->getText(textLength + 1);
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
        m_sciControlSlave->setSavePoint();
        emit modifiedNotification();
        return;
    }
    QMessageBox::warning(this, tr("Error"), tr("Cannot write to file %1.").arg(QDir::toNativeSeparators(filePath)), QMessageBox::Ok);
}

void CodeEditor::saveFile(const QString &filePath)
{
    if (!m_filePath.isEmpty())
    {
        Config *config = Config::instance();
        Q_ASSERT(config);
        m_lexerName = config->matchPatternLanguage(m_filePath);
    }

    QFileInfo saveFileInfo(filePath);
    QFileInfo fileInfo(m_filePath);
    if (m_filePath.isEmpty() || saveFileInfo != fileInfo)
    {
        m_filePath = filePath;
        emit filePathChanged(m_filePath);

        applyEditorStyles();
    }

    if (m_sciControlMaster->modify() || m_filePath.isEmpty() || saveFileInfo != fileInfo)
    {
        saveFileAsEncoding(filePath, m_encoding, m_bom);
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
    ScintillaConfig::initEditorStyle(m_sciControlMaster, m_lexerName);
    ScintillaConfig::initEditorStyle(m_sciControlSlave, m_lexerName);
    m_sciControlMaster->colourise(0, -1);
    m_sciControlSlave->colourise(0, -1);
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
    auto  *sci        = qobject_cast<ScintillaEdit *>(sender());
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
    m_encoding = encoding.toUtf8();
    m_bom      = BOM::None;
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
            m_bom = iter->second;
        }
    }

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    qint64 skipBytes = 0;
    if (m_bom != BOM::None)
    {
        auto bom  = EncodingUtils::generateBOM(m_bom);
        skipBytes = bom.length();
    }
    file.seek(skipBytes);
    auto data = file.readAll();
    loadFileAsEncoding(data, encoding);
    file.close();
}

void CodeEditor::saveAsEncoding(const QString &encoding, bool withBOM)
{
    m_encoding = encoding.toUtf8();
    m_bom      = BOM::None;
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
            m_bom = iter->second;
        }
    }
    saveFileAsEncoding(m_filePath, m_encoding, m_bom);
}

QString CodeEditor::encoding() const
{
    return m_encoding;
}

bool CodeEditor::hasBOM()
{
    return m_bom != BOM::None;
}

void CodeEditor::focusChanged(bool focused)
{
    if (focused)
    {
        m_sciFocusView = qobject_cast<ScintillaEdit *>(sender());
        Q_ASSERT(m_sciFocusView);

        emit focusIn();
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
