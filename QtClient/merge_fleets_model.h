/*
 * Copyright (C) 2026 Valery Kholodkov
 */

#ifndef _MERGE_FLEETS_MODEL_H
#define _MERGE_FLEETS_MODEL_H

#include <QAbstractTableModel>

#include <vector>

#include "FSServer.h"

namespace FreeStars {

class MergeFleetsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    MergeFleetsModel(const Fleet*, const Player*, QObject *parent=0);
    ~MergeFleetsModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QModelIndex getInitialFleetIndex() const;
    const Fleet *getFleet(const QModelIndex &index) const;

    QModelIndex topLeft() const;
    QModelIndex bottomRight() const;

private:
    const Player *player;
    const Fleet *fleet; 
    std::vector<const Fleet*> fleets;
};

};

#endif
