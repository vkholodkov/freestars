/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _PLANET_PRODUCTION_WIDGET_H_
#define _PLANET_PRODUCTION_WIDGET_H_

#include <QWidget>

#include "folding_widget.h"

#include "FSServer.h"

namespace FreeStars {

class PlanetProductionWidget : public FoldingWidget {
    Q_OBJECT

public:
    PlanetProductionWidget(const Planet*, const Player*, QWidget *parent = 0);

public slots:
    void changeButtonClicked(bool);
    void clearButtonClicked(bool);

signals:
    void changeProductionQueue(const Planet*);
    void clearProductionQueue(const Planet*);

private:
    const Planet *planet;
};

};

#endif
