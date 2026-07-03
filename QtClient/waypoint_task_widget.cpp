/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <memory>

#include <QKeyEvent>

#include "waypoint_task_widget.h"

namespace FreeStars {

WaypointTaskWidget::WaypointTaskWidget(QWidget *parent)
    : FoldingWidget(tr("Waypoint Task"))
    , currentWayorder()
{
    QWidget *widget = new QWidget;

    ui_WaypointTaskWidget.setupUi(widget);

    ui_WaypointTaskWidget.waypointTaskComboBox->addItem(tr("(no task here)"), QVariant(OT_NONE));
    ui_WaypointTaskWidget.waypointTaskComboBox->addItem(tr("Transport"), QVariant(OT_TRANSPORT));
    ui_WaypointTaskWidget.waypointTaskComboBox->addItem(tr("Colonize"), QVariant(OT_COLONIZE));
    ui_WaypointTaskWidget.waypointTaskComboBox->addItem(tr("Remote Mining"), QVariant(OT_REMOTEMINE));
    ui_WaypointTaskWidget.waypointTaskComboBox->addItem(tr("Merge with Fleet"), QVariant(OT_MERGE));
    ui_WaypointTaskWidget.waypointTaskComboBox->addItem(tr("Scrap Fleet "), QVariant(OT_SCRAP));
    ui_WaypointTaskWidget.waypointTaskComboBox->addItem(tr("Lay Mine Field"), QVariant(OT_LAYMINE));
    ui_WaypointTaskWidget.waypointTaskComboBox->addItem(tr("Patrol"), QVariant(OT_PATROL));
    ui_WaypointTaskWidget.waypointTaskComboBox->addItem(tr("Route"), QVariant(OT_ROUTE));
    ui_WaypointTaskWidget.waypointTaskComboBox->addItem(tr("Transfer Fleet"), QVariant(OT_TRANSFER));

    connect(ui_WaypointTaskWidget.waypointTaskComboBox, SIGNAL(activated(int)),
        this, SLOT(waypointTaskChanged(int)));

    this->addWidget(widget);
}

WaypointTaskWidget::~WaypointTaskWidget()
{
}

void WaypointTaskWidget::setWayorder(const WayOrder *order)
{
    int row = ui_WaypointTaskWidget.waypointTaskComboBox->findData(order->GetType());

    if(row >= 0) {
        ui_WaypointTaskWidget.waypointTaskComboBox->setCurrentIndex(row);
        currentWayorder = order;
    }
}

void WaypointTaskWidget::clearWayorder()
{
    currentWayorder = nullptr;
}

void WaypointTaskWidget::waypointTaskChanged(int row)
{
    auto data = ui_WaypointTaskWidget.waypointTaskComboBox->itemData(row); 

    if(data.isValid() && currentWayorder != nullptr) {
        int type = data.toInt();

        if(type >= OT_NONE && type <= OT_TRANSPORT) {
            std::unique_ptr<WayOrder> newOrder(new WayOrder(*currentWayorder));
            newOrder->SetType((enum OrderType)type);
            emit wayorderChanged(newOrder.release());
        }
    }
}

};
