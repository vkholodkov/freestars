/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include "fleet_composition_widget.h"

#include "ui_fleet_composition_widget.h"

namespace FreeStars {

FleetCompositionWidget::FleetCompositionWidget(const Player *_player, QWidget *parent)
    : FoldingWidget(tr("Fleet Composition"))
    , player(_player)
{
    QWidget *widget = new QWidget;

    ui_FleetCompositionWidget.setupUi(widget);

    this->setFleet(nullptr);

    ui_FleetCompositionWidget.fleetCompositionView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui_FleetCompositionWidget.fleetCompositionView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui_FleetCompositionWidget.fleetCompositionView->resizeColumnsToContents();

    connect(ui_FleetCompositionWidget.splitButton, SIGNAL(clicked(bool)),
        this, SLOT(splitButtonClicked(bool)));
    connect(ui_FleetCompositionWidget.splitAllButton, SIGNAL(clicked(bool)),
        this, SLOT(splitAllButtonClicked(bool)));
    connect(ui_FleetCompositionWidget.mergeButton, SIGNAL(clicked(bool)),
        this, SLOT(mergeButtonClicked(bool)));

    this->addWidget(widget);
}

FleetCompositionWidget::~FleetCompositionWidget()
{
    delete ui_FleetCompositionWidget.fleetCompositionView->model();
}

void FleetCompositionWidget::setFleet(const Fleet *_fleet)
{
    // Switch model
    std::unique_ptr<FleetCompositionModel> fleetCompositionModel(new FleetCompositionModel(_fleet, player));
    auto m = ui_FleetCompositionWidget.fleetCompositionView->model();
    ui_FleetCompositionWidget.fleetCompositionView->setModel(fleetCompositionModel.get());
    fleetCompositionModel.release();
    delete m;

    if(_fleet != nullptr) {
      long cloaked = _fleet->GetCloak(player, true);

      if(cloaked) {
          ui_FleetCompositionWidget.pctCloakedLabel->setText(tr("%0%")
              .arg(cloaked));
      }

      ui_FleetCompositionWidget.splitButton->setEnabled(_fleet->GetStacks() > 1 || _fleet->GetStack(0)->GetCount() > 1);
      ui_FleetCompositionWidget.splitAllButton->setEnabled(_fleet->GetStacks() > 1 || _fleet->GetStack(0)->GetCount() > 1);
    }
}

void FleetCompositionWidget::splitButtonClicked(bool)
{
    auto model = dynamic_cast<FleetCompositionModel*>(ui_FleetCompositionWidget.fleetCompositionView->model());

    if(model != nullptr) {
      emit splitFleet(model->getFleet());
    }
}

void FleetCompositionWidget::splitAllButtonClicked(bool)
{
    auto model = dynamic_cast<FleetCompositionModel*>(ui_FleetCompositionWidget.fleetCompositionView->model());

    if(model != nullptr) {
      emit splitAllFleet(model->getFleet());
    }
}

void FleetCompositionWidget::mergeButtonClicked(bool)
{
    auto model = dynamic_cast<FleetCompositionModel*>(ui_FleetCompositionWidget.fleetCompositionView->model());

    if(model != nullptr) {
      emit mergeFleet(model->getFleet());
    }
}

};
