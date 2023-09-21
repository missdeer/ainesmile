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
    FindResultNotifier notifier;
    connect(
        &notifier, &FindResultNotifier::found, FindReplaceResultModel::instance(), &FindReplaceResultModel::onAddFindResult, Qt::QueuedConnection);

    auto        insensitive = (fro.matchCase ? Qt::CaseSensitive : Qt::CaseInsensitive);
    QTextStream instream(&file);
    int         lineNr = 0;
    while (!instream.atEnd())
    {
        QString line = instream.readLine();
        lineNr++;
        int  startPos = 0;
        auto pos      = line.indexOf(fro.strToFind, startPos, insensitive);
        while (pos >= 0)
        {
            // notify find result
            int  from    = std::min((int)pos - 10, 0);
            auto context = line.mid(from, fro.strToFind.length() + 20);
            notifier.addResult(filePath, context, lineNr, from, fro.strToFind.length());

            // find next
            startPos = pos + fro.strToFind.length();
            pos      = line.indexOf(fro.strToFind, startPos, insensitive);
        }
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
            int  from    = std::min((int)pos - 10, 0);
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
    ScintillaEdit *sci   = page->getFocusView();
    int            flags = 0;
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
    sptr_t start = sci->currentPos();
    sptr_t end   = sci->textLength();
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
    bool inCurrentPage = findInDocument(currentPage, fro);
    if (inCurrentPage)
    {
        return currentPage;
    }
    CodeEditor *page = nullptr;
    if (fro.searchUp)
    {
        page          = previousPage(currentPage, pages);
        inCurrentPage = findInDocument(currentPage, fro);
        while (page && !inCurrentPage)
        {
            page = previousPage(page, pages);
            if (!page)
            {
                break;
            }
            inCurrentPage = findInDocument(currentPage, fro);
        }
    }
    else
    {
        //  get next page
        page          = nextPage(currentPage, pages);
        inCurrentPage = findInDocument(currentPage, fro);
        while (page && !inCurrentPage)
        {
            page = nextPage(page, pages);
            if (!page)
            {
                break;
            }
            inCurrentPage = findInDocument(currentPage, fro);
        }
    }
    if (inCurrentPage)
    {
        return page;
    }
    return nullptr;
}

bool FindReplacer::findAllInDocuments(std::vector<CodeEditor *> &pages, FindReplaceOption &fro)
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

    int count = static_cast<int>(pages.size());

    for (int i = 0; i < count; i++)
    {
        auto  *page  = pages.at(i);
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

#pragma omp parallel for
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
    ScintillaEdit *sci   = page->getFocusView();
    int            flags = 0;
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
    sptr_t start = sci->currentPos();
    sptr_t end   = sci->textLength();
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
    bool inCurrentPage = replaceInDocument(currentPage, fro);
    if (inCurrentPage)
    {
        return currentPage;
    }
    CodeEditor *page = nullptr;
    if (fro.searchUp)
    {
        page          = previousPage(currentPage, pages);
        inCurrentPage = replaceInDocument(currentPage, fro);
        while (page && !inCurrentPage)
        {
            page = previousPage(page, pages);
            if (!page)
            {
                break;
            }
            inCurrentPage = replaceInDocument(currentPage, fro);
        }
    }
    else
    {
        //  get next page
        page          = nextPage(currentPage, pages);
        inCurrentPage = replaceInDocument(currentPage, fro);
        while (page && !inCurrentPage)
        {
            page = nextPage(page, pages);
            if (!page)
            {
                break;
            }
            inCurrentPage = replaceInDocument(currentPage, fro);
        }
    }
    if (inCurrentPage)
    {
        return page;
    }
    return nullptr;
}

bool FindReplacer::replaceAllInDocument(CodeEditor *page, FindReplaceOption &fro)
{
    ScintillaEdit *sci   = page->getFocusView();
    int            flags = 0;
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
    sptr_t start = 0;
    sptr_t end   = sci->textLength();
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
    std::for_each(pages.begin(), pages.end(), [this, &fro](auto &&PH1) { return replaceAllInDocument(std::forward<decltype(PH1)>(PH1), fro); });
    return true;
}

bool FindReplacer::replaceAllInFile(const QString &filePath, FindReplaceOption &fro)
{
    return false;
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
#pragma omp parallel for
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
