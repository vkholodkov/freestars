/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _PRODUCTION_QUEUE_TEMPLATES_MODEL_H
#define _PRODUCTION_QUEUE_TEMPLATES_MODEL_H

#include <QAbstractTableModel>

#include <vector>

#include "FSServer.h"

namespace FreeStars {

class OrderTemplateListModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    OrderTemplateListModel(const Planet*, QObject *parent=0);
    ~OrderTemplateListModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());

    long getCompletionYears(int position) const;

    std::vector<ProdOrder*> order_template_list;

private:
    void releaseAll();

    const Planet *planet;
};

};

#endif
