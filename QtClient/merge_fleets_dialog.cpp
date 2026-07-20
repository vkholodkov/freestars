/*
 * Copyright (C) 2026 Valery Kholodkov
 */
#include <cmath>

#include <QGuiApplication>

#include "merge_fleets_dialog.h"
#include "merge_fleets_model.h"

namespace FreeStars {

MergeFleetsDialog::MergeFleetsDialog(const Fleet *_fleet, Player *_player, QWidget *parent)
    : QDialog(parent)
    , player(_player)
{
    setupUi(this);

    mergeFleetsModel = new MergeFleetsModel(_fleet, _player);
    auto m = fleetTableView->model();
    fleetTableView->setModel(mergeFleetsModel);
    delete m;

    fleetTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    fleetTableView->resizeColumnsToContents();

    auto sm = fleetTableView->selectionModel();
    sm->setCurrentIndex(mergeFleetsModel->getInitialFleetIndex(), QItemSelectionModel::Select);
    connect(sm, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
      this, SLOT(fleetSelectionChanged(const QItemSelection&, const QItemSelection&)));

    okButton->setEnabled(false);

    connect(selectAllButton, SIGNAL(clicked()), this, SLOT(selectAll()));
    connect(unselectAllButton, SIGNAL(clicked()), this, SLOT(unselectAll()));
}

MergeFleetsDialog::~MergeFleetsDialog()
{
}

void MergeFleetsDialog::fleetSelectionChanged(const QItemSelection&, const QItemSelection&)
{
    auto sm = fleetTableView->selectionModel();
    okButton->setEnabled(sm->selection().indexes().count() > 1);
}

void MergeFleetsDialog::selectAll()
{
    auto sm = fleetTableView->selectionModel();
    sm->select(QItemSelection(mergeFleetsModel->topLeft(), mergeFleetsModel->bottomRight()), QItemSelectionModel::Select);
}

void MergeFleetsDialog::unselectAll()
{
    auto sm = fleetTableView->selectionModel();
    sm->select(QItemSelection(mergeFleetsModel->topLeft(), mergeFleetsModel->bottomRight()), QItemSelectionModel::Deselect);
}

void MergeFleetsDialog::accept()
{
    auto sm = fleetTableView->selectionModel();

    Fleet *mergeTo = nullptr;

    for (auto index : sm->selection().indexes()) {
        Fleet *f = const_cast<Fleet*>(mergeFleetsModel->getFleet(index));

        if(mergeTo == nullptr) {
            mergeTo = f;
        }
        else {
            player->MergeFromTo(f, mergeTo);
        }
    }

    QDialog::accept();
}

};
