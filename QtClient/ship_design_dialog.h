/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _SHIP_DESIGN_DIALOG_H_
#define _SHIP_DESIGN_DIALOG_H_

#include <QDialog>
#include <QButtonGroup>

#include "FSServer.h"

#include "graphics_array.h"

#include "ui_ship_design_dialog.h"

namespace FreeStars {

struct CompCategory;

class ShipDesignDialog : public QDialog, private Ui_ShipDesignDialog {
    Q_OBJECT

    typedef enum {
        SDDDM_NONE, SDDDM_SHIPS, SDDDM_STARBASES
    } design_mode_t;

    typedef enum {
        SDDVM_NONE, SDDVM_EXISTING, SDDVM_AVAILABLE, SDDVM_ENEMY, SDDVM_COMPONENTS
    } view_mode_t;

public:
    ShipDesignDialog(Player*, const GraphicsArray*, QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent*);

private slots:
    void setDesignMode(int);
    void setViewMode(int);
    void setComponentCategory(int);
    void setShipDesign(int);
    void setStarbaseDesign(int);
    void copyDesign();
    void editDesign();
    void deleteDesign();

private:
    void switchMode(int, int, int, int);
    void populateExistingDesigns(int);
    void populateAvailableHullTypes(int);

private:
    const GraphicsArray *graphicsArray;
    Player *player;
    int currentDesignMode, currentViewMode;
    Ship *currentShip;
};

};

#endif
