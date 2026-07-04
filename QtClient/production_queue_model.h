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
    ProductionQueueModel(const Planet*, const deque<ProdOrder*> &_production_queue, QObject *parent=0);
    ~ProductionQueueModel();

    void setProductionQueue(const deque<ProdOrder*> &_production_queue);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
    virtual bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild);

    bool addOrderFromTemplate(int position, const ProdOrder*, int amount);
    bool removeOrder(int position, int amount);
    long getCompletionYears(int position) const;
    long getCompletionPct(int position) const;

    std::vector<ProdOrder*> production_queue;

private:
    bool insertOrderFromTemplate(int position, const ProdOrder*, int amount);
    bool mergeOrderWithTemplate(int position, ProdOrder*, const ProdOrder*, int amount);

    void releaseAll();

    const Planet *planet;
};

};

#endif
