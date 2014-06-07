/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _ORBITING_WIDGET_H_
#define _ORBITING_WIDGET_H_

#include <QWidget>
#include <QComboBox>

#include "folding_widget.h"
#include "cargo_widget.h"

#include "FSServer.h"

namespace FreeStars {

class OrbitingWidget : public FoldingWidget {
    Q_OBJECT

public:
    OrbitingWidget(const Planet*, const Fleet*, const Player*, QWidget *parent = 0);

signals:
    void selectObject(const SpaceObject*);
    void exchangeCargo(const Planet*, const Fleet*);

private slots:
    void gotoButtonClicked(bool);
    void xferButtonClicked(bool);

private:
    const Planet *planet;
    const Fleet *fleet;
    const Player *player;
};

};

#endif
