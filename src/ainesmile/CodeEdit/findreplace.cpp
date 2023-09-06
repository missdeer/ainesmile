/**************************************************************************
**   Author: Fan Yang
**   Email: missdeer@gmail.com
**   License: see the license.txt file
**************************************************************************/

#include "stdafx.h"

#include "findreplace.h"
#include "codeeditpage.h"

namespace FindReplace
{

    CodeEditor *previousPage(CodeEditor *currentPage, std::vector<CodeEditor *> &pages)
    {
        auto iter = std::find(pages.begin(), pages.end(), currentPage);
        if (pages.end() == iter || pages.begin() == iter)
        {
            return nullptr;
        }

        return *std::prev(iter);
    }

    CodeEditor *nextPage(CodeEditor *currentPage, std::vector<CodeEditor *> &pages)
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

    bool findInDocument(CodeEditor *page, FindReplaceOption &fro)
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

    CodeEditor *findInDocuments(CodeEditor *currentPage, std::vector<CodeEditor *> &pages, FindReplaceOption &fro)
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

    bool findAllInDocuments(std::vector<CodeEditor *> &pages, FindReplaceOption &fro)
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
        for (auto *page : pages)
        {
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

    bool findAllInDirectory(FindReplaceOption &fro)
    {
        QDir dir(fro.directory);
        dir.setFilter(QDir::Files | QDir::NoSymLinks);
        dir.setNameFilters(fro.filters.split(QChar(';')));

        QFileInfoList &&fileInfos = dir.entryInfoList();
        for (auto &fileInfo : fileInfos)
        {
            fileInfo.absoluteFilePath();
        }
        return true;
    }

    bool findAllInDirectories(FindReplaceOption &fro)
    {
        QDir dir(fro.directory);
        dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::AllDirs);
        dir.setNameFilters(fro.filters.split(QChar(';')));
        QDirIterator iter(dir, QDirIterator::Subdirectories);
        while (iter.hasNext())
        {
            QString filePath = iter.next();
        }
        return true;
    }

    bool replaceInDocument(CodeEditor *page, FindReplaceOption &fro)
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

    CodeEditor *replaceInDocuments(CodeEditor *currentPage, std::vector<CodeEditor *> &pages, FindReplaceOption &fro)
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

    bool replaceAllInDocument(CodeEditor *page, FindReplaceOption &fro)
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

    bool replaceAllInDocuments(std::vector<CodeEditor *> &pages, FindReplaceOption &fro)
    {
        std::for_each(pages.begin(), pages.end(), [&fro](auto &&PH1) { return replaceAllInDocument(std::forward<decltype(PH1)>(PH1), fro); });
        return true;
    }

    bool replaceAllInDirectory(FindReplaceOption &fro)
    {
        QDir dir(fro.directory);
        dir.setFilter(QDir::Files | QDir::NoSymLinks);
        dir.setNameFilters(fro.filters.split(QChar(';')));

        QFileInfoList &&fileInfos = dir.entryInfoList();
        for (auto &fileInfo : fileInfos)
        {
            fileInfo.absoluteFilePath();
        }
        return true;
    }

    bool replaceAllInDirectories(FindReplaceOption &fro)
    {
        QDir dir(fro.directory);
        dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::AllDirs);
        dir.setNameFilters(fro.filters.split(QChar(';')));
        QDirIterator iter(dir, QDirIterator::Subdirectories);
        while (iter.hasNext())
        {
            QString filePath = iter.next();
        }
        return true;
    }
} // namespace FindReplace
