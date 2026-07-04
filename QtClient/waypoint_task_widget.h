/*
 * Copyright (C) 2026 Valery Kholodkov
 */

#ifndef _WAYPOINT_TASK_WIDGET_H_
#define _WAYPOINT_TASK_WIDGET_H_

#include <QWidget>
#include <QListWidget>

#include "folding_widget.h"

#include "FSServer.h"

#include "ui_waypoint_task_widget.h"

namespace FreeStars {

class WaypointTaskWidget : public FoldingWidget {
    Q_OBJECT

public:
    WaypointTaskWidget(QWidget *parent = 0);
    ~WaypointTaskWidget();

signals:
    void wayorderChanged(WayOrder*);

public slots:
    void setWayorder(const WayOrder*);
    void clearWayorder();

private slots:
    void waypointTaskChanged(int);

private:
    void setTransportOrder(const WayOrderTransport*);
    void updateState();

    Ui_WaypointTaskWidget ui_WaypointTaskWidget;
    const WayOrder *currentWayorder;
};

};

#endif
