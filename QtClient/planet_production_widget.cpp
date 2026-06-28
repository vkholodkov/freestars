/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include "planet_production_widget.h"

#include "production_queue_model.h"

#include "ui_planet_production_widget.h"

namespace FreeStars {

PlanetProductionWidget::PlanetProductionWidget(Planet *_planet, const Player *_player, QWidget *parent)
    : FoldingWidget(tr("Production"))
    , planet(_planet)
{
    QWidget *widget = new QWidget;

    ui_PlanetProductionWidget.setupUi(widget);

    productionQueueModel = new ProductionQueueModel(_planet, _planet->GetProduction());
    auto m = ui_PlanetProductionWidget.productionQueueView->model();
    ui_PlanetProductionWidget.productionQueueView->setModel(productionQueueModel);
    delete m;

    auto sm = ui_PlanetProductionWidget.productionQueueView->selectionModel();
    connect(sm, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
      this, SLOT(orderSelectionChanged(const QItemSelection&, const QItemSelection&)));

    ui_PlanetProductionWidget.productionQueueView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui_PlanetProductionWidget.productionQueueView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui_PlanetProductionWidget.productionQueueView->resizeColumnsToContents();

    ui_PlanetProductionWidget.completionValueLabel->setText("");
    ui_PlanetProductionWidget.routeToLabel->setText(
        planet->GetRoute() != NULL ? QString(_planet->GetRoute()->GetName(_player).c_str()) : tr("none"));

    connect(ui_PlanetProductionWidget.changeButton, SIGNAL(clicked(bool)), this, SLOT(changeButtonClicked(bool)));
    connect(ui_PlanetProductionWidget.clearButton, SIGNAL(clicked(bool)), this, SLOT(clearButtonClicked(bool)));

    this->addWidget(widget);

    updateState();
}

PlanetProductionWidget::~PlanetProductionWidget()
{
    delete productionQueueModel;
}

void PlanetProductionWidget::changeButtonClicked(bool)
{
    emit changeProductionQueue(planet);
}

void PlanetProductionWidget::clearButtonClicked(bool)
{
    emit clearProductionQueue(planet);
}

void PlanetProductionWidget::productionQueueChanged()
{
  productionQueueModel->setProductionQueue(planet->GetProduction());
  updateState();
}

void PlanetProductionWidget::orderSelectionChanged(const QItemSelection&, const QItemSelection&)
{
    updateState();
}

void PlanetProductionWidget::updateState()
{
  auto m = ui_PlanetProductionWidget.productionQueueView->selectionModel();
  auto currentOrderIndex = m->currentIndex();
  bool hasSelection = currentOrderIndex.row() >= 0;
  auto selectedIndex = currentOrderIndex.row();
  auto rowCount = ui_PlanetProductionWidget.productionQueueView->model()->rowCount();

  long years = -1;

  if(hasSelection) {
    years = productionQueueModel->getCompletionYears(selectedIndex);
  }
  else if(rowCount != 0) {
    years = productionQueueModel->getCompletionYears(rowCount - 1);
  }

  ui_PlanetProductionWidget.completionValueLabel->setText(
    years <= 1 ? QString("1 year")
     : years < 5 ? QString("2 - 4 years")
     : years <= 10 ? QString("5 - 10 years")
     : QString("never"));

  ui_PlanetProductionWidget.completionValueLabel->setVisible(hasSelection || rowCount != 0);
}

};
