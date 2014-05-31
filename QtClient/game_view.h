
#ifndef _GAME_VIEW_H_
#define _GAME_VIEW_H_

#include <QSplitter>

#include "FSServer.h"

#include "ui_message_widget.h"
#include "ui_status_selector.h"

namespace FreeStars {

class GameView  : public QSplitter {
    Q_OBJECT

public:
    GameView(const Player*);

    void setupMessages();
    void displayMessage(const Message&);

public slots:
    void selectPlanet(const Planet*);

    void nextMessage();
    void prevMessage();

private:
    QAbstractItemModel *getOwnPlanetsModel() const;
    const Player *player;
    std::vector<Message*> messages;
    Ui_MessageWidget ui_MessageWidget;
    Ui_StatusSelector ui_StatusSelector;

    int currentMessage;
};

};

#endif
