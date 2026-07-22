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
    void waypointTaskChanged(OrderType);
    void waypointTaskTransport(std::vector<TransferType>, std::vector<long>);

public slots:
    void setWayorder(const WayOrder*);
    void clearWayorder();

private slots:
    void waypointTaskActivated(int);
    void cargoTypeActivated(int);
    void actionActivated(int);
    void valueChanged(const QString&);

private:
    void setTransportOrder(const WayOrderTransport*);
    void updateState();

    Ui_WaypointTaskWidget ui_WaypointTaskWidget;
    std::vector<TransferType> actions;
    std::vector<long> values;
};

};

#endif
