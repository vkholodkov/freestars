/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _SHIP_DESIGN_DIALOG_H_
#define _SHIP_DESIGN_DIALOG_H_

#include <QDialog>
#include <QButtonGroup>

#include "FSServer.h"

#include "ui_ship_design_dialog.h"

namespace FreeStars {

class ShipDesignDialog : public QDialog, private Ui_ShipDesignDialog {
    Q_OBJECT

public:
    ShipDesignDialog(Player*, QWidget *parent = 0);

public slots:
    void showShipDesigns();
    void showStarbaseDesigns();

    void showExistingDesigns();
    void showAvailableHullTypes();
    void showEnemyHulls();
    void showComponents();

    void copyDesign();
    void editDesign();
    void deleteDesign();

private:
    Player *player;
};

};

#endif
