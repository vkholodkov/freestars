
#include "production_queue_model.h"

namespace FreeStars {

ProductionQueueModel::ProductionQueueModel(const deque<ProdOrder*> &_production_queue, QObject *parent)
    : QAbstractTableModel(parent)
    , production_queue(_production_queue.begin(), _production_queue.end())
{
}

int ProductionQueueModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return production_queue.size();
}

int ProductionQueueModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant ProductionQueueModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::TextAlignmentRole) {
        return index.column() == 0 ? Qt::AlignLeft : Qt::AlignRight;
    }

    if (index.row() >= production_queue.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        ProdOrder *p = production_queue.at(index.row());

        if (index.column() == 0)
            return QString(p->TypeToString().c_str());
        else if (index.column() == 1)
            return QString(p->AmountToString().c_str());
    }
    return QVariant();
}

QVariant ProductionQueueModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Name");

            case 1:
                return tr("Qty");

            default:
                return QVariant();
        }
    }
    return QVariant();
}

bool ProductionQueueModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    return false;
}

bool ProductionQueueModel::removeRows(int position, int rows, const QModelIndex &index)
{
    return false;
}

bool ProductionQueueModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

Qt::ItemFlags ProductionQueueModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}

};
