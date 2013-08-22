#include <QMenu>
#include "tabwidget.h"

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent)
{
}

void TabWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        event->accept();
        QMenu* menu = new QMenu(this);
        menu->addAction(tr("Move to the other side"));
        menu->popup(mapToGlobal(event->pos()));
    }
    else
    {
        event->ignore();
    }
}
