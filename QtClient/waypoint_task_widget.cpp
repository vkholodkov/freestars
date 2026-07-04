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

    ui_WaypointTaskWidget.cargoTypeComboBox->addItem(tr("Fuel"), QVariant((int)FUEL));
    for(int i = 0 ; i != Rules::MaxMinType ; i++) {
        ui_WaypointTaskWidget.cargoTypeComboBox->addItem(Rules::GetCargoName(i).c_str(), QVariant(i));
    }
    ui_WaypointTaskWidget.cargoTypeComboBox->addItem(tr("Colonists"), QVariant((int)POPULATION));

    ui_WaypointTaskWidget.actionComboBox->addItem(tr("No Order"), QVariant((int)TRANSFER_NOORDER));
    ui_WaypointTaskWidget.actionComboBox->addItem(tr("Load all available"), QVariant((int)TRANSFER_LOADALL));
    ui_WaypointTaskWidget.actionComboBox->addItem(tr("Unload all"), QVariant((int)TRANSFER_UNLOADALL));
    ui_WaypointTaskWidget.actionComboBox->addItem(tr("Load amount"), QVariant((int)TRANSFER_LOADAMT));
    ui_WaypointTaskWidget.actionComboBox->addItem(tr("Unload amount"), QVariant((int)TRANSFER_UNLOADAMT));
    ui_WaypointTaskWidget.actionComboBox->addItem(tr("Fill up to %"), QVariant((int)TRANSFER_FILLPER));
    ui_WaypointTaskWidget.actionComboBox->addItem(tr("Wait up to %"), QVariant((int)TRANSFER_WAITPER));
    ui_WaypointTaskWidget.actionComboBox->addItem(tr("Load optimal"), QVariant((int)TRANSFER_LOADDUNN));
    ui_WaypointTaskWidget.actionComboBox->addItem(tr("Set Amount To"), QVariant((int)TRANSFER_AMOUNTTO));
    ui_WaypointTaskWidget.actionComboBox->addItem(tr("Set Dest Amount To"), QVariant((int)TRANSFER_DESTTO));
    ui_WaypointTaskWidget.actionComboBox->addItem(tr("Set to a %"), QVariant((int)TRANSFER_SETTOPER));
    ui_WaypointTaskWidget.actionComboBox->addItem(tr("Drop and Load"), QVariant((int)TRANSFER_DROPNLOAD));

    updateState();

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

        if(typeid(*order) == typeid(WayOrderTransport)) {
            setTransportOrder(dynamic_cast<const WayOrderTransport*>(order));
        }
        else {
            updateState();
        }
    }
}

void WaypointTaskWidget::clearWayorder()
{
    currentWayorder = nullptr;
    updateState();
}

void WaypointTaskWidget::updateState() {
    auto row = ui_WaypointTaskWidget.waypointTaskComboBox->currentIndex();
    auto data = ui_WaypointTaskWidget.waypointTaskComboBox->itemData(row); 

    int type = OT_NONE;

    if(data.isValid()) {
        type = data.toInt();
    }

    switch(type) {
        case OT_COLONIZE:
            ui_WaypointTaskWidget.stackedWidget->setCurrentWidget(ui_WaypointTaskWidget.colonizePage);
            break;
        case OT_REMOTEMINE:
            ui_WaypointTaskWidget.stackedWidget->setCurrentWidget(ui_WaypointTaskWidget.remoteMinePage);
            break;
        case OT_TRANSPORT:
            ui_WaypointTaskWidget.stackedWidget->setCurrentWidget(ui_WaypointTaskWidget.transportPage);
            break;
        default:
            ui_WaypointTaskWidget.stackedWidget->setCurrentWidget(ui_WaypointTaskWidget.blankPage);
            break;
    }
}

void WaypointTaskWidget::waypointTaskChanged(int row)
{
    auto data = ui_WaypointTaskWidget.waypointTaskComboBox->itemData(row); 

    if(data.isValid() && currentWayorder != nullptr) {
        int type = data.toInt();

        if(type == OT_TRANSPORT) {
        }

        if(type >= OT_NONE && type <= OT_TRANSPORT) {
            std::unique_ptr<WayOrder> newOrder(new WayOrder(*currentWayorder));
            newOrder->SetType((enum OrderType)type);
            emit wayorderChanged(newOrder.release());
        }
    }

    updateState();
}

void WaypointTaskWidget::setTransportOrder(const WayOrderTransport *order)
{
    updateState();
}

};
