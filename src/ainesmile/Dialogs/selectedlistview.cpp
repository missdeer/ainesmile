#include "stdafx.h"
#include "selectedlistview.h"

SelectedListView::SelectedListView(QWidget *parent) :
    QListView(parent)
{
}

void SelectedListView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    emit itemSelected(selected, deselected);
}
