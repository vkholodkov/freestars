/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <memory>

#include <QKeyEvent>

#include "waypoint_task_widget.h"

namespace FreeStars {

/*
QString cargoTypes[] = {
  QObject::tr("Fuel")
};

class CargoTypeModel : public QAbstractListModel
{
public:
   CargoTypeModel(QObject *parent = 0, const WayOrderTransport *_order = 0) : QAbstractListModel(parent), order(_order) {}

   int rowCount(const QModelIndex &parent = QModelIndex()) const { return TRANSFER_DROPNLOAD+1; }
   QVariant data(const QModelIndex &index, int role) const {
      if (!index.isValid())
          return QVariant();

      if (role == Qt::TextColorRole)
         return QColor(QColor::colorNames().at(index.row()));

      if (role == Qt::DisplayRole)
          return QString("Item %1").arg(index.row() + 1);
      else
          return QVariant();
   }

  const WayOrderTransport *order;
};
*/

WaypointTaskWidget::WaypointTaskWidget(QWidget *parent)
    : FoldingWidget(tr("Waypoint Task"))
    , currentWayorder()
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

    ui_WaypointTaskWidget.cargoTypeComboBox->addItem(tr("Fuel"), QVariant((int)FUEL));
    for(int i = 0 ; i != Rules::MaxMinType ; i++) {
        ui_WaypointTaskWidget.cargoTypeComboBox->addItem(Rules::GetCargoName(i).c_str(), QVariant(i));
    }
    ui_WaypointTaskWidget.cargoTypeComboBox->addItem(tr("Colonists"), QVariant((int)POPULATION));

    connect(ui_WaypointTaskWidget.cargoTypeComboBox, SIGNAL(activated(int)),
        this, SLOT(cargoTypeChanged(int)));

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
        this, SLOT(actionChanged(int)));

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
    std::cout << "WaypointTaskWidget::setWayorder" << std::endl;
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

    if(currentWayorder == nullptr) {
        ui_WaypointTaskWidget.stackedWidget->setCurrentWidget(ui_WaypointTaskWidget.blankPage);
        ui_WaypointTaskWidget.waypointTaskComboBox->setEnabled(false);
        return;
    }

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

    if(data.isValid()) {
        int type = data.toInt();

        if(!WayOrder::HasArguments((enum OrderType)type)) {
            std::unique_ptr<WayOrder> newOrder(currentWayorder->Copy());
            newOrder->SetType((enum OrderType)type);
            emit wayorderChanged(newOrder.release());
        }
    }

    updateState();
}

void WaypointTaskWidget::setTransportOrder(const WayOrderTransport *order)
{
    auto cargoTypeData = ui_WaypointTaskWidget.cargoTypeComboBox->currentData(); 

    if(!cargoTypeData.isValid()) {
        return;
    }

    auto cargoType = cargoTypeData.toInt();

    auto action = order->GetAction(cargoType);

    std::cout << "setTransportOrder cargoType=" << cargoType << " action=" << action << std::endl;

    int index = ui_WaypointTaskWidget.actionComboBox->findData(action);

    std::cout << "setTransportOrder index=" << index << std::endl;

    ui_WaypointTaskWidget.actionComboBox->setCurrentIndex(index);

    for(int i = FUEL ; i != Rules::MaxMinType ; i++) {
        auto action = order->GetAction(i);
        int index = ui_WaypointTaskWidget.cargoTypeComboBox->findData(i);
        ui_WaypointTaskWidget.cargoTypeComboBox->setItemData(index, action != TRANSFER_NOORDER ? QBrush(Qt::green) : QVariant(), Qt::ForegroundRole);
    }

    updateState();
}

void WaypointTaskWidget::cargoTypeChanged(int row)
{
    std::cout << "WaypointTaskWidget::cargoTypeChanged " << row  << std::endl;
    if(typeid(*currentWayorder) != typeid(WayOrderTransport)) {
        return;
    }

    auto typeData = ui_WaypointTaskWidget.waypointTaskComboBox->currentData(); 

    if(!typeData.isValid()) {
        return;
    }

    int type = typeData.toInt();

    if(type != OT_TRANSPORT) {
        return;
    }

    auto cargoTypeData = ui_WaypointTaskWidget.cargoTypeComboBox->currentData(); 

    if(!cargoTypeData.isValid()) {
        return;
    }

    int cargoType = cargoTypeData.toInt();

    auto action = dynamic_cast<const WayOrderTransport*>(currentWayorder)->GetAction(cargoType);

    std::cout << "setTransportOrder cargoType=" << cargoType << " action=" << action << std::endl;

    int index = ui_WaypointTaskWidget.actionComboBox->findData(action);

    std::cout << "setTransportOrder index=" << index << std::endl;

    ui_WaypointTaskWidget.actionComboBox->setCurrentIndex(index);
}

void WaypointTaskWidget::actionChanged(int row)
{
    std::cout << "WaypointTaskWidget::actionChanged " << row  << std::endl;
    auto typeData = ui_WaypointTaskWidget.waypointTaskComboBox->currentData(); 

    if(!typeData.isValid()) {
        return;
    }

    int type = typeData.toInt();

    if(type != OT_TRANSPORT) {
        return;
    }

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

    int index = ui_WaypointTaskWidget.cargoTypeComboBox->findData(cargoType);
    ui_WaypointTaskWidget.cargoTypeComboBox->setItemData(index, action != TRANSFER_NOORDER ? QBrush(Qt::green) : QVariant(), Qt::ForegroundRole);

    std::unique_ptr<WayOrderTransport> newOrder;

    if(typeid(*currentWayorder) == typeid(WayOrderTransport)) {
        newOrder.reset(new WayOrderTransport(*dynamic_cast<const WayOrderTransport*>(currentWayorder)));
    }
    else {
        newOrder.reset(new WayOrderTransport(const_cast<Location*>(currentWayorder->GetLocation())));
    }

    std::cout << "WaypointTaskWidget::actionChanged cargoType=" << cargoType << " action=" << action << std::endl;

    newOrder->SetType(OT_TRANSPORT);
    newOrder->SetAction(cargoType, (TransferType)action);
    emit wayorderChanged(newOrder.release());
}

};
