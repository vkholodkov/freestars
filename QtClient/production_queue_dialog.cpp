/*
 * Copyright (C) 2014 Valery Kholodkov
 */
#include <cmath>

#include <QGuiApplication>

#include "production_queue_dialog.h"

namespace FreeStars {

ProductionQueueDialog::ProductionQueueDialog(Planet *_planet, QWidget *parent)
    : QDialog(parent)
    , planet(_planet)
    , game(_planet->GetGame())
{
    setupUi(this);

    // Production Queue model
    productionQueueModel = new ProductionQueueModel(planet, planet->GetProduction());
    auto m = actualOrdersView->model();
    actualOrdersView->setModel(productionQueueModel);
    delete m;

    actualOrdersView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    actualOrdersView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    actualOrdersView->resizeColumnsToContents();

    auto sm = actualOrdersView->selectionModel();
    connect(sm, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
      this, SLOT(orderSelectionChanged(const QItemSelection&, const QItemSelection&)));

    // Order Template List model
    orderTemplateListModel = new OrderTemplateListModel(planet);
    auto otvm = orderTemplatesView->model();
    orderTemplatesView->setModel(orderTemplateListModel);
    delete otvm;

    orderTemplatesView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    orderTemplatesView->resizeColumnsToContents();

    auto otvsm = orderTemplatesView->selectionModel();
    otvsm->setCurrentIndex(orderTemplateListModel->index(0, 0), QItemSelectionModel::SelectCurrent);
    connect(otvsm, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
      this, SLOT(orderTemplatesSelectionChanged(const QItemSelection&, const QItemSelection&)));

    // Pay Tax checkbox
    payTaxCheckBox->setCheckState(planet->GetPayTax() ? Qt::Unchecked : Qt::Checked);

    // Connections
    connect(itemUpButton, SIGNAL(clicked()), this, SLOT(itemUp()));
    connect(itemDownButton, SIGNAL(clicked()), this, SLOT(itemDown()));
    connect(orderTemplatesView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(add()));
    connect(actualOrdersView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(remove()));
    connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    updateState();
    updateStateOrderTemplatesState();
}

ProductionQueueDialog::~ProductionQueueDialog()
{
  delete productionQueueModel;
}

void ProductionQueueDialog::updateStateOrderTemplatesState()
{
  auto m = orderTemplatesView->selectionModel();
  auto currentOrderIndex = m->currentIndex();
  bool hasSelection = currentOrderIndex.row() >= 0;
  auto selectedIndex = currentOrderIndex.row();

  addButton->setEnabled(hasSelection && orderTemplateListModel->rowCount() != 0);

  if(hasSelection) {
    auto po = orderTemplateListModel->order_template_list.at(selectedIndex);
    auto cost = po->GetCost(planet);

    ironiumValueLabel1->setText(tr("%0kT").arg(cost[0]));
    boraniumValueLabel1->setText(tr("%0kT").arg(cost[1]));
    germaniumValueLabel1->setText(tr("%0kT").arg(cost[2]));
    resourcesValueLabel1->setText(tr("%0").arg(cost[RESOURCES]));
  }
  else {
    ironiumValueLabel1->clear();
    boraniumValueLabel1->clear();
    germaniumValueLabel1->clear();
    resourcesValueLabel1->clear();
  }

  ironiumLabel1->setVisible(hasSelection);
  boraniumLabel1->setVisible(hasSelection);
  germaniumLabel1->setVisible(hasSelection);
  resourcesLabel1->setVisible(hasSelection);
}

void ProductionQueueDialog::updateState()
{
  auto m = actualOrdersView->selectionModel();
  auto currentOrderIndex = m->currentIndex();
  bool hasSelection = currentOrderIndex.row() >= 0;
  auto selectedIndex = currentOrderIndex.row();

  removeButton->setEnabled(m->hasSelection() && productionQueueModel->rowCount() != 0);
  itemUpButton->setEnabled(m->hasSelection() && selectedIndex > 0);
  itemDownButton->setEnabled(m->hasSelection() && selectedIndex < productionQueueModel->rowCount() - 1);
  clearButton->setEnabled(productionQueueModel->rowCount() != 0);

  this->setWindowTitle(tr("Production Queue -- %0").arg(planet->GetName().c_str()));

  if(hasSelection) {
    auto po = productionQueueModel->production_queue.at(selectedIndex);
    auto cost = po->GetCost(planet);

    ironiumValueLabel2->setText(tr("%0kT").arg(cost[0]));
    boraniumValueLabel2->setText(tr("%0kT").arg(cost[1]));
    germaniumValueLabel2->setText(tr("%0kT").arg(cost[2]));
    resourcesValueLabel2->setText(tr("%0").arg(cost[RESOURCES]));
  }
  else {
    ironiumValueLabel2->clear();
    boraniumValueLabel2->clear();
    germaniumValueLabel2->clear();
    resourcesValueLabel2->clear();
  }

  ironiumLabel2->setVisible(hasSelection);
  boraniumLabel2->setVisible(hasSelection);
  germaniumLabel2->setVisible(hasSelection);
  resourcesLabel2->setVisible(hasSelection);
  requiredMineralsLabel2->setVisible(hasSelection);

  auto years = productionQueueModel->getCompletionYears(selectedIndex);

  if(selectedIndex >= 0) {
    auto completionPct = productionQueueModel->getCompletionPct(selectedIndex);

    completionLabel->setText(
      years <= 1 ? QString("%0% Done, Completion 1 year").arg(completionPct)
       : years < 5 ? QString("%0% Done, Completion 2 - 4 years").arg(completionPct)
       : QString("%0% Done, Completion 5 - 10 years").arg(completionPct));
  }
  else {
    completionLabel->setText(
      years <= 1 ? QString("Completion 1 year")
       : years < 5 ? QString("Completion 2 - 4 years")
       : QString("Completion 5 - 10 years"));
  }

  completionLabel->setVisible(hasSelection);
}

void ProductionQueueDialog::orderSelectionChanged(const QItemSelection&, const QItemSelection&)
{
    updateState();
}

void ProductionQueueDialog::orderTemplatesSelectionChanged(const QItemSelection&, const QItemSelection&)
{
    updateStateOrderTemplatesState();
}

void ProductionQueueDialog::itemUp()
{
  auto m = actualOrdersView->selectionModel();

  if(m->hasSelection() && m->currentIndex().row() > 0) {
    productionQueueModel->moveRow(QModelIndex(), m->currentIndex().row(),
      QModelIndex(), m->currentIndex().row() - 1);
    updateState();
  }
}

void ProductionQueueDialog::itemDown()
{
  auto m = actualOrdersView->selectionModel();

  if(m->hasSelection() && m->currentIndex().row() < productionQueueModel->rowCount()) {
    productionQueueModel->moveRow(QModelIndex(), m->currentIndex().row(),
      QModelIndex(), m->currentIndex().row() + 2);
    updateState();
  }
}

void ProductionQueueDialog::add()
{
  auto m = orderTemplatesView->selectionModel();
  auto currentOrderIndex = m->currentIndex();
  auto selectedIndex = currentOrderIndex.row();

  if(selectedIndex >= 0 && selectedIndex < orderTemplateListModel->order_template_list.size()) {
    auto po = orderTemplateListModel->order_template_list.at(selectedIndex);

    auto am = actualOrdersView->selectionModel();
    auto destIndex = am->currentIndex().row();
    auto lastIndex = productionQueueModel->rowCount() == 0 ?
      0 : productionQueueModel->rowCount() - 1;

    int amount = QGuiApplication::queryKeyboardModifiers() & Qt::ShiftModifier ? 10 : 1;

    productionQueueModel->addOrderFromTemplate(destIndex != -1 ? destIndex : lastIndex, po, amount);

    updateState();
  }
}

void ProductionQueueDialog::remove()
{
  auto m = actualOrdersView->selectionModel();

  if(m->hasSelection()) {
    int amount = QGuiApplication::queryKeyboardModifiers() & Qt::ShiftModifier ? 10 : 1;
    productionQueueModel->removeOrder(m->currentIndex().row(), amount);
    updateState();
  }
}

void ProductionQueueDialog::clear()
{
  productionQueueModel->removeRows(0, productionQueueModel->rowCount());
  updateState();
}

void ProductionQueueDialog::accept()
{
    std::deque<ProdOrder*> production_queue;

    for(auto po : productionQueueModel->production_queue) {
      production_queue.push_back(po->Copy());
    }

    planet->SetProduction(production_queue);
    planet->SetPayTax(payTaxCheckBox->checkState() == Qt::Unchecked);
    QDialog::accept();
}

};
