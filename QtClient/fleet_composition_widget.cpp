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

    this->addWidget(widget);
}

};
