
#ifndef _GAME_VIEW_H_
#define _GAME_VIEW_H_

#include <QSplitter>

#include "FSServer.h"

#include "ui_message_widget.h"
#include "ui_status_selector.h"
#include "ui_planet_production_widget.h"
#include "ui_fleets_in_orbit_widget.h"
#include "ui_minerals_on_hand_widget.h"
#include "ui_planet_status_widget.h"

#include "production_queue_model.h"

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

    void changeProductionQueue();
    void clearProductionQueue();
    void setRouteDest();

private:
    void selectPlanet(const Planet*);
    void selectFleet(const Fleet*);
    void clearSelection();

private:
    QAbstractItemModel *getOwnPlanetsModel() const;
    const Player *player;
    std::vector<Message*> messages;
    Ui_MessageWidget ui_MessageWidget;
    Ui_StatusSelector ui_StatusSelector;
    Ui_FleetsInOrbitWidget ui_FleetsInOrbitWidget;
    Ui_PlanetProductionWidget ui_PlanetProductionWidget;
    Ui_MineralsOnHandWidget ui_MineralsOnHandWidget;
    Ui_PlanetStatusWidget ui_PlanetStatusWidget;
    QLayout *verticalFlowLayout;
    ProductionQueueModel *productionQueueModel;
    int currentMessage;
    const SpaceObject *currentSelection;
};

};

#endif
