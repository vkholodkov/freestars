/*
 * Copyright (C) 2014 Valery Kholodkov
 */
#ifndef _ADVANCED_NEW_GAME_WIZARD_H_
#define _ADVANCED_NEW_GAME_WIZARD_H_

#include <memory>

#include <QLabel>
#include <QDialog>
#include <QStackedWidget>
#include <QSignalMapper>
#include <QDataWidgetMapper>

#include "FSServer.h"
#include "MessageSink.h"

namespace FreeStars {

class AdvancedNewGameWizard : public QDialog, private MessageSink {
    Q_OBJECT

public:
    AdvancedNewGameWizard(QWidget *parent = 0);
    ~AdvancedNewGameWizard();

    std::auto_ptr<VictoryConditions> victoryConditions;

private slots:
    void helpClicked(bool);
    void cancelClicked(bool);
    void backClicked(bool);
    void nextClicked(bool);
    void finishClicked(bool);

    void selectPlayer(int);

    void newRace();
    void openRace();
    void editRace();

private:
    void pageChanged();

    Message *AddMessage(const std::string &type);

private:
    QAction *newRaceAction;
    QAction *openRaceAction;
    QAction *editRaceAction;

    QPushButton *nextButton;
    QPushButton *backButton;
    QStackedWidget *pagesWidget;

    QSignalMapper *playerButtonsMapper;

    QAbstractItemModel *victoryConditionsModel;
    QDataWidgetMapper *victoryConditionsMapper;

    std::list<Message*> messages;
    std::vector<Race*> races;
    std::vector<QLabel*> raceLabels;

    std::auto_ptr<Game> game;
    int currentPlayer;
};

};

#endif
