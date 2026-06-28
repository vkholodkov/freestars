/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _FLEET_COMPOSITION_MODEL_H
#define _FLEET_COMPOSITION_MODEL_H

#include <QAbstractTableModel>

#include <vector>

#include "FSServer.h"

namespace FreeStars {

class FleetCompositionModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    FleetCompositionModel(const Fleet*, const Player*, QObject *parent=0);
    ~FleetCompositionModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    const Fleet *fleet;
    const Player *player;
};

};

#endif
