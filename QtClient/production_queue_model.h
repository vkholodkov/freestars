/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _PRODUCTION_QUEUE_MODEL_H
#define _PRODUCTION_QUEUE_MODEL_H

#include <QAbstractTableModel>

#include <vector>

#include "FSServer.h"

namespace FreeStars {

class ProductionQueueModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    ProductionQueueModel(const deque<ProdOrder*> &_production_queue, QObject *parent=0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

private:
    const std::vector<ProdOrder*> production_queue;
};

};

#endif
