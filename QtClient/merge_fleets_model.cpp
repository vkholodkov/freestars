/*
 * Copyright (C) 2026 Valery Kholodkov
 */

#include <QFont>
#include <QBrush>

#include "merge_fleets_model.h"

namespace FreeStars {

MergeFleetsModel::MergeFleetsModel(const Fleet *_fleet, const Player *_player, QObject *parent)
    : QAbstractTableModel(parent)
    , player(_player)
    , fleet(_fleet)
{
    if(_fleet->GetOwner() == _player) {
        const deque<SpaceObject*> *alsoHere = _fleet->GetAlsoHere();

        for(auto so : *alsoHere) {
            auto fleet = dynamic_cast<Fleet*>(so);
            if(fleet != nullptr && !fleet->IsEmpty() && fleet->GetOwner() == _player) {
                fleets.push_back(fleet);
            }
        }
    }
}

MergeFleetsModel::~MergeFleetsModel()
{
}

QModelIndex MergeFleetsModel::getInitialFleetIndex() const {
    auto i = std::find(fleets.begin(), fleets.end(), fleet);
    return i != fleets.end() ? createIndex(i - fleets.begin(), 0) : QModelIndex();
}

const Fleet *MergeFleetsModel::getFleet(const QModelIndex &index) const {
    return fleets.at(index.row());
}


QModelIndex MergeFleetsModel::topLeft() const {
    return createIndex(0, 0);
}

QModelIndex MergeFleetsModel::bottomRight() const {
    return createIndex(fleets.size()-1, 0);
}

int MergeFleetsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return fleets.size();
}

int MergeFleetsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant MergeFleetsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::TextAlignmentRole) {
        return QVariant((index.column() == 0 ? Qt::AlignLeft : Qt::AlignRight) | Qt::AlignVCenter);
    }

    if (index.row() >= fleets.size() || index.row() < 0)
        return QVariant();

    const Fleet *f = fleets.at(index.row());

    if (role == Qt::DisplayRole) {
        if (index.column() == 0)
            return QVariant(f->GetName(player).c_str());
    }
    return QVariant();
}

QVariant MergeFleetsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Name");

            default:
                return QVariant();
        }
    }
    return QVariant();
}

Qt::ItemFlags MergeFleetsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}

};
