#ifndef CODEEDITPAGE_H
#define CODEEDITPAGE_H

#include <QFocusEvent>
#include <QSplitter>
#include <QWidget>

#include "scintillaconfig.h"

class CodeEditPage : public QWidget
{
    Q_OBJECT
private:
    bool            m_lastCopyAvailable;
    bool            m_lastPasteAvailable;
    bool            m_lastRedoAvailable;
    bool            m_lastUndoAvailable;
    bool            focusIn_;
    QWidget        *m_editorPane;
    QWidget        *m_webView;
    QSplitter      *m_verticalEditorSplitter;
    ScintillaEdit  *m_sciControlMaster;
    ScintillaEdit  *m_sciControlSlave;
    ScintillaEdit  *m_sciFocusView;
    QString         m_filePath;
    ScintillaConfig m_sc;

    void init();

public:
    explicit CodeEditPage(QWidget *parent = 0);

    ScintillaEdit *getFocusView();
    void           openFile(const QString &filePath);
    void           saveFile(const QString &filePath);
    const QString &getFilePath() const;
    bool           canClose();
    bool           canCut();
    bool           canCopy();
    bool           canPaste();
    bool           canUndo();
    bool           canRedo();
    bool           isModified();
    void           grabFocus();
    bool           focus();
    void           applyEditorStyles();
    void           setShowWhiteSpaceAndTAB(bool enabled);
    void           setShowEndOfLine(bool enabled);
    void           setShowIndentGuide(bool enabled);
    void           setShowWrapSymbol(bool enabled);

    bool initialDocument();
    //    void focusInEvent(QFocusEvent * event);
signals:
    void focusIn();
    void copyAvailableChanged();
    void pasteAvailableChanged();
    void redoAvailableChanged();
    void undoAvailableChanged();
    void modifiedNotification();
    void filePathChanged(const QString &filePath);
public slots:
    void updateUI();
    void modified(int type, int position, int length, int linesAdded, const QByteArray &text, int line, int foldNow, int foldPrev);
    void linesAdded(int linesAdded);
    void marginClicked(int position, int modifiers, int margin);
    void dwellEnd(int x, int y);

    void undo();
    void redo();
    void copy();
    void cut();
    void paste();
    void print();
    void printNow();
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
    void encodeInANSI();
    void encodeInUTF8WithoutBOM();
    void encodeInUTF8();
    void encodeInUCS2BigEndian();
    void encodeInUCS2LittleEndian();
    void convertToANSI();
    void convertToUTF8WithoutBOM();
    void convertToUTF8();
    void convertToUCS2BigEndian();
    void convertToUCS2LittleEndian();
    void zoomIn();
    void zoomOut();
    void restoreDefaultZoom();
};

#endif // CODEEDITPAGE_H
