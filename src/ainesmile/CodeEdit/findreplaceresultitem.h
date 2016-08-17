#ifndef FINDREPLACERESULTITEM_H
#define FINDREPLACERESULTITEM_H

#include <QVariant>
#include <QVector>
#include <QList>

class FindReplaceResultItem
{
public:
    explicit FindReplaceResultItem(const QVector<QVariant> &data, FindReplaceResultItem *parent = 0);
    ~FindReplaceResultItem();

    FindReplaceResultItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool insertChildren(int position, int count, int columns);
    bool insertColumns(int position, int columns);
    FindReplaceResultItem *parent();
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);
    int childNumber() const;
    bool setData(int column, const QVariant &value);

private:
    QList<FindReplaceResultItem*> childItems;
    QVector<QVariant> itemData;
    FindReplaceResultItem *parentItem;
};

#endif // FINDREPLACERESULTITEM_H
