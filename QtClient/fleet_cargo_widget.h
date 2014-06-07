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

namespace FreeStars {

class FleetCargoWidget : public FoldingWidget {
    Q_OBJECT

public:
    FleetCargoWidget(const Fleet*, const Player*, QWidget *parent = 0);

signals:
    void exchangeCargo(const Planet*, const Fleet*);

private:
    void updateLabels();

private:
    const Fleet *fleet;
    const Player *player;
};

};

#endif
