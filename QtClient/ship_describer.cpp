
#include <QLabel>

#include "ship_describer.h"

namespace FreeStars {

QString ShipDescriber::maxFuelHandler(const Ship *ship) const
{
    return ship->GetFuelCapacity() != 0 ? tr("%0mg").arg(ship->GetFuelCapacity()) : "";
}

QString ShipDescriber::armorHandler(const Ship *ship) const
{
    return ship->GetArmor(player) != 0 ? tr("%0dp").arg(ship->GetArmor(player)) : "";
}

QString ShipDescriber::shieldHandler(const Ship *ship) const
{
    return ship->GetShield(player) != 0 ? tr("%0dp").arg(ship->GetShield(player)) : "";
}

QString ShipDescriber::ratingHandler(const Ship *ship) const
{
    return ship->GetRating() != 0 ? QString::number(ship->GetRating()) : "";
}

QString ShipDescriber::cloakJamHandler(const Ship *ship) const
{
    return ship->GetCloaking() != 0 || ship->GetJamming() != 0
        ?  tr("%0%/%1%").arg(ship->GetCloaking()).arg(ship->GetJamming() * 100)
        : "";
}

QString ShipDescriber::initiativeMovesHandler(const Ship *ship) const
{
    return tr("%0 / %1").arg(ship->GetNetInitiative()).arg(ship->GetNetSpeed());
}

const ShipDescriber::ShipProperty ShipDescriber::interestingShipProperties[] = {
    { tr("Max Fuel"), &ShipDescriber::maxFuelHandler },
    { tr("Armor"), &ShipDescriber::armorHandler },
    { tr("Shield"), &ShipDescriber::shieldHandler },
    { tr("Rating"), &ShipDescriber::ratingHandler },
    { tr("Cloak/Jam"), &ShipDescriber::cloakJamHandler },
    { tr("Initiative/Moves"), &ShipDescriber::initiativeMovesHandler },
    { NULL, NULL },
};

void ShipDescriber::describe(const Ship *ship, QFormLayout *left, QFormLayout *right) const
{
    const ShipDescriber::ShipProperty *property = interestingShipProperties;

    while(property->name != NULL) {
        QString value((this->*property->handler)(ship));

        if(!value.isEmpty()) {
            QLabel *label = new QLabel(property->name + ":");
            label->setStyleSheet("QLabel { font-weight: bold; }");
            QLabel *text = new QLabel(value);
            text->setAlignment(Qt::AlignRight);
            right->addRow(label, text);
        }

        property++;
    }
}

QString HullDescriber::maxFuelHandler(const Hull *hull) const
{
    return hull->GetFuelCapacity() != 0 ? tr("%0mg").arg(hull->GetFuelCapacity()) : "";
}

QString HullDescriber::armorHandler(const Hull *hull) const
{
    return hull->GetArmor() != 0 ? tr("%0dp").arg(hull->GetArmor()) : "";
}

const HullDescriber::HullProperty HullDescriber::interestingHullProperties[] = {
    { tr("Max Fuel"), &HullDescriber::maxFuelHandler },
    { tr("Armor"), &HullDescriber::armorHandler },
    { NULL, NULL },
};

void HullDescriber::describe(const Hull *hull, QFormLayout *left, QFormLayout *right) const
{
    const HullDescriber::HullProperty *property = interestingHullProperties;

    while(property->name != NULL) {
        QString value((this->*property->handler)(hull));

        if(!value.isEmpty()) {
            QLabel *label = new QLabel(property->name + ":");
            label->setStyleSheet("QLabel { font-weight: bold; }");
            QLabel *text = new QLabel(value);
            text->setAlignment(Qt::AlignRight);
            right->addRow(label, text);
        }

        property++;
    }
}

QString ComponentDescriber::ironiumHandler(const Component *component) const {
    Cost cost(component->GetCost(player));
    return tr("%0kt").arg(cost[0]);
}

QString ComponentDescriber::boraniumHandler(const Component *component) const {
    Cost cost(component->GetCost(player));
    return tr("%0kt").arg(cost[1]);
}

QString ComponentDescriber::germaniumHandler(const Component *component) const {
    Cost cost(component->GetCost(player));
    return tr("%0kt").arg(cost[2]);
}

QString ComponentDescriber::resourcesHandler(const Component *component) const {
    Cost cost(component->GetCost(player));
    return QString::number(cost.GetResources());
}

QString ComponentDescriber::massHandler(const Component *component) const {
    return component->GetMass() != 0 ? tr("%0kt").arg(component->GetMass()) : "";
}

const ComponentDescriber::ComponentProperty ComponentDescriber::interestingComponentProperties[] = {
    { tr("Ironium"), &ComponentDescriber::ironiumHandler, true },
    { tr("Boranium"), &ComponentDescriber::boraniumHandler, true },
    { tr("Germanium"), &ComponentDescriber::germaniumHandler, true },
    { tr("Resources"), &ComponentDescriber::resourcesHandler, true },
    { tr("Mass"), &ComponentDescriber::massHandler, true },
    { NULL, NULL },
};

void ComponentDescriber::describe(const Component *component, QFormLayout *left, QFormLayout *right) const
{
    const ComponentDescriber::ComponentProperty *property = interestingComponentProperties;

    while(property->name != NULL) {
        if((property->left && left) || (!property->left && right)) {
            QString value((this->*property->handler)(component));

            if(!value.isEmpty()) {
                QLabel *label = new QLabel(property->name + ":");
                label->setStyleSheet("QLabel { font-weight: bold; }");
                QLabel *text = new QLabel(value);
                text->setAlignment(Qt::AlignRight);
                (property->left ? left : right)->addRow(label, text);
            }
        }

        property++;
    }
}

};
