#include <QMenu>
#include <QTabBar>
#include "tabwidget.h"

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent),
    contentMenu_(new QMenu(this))
{
    QAction* action = new QAction(tr("Move to the other side"), this);
    connect(action, SIGNAL(triggered()), this, SIGNAL(exchangeTab()));
    contentMenu_->addAction(action);

    tabBar()->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
}

void TabWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        event->accept();
        int index = tabBar()->tabAt(event->pos());
        if (index != -1)
        {
            setCurrentIndex(index);
            contentMenu_->popup(mapToGlobal(event->pos()));
        }
    }
    else
    {
        event->ignore();
    }
}
