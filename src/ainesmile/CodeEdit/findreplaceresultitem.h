#ifndef FINDREPLACERESULTITEM_H
#define FINDREPLACERESULTITEM_H

#include <QList>
#include <QVariant>
#include <QVector>

class FindReplaceResultItem
{
public:
    explicit FindReplaceResultItem(const QVector<QVariant> &data, FindReplaceResultItem *parent = 0);
    ~FindReplaceResultItem();

    FindReplaceResultItem *child(int number);
    [[nodiscard]] int      childCount() const;
    [[nodiscard]] int      columnCount() const;
    [[nodiscard]] QVariant data(int column) const;
    bool                   insertChildren(int position, int count, int columns);
    bool                   insertColumns(int position, int columns);
    FindReplaceResultItem *parent();
    bool                   removeChildren(int position, int count);
    bool                   removeColumns(int position, int columns);
    [[nodiscard]] int      childNumber() const;
    bool                   setData(int column, const QVariant &value);

private:
    QList<FindReplaceResultItem *> childItems;
    QVector<QVariant>              itemData;
    FindReplaceResultItem         *parentItem {nullptr};
};

#endif // FINDREPLACERESULTITEM_H
