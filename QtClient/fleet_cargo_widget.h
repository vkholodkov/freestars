/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _FLEET_CARGO_WIDGET_H_
#define _FLEET_CARGO_WIDGET_H_

#include <QWidget>
#include <QComboBox>

#include "folding_widget.h"
#include "cargo_widget.h"

#include "FSServer.h"

#include "ui_fleet_cargo_widget.h"

namespace FreeStars {

class FleetCargoWidget : public FoldingWidget {
    Q_OBJECT

public:
    FleetCargoWidget(const Fleet*, const Player*, QWidget *parent = 0);

public slots:
    void cargoUpdated();

signals:
    void exchangeCargo(const Planet*, const Fleet*);

private slots:
    void cargoWidgetClicked();

private:
    void updateLabels();

private:
    Ui_FleetCargoWidget ui_FleetCargoWidget;
    const Fleet *fleet;
    const Player *player;
};

};

#endif
