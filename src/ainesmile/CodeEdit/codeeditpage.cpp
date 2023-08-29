#include "stdafx.h"

#include <boost/scope_exit.hpp>
#include <unicode/ucnv.h>
#include <unicode/utext.h>

#include "codeeditpage.h"
#include "config.h"
#include "uchardet.h"

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
    m_sc.initScintilla(m_sciControlMaster);
    m_sc.initScintilla(m_sciControlSlave);

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

    auto &ptree              = Config::instance()->pt();
    bool  autoDetectEncoding = ptree.get<bool>("encoding.auto_detect", false);

    m_bom                                 = BOM::None;
    m_filePath                            = filePath;
    const qint64                headerLen = 4;
    std::array<char, headerLen> header {};
    qint64                      cbRead          = file.read(header.data(), headerLen);
    bool                        charsetDetected = false;
    auto [bom, length]                          = checkBOM(QByteArray::fromRawData(header.data(), cbRead));
    if (bom == BOM::UTF8)
    {
        m_encoding = QByteArrayLiteral("UTF-8");
        m_bom      = bom;
        loadRawFile(file, length);
        return;
    }
    if (bom != BOM::None)
    {
        auto encoding = encodingNameForBOM(bom);
        loadFileAsEncoding(file, encoding, length);
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
            QString encoding = fileEncodingDetect(file);
            if (!encoding.isEmpty() && encoding.toLower() != "utf-8")
            {
                loadFileAsEncoding(file, encoding);
                m_encoding      = encoding.toUtf8();
                charsetDetected = true;
                return;
            }
        }
    }
    if (!charsetDetected)
    {
        m_encoding = QByteArrayLiteral("UTF-8");
        file.seek(0);
        loadRawFile(file);
        return;
    }
}

void CodeEditor::loadFileAsEncoding(QFile &file, const QString &encoding, qint64 skipBytes)
{
    UErrorCode errorCode = U_ZERO_ERROR;

    UConverter *sourceConv = ucnv_open(encoding.toStdString().c_str(), &errorCode);
    if (U_FAILURE(errorCode))
    {
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
        return;
    }
    BOOST_SCOPE_EXIT(targetConv)
    {
        ucnv_close(targetConv);
    }
    BOOST_SCOPE_EXIT_END

    const qint64                       blockSize        = 4096;
    const qint64                       targetBufferSize = blockSize * 2;
    std::array<char, targetBufferSize> targetBuffer {};

    qint64 fileSize  = file.size();
    qint64 bytesLeft = file.size() - skipBytes;
    qint64 offset    = skipBytes;
    file.seek(offset);

    while (bytesLeft >= 0 && offset < fileSize)
    {
        qint64      requestSize = std::min(blockSize, bytesLeft);
        char       *pData       = (char *)file.map(offset, requestSize);
        const char *source      = pData;
        char       *sourceStart = pData;
        const char *sourceLimit = source + requestSize;
        char       *target      = targetBuffer.data();
        char       *targetStart = target;
        char       *targetLimit = target + targetBufferSize;
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
            break;
        }
        ptrdiff_t bytesConsumed  = source - sourceStart;
        ptrdiff_t bytesGenerated = target - targetStart;
        // display text
        m_sciControlMaster->appendText(bytesGenerated, (const char *)targetStart);
        file.unmap((uchar *)pData);
        bytesLeft -= bytesConsumed;
        offset += bytesConsumed;
    }

    documentChanged();
}

QString CodeEditor::fileEncodingDetect(QFile &file)
{
    qint64       fileSize  = file.size();
    qint64       offset    = 0;
    const qint64 blockSize = 4096;
    auto        *uchardet  = uchardet_new();
    while (fileSize >= blockSize)
    {
        uchar *pData = file.map(offset, blockSize);
        uchardet_handle_data(uchardet, (const char *)pData, blockSize);
        file.unmap(pData);
        m_sciControlMaster->gotoPos(m_sciControlMaster->length());
        fileSize -= blockSize;
        offset += blockSize;
    }
    if (fileSize > 0)
    {
        uchar *pData = file.map(offset, fileSize);
        uchardet_handle_data(uchardet, (const char *)pData, fileSize);
        file.unmap(pData);
    }
    uchardet_data_end(uchardet);
    QString charset = QString::fromLatin1(uchardet_get_charset(uchardet));
    uchardet_delete(uchardet);
    if (charset.toUpper() == QStringLiteral("ASCII"))
    {
        return QStringLiteral("UTF-8");
    }
    return charset;
}

void CodeEditor::loadRawFile(QFile &file, qint64 skipBytes)
{
    qint64 fileSize = file.size() - skipBytes;
    qint64 offset   = skipBytes;
    file.seek(offset);
    const qint64 blockSize = 4096;

    while (fileSize >= blockSize)
    {
        uchar *pData = file.map(offset, blockSize);
        m_sciControlMaster->appendText(blockSize, (const char *)pData);
        file.unmap(pData);
        fileSize -= blockSize;
        offset += blockSize;
    }
    if (fileSize > 0)
    {
        uchar *pData = file.map(offset, fileSize);
        m_sciControlMaster->appendText(fileSize, (const char *)pData);
        file.unmap(pData);
    }
    documentChanged();
}

void CodeEditor::documentChanged()
{
    m_sciControlMaster->gotoPos(0);

    m_sciControlMaster->emptyUndoBuffer();
    emit filePathChanged(m_filePath);
    m_sc.initEditorStyle(m_sciControlMaster, m_filePath);
    m_sc.initEditorStyle(m_sciControlSlave, m_filePath);
    m_sciControlMaster->colourise(0, -1);
    m_sciControlSlave->colourise(0, -1);
}

