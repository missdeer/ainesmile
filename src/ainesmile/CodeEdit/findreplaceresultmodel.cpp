#include "stdafx.h"

#include "findreplaceresultmodel.h"

FindReplaceResultModel *FindReplaceResultModel::m_instance = nullptr;

FindReplaceResultModel *FindReplaceResultModel::instance()
{
    if (!m_instance)
    {
        m_instance = new FindReplaceResultModel;
    }

    return m_instance;
}

QVariant FindReplaceResultModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return {};
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole)
    {
        return {};
    }

    FindReplaceResultItem *item = getItem(index);

    return item->data(index.column());
}

QVariant FindReplaceResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return rootItem->data(section);
    }

    return {};
}

QModelIndex FindReplaceResultModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
    {
        return {};
    }

    FindReplaceResultItem *parentItem = getItem(parent);

    FindReplaceResultItem *childItem = parentItem->child(row);
    if (childItem)
    {
        return createIndex(row, column, childItem);
    }

    return {};
}

QModelIndex FindReplaceResultModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return {};
    }

    FindReplaceResultItem *childItem  = getItem(index);
    FindReplaceResultItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
    {
        return {};
    }

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

int FindReplaceResultModel::rowCount(const QModelIndex &parent) const
{
    FindReplaceResultItem *parentItem = getItem(parent);

    return parentItem->childCount();
}

int FindReplaceResultModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return rootItem->columnCount();
}

Qt::ItemFlags FindReplaceResultModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

bool FindReplaceResultModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
    {
        return false;
    }

    auto *item   = getItem(index);
    bool  result = item->setData(index.column(), value);

    if (result)
    {
        emit dataChanged(index, index);
    }

    return result;
}

bool FindReplaceResultModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
    {
        return false;
    }

    bool result = rootItem->setData(section, value);

    if (result)
    {
        emit headerDataChanged(orientation, section, section);
    }

    return result;
}

bool FindReplaceResultModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    beginInsertColumns(parent, position, position + columns - 1);
    bool success = rootItem->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool FindReplaceResultModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    beginRemoveColumns(parent, position, position + columns - 1);
    bool success = rootItem->removeColumns(position, columns);
    endRemoveColumns();

    if (rootItem->columnCount() == 0)
    {
        removeRows(0, rowCount());
    }

    return success;
}

bool FindReplaceResultModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    FindReplaceResultItem *parentItem = getItem(parent);

    beginInsertRows(parent, position, position + rows - 1);
    bool success = parentItem->insertChildren(position, rows, rootItem->columnCount());
    endInsertRows();

    return success;
}

bool FindReplaceResultModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    FindReplaceResultItem *parentItem = getItem(parent);

    beginRemoveRows(parent, position, position + rows - 1);
    bool success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

FindReplaceResultModel::FindReplaceResultModel(QObject *parent) : QAbstractItemModel(parent)
{
    QVector<QVariant> rootData {tr("File"), tr("Position"), tr("Context")};

    rootItem = new FindReplaceResultItem(rootData);
}

FindReplaceResultModel::~FindReplaceResultModel()
{
    delete rootItem;
}

FindReplaceResultItem *FindReplaceResultModel::getItem(const QModelIndex &index) const
{
    if (index.isValid())
    {
        auto *item = static_cast<FindReplaceResultItem *>(index.internalPointer());
        if (item)
        {
            return item;
        }
    }
    return rootItem;
}

void FindReplaceResultModel::onAddFindResult(const QString &filePath, const QString &context, int line, int column, int length) {}
