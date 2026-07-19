/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <memory>

#include <QKeyEvent>

#include "fleet_waypoints_widget.h"

namespace FreeStars {

FleetWaypointsWidget::FleetWaypointsWidget(Fleet *_fleet, const Player *_player, QWidget *parent)
    : FoldingWidget(tr("Fleet Waypoints"))
    , fleet(_fleet)
    , player(_player)
    , orders()
{
    QWidget *widget = new QWidget;

    ui_FleetWaypointsWidget.setupUi(widget);

    waypointListBox = ui_FleetWaypointsWidget.waypointListBox;

    const std::deque<WayOrder *> &orders = fleet->GetOrders();

    for(std::deque<WayOrder*>::const_iterator o = orders.begin() ; o != orders.end() ; o++) {
        waypointListBox->addItem(getLocationName((*o)->GetLocation()));
        this->orders.push_back((*o)->Copy());
    }

    waypointListBox->installEventFilter(this);

    ui_FleetWaypointsWidget.repeatOrdersBox->setChecked(fleet->GetRepeat());

    connect(ui_FleetWaypointsWidget.repeatOrdersBox, SIGNAL(stateChanged(int)),
        this, SLOT(setRepeatOrders(int)));
    connect(ui_FleetWaypointsWidget.waypointListBox, SIGNAL(currentRowChanged(int)),
        this, SLOT(wayorderSelected(int)));

    this->addWidget(widget);
}

FleetWaypointsWidget::~FleetWaypointsWidget()
{
    for(std::deque<WayOrder*>::const_iterator o = orders.begin() ; o != orders.end() ; o++) {
        delete *o;
    }
}

void FleetWaypointsWidget::updateInitlalSelection()
{
    if(orders.size()) {
      waypointListBox->setCurrentRow(0);
    }
    else {
      wayorderSelected(-1);
    }
}

bool FleetWaypointsWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if(keyEvent->key() == Qt::Key_Delete) {
            wayorderDeleted();
            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}

void FleetWaypointsWidget::setRepeatOrders(int state)
{
    fleet->SetRepeat(state == Qt::Checked);
}

void FleetWaypointsWidget::wayorderAdded(const Location *location)
{
    int row = waypointListBox->currentRow();

    if(row >= 0 && row < orders.size()) {

        if(*orders[row]->GetLocation() == *location) {
            return;
        }

        if(row < orders.size()-1) {
            if(*orders[row+1]->GetLocation() == *location) {
                return;
            }
        }

        waypointListBox->insertItem(row + 1, getLocationName(location));
        waypointListBox->setCurrentRow(row + 1);

        auto prevLocation = row > 0 ? orders[row - 1]->GetLocation() : fleet;

        std::unique_ptr<WayOrder> order(new WayOrder(const_cast<Location*>(location), false));
        order->SetSpeed(fleet->GetBestSpeed(prevLocation, location, OT_ROUTE));
        orders.insert(orders.begin() + row + 1, order.get());
        order.release();

        changeWayorderList();

        wayorderSelected(row + 1);
    }
}

void FleetWaypointsWidget::wayorderSelected(int row)
{
    std::cout << "FleetWaypointsWidget::wayorderSelected " << row << std::endl;
    if(row >= 0 && row < orders.size()) {
        auto order = orders[row];
        auto speed = order->GetSpeed();
        auto prevLocation = row > 0 ? orders[row - 1]->GetLocation() : fleet;
        auto distance = order->GetLocation()->Distance(prevLocation);
        auto travelTime = speed > 0 ? ::ceil(distance / (speed * speed)) : 0.0;

        if(row > 0) {
          ui_FleetWaypointsWidget.originLabel->setText("Coming From"); 
          ui_FleetWaypointsWidget.originValueLabel->setText(getLocationName(prevLocation)); 
          ui_FleetWaypointsWidget.originValueLabel->show();
        }
        else if(orders.size() > 0) {
          ui_FleetWaypointsWidget.originLabel->setText("Next Way Pt"); 

          if(orders.size() > 1) {
            auto nextLocation = orders[row + 1]->GetLocation();
            ui_FleetWaypointsWidget.originValueLabel->setText(getLocationName(nextLocation)); 
            ui_FleetWaypointsWidget.originValueLabel->show();
          }
          else {
            ui_FleetWaypointsWidget.originValueLabel->hide();
          }
        }

        if(!orders.empty()) {
          ui_FleetWaypointsWidget.originLabel->show();
        }
        else {
          ui_FleetWaypointsWidget.originLabel->hide();
        }

        ui_FleetWaypointsWidget.distanceValueLabel->setText(QString("%0 Light Years").arg(distance, 0, 'g', -1)); 
        ui_FleetWaypointsWidget.distanceLabel->show();
        ui_FleetWaypointsWidget.distanceValueLabel->show();

        ui_FleetWaypointsWidget.warpFactorValueLabel->setText(QString("%0").arg(speed)); 
        ui_FleetWaypointsWidget.warpFactorLabel->show();
        ui_FleetWaypointsWidget.warpFactorValueLabel->show();

        ui_FleetWaypointsWidget.travelTimeValueLabel->setText(QString("%0").arg(travelTime)); 
        ui_FleetWaypointsWidget.travelTimeLabel->show();
        ui_FleetWaypointsWidget.travelTimeValueLabel->show();

        std::cout << "FleetWaypointsWidget::selectWayorder " << row << std::endl;
        emit selectWayorder(orders[row]);
    }
    else {
      ui_FleetWaypointsWidget.originLabel->hide();
      ui_FleetWaypointsWidget.originValueLabel->hide();
      ui_FleetWaypointsWidget.distanceValueLabel->hide();
      ui_FleetWaypointsWidget.distanceLabel->hide();
      ui_FleetWaypointsWidget.warpFactorLabel->hide();
      ui_FleetWaypointsWidget.warpFactorValueLabel->hide();
      ui_FleetWaypointsWidget.travelTimeLabel->hide();
      ui_FleetWaypointsWidget.travelTimeValueLabel->hide();

      emit clearSelection();
    }
}

void FleetWaypointsWidget::changeWayorder(WayOrder *order)
{
    int row = waypointListBox->currentRow();

    if(row >= 0 && row < orders.size()) {
        delete orders[row];
        orders[row] = order;
        changeWayorderList();
    }
}

void FleetWaypointsWidget::wayorderDeleted()
{
    int row = waypointListBox->currentRow();

    if(row > 0 && row < orders.size()) {
        delete orders[row];
        orders.erase(orders.begin() + row);
        delete waypointListBox->takeItem(row);

        changeWayorderList();

        emit clearSelection();
    }
}

void FleetWaypointsWidget::changeWayorderList()
{
    WayOrderList orderList;
    orderList.SetFleet(fleet->GetID());

    for(std::deque<WayOrder*>::const_iterator o = orders.begin() ; o != orders.end() ; o++) {
        orderList.GetOrders().push_back((*o)->Copy());
    }

    fleet->ChangeWaypoints(orderList);
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
