/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include "orbiting_widget.h"

#include "ui_orbiting_widget.h"

namespace FreeStars {

OrbitingWidget::OrbitingWidget(const Planet *_planet, const Fleet *_fleet, const Player *_player, QWidget *parent)
    : FoldingWidget(tr("Orbiting %0").arg(_planet->GetName().c_str()))
    , planet(_planet)
    , fleet(_fleet)
    , player(_player)
{
    QWidget *widget = new QWidget;

    Ui_OrbitingWidget ui_OrbitingWidget;
    ui_OrbitingWidget.setupUi(widget);

    connect(ui_OrbitingWidget.gotoButton, SIGNAL(clicked(bool)), this, SLOT(gotoButtonClicked(bool)));
    connect(ui_OrbitingWidget.xferButton, SIGNAL(clicked(bool)), this, SLOT(xferButtonClicked(bool)));

    this->addWidget(widget);
}

void OrbitingWidget::gotoButtonClicked(bool)
{
    emit selectObject(planet);
}

void OrbitingWidget::xferButtonClicked(bool)
{
    emit exchangeCargo(planet, fleet);
}

};
