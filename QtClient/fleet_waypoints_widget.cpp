/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <memory>

#include <QKeyEvent>

#include "fleet_waypoints_widget.h"

#include "ui_fleet_waypoints_widget.h"

namespace FreeStars {

FleetWaypointsWidget::FleetWaypointsWidget(Fleet *_fleet, const Player *_player, QWidget *parent)
    : FoldingWidget(tr("Fleet Waypoints"))
    , fleet(_fleet)
    , player(_player)
    , orders()
{
    QWidget *widget = new QWidget;

    Ui_FleetWaypointsWidget ui_FleetWaypointsWidget;
    ui_FleetWaypointsWidget.setupUi(widget);

    waypointListBox = ui_FleetWaypointsWidget.waypointListBox;

    const std::deque<WayOrder *> &orders = fleet->GetOrders();

    for(std::deque<WayOrder*>::const_iterator o = orders.begin() ; o != orders.end() ; o++) {
        waypointListBox->addItem(getLocationName((*o)->GetLocation()));
        this->orders.push_back(new WayOrder(**o));
    }

    waypointListBox->setCurrentRow(0);
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
        waypointListBox->insertItem(row + 1, getLocationName(location));
        waypointListBox->setCurrentRow(row + 1);

        std::auto_ptr<WayOrder> order(new WayOrder(const_cast<Location*>(location), false));
        orders.insert(orders.begin() + row + 1, order.get());
        order.release();

        changeWayorderList();

        wayorderSelected(row + 1);
    }
}

void FleetWaypointsWidget::wayorderSelected(int row)
{
    if(row >= 0 && row < orders.size()) {
        emit selectWaypoint(orders[row]->GetLocation());
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
    }
}

void FleetWaypointsWidget::changeWayorderList()
{
    WayOrderList orderList;
    orderList.SetFleet(fleet->GetID());

    for(std::deque<WayOrder*>::const_iterator o = orders.begin() ; o != orders.end() ; o++) {
        orderList.GetOrders().push_back(new WayOrder(**o));
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
