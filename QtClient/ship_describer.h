
#ifndef _SHIP_DESCRIBER_H_
#define _SHIP_DESCRIBER_H_

#include <QObject>
#include <QFormLayout>

#include "FSServer.h"
#include "Hull.h"

namespace FreeStars {

class ShipDescriber : public QObject {
    Q_OBJECT

    typedef struct {
        QString name;
        QString (ShipDescriber::*handler)(const Ship*) const;
    } ShipProperty;

public:
    ShipDescriber(const Player *_player, QObject *parent = 0)
        : QObject(parent)
        , player(_player)
    {
    }

    void describe(const Ship*, QFormLayout*, QFormLayout*) const;

private:
    QString maxFuelHandler(const Ship*) const;
    QString armorHandler(const Ship*) const;
    QString shieldHandler(const Ship*) const;
    QString ratingHandler(const Ship*) const;
    QString cloakJamHandler(const Ship*) const;
    QString initiativeMovesHandler(const Ship*) const;

    static const ShipProperty interestingShipProperties[];

private:
    const Player *player;
};

class HullDescriber : public QObject {
    Q_OBJECT

    typedef struct {
        QString name;
        QString (HullDescriber::*handler)(const Hull*) const;
    } HullProperty;

public:
    HullDescriber(QObject *parent = 0)
        : QObject(parent)
    {
    }

    void describe(const Hull*, QFormLayout*, QFormLayout*) const;

private:
    QString maxFuelHandler(const Hull*) const;
    QString armorHandler(const Hull*) const;

    static const HullProperty interestingHullProperties[];
};

class ComponentDescriber : public QObject {
    Q_OBJECT

    typedef struct {
        QString name;
        QString (ComponentDescriber::*handler)(const Component*) const;
        const char *style;
        bool left;
    } ComponentProperty;

public:
    ComponentDescriber(const Player *_player, QObject *parent = 0)
        : QObject(parent)
        , player(_player)
    {
    }

    void describe(const Component*, QFormLayout*, QFormLayout*) const;

private:
    QString ironiumHandler(const Component*) const;
    QString boraniumHandler(const Component*) const;
    QString germaniumHandler(const Component*) const;
    QString resourcesHandler(const Component*) const;
    QString massHandler(const Component*) const;

    QString armorHandler(const Component*) const;
    QString shieldHandler(const Component*) const;
    QString powerHandler(const Component*) const;
    QString rangeHandler(const Component*) const;
    QString initiativeHandler(const Component*) const;
    QString accuracyHandler(const Component*) const;
    QString cargoCapacityHandler(const Component*) const;
    QString fuelCapacityHandler(const Component*) const;
    QString scanSpaceHandler(const Component*) const;

    static const ComponentProperty interestingComponentProperties[];

private:
    const Player *player;
};

};

#endif
