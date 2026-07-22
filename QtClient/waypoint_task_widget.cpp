/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <memory>

#include <QKeyEvent>

#include "waypoint_task_widget.h"

namespace FreeStars {

WaypointTaskWidget::WaypointTaskWidget(QWidget *parent)
    : FoldingWidget(tr("Waypoint Task"))
    , actions(Rules::MaxMinType-FUEL, TRANSFER_NOORDER)
    , values(Rules::MaxMinType-FUEL, 0L)
{
    QWidget *widget = new QWidget;

    ui_WaypointTaskWidget.setupUi(widget);

    ui_WaypointTaskWidget.waypointTaskComboBox->setEnabled(false);
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
        this, SLOT(waypointTaskActivated(int)));

    ui_WaypointTaskWidget.cargoTypeComboBox->addItem(tr("Fuel"), QVariant((int)FUEL));
    for(int i = 0 ; i != Rules::MaxMinType ; i++) {
        ui_WaypointTaskWidget.cargoTypeComboBox->addItem(Rules::GetCargoName(i).c_str(), QVariant(i));
    }
    ui_WaypointTaskWidget.cargoTypeComboBox->addItem(tr("Colonists"), QVariant((int)POPULATION));

    connect(ui_WaypointTaskWidget.cargoTypeComboBox, SIGNAL(activated(int)),
        this, SLOT(cargoTypeActivated(int)));

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

    connect(ui_WaypointTaskWidget.actionComboBox, SIGNAL(activated(int)),
        this, SLOT(actionActivated(int)));

    connect(ui_WaypointTaskWidget.valueEdit, SIGNAL(textChanged(const QString&)),
        this, SLOT(valueChanged(const QString&)));

    this->addWidget(widget);

    updateState();
}

WaypointTaskWidget::~WaypointTaskWidget()
{
}

void WaypointTaskWidget::setWayorder(const WayOrder *order)
{
    int row = ui_WaypointTaskWidget.waypointTaskComboBox->findData(order->GetType());

    if(row >= 0) {
        ui_WaypointTaskWidget.waypointTaskComboBox->setCurrentIndex(row);

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
    updateState();
}

void WaypointTaskWidget::updateState() {

    ui_WaypointTaskWidget.waypointTaskComboBox->setEnabled(true);

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

            for(int i = FUEL ; i != Rules::MaxMinType ; i++) {
                auto action = actions[i-FUEL];
                int index = ui_WaypointTaskWidget.cargoTypeComboBox->findData(i);
                ui_WaypointTaskWidget.cargoTypeComboBox->setItemData(index, action != TRANSFER_NOORDER ? QBrush(Qt::darkGreen) : QVariant(), Qt::ForegroundRole);
            }

            ui_WaypointTaskWidget.stackedWidget->setCurrentWidget(ui_WaypointTaskWidget.transportPage);
            break;
        default:
            ui_WaypointTaskWidget.stackedWidget->setCurrentWidget(ui_WaypointTaskWidget.blankPage);
            break;
    }
}

void WaypointTaskWidget::waypointTaskActivated(int row)
{
    auto data = ui_WaypointTaskWidget.waypointTaskComboBox->itemData(row); 

    if(data.isValid()) {
        OrderType type = (OrderType)data.toInt();

        if(!WayOrder::HasArguments(type)) {
            emit waypointTaskChanged(type);
        }
    }

    updateState();
}

