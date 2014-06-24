
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

QString ComponentDescriber::armorHandler(const Component *component) const
{
    return component->GetArmor() != 0 ? tr("%0dp").arg(component->GetArmor()) : "";
}

QString ComponentDescriber::shieldHandler(const Component *component) const
{
    return component->GetShield() != 0 ? tr("%0dp").arg(component->GetShield()) : "";
}

QString ComponentDescriber::powerHandler(const Component *component) const
{
    return component->GetPower() != 0 ? tr("%0dp").arg(component->GetPower()) : "";
}

QString ComponentDescriber::rangeHandler(const Component *component) const
{
    return component->GetRange() != 0 ? QString::number(component->GetRange()) : "";
}

QString ComponentDescriber::initiativeHandler(const Component *component) const
{
    return component->GetInitiative() != 0 ? QString::number(component->GetInitiative()) : "";
}

QString ComponentDescriber::accuracyHandler(const Component *component) const
{
    return component->GetAccuracy() != 0 ? tr("%0%").arg(component->GetAccuracy() * 100) : "";
}

QString ComponentDescriber::cargoCapacityHandler(const Component *component) const
{
    return component->GetCargoCapacity() != 0 ? tr("%0kT").arg(component->GetCargoCapacity()) : "";
}

QString ComponentDescriber::fuelCapacityHandler(const Component *component) const
{
    return component->GetFuelCapacity() != 0 ? tr("%0mg").arg(component->GetFuelCapacity()) : "";
}

QString ComponentDescriber::scanSpaceHandler(const Component *component) const
{
    return component->GetType() & CT_SCANNER ? tr("%0 l.y.").arg(component->GetScanSpace()) : "";
}

const ComponentDescriber::ComponentProperty ComponentDescriber::interestingComponentProperties[] = {
    { tr("Ironium"), &ComponentDescriber::ironiumHandler, "QLabel { font-weight: bold; color: blue; }", true },
    { tr("Boranium"), &ComponentDescriber::boraniumHandler, "QLabel { font-weight: bold; color: green; }", true },
    { tr("Germanium"), &ComponentDescriber::germaniumHandler, "QLabel { font-weight: bold; color: rgb(255, 207, 94); }", true },
    { tr("Resources"), &ComponentDescriber::resourcesHandler, "QLabel { font-weight: bold; }", true },
    { tr("Mass"), &ComponentDescriber::massHandler, "QLabel { font-weight: bold; }", true },

    { tr("Armor strength"), &ComponentDescriber::armorHandler, "QLabel { font-weight: bold; }", false },
    { tr("Shield strength"), &ComponentDescriber::shieldHandler, "QLabel { font-weight: bold; }", false },
    { tr("Weapon strength"), &ComponentDescriber::powerHandler, "QLabel { font-weight: bold; }", false },
    { tr("Range"), &ComponentDescriber::rangeHandler, "QLabel { font-weight: bold; }", false },
    { tr("Initiative"), &ComponentDescriber::initiativeHandler, "QLabel { font-weight: bold; }", false },
    { tr("Accuracy"), &ComponentDescriber::accuracyHandler, "QLabel { font-weight: bold; }", false },
    { tr("Capacity"), &ComponentDescriber::cargoCapacityHandler, "QLabel { font-weight: bold; }", false },
    { tr("Capacity"), &ComponentDescriber::fuelCapacityHandler, "QLabel { font-weight: bold; }", false },
    { tr("Scan space"), &ComponentDescriber::scanSpaceHandler, "QLabel { font-weight: bold; }", false },

    { NULL, NULL },
};

void ComponentDescriber::describe(const Component *component, QFormLayout *left, QFormLayout *right) const
{
    const ComponentDescriber::ComponentProperty *property = interestingComponentProperties;

    QFont font;
    font.setFamily(QString::fromUtf8("Arial"));
    font.setPointSize(10);

    QFont bold(font);
    bold.setBold(true);
    bold.setWeight(75);

    while(property->name != NULL) {
        if((property->left && left) || (!property->left && right)) {
            QString value((this->*property->handler)(component));

            if(!value.isEmpty()) {
                QLabel *label = new QLabel(property->left ? property->name : property->name + ":");
                label->setStyleSheet(property->style);
                label->setFont(bold);
                QLabel *text = new QLabel(value);
                text->setAlignment(Qt::AlignRight);
                (property->left ? left : right)->addRow(label, text);
            }
        }

        property++;
    }
}

};
