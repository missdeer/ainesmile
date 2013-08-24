#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QMouseEvent>
#include <QMenu>
#include <QFileInfo>

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = 0);
    
    bool focus();
    void doCloseRequested(int index);
    bool fileExists(const QString& filePath);
    bool fileExists(const QFileInfo& fileInfo);
    int openFile(const QString& filePath);
    int newDocument(const QString& title);
protected:
    void mousePressEvent(QMouseEvent * event);
signals:
    void exchangeTab();
public slots:
    
private:
    QMenu* contentMenu_;
};

#endif // TABWIDGET_H
