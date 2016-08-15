/**************************************************************************
**   Author: Fan Yang
**   Email: missdeer@gmail.com
**   License: see the license.txt file
**************************************************************************/

#include "stdafx.h"
#include "ScintillaEdit.h"
#include "findreplace.h"


namespace FindReplace {

bool findInDocument(ScintillaEdit *doc, FindReplaceOption &fro)
{
    return true;
}

bool findInDocuments(QList<ScintillaEdit *> &docs, FindReplaceOption &fro)
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

bool replaceInDocument(ScintillaEdit *doc, FindReplaceOption &fro)
{
    return true;
}

bool replaceAllInDocument(ScintillaEdit *doc, FindReplaceOption &fro)
{
    return true;
}

bool replaceAllInDocuments(QList<ScintillaEdit *> &docs, FindReplaceOption &fro)
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
