/**************************************************************************
**   Author: Fan Yang
**   Email: missdeer@gmail.com
**   License: see the license.txt file
**************************************************************************/

#include "stdafx.h"

#include "findreplace.h"
#include "codeeditpage.h"
#include "findreplaceresultmodel.h"
#include "findresultnotifier.h"

#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "CodeEdit/findreplace.cpp"
#include <hwy/foreach_target.h>
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace FindReplaceInternal
{
    namespace HWY_NAMESPACE
    {
        // SIMD-accelerated string search that finds all occurrences in a buffer
        // Returns a vector of byte offsets where matches were found
        std::vector<size_t> findAllOccurrencesImpl(const char *buffer, size_t bufferLen, const char *searchStr, size_t searchLen, bool caseSensitive)
        {
            std::vector<size_t> results;
            if (searchLen == 0 || bufferLen < searchLen)
            {
                return results;
            }

            namespace hn = hwy::HWY_NAMESPACE;
            using D      = hn::ScalableTag<uint8_t>;
            const D d;

            const size_t  N         = hn::Lanes(d);
            const char   *bufferEnd = buffer + bufferLen;
            const char   *searchEnd = bufferEnd - searchLen + 1;
            const char   *p         = buffer;
            const uint8_t firstChar =
                caseSensitive ? static_cast<uint8_t>(searchStr[0]) : static_cast<uint8_t>(std::tolower(static_cast<unsigned char>(searchStr[0])));

            // Create a vector with the first character of the search string
            const auto firstCharVec = hn::Set(d, firstChar);

            // Main SIMD loop: scan for first character
            while (p + N <= searchEnd)
            {
                // Load chunk of buffer
                auto chunk = hn::LoadU(d, reinterpret_cast<const uint8_t *>(p));

                // Case-insensitive: convert to lowercase
                if (!caseSensitive)
                {
                    // Convert uppercase A-Z (0x41-0x5A) to lowercase by setting bit 5
                    const auto isUpper = hn::And(hn::Ge(chunk, hn::Set(d, 'A')), hn::Le(chunk, hn::Set(d, 'Z')));
                    chunk              = hn::IfThenElse(isUpper, hn::Or(chunk, hn::Set(d, 0x20)), chunk);
                }

                // Find matches of first character
                const auto mask = hn::Eq(chunk, firstCharVec);

                // Check each match
                uint64_t bits = hn::detail::BitsFromMask(mask);
                while (bits != 0)
                {
                    const size_t lane     = hwy::Num0BitsBelowLS1Bit_Nonzero64(bits);
                    const char  *matchPos = p + lane;

                    // Verify full string match
                    bool fullMatch = true;
                    for (size_t i = 1; i < searchLen && fullMatch; ++i)
                    {
                        if (caseSensitive)
                        {
                            fullMatch = (matchPos[i] == searchStr[i]);
                        }
                        else
                        {
                            fullMatch =
                                (std::tolower(static_cast<unsigned char>(matchPos[i])) == std::tolower(static_cast<unsigned char>(searchStr[i])));
                        }
                    }

                    if (fullMatch)
                    {
                        results.push_back(matchPos - buffer);
                    }

                    bits &= bits - 1; // Clear the lowest set bit
                }

                p += N;
            }

            // Process remaining bytes with scalar code
            while (p < searchEnd)
            {
                bool match;
                if (caseSensitive)
                {
                    match = (*p == searchStr[0]);
                }
                else
                {
                    match = (std::tolower(static_cast<unsigned char>(*p)) == std::tolower(static_cast<unsigned char>(searchStr[0])));
                }

                if (match)
                {
                    bool fullMatch = true;
                    for (size_t i = 1; i < searchLen && fullMatch; ++i)
                    {
                        if (caseSensitive)
                        {
                            fullMatch = (p[i] == searchStr[i]);
                        }
                        else
                        {
                            fullMatch = (std::tolower(static_cast<unsigned char>(p[i])) == std::tolower(static_cast<unsigned char>(searchStr[i])));
                        }
                    }

                    if (fullMatch)
                    {
                        results.push_back(p - buffer);
                    }
                }
                ++p;
            }

            return results;
        }
    } // namespace HWY_NAMESPACE
} // namespace FindReplaceInternal
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace FindReplaceInternal
{
    HWY_EXPORT(findAllOccurrencesImpl);

    std::vector<size_t> findAllOccurrences(const char *buffer, size_t bufferLen, const char *searchStr, size_t searchLen, bool caseSensitive)
    {
        return HWY_DYNAMIC_DISPATCH(findAllOccurrencesImpl)(buffer, bufferLen, searchStr, searchLen, caseSensitive);
    }
} // namespace FindReplaceInternal

