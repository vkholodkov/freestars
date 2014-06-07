/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include "fleet_widget.h"

#include "ui_fleet_widget.h"

namespace FreeStars {

FleetWidget::FleetWidget(const Fleet *_fleet, const Player *_player, QWidget *parent)
    : FoldingWidget(QString(_fleet->GetName(_player).c_str()))
    , fleet(_fleet)
    , player(_player)
{
    QWidget *widget = new QWidget;

    Ui_FleetWidget ui_FleetWidget;
    ui_FleetWidget.setupUi(widget);

    connect(ui_FleetWidget.nextButton, SIGNAL(clicked(bool)), this, SLOT(nextButtonClicked(bool)));
    connect(ui_FleetWidget.prevButton, SIGNAL(clicked(bool)), this, SLOT(prevButtonClicked(bool)));
    connect(ui_FleetWidget.renameButton, SIGNAL(clicked(bool)), this, SLOT(renameButtonClicked(bool)));

    this->addWidget(widget);
}

void FleetWidget::nextButtonClicked(bool)
{
    emit nextObject();
}

void FleetWidget::prevButtonClicked(bool)
{
    emit prevObject();
}

void FleetWidget::renameButtonClicked(bool)
{
    emit renameObject(fleet);
}

};
