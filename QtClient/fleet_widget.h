/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _FLEET_WIDGET_H_
#define _FLEET_WIDGET_H_

#include <QWidget>
#include <QComboBox>

#include "folding_widget.h"
#include "cargo_widget.h"
#include "graphics_array.h"

#include "FSServer.h"

namespace FreeStars {

class FleetWidget : public FoldingWidget {
    Q_OBJECT

public:
    FleetWidget(const GraphicsArray*, const Fleet*, const Player*, QWidget *parent = 0);

signals:
    void nextObject();
    void prevObject();
    void renameObject(const SpaceObject*);

private slots:
    void nextButtonClicked(bool);
    void prevButtonClicked(bool);
    void renameButtonClicked(bool);

private:
    const Fleet *fleet;
    const Player *player;
};

};

#endif
