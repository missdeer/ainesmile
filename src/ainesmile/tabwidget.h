#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QMouseEvent>
#include <QMenu>

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = 0);
    
    void mousePressEvent(QMouseEvent * event);
signals:
    void exchangeTab();
public slots:
    
private:
    QMenu* contentMenu_;
};

#endif // TABWIDGET_H
