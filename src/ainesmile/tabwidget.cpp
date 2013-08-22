#include <QMenu>
#include "tabwidget.h"

TabWidget::TabWidget(QWidget *parent) :
    QTabWidget(parent),
    contentMenu_(new QMenu(this))
{
    QAction* action = new QAction(tr("Exchange"), this);
    connect(action, SIGNAL(triggered()), this, SIGNAL(exchangeTab()));
    contentMenu_->addAction(action);
}

void TabWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        event->accept();
        contentMenu_->popup(mapToGlobal(event->pos()));
    }
    else
    {
        event->ignore();
    }
}
