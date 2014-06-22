/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include "fleet_waypoints_widget.h"

#include "ui_fleet_waypoints_widget.h"

namespace FreeStars {

FleetWaypointsWidget::FleetWaypointsWidget(const Fleet *_fleet, const Player *_player, QWidget *parent)
    : FoldingWidget(tr("Fleet Waypoints"))
    , fleet(_fleet)
    , player(_player)
{
    QWidget *widget = new QWidget;

    Ui_FleetWaypointsWidget ui_FleetWaypointsWidget;
    ui_FleetWaypointsWidget.setupUi(widget);

    this->addWidget(widget);
}

};
