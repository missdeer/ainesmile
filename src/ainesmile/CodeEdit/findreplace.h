﻿/**************************************************************************
**   Author: Fan Yang
**   Email: missdeer@gmail.com
**   License: see the license.txt file
**************************************************************************/
#ifndef FINDREPLACE_H
#define FINDREPLACE_H

#include <vector>

#include <QObject>
#include <QString>


class CodeEditor;

namespace FindReplaceConst
{
    inline const QString FS_DOCUMENT                      = QStringLiteral("Current Document");
    inline const QString FS_ALLOPENED_DOCUMENTS           = QStringLiteral("All Opened Documents");
    inline const QString FS_DIRECTORY                     = QStringLiteral("Directory");
    inline const QString FS_DIRECTORY_WITH_SUBDIRECTORIES = QStringLiteral("Directory with Sub-directories");
} // namespace FindReplaceConst

struct FindReplaceOption
{
    bool    matchCase;
    bool    matchWholeWord;
    bool    searchUp;
    bool    regexp;
    QString scope;
    QString strToFind;
    QString strReplaceWith;
    QString directory;
    QString filters;
};

class FindReplacer : public QObject
{
    Q_OBJECT

public:
    explicit FindReplacer(QObject *parent = nullptr) : QObject(parent) {}

    // find in current document
    bool findInDocument(CodeEditor *page, FindReplaceOption &fro);
    //  find in all opened documents
    CodeEditor *findInDocuments(CodeEditor *currentPage, std::vector<CodeEditor *> &pages, FindReplaceOption &fro);
    //  find all results in all opened documents
    bool findAllInDocuments(std::vector<CodeEditor *> &pages, FindReplaceOption &fro);
    //  find all result in directory
    bool findAllInDirectory(FindReplaceOption &fro);
    //  find all results in directory with it's sub-directories
    bool findAllInDirectories(FindReplaceOption &fro);
    // replace in current document
    bool replaceInDocument(CodeEditor *page, FindReplaceOption &fro);
    //  replace in all opened documents
    CodeEditor *replaceInDocuments(CodeEditor *currentPage, std::vector<CodeEditor *> &pages, FindReplaceOption &fro);
    // replace all in current document
    bool replaceAllInDocument(CodeEditor *page, FindReplaceOption &fro);
    //  replace all in all opened documents
    bool replaceAllInDocuments(std::vector<CodeEditor *> &pages, FindReplaceOption &fro);
    //  replace all result in directory
    bool replaceAllInDirectory(FindReplaceOption &fro);
    //  replace all results in directory with it's sub-directories
    bool replaceAllInDirectories(FindReplaceOption &fro);

signals:

private:
    CodeEditor *previousPage(CodeEditor *currentPage, std::vector<CodeEditor *> &pages);
    CodeEditor *nextPage(CodeEditor *currentPage, std::vector<CodeEditor *> &pages);
};

#endif // FINDREPLACE_H
