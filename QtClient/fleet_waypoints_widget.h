/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _FLEET_WAYPOINTS_WIDGET_H_
#define _FLEET_WAYPOINTS_WIDGET_H_

#include <QWidget>
#include <QComboBox>

#include "folding_widget.h"
#include "cargo_widget.h"

#include "FSServer.h"

namespace FreeStars {

class FleetWaypointsWidget : public FoldingWidget {
    Q_OBJECT

public:
    FleetWaypointsWidget(const Fleet*, const Player*, QWidget *parent = 0);

private:
    const Fleet *fleet;
    const Player *player;
};

};

#endif
