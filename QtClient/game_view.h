/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _GAME_VIEW_H_
#define _GAME_VIEW_H_

#include <QScrollArea>
#include <QSplitter>

#include "FSServer.h"

#include "graphics_array.h"
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
    GameView(Player*, const GraphicsArray*);

    void setupMessages();
    void displayMessage(const Message&);

    const MapView *getMapView() const { return mapView; }
    MapView *getMapView() { return mapView; }

signals:
    void selectionChanged(const SpaceObject*);

public slots:
    void selectObject(const SpaceObject*);
    void listObjectsInLocation(const SpaceObject*, const QPoint&);
    void renameObject(const SpaceObject*);

    void nextMessage();
    void prevMessage();

    void nextObject();
    void prevObject();

    void changeProductionQueue(const Planet*);
    void clearProductionQueue(const Planet*);
    void setRouteDest();
    void showProductionDialog(bool);
    void exchangeCargo(const Planet*, const Fleet*);
    void splitFleet(const Fleet*);
    void splitAllFleet(const Fleet*);
    void mergeFleet(const Fleet*);

    void shipDesignDialog();
    void researchDialog();
    void battlePlansDialog();
    void playerRelationsDialog();
    void viewRaceDialog();

    void submitTurn();

private:
    void setBriefSelection(const Planet*);
    void setBriefSelection(const Fleet*);
    void setDetailedSelection(const Planet*);
    void setDetailedSelection(const Fleet*);
    void clearBriefSelection();
    void clearDetailedSelection();

private:
    const GraphicsArray *graphicsArray;
    Player *player;
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
