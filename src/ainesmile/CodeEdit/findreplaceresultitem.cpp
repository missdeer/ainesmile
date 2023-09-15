#include "stdafx.h"

#include <QStringList>

#include "findreplaceresultitem.h"

FindReplaceResultItem::FindReplaceResultItem(const QVector<QVariant> &data, FindReplaceResultItem *parent) : itemData(data), parentItem(parent) {}

FindReplaceResultItem::~FindReplaceResultItem()
{
    qDeleteAll(childItems);
}

FindReplaceResultItem *FindReplaceResultItem::child(int number)
{
    return childItems.value(number);
}

int FindReplaceResultItem::childCount() const
{
    return childItems.count();
}

int FindReplaceResultItem::childNumber() const
{
    if (parentItem)
    {
        return parentItem->childItems.indexOf(const_cast<FindReplaceResultItem *>(this));
    }

    return 0;
}

int FindReplaceResultItem::columnCount() const
{
    return itemData.count();
}

QVariant FindReplaceResultItem::data(int column) const
{
    return itemData.value(column);
}

bool FindReplaceResultItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > childItems.size())
    {
        return false;
    }

    for (int row = 0; row < count; ++row)
    {
        QVector<QVariant> data(columns);
        auto             *item = new FindReplaceResultItem(data, this);
        childItems.insert(position, item);
    }

    return true;
}

bool FindReplaceResultItem::insertColumns(int position, int columns)
{
    if (position < 0 || position > itemData.size())
    {
        return false;
    }

    for (int column = 0; column < columns; ++column)
    {
        itemData.insert(position, QVariant());
    }

    for (auto *child : childItems)
    {
        child->insertColumns(position, columns);
    }

    return true;
}

FindReplaceResultItem *FindReplaceResultItem::parent()
{
    return parentItem;
}

bool FindReplaceResultItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
    {
        return false;
    }

    for (int row = 0; row < count; ++row)
    {
        delete childItems.takeAt(position);
    }

    return true;
}

bool FindReplaceResultItem::removeColumns(int position, int columns)
{
    if (position < 0 || position + columns > itemData.size())
    {
        return false;
    }

    for (int column = 0; column < columns; ++column)
    {
        itemData.remove(position);
    }

    for (auto *child : childItems)
    {
        child->removeColumns(position, columns);
    }

    return true;
}

bool FindReplaceResultItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
    {
        return false;
    }

    itemData[column] = value;
    return true;
}
