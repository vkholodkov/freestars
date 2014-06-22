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

    const std::deque<WayOrder *> &orders = _fleet->GetOrders();

    for(std::deque<WayOrder*>::const_iterator o = orders.begin() ; o != orders.end() ; o++) {
        ui_FleetWaypointsWidget.waypointListBox->addItem(getLocationName((*o)->GetLocation()));
    }

    ui_FleetWaypointsWidget.waypointListBox->setCurrentRow(0);

    this->addWidget(widget);
}

QString FleetWaypointsWidget::getLocationName(const Location *loc) const
{
    const Planet *p = dynamic_cast<const Planet*>(loc);

    if(p != NULL) {
        return QString(p->GetName().c_str());
    }

    const Fleet *f = dynamic_cast<const Fleet*>(loc);

    if(f != NULL) {
        return QString(f->GetName(player).c_str());
    }

    const MineField *m = dynamic_cast<const MineField*>(loc);

    if(m != NULL) {
        return tr("Minefield #%0").arg(m->GetID());
    }

    const Salvage *s = dynamic_cast<const Salvage*>(loc);

    if(s != NULL) {
        return tr("Salvage #%0").arg(s->GetID());
    }

    return tr("Space(%0,%1)").arg(loc->GetPosX()).arg(loc->GetPosY());
}

};
