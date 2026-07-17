/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include "fleets_in_orbit_widget.h"

#include "ui_fleets_in_orbit_widget.h"

#include "space_object_sorter.h"

namespace FreeStars {

FleetsInOrbitWidget::FleetsInOrbitWidget(const Planet *_planet, const Player *_player, QWidget *parent)
    : FoldingWidget(tr("Fleets in Orbit"))
    , planet(_planet)
    , player(_player)
{
    QWidget *widget = new QWidget;

    Ui_FleetsInOrbitWidget ui_FleetsInOrbitWidget;
    ui_FleetsInOrbitWidget.setupUi(widget);

    fuelWidget = ui_FleetsInOrbitWidget.fuelWidget;
    cargoWidget = ui_FleetsInOrbitWidget.cargoWidget;

    fuelWidget->setChangeable(false);
    fuelWidget->setCargoColor(Qt::red);
    fuelWidget->setUnit(tr("mg"));

    cargoWidget->setUnit(tr("kT"));

    fleetsComboBox = ui_FleetsInOrbitWidget.fleetsComboBox;
    textWidget = ui_FleetsInOrbitWidget.textWidget;

    ui_FleetsInOrbitWidget.textWidget->setVisible(false);

    SpaceObjectSorter sos(_planet, _player, SOT_OWN|SOT_FLEET);

    if(!sos.m_object_list.empty()) {
        for(std::vector<const SpaceObject*>::const_iterator i = sos.m_object_list.begin() ; i != sos.m_object_list.end() ; i++) {
            ui_FleetsInOrbitWidget.fleetsComboBox->addItem(QString((*i)->GetName(_player).c_str()), (qlonglong)(*i)->GetID());
        }

        objectActivated(ui_FleetsInOrbitWidget.fleetsComboBox->currentIndex());
    }

    connect(ui_FleetsInOrbitWidget.cargoWidget, SIGNAL(clicked()), this, SLOT(cargoWidgetClicked()));
    connect(ui_FleetsInOrbitWidget.fuelWidget, SIGNAL(clicked()), this, SLOT(cargoWidgetClicked()));
    connect(ui_FleetsInOrbitWidget.gotoButton, SIGNAL(clicked(bool)), this, SLOT(gotoButtonClicked(bool)));
    connect(ui_FleetsInOrbitWidget.cargoButton, SIGNAL(clicked(bool)), this, SLOT(cargoButtonClicked(bool)));
    connect(ui_FleetsInOrbitWidget.fleetsComboBox, SIGNAL(activated(int)), this, SLOT(objectActivated(int)));

    this->addWidget(widget);
}

void FleetsInOrbitWidget::cargoWidgetClicked()
{
    long id = fleetsComboBox->itemData(fleetsComboBox->currentIndex()).toLongLong();
    const Fleet *fleet = player->GetFleet(id);

    if(fleet == NULL)
        return;

    emit exchangeCargo(planet, fleet);
}

void FleetsInOrbitWidget::gotoButtonClicked(bool)
{
    long id = fleetsComboBox->itemData(fleetsComboBox->currentIndex()).toLongLong();
    const Fleet *fleet = player->GetFleet(id);

    if(fleet == NULL)
        return;

    emit selectObject(fleet);
}

void FleetsInOrbitWidget::cargoButtonClicked(bool)
{
    cargoWidgetClicked();
}

void FleetsInOrbitWidget::objectActivated(int index)
{
    long id = fleetsComboBox->itemData(index).toLongLong();
    const Fleet *fleet = player->GetFleet(id);

    if(fleet == NULL)
        return;

    textWidget->setVisible(true);
    
    fuelWidget->setCargo(fleet->GetFuel());
    fuelWidget->setMaxCargo(fleet->GetFuelCapacity());

    cargoWidget->setCargoHolder(fleet);
}

void FleetsInOrbitWidget::cargoUpdated()
{
    objectActivated(fleetsComboBox->currentIndex());
}

};
