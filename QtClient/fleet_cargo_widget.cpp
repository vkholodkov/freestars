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
{
    QWidget *widget = new QWidget;

    Ui_FleetCargoWidget ui_FleetCargoWidget;
    ui_FleetCargoWidget.setupUi(widget);

    ui_FleetCargoWidget.fuelWidget->setChangeable(false);
    ui_FleetCargoWidget.fuelWidget->setCargoColor(Qt::red);
    ui_FleetCargoWidget.fuelWidget->setCargo(_fleet->GetFuel());
    ui_FleetCargoWidget.fuelWidget->setMaxCargo(_fleet->GetFuelCapacity());
    ui_FleetCargoWidget.fuelWidget->setUnit(tr("mg"));

    long ironium = _fleet->GetContain(0);
    long boranium = _fleet->GetContain(1);
    long germanium = _fleet->GetContain(2);
    long colonists = _fleet->GetContain(POPULATION);

    ui_FleetCargoWidget.ironiumLabel->setText(tr("%0kT").arg(ironium));
    ui_FleetCargoWidget.boraniumLabel->setText(tr("%0kT").arg(boranium));
    ui_FleetCargoWidget.germaniumLabel->setText(tr("%0kT").arg(germanium));
    ui_FleetCargoWidget.colonistsLabel->setText(tr("%0kT").arg(colonists));

    this->addWidget(widget);
}

void FleetCargoWidget::updateLabels()
{
}

};
