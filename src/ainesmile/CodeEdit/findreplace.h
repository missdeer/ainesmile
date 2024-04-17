/**************************************************************************
**   Author: Fan Yang
**   Email: missdeer@gmail.com
**   License: see the license.txt file
**************************************************************************/
#ifndef FINDREPLACE_H
#define FINDREPLACE_H

#include <vector>

#include <QCoreApplication>
#include <QObject>

class CodeEditor;

namespace FindReplaceConst
{
    inline QString FS_DOCUMENT;
    inline QString FS_ALLOPENED_DOCUMENTS;
    inline QString FS_DIRECTORY;
    inline QString FS_DIRECTORY_WITH_SUBDIRECTORIES;
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
    CodeEditor *handleInDocuments(CodeEditor                                            *currentPage,
                                  std::vector<CodeEditor *>                             &pages,
                                  FindReplaceOption                                     &fro,
                                  std::function<bool(CodeEditor *, FindReplaceOption &)> handleInDocument);

    CodeEditor *handlePages(CodeEditor                                                            *currentPage,
                            std::vector<CodeEditor *>                                             &pages,
                            FindReplaceOption                                                     &fro,
                            std::function<bool(CodeEditor *, FindReplaceOption &)>                 handleInDocument,
                            std::function<CodeEditor *(CodeEditor *, std::vector<CodeEditor *> &)> getPage);

    bool findAllInFile(const QString &filePath, FindReplaceOption &fro);
    bool findAllStringInFile(const QString &filePath, FindReplaceOption &fro);
    bool findAllRegexpInFile(const QString &filePath, FindReplaceOption &fro);
    bool findAllInDirectory(const QString &dirPath, FindReplaceOption &fro, bool recursive);
    bool replaceAllInFile(const QString &filePath, FindReplaceOption &fro);
    bool replaceAllStringInFile(const QString &filePath, FindReplaceOption &fro);
    bool replaceAllRegexpInFile(const QString &filePath, FindReplaceOption &fro);
    bool replaceAllInDirectory(const QString &dirPath, FindReplaceOption &fro, bool recursive);
    int  setFlags(FindReplaceOption &fro);
};

#endif // FINDREPLACE_H
