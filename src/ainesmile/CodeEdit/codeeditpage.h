#ifndef CODEEDITPAGE_H
#define CODEEDITPAGE_H

#include <QWidget>
#include <QWebEngineView>
#include <QSplitter>
#include <QFocusEvent>
#include "scintillaconfig.h"

class CodeEditPage : public QWidget
{
    Q_OBJECT
private:
    bool m_lastCopyAvailable;
    bool m_lastPasteAvailable;
    bool m_lastRedoAvailable;
    bool m_lastUndoAvailable;
    bool focusIn_;
    QSplitter* m_horizontalMainSplitter;
    QWidget* m_editorPane;
    QWebEngineView* m_webView;
    QSplitter* m_verticalEditorSplitter;
    ScintillaEdit* m_sciControlMaster;
    ScintillaEdit* m_sciControlSlave;

    QString m_filePath;
    ScintillaConfig m_sc;

    void init();
    ScintillaEdit* getFocusView();

public:
    explicit CodeEditPage(QWidget *parent = 0);

    void openFile(const QString& filePath);
    void saveFile(const QString& filePath);
    const QString& getFilePath() const;
    bool canClose();
    bool canCut();
    bool canCopy();
    bool canPaste();
    bool canUndo();
    bool canRedo();
    bool isModified();
    void grabFocus();
    bool focus();
    void applyEditorStyles();
    void setShowWhiteSpaceAndTAB(bool enabled);
    void setShowEndOfLine(bool enabled);
    void setShowIndentGuide(bool enabled);
    void setShowWrapSymbol(bool enabled);

//    void focusInEvent(QFocusEvent * event);
signals:
    void focusIn();
    void copyAvailableChanged();
    void pasteAvailableChanged();
    void redoAvailableChanged();
    void undoAvailableChanged();
    void modifiedNotification();
    void filePathChanged(const QString& filePath);
public slots:
    void updateUI();
    void modified(int type, int position, int length, int linesAdded,
                  const QByteArray &text, int line, int foldNow, int foldPrev);    
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
    void columnMode();
    void columnEditor();
    void characterPanel();
    void clipboardHistory();
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
    void toggleBlockComment();
    void blockComment();
    void blockUncomment();
    void streamComment();
    void functionCompletion();
    void wordCompletion();
    void functionParametersHint();
    void eolWindowsFormat();
    void eolUNIXFormat();
    void eolMacFormat();
    void trimTrailingSpace();
    void trimLeadingSpace();
    void trimLeadingAndTrailingSpace();
    void eolToSpace();
    void removeUnnecessaryBlankAndEOL();
    void tabToSpace();
    void spaceToTab();
    void pasteHTMLContent();
    void pasteRTFContent();
    void copyBinaryContent();
    void cutBinaryContent();
    void pasteBinaryContent();
    void findNext();
    void findPrevious();
    void selectAndFindNext();
    void selectAndFindPrevious();
    void findVolatieNext();
    void findVolatiePrevious();
    void incrementalSearch();
    void gotoLine();
    void gotoMatchingBrace();
    void findCharactersInRange();
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
    void startRecording();
    void abortRecording();
    void saveLastRecording();
    void replayLastRecording();
    void wordWrap();
    void focusOnAnotherView();
    void hideLines();
    void foldAll();
    void unfoldAll();
    void collapseCurrentLevel();
    void uncollapseCurrentLevel();
    void collapseLevel();
    void uncollapseLevel();
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
