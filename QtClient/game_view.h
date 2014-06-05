/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _GAME_VIEW_H_
#define _GAME_VIEW_H_

#include <QScrollArea>
#include <QSplitter>

#include "FSServer.h"

#include "map_view.h"
#include "planet_production_widget.h"

#include "ui_message_widget.h"
#include "ui_status_selector.h"
#include "ui_minerals_on_hand_widget.h"
#include "ui_planet_status_widget.h"

namespace FreeStars {

class GameView  : public QSplitter {
    Q_OBJECT

public:
    GameView(const Player*);

    void setupMessages();
    void displayMessage(const Message&);

signals:
    void selectionChanged(const SpaceObject*);

public slots:
    void selectObject(const SpaceObject*);

    void nextMessage();
    void prevMessage();

    void nextObject();

    void exchangeCargo(const Planet*, const Fleet*);
    void changeProductionQueue(const Planet*);
    void clearProductionQueue(const Planet*);
    void setRouteDest();
    void showProductionDialog(bool);

    void shipDesignDialog();
    void researchDialog();
    void battlePlansDialog();
    void playerRelationsDialog();

    void submitTurn();

private:
    void selectPlanet(const Planet*);
    void selectFleet(const Fleet*);
    void clearSelection();

private:
    const Player *player;
    std::vector<Message*> messages;
    PlanetProductionWidget *planetProductionWidget;
    Ui_MessageWidget ui_MessageWidget;
    Ui_StatusSelector ui_StatusSelector;
    Ui_MineralsOnHandWidget ui_MineralsOnHandWidget;
    Ui_PlanetStatusWidget ui_PlanetStatusWidget;
    MapView *mapView;
    QScrollArea *mapScroller;
    QLayout *verticalFlowLayout;
    unsigned currentMessage;
    unsigned currentObject;
    const SpaceObject *currentSelection;
    std::vector<const SpaceObject*> currentStack;
};

};

#endif