void CodeEditor::saveFileAsEncoding(const QString &filePath, const QByteArray &encoding, BOM bom)
{
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        sptr_t len = m_sciControlMaster->textLength();
        //  check bom & encoding
        auto bytes = generateBOM(bom);
        if (!bytes.isEmpty())
        {
            file.write(bytes);
        }
        auto data = m_sciControlMaster->getText(len + 1);
        if (encoding.toLower() != "utf-8" && encoding.toLower() != "ascii")
        {
            UErrorCode errorCode = U_ZERO_ERROR;

            UConverter *sourceConv = ucnv_open("UTF-8", &errorCode);
            if (U_FAILURE(errorCode))
            {
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
                return;
            }
            BOOST_SCOPE_EXIT(targetConv)
            {
                ucnv_close(targetConv);
            }
            BOOST_SCOPE_EXIT_END

            const qint64                       blockSize        = 4096;
            const qint64                       targetBufferSize = blockSize * 2;
            std::array<char, targetBufferSize> targetBuffer {};
            qint64                             bytesLeft = data.length();
            const char                        *source    = data.constData();

            while (bytesLeft >= 0)
            {
                qint64 requestSize = std::min(blockSize, bytesLeft);

                const char *sourceStart = source;
                const char *sourceLimit = source + requestSize;
                char       *target      = targetBuffer.data();
                char       *targetStart = target;
                char       *targetLimit = target + targetBufferSize;
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
                    break;
                }
                ptrdiff_t bytesConsumed  = source - sourceStart;
                ptrdiff_t bytesGenerated = target - targetStart;

                // write to file
                file.write(targetStart, bytesGenerated);
                bytesLeft -= bytesConsumed;
            }
        }
        else
        {
            qint64 size = file.write(data);
        }
        file.close();

        m_sciControlMaster->setSavePoint();
        m_sciControlSlave->setSavePoint();
        emit modifiedNotification();
        return;
    }
    QMessageBox::warning(this, tr("Saving file failed:"), tr("Not all data is saved to file."), QMessageBox::Ok);
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

void CodeEditor::updateUI(Scintilla::Update /*updated*/) {}

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

std::pair<BOM, std::uint8_t> CodeEditor::checkBOM(const QByteArray &data)
{
    static std::map<QByteArray, BOM> bomMap = {
        {QByteArrayLiteral("\xEF\xBB\xBF"), BOM::UTF8},
        {QByteArrayLiteral("\xFF\xFE"), BOM::UTF16LE},
        {QByteArrayLiteral("\xFE\xFF"), BOM::UTF16BE},
        {QByteArrayLiteral("\xFF\xFE\x00\x00"), BOM::UTF32LE},
        {QByteArrayLiteral("\x00\x00\xFE\xFF"), BOM::UTF32BE},
        {QByteArrayLiteral("\x2B\x2F\x76"), BOM::UTF7},
        {QByteArrayLiteral("\xF7\x64\x4C"), BOM::UTF1},
        {QByteArrayLiteral("\xDD\x73\x66\x73"), BOM::UTFEBCDIC},
        {QByteArrayLiteral("\x0E\xFE\xFF"), BOM::SCSU},
        {QByteArrayLiteral("\xFB\xEE\x28"), BOM::BOCU1},
        {QByteArrayLiteral("\x84\x31\x95\x33"), BOM::GB18030},
    };
    for (auto &[bytes, bom] : bomMap)
    {
        if (data.length() >= bytes.length() && bytes == data.mid(0, bytes.length()))
        {
            return {bom, bytes.length()};
        }
    }
    return {BOM::None, 0};
}

QByteArray CodeEditor::encodingNameForBOM(BOM bom)
{
    static std::map<BOM, QByteArray> encodingNameMap = {
        {BOM::UTF8, QByteArrayLiteral("UTF-8")},
        {BOM::UTF16LE, QByteArrayLiteral("UTF-16LE")},
        {BOM::UTF16BE, QByteArrayLiteral("UTF-16BE")},
        {BOM::UTF32LE, QByteArrayLiteral("UTF-32LE")},
        {BOM::UTF32BE, QByteArrayLiteral("UTF-32BE")},
        {BOM::GB18030, QByteArrayLiteral("GB18030")},
    };
    auto iter = encodingNameMap.find(bom);
    if (encodingNameMap.end() != iter)
    {
        return iter->second;
    }

    return {};
}

QByteArray CodeEditor::generateBOM(BOM bom)
{
    static std::map<BOM, QByteArray> codecNameMap = {
        {BOM::UTF8, QByteArrayLiteral("\xEF\xBB\xBF")},
        {BOM::UTF16LE, QByteArrayLiteral("\xFF\xFE")},
        {BOM::UTF16BE, QByteArrayLiteral("\xFE\xFF")},
        {BOM::UTF32LE, QByteArrayLiteral("\xFF\xFE\x00\x00")},
        {BOM::UTF32BE, QByteArrayLiteral("\x00\x00\xFE\xFF")},
        {BOM::GB18030, QByteArrayLiteral("\x84\x31\x95\x33")},
    };
    auto iter = codecNameMap.find(bom);
    if (codecNameMap.end() != iter)
    {
        return iter->second;
    }
    return {};
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
        auto bom  = generateBOM(m_bom);
        skipBytes = bom.length();
    }
    m_sciControlMaster->clearAll();
    loadFileAsEncoding(file, encoding, skipBytes);
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
