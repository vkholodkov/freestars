
#include <QTableView>
#include <QStandardItemModel>

#include "game_view.h"

#include "message_formatter.h"
#include "folding_widget.h"
#include "vertical_flow_layout.h"

#include "ui_status_selector.h"
#include "ui_planet_report.h"
#include "ui_fleet_report.h"

namespace FreeStars {

GameView::GameView(const Player *_player)
    : player(_player)
    , QSplitter(Qt::Horizontal)
    , verticalFlowLayout(0)
    , productionQueueModel(0)
    , currentMessage(0)
    , currentSelection(0)
{
    QTableView *tableView = new QTableView;
    tableView->setModel(this->getOwnPlanetsModel());

    QSplitter *leftSplitter = new QSplitter(Qt::Vertical);
    QSplitter *rightSplitter = new QSplitter(Qt::Vertical);

    QFrame *container = new QFrame;
    container->setFrameShadow(QFrame::Sunken);
    container->setFrameShape(QFrame::Panel);

    QWidget *messageWidget = new QWidget;
    ui_MessageWidget.setupUi(messageWidget);

    QWidget *statusSelector = new QWidget;
    ui_StatusSelector.setupUi(statusSelector);
    ui_StatusSelector.titleLabel->setText("");

    leftSplitter->addWidget(container);
    leftSplitter->addWidget(messageWidget);
    leftSplitter->setStretchFactor(0, 20);
    leftSplitter->setStretchFactor(1, 1);

    rightSplitter->addWidget(tableView);
    rightSplitter->addWidget(statusSelector);
    rightSplitter->setStretchFactor(0, 10);
    rightSplitter->setStretchFactor(1, 1);

    this->addWidget(leftSplitter);
    this->addWidget(rightSplitter);
    this->setStretchFactor(0, 1);
    this->setStretchFactor(1, 2);

    verticalFlowLayout = new VerticalFlowLayout;
    container->setLayout(verticalFlowLayout);

    setupMessages();

    unsigned num_planets = TheGalaxy->GetPlanetCount();

    for(unsigned n = 1 ; n <= num_planets ; n++) {
        Planet *p = TheGalaxy->GetPlanet(n);

        if(p->GetOwner() == player) {
            clearSelection();
            selectPlanet(p);
            break;
        }
    }

    connect(ui_StatusSelector.nextButton, SIGNAL(clicked()), this, SLOT(nextObject()));
    connect(this, SIGNAL(selectionChanged(const SpaceObject*)), this, SLOT(selectObject(const SpaceObject*)));
}

QAbstractItemModel *GameView::getOwnPlanetsModel() const {
    static const char *column_names[] = {
        "Planet Name", "Starbase", "Population", "Cap", "Value", "Production",
        "Mines", "Factories", "Defenses", "Minerals", "Mining Rate",
        "Min Conc", "Resources", "Driver Dest", "Routing Dest", NULL
    };

    const char **column_name = column_names;

    unsigned num_planets = TheGalaxy->GetPlanetCount();
    unsigned num_owned = 0;

    for(unsigned n = 1 ; n <= num_planets ; n++) {
        Planet *p = TheGalaxy->GetPlanet(n);

        if(p->GetOwner() == player) {
            num_owned++;
        }
    }

    QStandardItemModel *model = new QStandardItemModel(num_owned, 5);

    int row = 0;
    int column = 0;

    QStandardItem *item;

    while(*column_name != NULL) {
        item = new QStandardItem(QString(*column_name));
        model->setHorizontalHeaderItem(column, item);
        column_name++;
        column++;
    }

    for(unsigned n = 1 ; n <= num_planets ; n++) {
        Planet *p = TheGalaxy->GetPlanet(n);

        if(p->GetOwner() != player) {
            continue;
        }

        QStandardItem *item = new QStandardItem(QString("%0").arg(p->GetName(player).c_str()));
        model->setItem(row, 0, item);

        if(p->GetBaseNumber() >= 0) {
            const Ship *base = p->GetBaseDesign();

            QStandardItem *item = new QStandardItem(QString("%0").arg(base->GetName().c_str()));
            model->setItem(row, 1, item);
        }

        long pop = p->GetDisplayPop();

        if(pop != 0) {
            QStandardItem *item = new QStandardItem(QString("%0").arg(pop));
            model->setItem(row, 2, item);
        }

        {
            QStandardItem *item = new QStandardItem(QString("%0").arg(p->GetMines()));
            model->setItem(row, 6, item);
        }

        {
            QStandardItem *item = new QStandardItem(QString("%0").arg(p->GetFactories()));
            model->setItem(row, 7, item);
        }

        double defense_value = p->GetDefenseValue();

        if(defense_value > 0.0) {
            QStandardItem *item = new QStandardItem(QString("%0%").arg(defense_value * 100, 2, 'g', 2));
            model->setItem(row, 8, item);
        }

        row++;
    }

    return model;
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

void GameView::selectPlanet(const Planet *_planet) {
    QStackedWidget *statusBed = ui_StatusSelector.statusBed;

    ui_StatusSelector.titleLabel->setText(tr("%0 Summary")
        .arg(_planet->GetName().c_str()));    

    QWidget *newPage = new QWidget;
    Ui_PlanetReport ui_PlanetReport;
    ui_PlanetReport.setupUi(newPage);
    statusBed->addWidget(newPage);

    ui_PlanetReport.valueLabel->setText(tr("Value: %0%")
        .arg(_planet->GetOwner()->HabFactor(_planet)));

    ui_PlanetReport.popLabel->setText(tr("Population: %0")
        .arg(_planet->GetDisplayPop()));

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

    long ironium = _planet->GetContain(0);
    long boranium = _planet->GetContain(1);
    long germanium = _planet->GetContain(2);

    ui_PlanetReport.mineralReport->setIronium(ironium);
    ui_PlanetReport.mineralReport->setBoranium(boranium);
    ui_PlanetReport.mineralReport->setGermanium(germanium);

    ui_PlanetReport.ironiumOverflowLabel->setText(ironium > 5000 ? "+" : " ");
    ui_PlanetReport.boraniumOverflowLabel->setText(boranium > 5000 ? "+" : " ");
    ui_PlanetReport.germaniumOverflowLabel->setText(germanium > 5000 ? "+" : " ");

    ui_PlanetReport.mineralReport->setIroniumConc(_planet->GetMinConc(0));
    ui_PlanetReport.mineralReport->setBoraniumConc(_planet->GetMinConc(1));
    ui_PlanetReport.mineralReport->setGermaniumConc(_planet->GetMinConc(2));

    ui_PlanetReport.scaleWidget->setMineralReport(ui_PlanetReport.mineralReport);;

    FoldingWidget *w1 = new FoldingWidget(tr("Planet"));
    w1->setObjectName("w1_column1");

    FoldingWidget *w11 = new FoldingWidget(tr("Minerals on hand"));
    w11->setObjectName("w11_column1");

    FoldingWidget *w12 = new FoldingWidget(tr("Status"));
    w12->setObjectName("w12_column1");

    FoldingWidget *w2 = new FoldingWidget(tr("Fleets in Orbit"));
    w2->setObjectName("w2_column2");

    FoldingWidget *w3 = new FoldingWidget(tr("Production"));
    w3->setObjectName("w3_column2");

    verticalFlowLayout->addWidget(w1);
    verticalFlowLayout->addWidget(w11);
    verticalFlowLayout->addWidget(w12);
    verticalFlowLayout->addWidget(w2);
    verticalFlowLayout->addWidget(w3);

    QWidget *fleetsInOrbitWidget = new QWidget;
    ui_FleetsInOrbitWidget.setupUi(fleetsInOrbitWidget);

    w2->addWidget(fleetsInOrbitWidget);

    /*
     * Planet production
     */
    QWidget *planetProductionWidget = new QWidget;
    ui_PlanetProductionWidget.setupUi(planetProductionWidget);

    productionQueueModel = new ProductionQueueModel(_planet->GetProduction());
    QItemSelectionModel *m = ui_PlanetProductionWidget.productionQueueView->selectionModel();
    ui_PlanetProductionWidget.productionQueueView->setModel(productionQueueModel);
    delete m;

    ui_PlanetProductionWidget.productionQueueView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    ui_PlanetProductionWidget.productionQueueView->horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);

    ui_PlanetProductionWidget.completionLabel->setText("");
    ui_PlanetProductionWidget.routeToLabel->setText(
        _planet->GetRoute() != NULL ? QString(_planet->GetRoute()->GetName(player).c_str()) : tr("none"));

    w3->addWidget(planetProductionWidget);

    /*
     * Minerals on hand
     */
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

    currentSelection = _planet;
}

void GameView::selectFleet(const Fleet *_fleet) {
    QStackedWidget *statusBed = ui_StatusSelector.statusBed;

    ui_StatusSelector.titleLabel->setText(tr("%0 Summary")
        .arg(_fleet->GetName(player).c_str()));    

    QWidget *newPage = new QWidget;
    Ui_FleetReport ui_FleetReport;
    ui_FleetReport.setupUi(newPage);
    statusBed->addWidget(newPage);

    currentSelection = _fleet;
}

void GameView::clearSelection()
{
    QStackedWidget *statusBed = ui_StatusSelector.statusBed;

    while(statusBed->count()) {
        QWidget *widget = statusBed->currentWidget();
        statusBed->removeWidget(widget);
        delete widget;
    }

    QLayoutItem *child;
    while((child = verticalFlowLayout->takeAt(0)) != 0) {
        delete child;
    }
}

void GameView::selectObject(const SpaceObject *so)
{
    const Planet *p = dynamic_cast<const Planet*>(so);

    if(p != NULL) {
        clearSelection();
        selectPlanet(p);
        return;
    }

    const Fleet *f = dynamic_cast<const Fleet*>(so);

    if(f != NULL) {
        clearSelection();
        selectFleet(f);
        return;
    }
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
        const deque<SpaceObject *> * deq = currentSelection->GetAlsoHere();

        if(deq != NULL) {
            deque<SpaceObject *>::const_iterator i = deq->begin();

            while(i != deq->end()) {
                if(*i != currentSelection) {
                    emit selectionChanged(*i);
                    break;
                }
                i++;
            }
        }
    }
}

void GameView::changeProductionQueue()
{
}

void GameView::clearProductionQueue()
{
}

void GameView::setRouteDest()
{
}

};