void WaypointTaskWidget::setTransportOrder(const WayOrderTransport *order)
{
    // Copy actions and values
    for(int i = FUEL ; i != Rules::MaxMinType ; i++) {
        actions[i-FUEL] = order->GetAction(i);
        values[i-FUEL] = order->GetValue(i);
    }

    const CargoType cargoType = FUEL;

    // Reset the form to FUEL cargo type
    int row = ui_WaypointTaskWidget.cargoTypeComboBox->findData(QVariant((int)cargoType));
    
    if(row >= 0) {
        ui_WaypointTaskWidget.cargoTypeComboBox->setCurrentIndex(row);
        auto action = actions[cargoType-FUEL];
        int index = ui_WaypointTaskWidget.actionComboBox->findData(action);
        ui_WaypointTaskWidget.actionComboBox->setCurrentIndex(index);

        if(WayOrderTransport::HasValue(action)) {
          ui_WaypointTaskWidget.valueEdit->setEnabled(true);
          ui_WaypointTaskWidget.valueEdit->setText(tr("%0").arg(values[cargoType-FUEL]));
        }
        else {
          ui_WaypointTaskWidget.valueEdit->setEnabled(false);
          ui_WaypointTaskWidget.valueEdit->setText("");
        }
    }

    updateState();
}

void WaypointTaskWidget::cargoTypeActivated(int row)
{
    auto cargoTypeData = ui_WaypointTaskWidget.cargoTypeComboBox->currentData(); 

    if(!cargoTypeData.isValid()) {
        return;
    }

    int cargoType = cargoTypeData.toInt();

    auto action = actions[cargoType-FUEL];

    int index = ui_WaypointTaskWidget.actionComboBox->findData(action);

    ui_WaypointTaskWidget.actionComboBox->setCurrentIndex(index);

    if(WayOrderTransport::HasValue(action)) {
      ui_WaypointTaskWidget.valueEdit->setEnabled(true);
      ui_WaypointTaskWidget.valueEdit->setText(tr("%0").arg(values[cargoType-FUEL]));
    }
    else {
      ui_WaypointTaskWidget.valueEdit->setEnabled(false);
      ui_WaypointTaskWidget.valueEdit->setText("");
    }
}

void WaypointTaskWidget::actionActivated(int row)
{
    auto cargoTypeData = ui_WaypointTaskWidget.cargoTypeComboBox->currentData(); 

    if(!cargoTypeData.isValid()) {
        return;
    }

    int cargoType = cargoTypeData.toInt();

    auto actionData = ui_WaypointTaskWidget.actionComboBox->itemData(row);

    if(!actionData.isValid()) {
        return;
    }

    int action = actionData.toInt();

    if(action >= TRANSFER_NOORDER && action <= TRANSFER_DROPNLOAD) {

        if(WayOrderTransport::HasValue((TransferType)action)) {
          ui_WaypointTaskWidget.valueEdit->setEnabled(true);
          ui_WaypointTaskWidget.valueEdit->setText(tr("%0").arg(values[cargoType-FUEL]));
        }
        else {
          ui_WaypointTaskWidget.valueEdit->setEnabled(false);
          ui_WaypointTaskWidget.valueEdit->setText("");
        }

        actions[cargoType-FUEL] = (TransferType)action;
        updateState();
        emit waypointTaskTransport(actions, values);
    }
}

void WaypointTaskWidget::valueChanged(const QString &text)
{
    auto cargoTypeData = ui_WaypointTaskWidget.cargoTypeComboBox->currentData(); 

    if(!cargoTypeData.isValid()) {
        return;
    }

    int cargoType = cargoTypeData.toInt();

    auto actionData = ui_WaypointTaskWidget.actionComboBox->currentData();

    if(!actionData.isValid()) {
        return;
    }

    int action = actionData.toInt();

    if(action >= TRANSFER_NOORDER && action <= TRANSFER_DROPNLOAD) {
        if(!text.isEmpty()) {
          if(action == TRANSFER_FILLPER || action == TRANSFER_WAITPER
            || action == TRANSFER_SETTOPER)
          {
              values[cargoType-FUEL] = std::min(100, std::max(0, text.toInt()));
          }
          else {
              values[cargoType-FUEL] = std::max(0, text.toInt());
          }
        }
        else {
          values[cargoType-FUEL] = 0;
        }
        updateState();
        emit waypointTaskTransport(actions, values);
    }
}

};
