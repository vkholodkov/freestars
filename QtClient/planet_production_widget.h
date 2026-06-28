/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _PLANET_PRODUCTION_WIDGET_H_
#define _PLANET_PRODUCTION_WIDGET_H_

#include <QWidget>

#include "production_queue_model.h"

#include "folding_widget.h"

#include "FSServer.h"

#include "ui_planet_production_widget.h"

namespace FreeStars {

class PlanetProductionWidget : public FoldingWidget {
    Q_OBJECT

public:
    PlanetProductionWidget(Planet*, const Player*, QWidget *parent = 0);
    ~PlanetProductionWidget();

private slots:
    void changeButtonClicked(bool);
    void clearButtonClicked(bool);
    void orderSelectionChanged(const QItemSelection&, const QItemSelection&);

public slots:
    void productionQueueChanged();

signals:
    void changeProductionQueue(const Planet*);
    void clearProductionQueue(const Planet*);

private:
    void updateState();

    Ui_PlanetProductionWidget ui_PlanetProductionWidget;
    Planet *planet;
    ProductionQueueModel *productionQueueModel;
};

};

#endif
