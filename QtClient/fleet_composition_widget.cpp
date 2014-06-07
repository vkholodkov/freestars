/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include "fleet_composition_widget.h"

#include "ui_fleet_composition_widget.h"

namespace FreeStars {

FleetCompositionWidget::FleetCompositionWidget(const Fleet *_fleet, const Player *_player, QWidget *parent)
    : FoldingWidget(tr("Fleet Composition"))
    , fleet(_fleet)
    , player(_player)
{
    QWidget *widget = new QWidget;

    Ui_FleetCompositionWidget ui_FleetCompositionWidget;
    ui_FleetCompositionWidget.setupUi(widget);

    long cloaked = _fleet->GetCloak(_player, true);

    if(cloaked) {
        ui_FleetCompositionWidget.pctCloakedLabel->setText(tr("%0%")
            .arg(cloaked));
    }

    connect(ui_FleetCompositionWidget.splitButton, SIGNAL(clicked(bool)),
        this, SLOT(splitButtonClicked(bool)));
    connect(ui_FleetCompositionWidget.splitAllButton, SIGNAL(clicked(bool)),
        this, SLOT(splitAllButtonClicked(bool)));
    connect(ui_FleetCompositionWidget.mergeButton, SIGNAL(clicked(bool)),
        this, SLOT(mergeButtonClicked(bool)));

    this->addWidget(widget);
}

void FleetCompositionWidget::splitButtonClicked(bool)
{
    emit splitFleet(fleet);
}

void FleetCompositionWidget::splitAllButtonClicked(bool)
{
    emit splitAllFleet(fleet);
}

void FleetCompositionWidget::mergeButtonClicked(bool)
{
    emit mergeFleet(fleet);
}

};
