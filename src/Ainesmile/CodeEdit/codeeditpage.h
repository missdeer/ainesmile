#ifndef CODEEDITPAGE_H
#define CODEEDITPAGE_H

#include <QWidget>
#include <QWebView>
#include <QSplitter>
#include "ScintillaEditBase.h"

class CodeEditPage : public QWidget
{
    Q_OBJECT
private:
    QSplitter* m_splitter;
    ScintillaEditBase* m_sciControlMaster;
    ScintillaEditBase* m_sciControlSlave;
    QWebView* m_webView;

    QString m_filePath;
    void init();
public:
    explicit CodeEditPage(QWidget *parent = 0);

    void openFile(const QString& filePath);
    void saveFile(const QString& filePath);
    const QString& getFilePath() const;
    bool canClose();
signals:
    
public slots:

};

#endif // CODEEDITPAGE_H
