
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
    HullDescriber(const Player *_player, QObject *parent = 0)
        : QObject(parent)
        , player(_player)
    {
    }

    void describe(const Hull*, QFormLayout*, QFormLayout*) const;

private:
    QString maxFuelHandler(const Hull*) const;
    QString armorHandler(const Hull*) const;

    static const HullProperty interestingHullProperties[];

private:
    const Player *player;
};

};

#endif
