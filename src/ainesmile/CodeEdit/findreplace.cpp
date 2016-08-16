/**************************************************************************
**   Author: Fan Yang
**   Email: missdeer@gmail.com
**   License: see the license.txt file
**************************************************************************/

#include "stdafx.h"
#include "ScintillaEdit.h"
#include "mainwindow.h"
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

    QPair<int, int> p = sci->findText(flags, fro.strToFind.toStdString().c_str(), sci->currentPos(), sci->textLength());
    qDebug() << flags << fro.strToFind << sci->currentPos() << sci->textLength() << p << sci->lineFromPosition(p.first);
    if (p.first >= 0)
    {
//        g_mainWindow->activateWindow();
//        g_mainWindow->raise();
//        sci->activateWindow();
//        sci->raise();
//        sci->setFocus(true);
        sci->grabFocus();
        sci->gotoPos(p.second);
        sci->setCurrentPos(p.second);
        sci->setSel(p.first, p.second);
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
