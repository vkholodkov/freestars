
#include <QTableView>

#include "planets_view.h"

PlanetsView::PlanetsView()
{
#if 0
    QTableView *tableView = new QTableView;
    tableView->setModel(this->getOwnPlanetsModel());
#endif
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
