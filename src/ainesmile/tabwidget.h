#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include <QMouseEvent>

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = 0);
    
    void mousePressEvent(QMouseEvent * event);
signals:
    
public slots:
    
};

#endif // TABWIDGET_H
