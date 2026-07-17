/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include "fleet_composition_model.h"
#include "translations.h"

namespace FreeStars {

FleetCompositionModel::FleetCompositionModel(const Fleet *_fleet, const Player *_player, QObject *parent)
    : QAbstractTableModel(parent)
    , fleet(_fleet)
    , player(_player)
{
}

FleetCompositionModel::~FleetCompositionModel()
{
}

int FleetCompositionModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return fleet != nullptr ? fleet->GetStacks() : 0;
}

int FleetCompositionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant FleetCompositionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::TextAlignmentRole) {
        return QVariant((index.column() == 0 ? Qt::AlignLeft : Qt::AlignRight) | Qt::AlignVCenter);
    }

    if (fleet != nullptr && index.row() >= fleet->GetStacks() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole && fleet != nullptr) {
        const Stack *stack = fleet->GetStack(index.row());

        if (index.column() == 0) {
            auto design = stack->GetDesign();
            return QString(design->GetName().c_str());
        }
        else if (index.column() == 1)
            return QVariant::fromValue(stack->GetCount());
    }
    return QVariant();
}

QVariant FleetCompositionModel::headerData(int section, Qt::Orientation orientation, int role) const
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

Qt::ItemFlags FleetCompositionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}

};