CodeEditor *FindReplacer::previousPage(CodeEditor *currentPage, std::vector<CodeEditor *> &pages)
{
    auto iter = std::find(pages.begin(), pages.end(), currentPage);
    if (pages.end() == iter || pages.begin() == iter)
    {
        return nullptr;
    }

    return *std::prev(iter);
}

CodeEditor *FindReplacer::nextPage(CodeEditor *currentPage, std::vector<CodeEditor *> &pages)
{
    auto iter = std::find(pages.begin(), pages.end(), currentPage);
    if (pages.end() == iter)
    {
        return nullptr;
    }
    iter = std::next(iter);
    if (pages.end() == iter)
    {
        return nullptr;
    }
    return *iter;
}

bool FindReplacer::findAllStringInFile(const QString &filePath, FindReplaceOption &fro)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    // Read entire file into buffer for SIMD processing
    QByteArray fileData = file.readAll();
    file.close();

    if (fileData.isEmpty())
    {
        return true;
    }

    FindResultNotifier notifier;
    connect(
        &notifier, &FindResultNotifier::found, FindReplaceResultModel::instance(), &FindReplaceResultModel::onAddFindResult, Qt::QueuedConnection);

    // Convert search string to UTF-8
    QByteArray searchBytes = fro.strToFind.toUtf8();

    // Use Highway-accelerated search
    const std::vector<size_t> matches =
        FindReplaceInternal::findAllOccurrences(fileData.constData(), fileData.size(), searchBytes.constData(), searchBytes.size(), fro.matchCase);

    // Pre-calculate line start positions for efficient line number lookup
    std::vector<size_t> lineStarts;
    lineStarts.push_back(0);
    for (size_t i = 0; i < static_cast<size_t>(fileData.size()); ++i)
    {
        if (fileData[i] == '\n')
        {
            lineStarts.push_back(i + 1);
        }
    }

    // Process each match and calculate line/column
    for (size_t matchPos : matches)
    {
        // Binary search to find line number
        auto lineIter = std::upper_bound(lineStarts.begin(), lineStarts.end(), matchPos);
        int  lineNr   = static_cast<int>(std::distance(lineStarts.begin(), lineIter));

        // Calculate column position (within the line)
        size_t lineStart = lineStarts[lineNr - 1];
        int    colPos    = static_cast<int>(matchPos - lineStart);

        // Extract context: 10 chars before match + match + 10 chars after
        size_t     contextStart = (matchPos >= 10) ? (matchPos - 10) : 0;
        size_t     contextLen   = std::min(static_cast<size_t>(searchBytes.size() + 20), static_cast<size_t>(fileData.size() - contextStart));
        QByteArray contextBytes = fileData.mid(contextStart, contextLen);
        QString    context      = QString::fromUtf8(contextBytes);

        // Notify result
        notifier.addResult(filePath, context, lineNr, colPos, fro.strToFind.length());
    }

    return true;
}

