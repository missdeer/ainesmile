#ifndef CODEEDITPAGE_H
#define CODEEDITPAGE_H

#include <QWidget>
#include <QWebView>
#include <QSplitter>
#include "ScintillaEdit.h"

class CodeEditPage : public QWidget
{
    Q_OBJECT
private:
    bool m_lastCopyAvailable;
    bool m_lastPasteAvailable;
    bool m_lastRedoAvailable;
    bool m_lastUndoAvailable;
    QSplitter* m_splitter;
    ScintillaEdit* m_sciControlMaster;
    ScintillaEdit* m_sciControlSlave;
    QWebView* m_webView;

    QString m_filePath;
    void init();
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
signals:
    void copyAvailableChanged();
    void pasteAvailableChanged();
    void redoAvailableChanged();
    void undoAvailableChanged();
    void modifiedNotification();
public slots:
    void updateUI();
    void modified(int type, int position, int length, int linesAdded,
                  const QByteArray &text, int line, int foldNow, int foldPrev);

    void undo();
    void redo();
    void copy();
    void cut();
    void paste();
};

#endif // CODEEDITPAGE_H
