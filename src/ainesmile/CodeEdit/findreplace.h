/**************************************************************************
**   Author: Fan Yang
**   Email: missdeer@gmail.com
**   License: see the license.txt file
**************************************************************************/
#ifndef FINDREPLACE_H
#define FINDREPLACE_H

class ScintillaEdit;

namespace FindReplace {

enum FindScope {
    FS_DOCUMENT,
    FS_ALLOPENED_DOCUMENT,
    FS_DIRECOTRY,
    FS_DIRECTORY_WITH_SUBDIRECTORY,
};

struct FindReplaceOption {
    bool matchCase;
    bool matchWholeWord;
    bool searchUp;
    bool regexp;
    FindScope scope;
    QString strToFind;
    QString strReplaceWith;
    QString directory;
    QString filters;
};

bool findInDocument(ScintillaEdit* sci, FindReplaceOption& fro);
bool findInDocuments(QList<ScintillaEdit*>& scis, FindReplaceOption& fro);
bool findInDirectory(FindReplaceOption& fro);
bool findInDirectories(FindReplaceOption& fro);
bool replaceInDocument(ScintillaEdit* sci, FindReplaceOption& fro);
bool replaceAllInDocument(ScintillaEdit* sci, FindReplaceOption& fro);
bool replaceAllInDocuments(QList<ScintillaEdit*>& scis, FindReplaceOption& fro);
bool replaceAllInDirectory(FindReplaceOption& fro);
bool replaceAllInDirectories(FindReplaceOption& fro);
}


#endif // FINDREPLACE_H
