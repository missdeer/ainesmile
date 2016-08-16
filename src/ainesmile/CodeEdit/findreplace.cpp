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
    int start = sci->currentPos();
    int end = sci->textLength();
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
    }
    return true;
}

bool findInDocuments(QList<ScintillaEdit *> &scis, FindReplaceOption &fro)
{
    return true;
}

bool findInDirectory(FindReplaceOption &fro)
{
    return true;
}

bool findInDirectories(FindReplaceOption &fro)
{
    return true;
}

bool replaceInDocument(ScintillaEdit *sci, FindReplaceOption &fro)
{
    return true;
}

bool replaceAllInDocument(ScintillaEdit *sci, FindReplaceOption &fro)
{
    return true;
}

bool replaceAllInDocuments(QList<ScintillaEdit *> &scis, FindReplaceOption &fro)
{
    return true;
}

bool replaceAllInDirectory(FindReplaceOption &fro)
{
    return true;
}

bool replaceAllInDirectories(FindReplaceOption &fro)
{
    return true;
}

}
