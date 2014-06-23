
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

};
