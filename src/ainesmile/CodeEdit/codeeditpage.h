﻿#ifndef CODEEDITPAGE_H
#define CODEEDITPAGE_H

#include <vector>

#include <QFocusEvent>
#include <QSplitter>
#include <QWidget>

#include "ScintillaEdit.h"
#include "document.h"
#include "encodingutils.h"

QT_FORWARD_DECLARE_CLASS(QPainter);

class CodeEditor : public QWidget
{
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = nullptr);

    [[nodiscard]] ScintillaEdit *getFocusView();
    bool                         openFile(const QString &filePath);
    void                         saveFile(const QString &filePath);
    void                         reopenAsEncoding(const QString &encoding, bool withBOM);
    void                         saveAsEncoding(const QString &encoding, bool withBOM);
    [[nodiscard]] QString        getFilePath() const;
    [[nodiscard]] bool           canClose();
    [[nodiscard]] bool           canCut();
    [[nodiscard]] bool           canCopy();
    [[nodiscard]] bool           canPaste();
    [[nodiscard]] bool           canUndo();
    [[nodiscard]] bool           canRedo();
    [[nodiscard]] bool           isModified();
    void                         grabFocus();
    [[nodiscard]] bool           focus();
    void                         applyEditorStyles();
    void                         setShowWhiteSpaceAndTAB(bool enabled);
    void                         setShowEndOfLine(bool enabled);
    void                         setShowIndentGuide(bool enabled);
    void                         setShowWrapSymbol(bool enabled);
    [[nodiscard]] bool           getShowWhiteSpaceAndTAB();
    [[nodiscard]] bool           getShowEndOfLine();
    [[nodiscard]] bool           getShowIndentGuide();
    [[nodiscard]] bool           getShowWrapSymbol();
    [[nodiscard]] QString        encoding() const;
    [[nodiscard]] bool           hasBOM();
    [[nodiscard]] bool           isWordWrap() const;
    [[nodiscard]] QString        lexerName() const;
    void                         setLexerName(const QString &lexerName);

    bool initialDocument();
    //    void focusInEvent(QFocusEvent * event);
    [[nodiscard]] bool isSplittered() const;
signals:
    void focusIn();
    void modifiedNotification();
    void filePathChanged(const QString &filePath);
    void openFilesRequest(const QStringList &filePath);
public slots:
    void linesAdded(Scintilla::Position linesAdded);
    void marginClicked(Scintilla::Position position, Scintilla::KeyMod modifiers, int margin);
    void modified(Scintilla::ModificationFlags type,
                  Scintilla::Position          position,
                  Scintilla::Position          length,
                  Scintilla::Position          linesAdded,
                  const QByteArray            &text,
                  Scintilla::Position          line,
                  Scintilla::FoldLevel         foldNow,
                  Scintilla::FoldLevel         foldPrev);
    void updateUI(Scintilla::Update updated);
    void dwellEnd(int x, int y);
    void uriDropped(const QString &uri);
    void focusChanged(bool focused);

    void undo();
    void redo();
    void copy();
    void cut();
    void paste();
    void print(QPainter& painter);
    void deleteCurrent();
    void selectAll();
    void setReadOnly();
    void clearReadOnlyFlag();
    void currentFullFilePathToClipboard();
    void currentFileNameToClipboard();
    void currentDirectoryPathToClipboard();
    void increaseLineIndent();
    void decreaseLineIndent();
    void upperCase();
    void lowerCase();
    void duplicateCurrentLine();
    void splitLines();
    void joinLines();
    void moveUpCurrentLine();
    void moveDownCurrentLine();
    void eolWindowsFormat();
    void eolUNIXFormat();
    void eolMacFormat();
    void trimTrailingSpace();
    void trimLeadingSpace();
    void trimLeadingAndTrailingSpace();
    void eolToSpace();
    void gotoLine();
    void gotoMatchingBrace();
    void toggleBookmark();
    void nextBookmark();
    void previousBookmark();
    void clearAllBookmarks();
    void cutBookmarkLines();
    void copyBookmarkLines();
    void pasteToReplaceBookmarkedLines();
    void removeBookmarkedLines();
    void removeUnbookmarkedLines();
    void inverseBookmark();
    void wordWrap();
    void focusOnAnotherView();
    void zoomIn();
    void zoomOut();
    void restoreDefaultZoom();

    void splitEditorRight();
    void splitEditorDown();
    void cancelSplit();

private:
    QWidget       *m_editorPane;
    QSplitter     *m_editorSplitter {nullptr};
    ScintillaEdit *m_sciControlMaster;
    ScintillaEdit *m_sciControlSlave {nullptr};
    ScintillaEdit *m_sciFocusView;
    QString        m_lexerName {QStringLiteral("normal")};
    ASDocument     m_document;

    void init();
    void initSci(ScintillaEdit *sci);
    void documentChanged();
    void saveFileAsEncoding(const QString &filePath, const QString &encoding, BOM bom);
    void loadDataFromFile();
    void toggleBookmarkAtLine(ScintillaEdit *sci, int line);
    void updateLineNumberMargin(ScintillaEdit *sci);
    void splitEditor(Qt::Orientation orientation, int size);

    static void                deleteLine(ScintillaEdit *sci, sptr_t line);
    static std::vector<sptr_t> bookmarkedLines(ScintillaEdit *sci);
};

#endif // CODEEDITPAGE_H
