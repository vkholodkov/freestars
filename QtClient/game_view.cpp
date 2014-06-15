/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <QMenu>
#include <QSplitter>
#include <QScrollArea>
#include <QStandardItemModel>

#include "game_view.h"

#include "message_formatter.h"
#include "folding_widget.h"
#include "vertical_flow_layout.h"
#include "space_object_sorter.h"

#include "fleets_in_orbit_widget.h"
#include "fleet_widget.h"
#include "orbiting_widget.h"
#include "fleet_cargo_widget.h"
#include "fleet_composition_widget.h"

#include "research_dialog.h"
#include "ship_design_dialog.h"

#include "ui_planet_widget.h"
#include "ui_starbase_widget.h"
#include "ui_status_selector.h"
#include "ui_planet_report.h"
#include "ui_fleet_report.h"

namespace FreeStars {

GameView::GameView(Player *_player, const GraphicsArray *_graphicsArray)
    : QSplitter(Qt::Horizontal)
    , graphicsArray(_graphicsArray)
    , player(_player)
    , mapView(0)
    , mapScroller(0)
    , verticalFlowLayout(0)
    , currentMessage(0)
    , currentSelection(0)
{
    mapView = new MapView(TheGalaxy, TheGame, _player);
    mapScroller = new QScrollArea;
    mapScroller->setWidget(mapView);
//    mapScroller->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    QSplitter *leftSplitter = new QSplitter(Qt::Vertical);
    QSplitter *rightSplitter = new QSplitter(Qt::Vertical);

    QFrame *container = new QFrame;
    container->setFrameShadow(QFrame::Sunken);
    container->setFrameShape(QFrame::Panel);

    verticalFlowLayout = new VerticalFlowLayout(3, 3, 2);
    container->setLayout(verticalFlowLayout);

    QWidget *messageWidget = new QWidget;
    ui_MessageWidget.setupUi(messageWidget);

    QWidget *statusSelector = new QWidget;
    ui_StatusSelector.setupUi(statusSelector);
    ui_StatusSelector.titleLabel->setText("");

    leftSplitter->addWidget(container);
    leftSplitter->addWidget(messageWidget);
    leftSplitter->setStretchFactor(0, 20);
    leftSplitter->setStretchFactor(1, 1);

    rightSplitter->addWidget(mapScroller);
    rightSplitter->addWidget(statusSelector);
    rightSplitter->setStretchFactor(0, 10);
    rightSplitter->setStretchFactor(1, 1);

    this->addWidget(leftSplitter);
    this->addWidget(rightSplitter);
    this->setStretchFactor(0, 1);
    this->setStretchFactor(1, 1);

    setupMessages();

    unsigned num_planets = TheGalaxy->GetPlanetCount();

    for(unsigned n = 1 ; n <= num_planets ; n++) {
        Planet *p = TheGalaxy->GetPlanet(n);

        if(p->GetOwner() == player) {
            selectObject(p);
            break;
        }
    }

    connect(ui_StatusSelector.nextButton, SIGNAL(clicked()), this, SLOT(nextObject()));
    connect(this, SIGNAL(selectionChanged(const SpaceObject*)), this, SLOT(selectObject(const SpaceObject*)));
    connect(mapView, SIGNAL(selectionChanged(const SpaceObject*)), this, SLOT(selectObject(const SpaceObject*)));
    connect(mapView, SIGNAL(listObjectsInLocation(const SpaceObject*, const QPoint&)),
        this, SLOT(listObjectsInLocation(const SpaceObject*, const QPoint&)));
}

void GameView::setupMessages() {
    const deque<Message *> &_messages = player->GetMessages();

    messages.reserve(_messages.size());

    for(deque<Message *>::const_iterator i = _messages.begin() ; i != _messages.end() ; i++) {
        messages.push_back(*i);
    }

    connect(ui_MessageWidget.nextButton, SIGNAL(clicked()), this, SLOT(nextMessage()));
    connect(ui_MessageWidget.prevButton, SIGNAL(clicked()), this, SLOT(prevMessage()));

    currentMessage = 0;

    displayMessage(*messages[currentMessage]);
}

void GameView::setBriefSelection(const Planet *_planet) {
    long seen = _planet->SeenBy(player);

    QStackedWidget *statusBed = ui_StatusSelector.statusBed;

    ui_StatusSelector.titleLabel->setText(tr("%0 Summary")
        .arg(_planet->GetName().c_str()));    

    if(seen & (SEEN_PLANETHAB|SEEN_PLANETPOP|SEEN_PLANETMC)) {
        QWidget *newPage = new QWidget;
        Ui_PlanetReport ui_PlanetReport;
        ui_PlanetReport.setupUi(newPage);
        statusBed->addWidget(newPage);

        if(seen & SEEN_PLANETHAB) {
            ui_PlanetReport.valueLabel->setText(tr("Value: %0%")
                .arg(_planet->GetOwner()->HabFactor(_planet)));
        }

        if(seen & SEEN_PLANETPOP) {
            ui_PlanetReport.popLabel->setText(tr("Population: %0")
                .arg(_planet->GetDisplayPop()));
        }

        if(seen & SEEN_PLANETHAB) {
            ui_PlanetReport.gravityLabel->setText(tr("%0g")
                .arg((double)_planet->GetHabValue(0) / 10, 0, 'f', 1));

            ui_PlanetReport.habitationBar->setGravityRange(HabRange(player->HabCenter(0),
                player->HabWidth(0)));

            ui_PlanetReport.habitationBar->setGravityValue(_planet->GetHabValue(0));

            ui_PlanetReport.tempLabel->setText(trUtf8("%0°C")
                .arg(_planet->GetHabValue(1)));

            ui_PlanetReport.habitationBar->setTempRange(HabRange(player->HabCenter(1),
                player->HabWidth(1)));

            ui_PlanetReport.habitationBar->setTempValue(_planet->GetHabValue(1));

            ui_PlanetReport.radLabel->setText(tr("%0mR")
                .arg(_planet->GetHabValue(2)));

            ui_PlanetReport.habitationBar->setRadRange(HabRange(player->HabCenter(2),
                player->HabWidth(2)));

            ui_PlanetReport.habitationBar->setRadValue(_planet->GetHabValue(2));
        }

        if(seen & SEEN_PLANETMC) {
            long ironium = _planet->GetContain(0);
            long boranium = _planet->GetContain(1);
            long germanium = _planet->GetContain(2);

            ui_PlanetReport.mineralReport->setIronium(ironium);
            ui_PlanetReport.mineralReport->setBoranium(boranium);
            ui_PlanetReport.mineralReport->setGermanium(germanium);

            ui_PlanetReport.mineralReport->setIroniumVelocity(_planet->GetMiningVelocity(0));
            ui_PlanetReport.mineralReport->setBoraniumVelocity(_planet->GetMiningVelocity(1));
            ui_PlanetReport.mineralReport->setGermaniumVelocity(_planet->GetMiningVelocity(2));

            ui_PlanetReport.ironiumOverflowLabel->setText(ironium > 5000 ? "+" : " ");
            ui_PlanetReport.boraniumOverflowLabel->setText(boranium > 5000 ? "+" : " ");
            ui_PlanetReport.germaniumOverflowLabel->setText(germanium > 5000 ? "+" : " ");

            ui_PlanetReport.mineralReport->setIroniumConc(_planet->GetMinConc(0));
            ui_PlanetReport.mineralReport->setBoraniumConc(_planet->GetMinConc(1));
            ui_PlanetReport.mineralReport->setGermaniumConc(_planet->GetMinConc(2));

            ui_PlanetReport.scaleWidget->setMineralReport(ui_PlanetReport.mineralReport);;
        }
    }

    SpaceObjectSorter sos(_planet);
    currentStack.assign(sos.m_object_list.begin(), sos.m_object_list.end());
    currentSelection = _planet;
    currentObject = std::find(currentStack.begin(), currentStack.end(), currentSelection) - currentStack.begin();
}

void GameView::setBriefSelection(const Fleet *_fleet) {
    QStackedWidget *statusBed = ui_StatusSelector.statusBed;

    ui_StatusSelector.titleLabel->setText(tr("%0 Summary")
        .arg(_fleet->GetName(player).c_str()));    

    QWidget *newPage = new QWidget;
    Ui_FleetReport ui_FleetReport;
    ui_FleetReport.setupUi(newPage);
#if 0
    const Stack *stack = _fleet->GetStack(0);
    const Ship *ship = stack->GetDesign();

    ui_FleetReport.shipAvatarWidget->setGraphicsArray(graphicsArray);
    ui_FleetReport.shipAvatarWidget->setHullName(ship->GetHull()->GetName().c_str());
#endif
    statusBed->addWidget(newPage);

    currentSelection = _fleet;
}

void GameView::setDetailedSelection(const Planet *_planet) {
    FoldingWidget *w1 = new FoldingWidget(_planet->GetName().c_str());
    w1->setObjectName("w1_column1");

    FoldingWidget *w11 = new FoldingWidget(tr("Minerals on hand"));
    w11->setObjectName("w11_column1");

    FoldingWidget *w12 = new FoldingWidget(tr("Status"));
    w12->setObjectName("w12_column1");

    verticalFlowLayout->addWidget(w1);
    verticalFlowLayout->addWidget(w11);
    verticalFlowLayout->addWidget(w12);

    /*
     * Fleets in orbit widget
     */
    FleetsInOrbitWidget *fleetsInOrbitWidget = new FleetsInOrbitWidget(_planet, player);
    fleetsInOrbitWidget->setObjectName("w2_column2");
    verticalFlowLayout->addWidget(fleetsInOrbitWidget);
    
    connect(fleetsInOrbitWidget, SIGNAL(selectObject(const SpaceObject*)),
        this, SLOT(selectObject(const SpaceObject*)));
    connect(fleetsInOrbitWidget, SIGNAL(exchangeCargo(const Planet*, const Fleet*)),
        this, SLOT(exchangeCargo(const Planet*, const Fleet*)));

    /*
     * Planet production widget
     */
    planetProductionWidget = new PlanetProductionWidget(_planet, player);
    planetProductionWidget->setObjectName("w3_column2");
    verticalFlowLayout->addWidget(planetProductionWidget);

    connect(planetProductionWidget, SIGNAL(changeProductionQueue(const Planet*)),
        this, SLOT(changeProductionQueue(const Planet*)));
    connect(planetProductionWidget, SIGNAL(clearProductionQueue(const Planet*)),
        this, SLOT(clearProductionQueue(const Planet*)));

    /*
     * Planet widget
     */
    QWidget *planetWidget = new QWidget;
    Ui_PlanetWidget ui_PlanetWidget;
    ui_PlanetWidget.setupUi(planetWidget);

    w1->addWidget(planetWidget);

    /*
     * Minerals on hand
     */
    long ironium = _planet->GetContain(0);
    long boranium = _planet->GetContain(1);
    long germanium = _planet->GetContain(2);

    QWidget *mineralsOnHandWidget = new QWidget;
    ui_MineralsOnHandWidget.setupUi(mineralsOnHandWidget);

    ui_MineralsOnHandWidget.ironiumLabel->setText(tr("%0kT").arg(ironium));
    ui_MineralsOnHandWidget.boraniumLabel->setText(tr("%0kT").arg(boranium));
    ui_MineralsOnHandWidget.germaniumLabel->setText(tr("%0kT").arg(germanium));
    ui_MineralsOnHandWidget.minesLabel->setText(tr("%0 of %1").arg(_planet->GetMines()).arg(_planet->MaxMines()));
    ui_MineralsOnHandWidget.factoriesLabel->setText(tr("%0 of %1").arg(_planet->GetFactories()).arg(_planet->MaxFactories()));

    w11->addWidget(mineralsOnHandWidget);

    /*
     * Planet status
     */
    QWidget *planetStatusWidget = new QWidget;
    ui_PlanetStatusWidget.setupUi(planetStatusWidget);

    ui_PlanetStatusWidget.populationLabel->setText(QString::number(_planet->GetDisplayPop()));
    ui_PlanetStatusWidget.resourcesLabel->setText(tr("%0 of %1").arg(_planet->GetResources()).arg(_planet->GetResources()));

    if(_planet->GetScanner()) {
        ui_PlanetStatusWidget.scannerTypeLabel->setText(tr("yes"));
        ui_PlanetStatusWidget.scannerRangeLabel->setText(tr("%0 l.y.").arg(_planet->GetScanSpace()));
    }
    else {
        ui_PlanetStatusWidget.scannerTypeLabel->setText(tr("none"));
        ui_PlanetStatusWidget.scannerRangeLabel->setText(tr("none"));
    }

    ui_PlanetStatusWidget.defencesLabel->setText(tr("%0 of %1").arg(_planet->GetDefenses()).arg(_planet->MaxDefenses()));
    ui_PlanetStatusWidget.defenceTypeLabel->setText(tr("none"));
    ui_PlanetStatusWidget.defencesCoverageLabel->setText(tr("none"));

    w12->addWidget(planetStatusWidget);

    /*
     * Starbase view
     */
    const Ship *base = _planet->GetBaseDesign();

    if(base != NULL) {
        FoldingWidget *w4 = new FoldingWidget(tr("Starbase"));
        w4->setObjectName("w4_column2");

        QWidget *planetStatusWidget = new QWidget;
        Ui_StarbaseWidget ui_StarbaseWidget;
        ui_StarbaseWidget.setupUi(planetStatusWidget);

        ui_StarbaseWidget.dockCapacityLabel->setText(base->GetDockBuildCapacity() != -1 ?
            QString("%0").arg(base->GetDockBuildCapacity()) : tr("Unlimited"));
        ui_StarbaseWidget.armorLabel->setText(tr("%0dp").arg(base->GetArmor(player)));
        ui_StarbaseWidget.shieldsLabel->setText(tr("%0dp").arg(base->GetShield(player)));
        ui_StarbaseWidget.damageLabel->setText(_planet->GetBaseDamage() != 0 ?
            tr("%0dp").arg(_planet->GetBaseDamage()) : tr("none"));

        ui_StarbaseWidget.packetSpeedLabel->setText(_planet->GetPacketSpeed() != 0 ?
            tr("Warp %0").arg(_planet->GetPacketSpeed()) : tr("none"));

        ui_StarbaseWidget.packetDestLabel->setText(_planet->GetPacketDest() != 0 ?
            _planet->GetPacketDest()->GetName().c_str() : tr("none"));

        ui_StarbaseWidget.setDestButton->setEnabled(_planet->GetPacketSpeed() != 0);

        w4->addWidget(planetStatusWidget);
        
        verticalFlowLayout->addWidget(w4);
    }
}

void GameView::setDetailedSelection(const Fleet *_fleet) {
    /*
     * Fleet widget
     */
    FleetWidget *fleetWidget = new FleetWidget(graphicsArray, _fleet, player);
    fleetWidget->setObjectName("w1_column1");
    verticalFlowLayout->addWidget(fleetWidget);
    
    connect(fleetWidget, SIGNAL(nextObject()), this, SLOT(nextObject()));
    connect(fleetWidget, SIGNAL(prevObject()), this, SLOT(prevObject()));
    connect(fleetWidget, SIGNAL(renameObject(const SpaceObject*)),
        this, SLOT(renameObject(const SpaceObject*)));

    const Planet *inOrbit = _fleet->InOrbit();

    if(inOrbit != NULL) {
        /*
         * Orbiting widget
         */
        OrbitingWidget *orbitingWidget = new OrbitingWidget(inOrbit, _fleet, player);
        orbitingWidget->setObjectName("w2_column1");
        verticalFlowLayout->addWidget(orbitingWidget);
        
        connect(orbitingWidget, SIGNAL(selectObject(const SpaceObject*)),
            this, SLOT(selectObject(const SpaceObject*)));
        connect(orbitingWidget, SIGNAL(exchangeCargo(const Planet*, const Fleet*)),
            this, SLOT(exchangeCargo(const Planet*, const Fleet*)));
    }

    /*
     * Fleet cargo widget
     */
    FleetCargoWidget *fleetCargoWidget = new FleetCargoWidget(_fleet, player);
    fleetCargoWidget->setObjectName("w1_column2");
    verticalFlowLayout->addWidget(fleetCargoWidget);
    
    connect(fleetCargoWidget, SIGNAL(exchangeCargo(const Planet*, const Fleet*)),
        this, SLOT(exchangeCargo(const Planet*, const Fleet*)));

    /*
     * Fleet composition widget
     */
    FleetCompositionWidget *fleetCompositionWidget = new FleetCompositionWidget(_fleet, player);
    fleetCompositionWidget->setObjectName("w2_column2");
    verticalFlowLayout->addWidget(fleetCompositionWidget);
    
    connect(fleetCompositionWidget, SIGNAL(splitFleet(const Fleet*)),
        this, SLOT(splitFleet(const Fleet*)));
    connect(fleetCompositionWidget, SIGNAL(splitAllFleet(const Fleet*)),
        this, SLOT(splitAllFleet(const Fleet*)));
    connect(fleetCompositionWidget, SIGNAL(mergeFleet(const Fleet*)),
        this, SLOT(mergeFleet(const Fleet*)));
}

void GameView::clearBriefSelection()
{
    mapView->clearSelection();

    QStackedWidget *statusBed = ui_StatusSelector.statusBed;

    while(statusBed->count()) {
        QWidget *widget = statusBed->currentWidget();
        statusBed->removeWidget(widget);
        delete widget;
    }
}

void GameView::clearDetailedSelection()
{
    QLayoutItem *item;
    while ((item = verticalFlowLayout->itemAt(0))) {
        verticalFlowLayout->removeItem(item);
        delete item->widget();
        delete item;
    }
}

void GameView::selectObject(const SpaceObject *so)
{
    QPoint pos(mapView->galaxyToScreen(QPoint(so->GetPosX(), so->GetPosY())));

    clearBriefSelection();

    if(so->GetOwner() == player) {
        clearDetailedSelection();
    }

    mapScroller->ensureVisible(pos.x(), pos.y());
    mapView->setSelection(so);

    const Planet *p = dynamic_cast<const Planet*>(so);

    if(p != NULL) {
        setBriefSelection(p);

        if(so->GetOwner() == player) {
            setDetailedSelection(p);
        }
        return;
    }

    const Fleet *f = dynamic_cast<const Fleet*>(so);

    if(f != NULL) {
        setBriefSelection(f);

        if(so->GetOwner() == player) {
            setDetailedSelection(f);
        }
        return;
    }
}

void GameView::listObjectsInLocation(const SpaceObject *o, const QPoint &pos)
{
    typedef std::map<QAction*, const SpaceObject*> action_map_t;
    SpaceObjectSorter sos(o);
    action_map_t actions;

    QMenu menu(this);

    for(std::vector<const SpaceObject*>::const_iterator i = sos.m_object_list.begin() ;
        i != sos.m_object_list.end() ; i++)
    {
        const Planet *p = dynamic_cast<const Planet*>(*i);

        if(p != NULL) {
            QAction *action = menu.addAction(QString(p->GetName().c_str()));
            menu.addSeparator();
            actions.insert(std::make_pair(action, *i));
            continue;
        }

        const Fleet *f = dynamic_cast<const Fleet*>(*i);

        if(f != NULL) {
            QAction *action = menu.addAction(QString(f->GetName(player).c_str()));
            actions.insert(std::make_pair(action, *i));
            continue;
        }

        menu.addAction(tr("Unknown object #%0").arg((*i)->GetID()));
    }

    QAction *clicked = menu.exec(pos);

    action_map_t::const_iterator ai = actions.find(clicked);

    if(ai != actions.end()) {
        emit selectionChanged(ai->second);
    }
}

void GameView::renameObject(const SpaceObject *o)
{
    std::cout << "GameView::renameObject o=" << o << std::endl;
}

void GameView::displayMessage(const Message &_message) {
    MessageFormatter messageFormatter(player);
    _message.ApplyVisitor(messageFormatter);
    ui_MessageWidget.messagePane->setText(messageFormatter.toString());    
    ui_MessageWidget.titleLabel->setText(tr("Year %0   Messages: %1 of %2")
        .arg(TheGame->GetTurn())
        .arg(currentMessage + 1)
        .arg(messages.size()));
    
    ui_MessageWidget.nextButton->setEnabled(currentMessage < messages.size()-1);
    ui_MessageWidget.gotoButton->setEnabled(false);
    ui_MessageWidget.prevButton->setEnabled(currentMessage > 0);
}

void GameView::nextMessage()
{
    if(currentMessage < messages.size()-1) {
        currentMessage++;
        displayMessage(*messages[currentMessage]);
    }
}

void GameView::prevMessage()
{
    if(currentMessage > 0) {
        currentMessage--;
        displayMessage(*messages[currentMessage]);
    }
}

void GameView::nextObject()
{
    if(currentSelection != NULL) {
        if(currentObject < currentStack.size()-1) {
            currentObject++;
        }
        else {
            currentObject = 0;
        }
        emit selectionChanged(currentStack[currentObject]);
    }
}

void GameView::prevObject()
{
    if(currentSelection != NULL) {
        if(currentObject > 0) {
            currentObject--;
        }
        else {
            currentObject = currentStack.size()-1;
        }
        emit selectionChanged(currentStack[currentObject]);
    }
}

void GameView::changeProductionQueue(const Planet *planet)
{
    std::cout << "GameView::changeProductionQueue " << planet << std::endl;
}

void GameView::clearProductionQueue(const Planet *planet)
{
    std::cout << "GameView::clearProductionQueue " << planet << std::endl;
}

void GameView::setRouteDest()
{
    std::cout << "GameView::setRouteDest " << std::endl;
}

void GameView::exchangeCargo(const Planet *planet, const Fleet *fleet)
{
    std::cout << "GameView::exchangeCargo planet=" << planet << " fleet=" << fleet << std::endl;
}

void GameView::splitFleet(const Fleet *_fleet)
{
    std::cout << "GameView::splitFleet fleet=" << _fleet << std::endl;
}

void GameView::splitAllFleet(const Fleet *_fleet)
{
    std::cout << "GameView::splitAllFleet fleet=" << _fleet << std::endl;
}

void GameView::mergeFleet(const Fleet *_fleet)
{
    std::cout << "GameView::mergeFleet fleet=" << _fleet << std::endl;
}

void GameView::showProductionDialog(bool)
{
    std::cout << "GameView::showProductionDialog" << std::endl;
}

void GameView::shipDesignDialog()
{
    ShipDesignDialog shipDesignDialog(player, graphicsArray);
    shipDesignDialog.exec();
}

void GameView::researchDialog()
{
    ResearchDialog researchDialog(player);
    researchDialog.exec();
}

void GameView::battlePlansDialog()
{
    std::cout << "GameView::battlePlansDialog" << std::endl;
}

void GameView::playerRelationsDialog()
{
    std::cout << "GameView::playerRelationsDialog" << std::endl;
}

void GameView::submitTurn()
{
    std::cout << "GameView::submitTurn" << std::endl;
}

};
