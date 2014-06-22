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
{
    QWidget *widget = new QWidget;

    Ui_FleetWaypointsWidget ui_FleetWaypointsWidget;
    ui_FleetWaypointsWidget.setupUi(widget);

    waypointListBox = ui_FleetWaypointsWidget.waypointListBox;

    const std::deque<WayOrder *> &orders = _fleet->GetOrders();

    for(std::deque<WayOrder*>::const_iterator o = orders.begin() ; o != orders.end() ; o++) {
        waypointListBox->addItem(getLocationName((*o)->GetLocation()));
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
    std::deque<WayOrder *> &orders = fleet->GetOrders();

    int row = waypointListBox->currentRow();
    
    if(row >= 0 && row < orders.size()) {
        std::auto_ptr<WayOrder> order(new WayOrder(const_cast<Location*>(location), false));
        waypointListBox->insertItem(row + 1, getLocationName(order->GetLocation()));
        orders.insert(orders.begin() + row + 1, order.get());
        order.release();

        waypointListBox->setCurrentRow(row + 1);
    }
}

void FleetWaypointsWidget::wayorderSelected(int row)
{
    const std::deque<WayOrder *> &orders = fleet->GetOrders();

    if(row >= 0 && row < orders.size()) {
        emit selectWaypoint(orders[row]->GetLocation());
    }
}

void FleetWaypointsWidget::wayorderDeleted()
{
    int row = waypointListBox->currentRow();
    std::deque<WayOrder *> &orders = fleet->GetOrders();

    if(row > 0 && row < orders.size()) {
        orders.erase(orders.begin() + row);
        delete waypointListBox->takeItem(row);
    }
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
