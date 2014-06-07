/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _FLEET_COMPOSITION_WIDGET_H_
#define _FLEET_COMPOSITION_WIDGET_H_

#include <QWidget>
#include <QComboBox>

#include "folding_widget.h"
#include "cargo_widget.h"

#include "FSServer.h"

namespace FreeStars {

class FleetCompositionWidget : public FoldingWidget {
    Q_OBJECT

public:
    FleetCompositionWidget(const Fleet*, const Player*, QWidget *parent = 0);

signals:
    void splitFleet(const Fleet*);
    void splitAllFleet(const Fleet*);
    void mergeFleet(const Fleet*);

private slots:
    void splitButtonClicked(bool);
    void splitAllButtonClicked(bool);
    void mergeButtonClicked(bool);

private:
    const Fleet *fleet;
    const Player *player;
};

};

#endif
