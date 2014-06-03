
#include "planet_production_widget.h"

#include "production_queue_model.h"

#include "ui_planet_production_widget.h"

namespace FreeStars {

PlanetProductionWidget::PlanetProductionWidget(const Planet *_planet, const Player *_player, QWidget *parent)
    : FoldingWidget(tr("Production"))
    , planet(_planet)
{
    QWidget *widget = new QWidget;

    Ui_PlanetProductionWidget ui_PlanetProductionWidget;
    ui_PlanetProductionWidget.setupUi(widget);

    ProductionQueueModel *productionQueueModel = new ProductionQueueModel(_planet->GetProduction());
    QItemSelectionModel *m = ui_PlanetProductionWidget.productionQueueView->selectionModel();
    ui_PlanetProductionWidget.productionQueueView->setModel(productionQueueModel);
    delete m;

    ui_PlanetProductionWidget.productionQueueView->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    ui_PlanetProductionWidget.productionQueueView->horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);

    ui_PlanetProductionWidget.completionLabel->setText("");
    ui_PlanetProductionWidget.routeToLabel->setText(
        planet->GetRoute() != NULL ? QString(_planet->GetRoute()->GetName(_player).c_str()) : tr("none"));

    connect(ui_PlanetProductionWidget.changeButton, SIGNAL(clicked(bool)), this, SLOT(changeButtonClicked(bool)));
    connect(ui_PlanetProductionWidget.clearButton, SIGNAL(clicked(bool)), this, SLOT(clearButtonClicked(bool)));

    this->addWidget(widget);
}

void PlanetProductionWidget::changeButtonClicked(bool)
{
    emit changeProductionQueue(planet);
}

void PlanetProductionWidget::clearButtonClicked(bool)
{
    emit clearProductionQueue(planet);
}

};