bool FindReplacer::findAllRegexpInFile(const QString &filePath, FindReplaceOption &fro)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    FindResultNotifier notifier;
    connect(
        &notifier, &FindResultNotifier::found, FindReplaceResultModel::instance(), &FindReplaceResultModel::onAddFindResult, Qt::QueuedConnection);

    QRegularExpression pattern(fro.strToFind);
    if (!fro.matchCase)
    {
        pattern.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    }
    QTextStream instream(&file);

    int lineNr = 0;
    while (!instream.atEnd())
    {
        QString line = instream.readLine();
        lineNr++;
        int                     startPos = 0;
        QRegularExpressionMatch match;
        auto                    pos = line.indexOf(pattern, startPos, &match);
        while (pos >= 0)
        {
            // notify find result
            int  from    = std::min(static_cast<int>(pos) - 10, 0);
            auto context = line.mid(from, match.capturedLength() + 20);
            notifier.addResult(filePath, context, lineNr, from, match.capturedLength());

            // find next
            startPos = pos + match.capturedLength();
            pos      = line.indexOf(pattern, startPos, &match);
        }
    }

    return false;
}

bool FindReplacer::findAllInFile(const QString &filePath, FindReplaceOption &fro)
{
    if (fro.regexp)
    {
        return findAllRegexpInFile(filePath, fro);
    }

    return findAllStringInFile(filePath, fro);
}

bool FindReplacer::findInDocument(CodeEditor *page, FindReplaceOption &fro)
{
    int            flags = setFlags(fro);
    ScintillaEdit *sci   = page->getFocusView();
    sptr_t         start = sci->currentPos();
    sptr_t         end   = sci->textLength();
    if (fro.searchUp)
    {
        end = 0;
    }
    QPair<int, int> findResult = sci->findText(flags, fro.strToFind.toStdString().c_str(), start, end);
    if (findResult.first >= 0)
    {
        sci->grabFocus();
        sci->gotoPos(findResult.second);
        sci->setCurrentPos(findResult.second);
        if (fro.searchUp)
        {
            sci->setSel(findResult.second, findResult.first);
        }
        else
        {
            sci->setSel(findResult.first, findResult.second);
        }

        sptr_t endStyled = sci->endStyled();
        if (endStyled < findResult.second)
        {
            sci->colourise(endStyled, findResult.second);
        }
        sptr_t lineStart = sci->lineFromPosition(findResult.first);
        sptr_t lineEnd   = sci->lineFromPosition(findResult.second);
        for (sptr_t line = lineStart; line <= lineEnd; line++)
        {
            sci->ensureVisibleEnforcePolicy(line);
        }
        return true;
    }

    return false;
}

CodeEditor *FindReplacer::findInDocuments(CodeEditor *currentPage, std::vector<CodeEditor *> &pages, FindReplaceOption &fro)
{
    return handleInDocuments(currentPage, pages, fro, [this](auto &&page, auto &&fro) {
        return findInDocument(std::forward<decltype(page)>(page), std::forward<decltype(fro)>(fro));
    });
}

bool FindReplacer::findAllInDocuments(std::vector<CodeEditor *> &pages, FindReplaceOption &fro)
{
    int flags = setFlags(fro);

    for (auto *page : pages)
    {
        Q_ASSERT(page);
        auto  *sci   = page->getFocusView();
        sptr_t start = 0;
        sptr_t end   = sci->textLength();
        if (fro.searchUp)
        {
            end = 0;
        }
        QPair<int, int> findResult = sci->findText(flags, fro.strToFind.toStdString().c_str(), start, end);
        while (findResult.first >= 0)
        {
            sptr_t lineStart = sci->lineFromPosition(findResult.first);
            sptr_t lineEnd   = sci->lineFromPosition(findResult.second);

            sci->setCurrentPos(findResult.second);
            sptr_t start = findResult.second;
            sptr_t end   = sci->textLength();
            findResult   = sci->findText(flags, fro.strToFind.toStdString().c_str(), start, end);
        }
    }
    return true;
}

bool FindReplacer::findAllInDirectory(FindReplaceOption &fro)
{
    return findAllInDirectory(fro.directory, fro, false);
}

