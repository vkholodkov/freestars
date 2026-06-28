/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _PRODUCTION_QUEUE_DIALOG_H_
#define _PRODUCTION_QUEUE_DIALOG_H_

#include <QDialog>
#include <QButtonGroup>

#include "FSServer.h"

#include "production_queue_model.h"
#include "order_template_list_model.h"

#include "ui_production_queue_dialog.h"

namespace FreeStars {

class ProductionQueueDialog : public QDialog, private Ui_ProductionQueueDialog {
    Q_OBJECT

public:
    ProductionQueueDialog(Planet*, QWidget *parent = 0);
    ~ProductionQueueDialog();

private slots:
    void orderSelectionChanged(const QItemSelection&, const QItemSelection&);
    void orderTemplatesSelectionChanged(const QItemSelection&, const QItemSelection&);

    void itemUp();
    void itemDown();
    void add();
    void remove();
    void clear();
    void accept();

    void updateStateOrderTemplatesState();
    void updateState();

private:
    Game *game;
    Planet *planet;
    ProductionQueueModel *productionQueueModel;
    OrderTemplateListModel *orderTemplateListModel;
};

};

#endif
