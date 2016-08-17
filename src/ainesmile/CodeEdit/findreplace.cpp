/**************************************************************************
**   Author: Fan Yang
**   Email: missdeer@gmail.com
**   License: see the license.txt file
**************************************************************************/

#include "stdafx.h"
#include "ScintillaEdit.h"
#include "findreplace.h"


namespace FindReplace {

bool findInDocument(ScintillaEdit *sci, FindReplaceOption &fro)
{
    int flags = 0;
    if (fro.matchCase)
        flags |= SCFIND_MATCHCASE;
    if (fro.matchWholeWord)
        flags |= SCFIND_WHOLEWORD;
    if (fro.regexp)
        flags |= SCFIND_CXX11REGEX;
    sptr_t start = sci->currentPos();
    sptr_t end = sci->textLength();
    if (fro.searchUp)
        std::swap(start, end);
    QPair<int, int> p = sci->findText(flags, fro.strToFind.toStdString().c_str(), start, end);
    if (p.first >= 0)
    {
        sci->grabFocus();
        sci->gotoPos(p.second);
        sci->setCurrentPos(p.second);
        sci->setSel(p.first, p.second);

        sptr_t endStyled = sci->endStyled();
        if (endStyled < p.second)
            sci->colourise(endStyled, p.second);
        sptr_t lineStart = sci->lineFromPosition(p.first);
        sptr_t lineEnd = sci->lineFromPosition(p.second);
        for (sptr_t line = lineStart; line <=lineEnd; line++)
            sci->ensureVisibleEnforcePolicy(line);
        return true;
    }

    return false;
}

bool findAllInDocuments(QList<ScintillaEdit *> &scis, FindReplaceOption &fro)
{
    int flags = 0;
    if (fro.matchCase)
        flags |= SCFIND_MATCHCASE;
    if (fro.matchWholeWord)
        flags |= SCFIND_WHOLEWORD;
    if (fro.regexp)
        flags |= SCFIND_CXX11REGEX;
    for (ScintillaEdit* sci: scis)
    {
        sptr_t start = 0;
        sptr_t end = sci->textLength();
        if (fro.searchUp)
            std::swap(start, end);
        QPair<int, int> p = sci->findText(flags, fro.strToFind.toStdString().c_str(), start, end);
        while (p.first >= 0)
        {
            sptr_t lineStart = sci->lineFromPosition(p.first);
            sptr_t lineEnd = sci->lineFromPosition(p.second);

            sci->setCurrentPos(p.second);
            sptr_t start = p.second;
            sptr_t end = sci->textLength();
            p = sci->findText(flags, fro.strToFind.toStdString().c_str(), start, end);
        }
    }
    return true;
}

bool findAllInDirectory(FindReplaceOption &fro)
{
    QDir dir(fro.directory);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(fro.filters.split(QChar(';')));

    QFileInfoList&& files = dir.entryInfoList();
    for (auto fi : files)
    {
        fi.absoluteFilePath();
    }
    return true;
}

bool findAllInDirectories(FindReplaceOption &fro)
{
    QDir dir(fro.directory);
    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::AllDirs);
    dir.setNameFilters(fro.filters.split(QChar(';')));
    QDirIterator it(dir, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
    }
    return true;
}

bool replaceInDocument(ScintillaEdit *sci, FindReplaceOption &fro)
{
    int flags = 0;
    if (fro.matchCase)
        flags |= SCFIND_MATCHCASE;
    if (fro.matchWholeWord)
        flags |= SCFIND_WHOLEWORD;
    if (fro.regexp)
        flags |= SCFIND_CXX11REGEX;
    sptr_t start = sci->currentPos();
    sptr_t end = sci->textLength();
    if (fro.searchUp)
        std::swap(start, end);
    QPair<int, int> p = sci->findText(flags, fro.strToFind.toStdString().c_str(), start, end);
    if (p.first >= 0)
    {
        sci->grabFocus();
        sci->setSel(p.first, p.second);
        sci->replaceSel(fro.strReplaceWith.toStdString().c_str());
        sptr_t newEnd = p.first + fro.strReplaceWith.length();
        sci->gotoPos(newEnd);
        sci->setCurrentPos(newEnd);

        sptr_t endStyled = sci->endStyled();
        if (endStyled < newEnd)
            sci->colourise(endStyled, newEnd);
        sptr_t lineStart = sci->lineFromPosition(p.first);
        sptr_t lineEnd = sci->lineFromPosition(newEnd);
        for (sptr_t line = lineStart; line <= lineEnd; line++)
            sci->ensureVisibleEnforcePolicy(line);
        return true;
    }

    return false;
}

bool replaceAllInDocument(ScintillaEdit *sci, FindReplaceOption &fro)
{
    int flags = 0;
    if (fro.matchCase)
        flags |= SCFIND_MATCHCASE;
    if (fro.matchWholeWord)
        flags |= SCFIND_WHOLEWORD;
    if (fro.regexp)
        flags |= SCFIND_CXX11REGEX;
    sptr_t start = 0;
    sptr_t end = sci->textLength();
    if (fro.searchUp)
        std::swap(start, end);
    QPair<int, int> p = sci->findText(flags, fro.strToFind.toStdString().c_str(), start, end);
    while (p.first >= 0)
    {
        sci->setSel(p.first, p.second);
        sci->replaceSel(fro.strReplaceWith.toStdString().c_str());
        sptr_t newEnd = p.first + fro.strReplaceWith.length();
        sci->setCurrentPos(newEnd);

        sptr_t start = newEnd;
        sptr_t end = sci->textLength();
        p = sci->findText(flags, fro.strToFind.toStdString().c_str(), start, end);
    }
    return true;
}

bool replaceAllInDocuments(QList<ScintillaEdit *> &scis, FindReplaceOption &fro)
{
    std::for_each(scis.begin(), scis.end(), std::bind(&replaceAllInDocument, std::placeholders::_1, std::ref(fro)));
    return true;
}

bool replaceAllInDirectory(FindReplaceOption &fro)
{
    QDir dir(fro.directory);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setNameFilters(fro.filters.split(QChar(';')));

    QFileInfoList&& files = dir.entryInfoList();
    for (auto fi : files)
    {
        fi.absoluteFilePath();
    }
    return true;
}

bool replaceAllInDirectories(FindReplaceOption &fro)
{
    QDir dir(fro.directory);
    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::AllDirs);
    dir.setNameFilters(fro.filters.split(QChar(';')));
    QDirIterator it(dir, QDirIterator::Subdirectories);
    while (it.hasNext()) 
    {
    }
    return true;
}

}