bool FindReplacer::findAllInDirectory(const QString &dirPath, FindReplaceOption &fro, bool recursive)
{
    QDir dir(dirPath);
    dir.setNameFilters(fro.filters.split(QChar(';')));
    dir.setFilter(QDir::Files | QDir::NoSymLinks);

    QFileInfoList fileInfos = dir.entryInfoList();
    int           count     = static_cast<int>(fileInfos.size());

#    pragma omp parallel for
    for (int i = 0; i < count; i++)
    {
        const auto &fileInfo = fileInfos.at(i);
        findAllInFile(fileInfo.absoluteFilePath(), fro);
    }

    if (recursive)
    {
        dir.setNameFilters(QStringList());
        dir.setFilter(QDir::NoSymLinks | QDir::Dirs | QDir::NoDotAndDotDot);

        QFileInfoList fileInfos = dir.entryInfoList();
        int           count     = static_cast<int>(fileInfos.size());

        for (int i = 0; i < count; i++)
        {
            const auto &fileInfo = fileInfos.at(i);
            findAllInDirectory(fileInfo.absoluteFilePath(), fro, true);
        }
    }

    return true;
}

bool FindReplacer::findAllInDirectories(FindReplaceOption &fro)
{
    return findAllInDirectory(fro.directory, fro, true);
}

bool FindReplacer::replaceInDocument(CodeEditor *page, FindReplaceOption &fro)
{
    int            flags = setFlags(fro);
    ScintillaEdit *sci   = page->getFocusView();
    sptr_t         start = sci->currentPos();
    sptr_t         end   = sci->textLength();
    if (fro.searchUp)
    {
        end = 0;
    }
    QPair<int, int> findResult = sci->findText(flags, fro.strToFind.toStdString().c_str(), start, end);
    if (findResult.first >= 0)
    {
        sci->grabFocus();
        sci->setSel(findResult.first, findResult.second);
        sci->replaceSel(fro.strReplaceWith.toStdString().c_str());
        sptr_t newEnd = findResult.first + fro.strReplaceWith.length();

        sptr_t endStyled = sci->endStyled();
        if (endStyled < newEnd)
        {
            sci->colourise(endStyled, newEnd);
        }
        sptr_t lineStart = sci->lineFromPosition(findResult.first);
        sptr_t lineEnd   = sci->lineFromPosition(newEnd);
        for (sptr_t line = lineStart; line <= lineEnd; line++)
        {
            sci->ensureVisibleEnforcePolicy(line);
        }
        return true;
    }

    return false;
}

CodeEditor *FindReplacer::replaceInDocuments(CodeEditor *currentPage, std::vector<CodeEditor *> &pages, FindReplaceOption &fro)
{
    return handleInDocuments(currentPage, pages, fro, [this](auto &&page, auto &&fro) {
        return replaceInDocument(std::forward<decltype(page)>(page), std::forward<decltype(fro)>(fro));
    });
}

bool FindReplacer::replaceAllInDocument(CodeEditor *page, FindReplaceOption &fro)
{
    int            flags = setFlags(fro);
    ScintillaEdit *sci   = page->getFocusView();
    sptr_t         start = 0;
    sptr_t         end   = sci->textLength();
    if (fro.searchUp)
    {
        end = 0;
    }
    QPair<int, int> findResult = sci->findText(flags, fro.strToFind.toStdString().c_str(), start, end);
    while (findResult.first >= 0)
    {
        sci->setSel(findResult.first, findResult.second);
        sci->replaceSel(fro.strReplaceWith.toStdString().c_str());
        sptr_t newEnd = findResult.first + fro.strReplaceWith.length();

        sptr_t start = newEnd;
        sptr_t end   = sci->textLength();
        findResult   = sci->findText(flags, fro.strToFind.toStdString().c_str(), start, end);
    }
    return true;
}

bool FindReplacer::replaceAllInDocuments(std::vector<CodeEditor *> &pages, FindReplaceOption &fro)
{
    std::for_each(pages.begin(), pages.end(), [this, &fro](auto &&page) { return replaceAllInDocument(std::forward<decltype(page)>(page), fro); });
    return true;
}

