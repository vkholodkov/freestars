/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include "fleet_cargo_widget.h"

#include "ui_fleet_cargo_widget.h"

namespace FreeStars {

FleetCargoWidget::FleetCargoWidget(const Fleet *_fleet, const Player *_player, QWidget *parent)
    : FoldingWidget(tr("Fuel & Cargo"))
    , fleet(_fleet)
    , player(_player)
    , ui_FleetCargoWidget()
{
    QWidget *widget = new QWidget;

    ui_FleetCargoWidget.setupUi(widget);

    connect(ui_FleetCargoWidget.cargoWidget, SIGNAL(clicked()), this, SLOT(cargoWidgetClicked()));

    cargoUpdated();

    this->addWidget(widget);
}

void FleetCargoWidget::cargoWidgetClicked()
{
    const Planet *planet = fleet->InOrbit();

    if(planet != NULL) {
      emit exchangeCargo(planet, fleet);
    }
}

void FleetCargoWidget::updateLabels()
{
}

void FleetCargoWidget::cargoUpdated()
{
    ui_FleetCargoWidget.fuelWidget->setChangeable(false);
    ui_FleetCargoWidget.fuelWidget->setCargoColor(Qt::red);
    ui_FleetCargoWidget.fuelWidget->setCargo(fleet->GetFuel());
    ui_FleetCargoWidget.fuelWidget->setMaxCargo(fleet->GetFuelCapacity());
    ui_FleetCargoWidget.fuelWidget->setUnit(tr("mg"));

    ui_FleetCargoWidget.cargoWidget->setCargoHolder(fleet);
    ui_FleetCargoWidget.cargoWidget->setUnit(tr("KT"));

    long ironium = fleet->GetContain(0);
    long boranium = fleet->GetContain(1);
    long germanium = fleet->GetContain(2);
    long colonists = fleet->GetContain(POPULATION) / Rules::PopEQ1kT;

    ui_FleetCargoWidget.ironiumLabel->setText(tr("%0kT").arg(ironium));
    ui_FleetCargoWidget.boraniumLabel->setText(tr("%0kT").arg(boranium));
    ui_FleetCargoWidget.germaniumLabel->setText(tr("%0kT").arg(germanium));
    ui_FleetCargoWidget.colonistsLabel->setText(tr("%0kT").arg(colonists));
}

};
