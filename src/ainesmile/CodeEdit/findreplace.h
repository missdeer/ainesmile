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

bool findInDocument(ScintillaEdit* doc, FindReplaceOption& fro);
bool findInDocuments(QList<ScintillaEdit*>& docs, FindReplaceOption& fro);
bool findInDirectory(const QString& directory, FindReplaceOption& fro);
bool findInDirectories(const QString& directory, FindReplaceOption& fro);
bool replaceInDocument(ScintillaEdit* doc, FindReplaceOption& fro);
bool replaceAllInDocument(ScintillaEdit* doc, FindReplaceOption& fro);
bool replaceAllInDocuments(QList<ScintillaEdit*>& docs, FindReplaceOption& fro);
bool replaceAllInDirectory(const QString& directory, FindReplaceOption& fro);
bool replaceAllInDirectories(const QString& directory, FindReplaceOption& fro);
}


#endif // FINDREPLACE_H