bool FindReplacer::replaceAllRegexpInFile(const QString &filePath, FindReplaceOption &fro)
{
    // 使用QFile读取文件
    // 在文件中查找fro.strToFind正则表达式，要匹配fro.matchCase和fro.matchWholeWord标志
    // 然后将其替换为fro.strReplaceWith
    // 最后写回文件
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite))
    {
        return false;
    }
    QFileInfo   fileInfo(file);
    QTextStream in(&file);

    QString tempFileName = QDir::tempPath() + "/." + fileInfo.fileName() + ".tmp";
    QFile   tempFile(tempFileName);
    if (!tempFile.open(QIODevice::WriteOnly))
    {
        file.close();
        return false;
    }

    QTextStream out(&tempFile);

    QString searchString  = fro.strToFind;
    QString replaceString = fro.strReplaceWith;

    if (fro.matchWholeWord)
    {
        searchString = "\\b" + searchString + "\\b";
    }

    QRegularExpression regex(searchString);
    if (!fro.matchCase)
    {
        regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    }

    while (!in.atEnd())
    {
        QString line = in.readLine();
        line.replace(regex, replaceString);
        out << line << "\n";
    }

    file.resize(0);
    file.close();

    tempFile.rename(filePath);
    tempFile.close();
    return true;
}

