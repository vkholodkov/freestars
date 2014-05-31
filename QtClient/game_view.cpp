
#include <QTableView>
#include <QStandardItemModel>

#include "game_view.h"

#include "message_formatter.h"
#include "folding_widget.h"
#include "vertical_flow_layout.h"

#include "ui_status_selector.h"
#include "ui_planet_report.h"
#include "ui_planet_production_widget.h"
#include "ui_fleets_in_orbit_widget.h"

namespace FreeStars {

GameView::GameView(const Player *_player)
    : player(_player)
    , QSplitter(Qt::Horizontal)
    , currentMessage(0)
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

    FoldingWidget *w1 = new FoldingWidget("Planet");
    w1->setObjectName("w1_column1");

    FoldingWidget *w2 = new FoldingWidget("Fleets in Orbit");
    w2->setObjectName("w2_column2");

    FoldingWidget *w3 = new FoldingWidget("Production");
    w3->setObjectName("w3_column2");

    QLayout *verticalFlowLayout = new VerticalFlowLayout;
    container->setLayout(verticalFlowLayout);

    verticalFlowLayout->addWidget(w1);
    verticalFlowLayout->addWidget(w2);
    verticalFlowLayout->addWidget(w3);

    QWidget *fleetsInOrbitWidget = new QWidget;
    Ui_FleetsInOrbitWidget ui_FleetsInOrbitWidget;
    ui_FleetsInOrbitWidget.setupUi(fleetsInOrbitWidget);

    w2->addWidget(fleetsInOrbitWidget);

    QWidget *planetProductionWidget = new QWidget;
    Ui_PlanetProductionWidget ui_PlanetProductionWidget;
    ui_PlanetProductionWidget.setupUi(planetProductionWidget);

    w3->addWidget(planetProductionWidget);

    container->adjustSize();

    setupMessages();

    unsigned num_planets = TheGalaxy->GetPlanetCount();

    for(unsigned n = 1 ; n <= num_planets ; n++) {
        Planet *p = TheGalaxy->GetPlanet(n);

        if(p->GetOwner() == player) {
            selectPlanet(p);
            break;
        }
    }
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

    while(statusBed->count()) {
        QWidget *widget = statusBed->currentWidget();
        statusBed->removeWidget(widget);
        delete widget;
    }

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

};
