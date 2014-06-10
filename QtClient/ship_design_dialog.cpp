/*
 * Copyright (C) 2014 Valery Kholodkov
 */
#include <cmath>

#include "ship_design_dialog.h"

namespace FreeStars {

ShipDesignDialog::ShipDesignDialog(Player *_player, QWidget *parent)
    : QDialog(parent)
    , player(_player)
{
    setupUi(this);

    connect(shipDesignsButton, SIGNAL(clicked(bool)), this, SLOT(showShipDesigns()));
    connect(starbaseDesignsButton, SIGNAL(clicked(bool)), this, SLOT(showStarbaseDesigns()));

    connect(existingDesignsButton, SIGNAL(clicked(bool)), this, SLOT(showExistingDesigns()));
    connect(availableHullTypesButton, SIGNAL(clicked(bool)), this, SLOT(showAvailableHullTypes()));
    connect(enemyHullsButton, SIGNAL(clicked(bool)), this, SLOT(showEnemyHulls()));
    connect(componentsButton, SIGNAL(clicked(bool)), this, SLOT(showComponents()));

    connect(copyDesignButton, SIGNAL(clicked(bool)), this, SLOT(copyDesign()));
    connect(editDesignButton, SIGNAL(clicked(bool)), this, SLOT(editDesign()));
    connect(deleteDesignButton, SIGNAL(clicked(bool)), this, SLOT(deleteDesign()));

    connect(doneButton, SIGNAL(clicked(bool)), this, SLOT(accept()));
}

void ShipDesignDialog::showShipDesigns()
{
}

void ShipDesignDialog::showStarbaseDesigns()
{
}

void ShipDesignDialog::showExistingDesigns()
{
    stackedWidget2->setCurrentIndex(0);
}

void ShipDesignDialog::showAvailableHullTypes()
{
    stackedWidget2->setCurrentIndex(0);
}

void ShipDesignDialog::showEnemyHulls()
{
    stackedWidget2->setCurrentIndex(0);
}

void ShipDesignDialog::showComponents()
{
    stackedWidget2->setCurrentIndex(1);
}

void ShipDesignDialog::copyDesign()
{
}

void ShipDesignDialog::editDesign()
{
}

void ShipDesignDialog::deleteDesign()
{
}

};