bool FindReplacer::replaceAllStringInFile(const QString &filePath, FindReplaceOption &fro)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    // Read entire file into buffer
    QByteArray fileData = file.readAll();
    file.close();

    if (fileData.isEmpty())
    {
        return true;
    }

    // Convert search and replace strings to UTF-8
    QByteArray searchBytes  = fro.strToFind.toUtf8();
    QByteArray replaceBytes = fro.strReplaceWith.toUtf8();

    // Use Highway-accelerated search to find all matches
    std::vector<size_t> matches =
        FindReplaceInternal::findAllOccurrences(fileData.constData(), fileData.size(), searchBytes.constData(), searchBytes.size(), fro.matchCase);

    if (matches.empty())
    {
        return true; // No matches, file unchanged
    }

    // Handle whole word matching if required
    if (fro.matchWholeWord)
    {
        std::vector<size_t> filteredMatches;
        filteredMatches.reserve(matches.size());

        // Work directly on byte buffer - word characters are all ASCII
        auto isWordChar = [](char c) -> bool { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_'; };

        for (size_t matchPos : matches)
        {
            // Check byte before match
            bool validBefore = (matchPos == 0) || !isWordChar(fileData[matchPos - 1]);

            // Check byte after match
            size_t afterPos   = matchPos + searchBytes.size();
            bool   validAfter = (afterPos >= static_cast<size_t>(fileData.size())) || !isWordChar(fileData[afterPos]);

            if (validBefore && validAfter)
            {
                filteredMatches.push_back(matchPos);
            }
        }
        matches = std::move(filteredMatches);
    }

    if (matches.empty())
    {
        return true; // No valid matches after whole word filtering
    }

    // Build new content with replacements (reverse order to maintain positions)
    QByteArray newData;
    newData.reserve(fileData.size() + matches.size() * (replaceBytes.size() - searchBytes.size()));

    size_t lastPos = 0;
    for (size_t matchPos : matches)
    {
        // Copy data before match
        newData.append(fileData.mid(lastPos, matchPos - lastPos));
        // Append replacement
        newData.append(replaceBytes);
        // Skip the matched string
        lastPos = matchPos + searchBytes.size();
    }
    // Copy remaining data
    newData.append(fileData.mid(lastPos));

    // Use QSaveFile for atomic, safe file replacement
    QSaveFile saveFile(filePath);
    if (!saveFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    if (saveFile.write(newData) != newData.size())
    {
        saveFile.cancelWriting();
        return false;
    }

    if (!saveFile.commit())
    {
        return false;
    }

    return true;
}

bool FindReplacer::replaceAllInFile(const QString &filePath, FindReplaceOption &fro)
{
    if (fro.regexp)
    {
        return replaceAllRegexpInFile(filePath, fro);
    }

    return replaceAllStringInFile(filePath, fro);
}

bool FindReplacer::replaceAllInDirectory(FindReplaceOption &fro)
{
    return replaceAllInDirectory(fro.directory, fro, false);
}

bool FindReplacer::replaceAllInDirectory(const QString &dirPath, FindReplaceOption &fro, bool recursive)
{
    QDir dir(dirPath);
    dir.setNameFilters(fro.filters.split(QChar(';')));
    dir.setFilter(QDir::Files | QDir::NoSymLinks);

    QFileInfoList &&fileInfos = dir.entryInfoList();
    int             count     = static_cast<int>(fileInfos.size());
#    pragma omp     parallel for
    for (int i = 0; i < count; i++)
    {
        const auto &fileInfo = fileInfos.at(i);
        replaceAllInFile(fileInfo.absoluteFilePath(), fro);
    }

    if (recursive)
    {
        dir.setNameFilters(QStringList());
        dir.setFilter(QDir::NoSymLinks | QDir::Dirs | QDir::NoDotAndDotDot);

        QFileInfoList &&fileInfos = dir.entryInfoList();
        int             count     = static_cast<int>(fileInfos.size());

        for (int i = 0; i < count; i++)
        {
            const auto &fileInfo = fileInfos.at(i);
            replaceAllInDirectory(fileInfo.absoluteFilePath(), fro, true);
        }
    }
    return true;
}

bool FindReplacer::replaceAllInDirectories(FindReplaceOption &fro)
{
    return replaceAllInDirectory(fro.directory, fro, true);
}

int FindReplacer::setFlags(FindReplaceOption &fro)
{
    int flags = 0;
    if (fro.matchCase)
    {
        flags |= SCFIND_MATCHCASE;
    }
    if (fro.matchWholeWord)
    {
        flags |= SCFIND_WHOLEWORD;
    }
    if (fro.regexp)
    {
        flags |= SCFIND_CXX11REGEX;
    }

    return flags;
}

CodeEditor *FindReplacer::handleInDocuments(CodeEditor                                            *currentPage,
                                            std::vector<CodeEditor *>                             &pages,
                                            FindReplaceOption                                     &fro,
                                            std::function<bool(CodeEditor *, FindReplaceOption &)> handleInDocument)
{
    bool inCurrentPage = handleInDocument(currentPage, fro);
    if (inCurrentPage)
    {
        return currentPage;
    }
    bool isSearchUp = fro.searchUp;
    return handlePages(currentPage, pages, fro, handleInDocument, [this, isSearchUp](auto &&currentPage, auto &&pages) {
        if (isSearchUp)
        {
            return previousPage(std::forward<decltype(currentPage)>(currentPage), std::forward<decltype(pages)>(pages));
        }
        return nextPage(std::forward<decltype(currentPage)>(currentPage), std::forward<decltype(pages)>(pages));
    });
}

CodeEditor *FindReplacer::handlePages(CodeEditor                                                            *currentPage,
                                      std::vector<CodeEditor *>                                             &pages,
                                      FindReplaceOption                                                     &fro,
                                      std::function<bool(CodeEditor *, FindReplaceOption &)>                 handleInDocument,
                                      std::function<CodeEditor *(CodeEditor *, std::vector<CodeEditor *> &)> getPage)
{
    auto *page          = getPage(currentPage, pages);
    bool  inCurrentPage = handleInDocument(currentPage, fro);

    while (page && !inCurrentPage)
    {
        page = getPage(page, pages);
        if (!page)
        {
            break;
        }

        inCurrentPage = handleInDocument(currentPage, fro);
    }

    return (inCurrentPage ? page : nullptr);
}

#endif // HWY_ONCE
