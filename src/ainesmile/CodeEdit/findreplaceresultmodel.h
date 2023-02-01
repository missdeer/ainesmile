#ifndef FINDREPLACERESULTMODEL_H
#define FINDREPLACERESULTMODEL_H

#include <QAbstractItemModel>

#include "findreplaceresultitem.h"

class FindReplaceResultModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    static FindReplaceResultModel *instance();

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;

    [[nodiscard]] int           rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    [[nodiscard]] int           columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    bool                        setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    bool insertColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;

private:
    static FindReplaceResultModel *m_instance;
    FindReplaceResultModel();
    ~FindReplaceResultModel() override;

    [[nodiscard]] FindReplaceResultItem *getItem(const QModelIndex &index) const;

    FindReplaceResultItem *rootItem {nullptr};
};

#endif // FINDREPLACERESULTMODEL_H
