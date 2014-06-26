/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _FLEET_WAYPOINTS_WIDGET_H_
#define _FLEET_WAYPOINTS_WIDGET_H_

#include <QWidget>
#include <QListWidget>

#include "folding_widget.h"
#include "cargo_widget.h"

#include "FSServer.h"

namespace FreeStars {

class FleetWaypointsWidget : public FoldingWidget {
    Q_OBJECT

public:
    FleetWaypointsWidget(Fleet*, const Player*, QWidget *parent = 0);
    ~FleetWaypointsWidget();

signals:
    void selectWaypoint(const Location*);

private slots:
    void setRepeatOrders(int);
    void wayorderAdded(const Location*);
    void wayorderSelected(int);
    void wayorderDeleted();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    QString getLocationName(const Location*) const;
    void changeWayorderList();

private:
    QListWidget *waypointListBox;
    Fleet *fleet;
    const Player *player;
    std::deque<WayOrder *> orders;
};

};

#endif
