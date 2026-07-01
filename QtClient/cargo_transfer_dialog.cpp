/*
 * Copyright (C) 2014 Valery Kholodkov
 */
#include <cmath>

#include "cargo_transfer_dialog.h"

namespace FreeStars {

CargoTransferDialog::CargoTransferDialog(CargoHolder *_source, CargoHolder *_dest, Player *_player, QWidget *parent)
    : QDialog(parent)
    , source(_source)
    , dest(_dest)
    , player(_player)
{
  setupUi(this);

  // Source
  sourceNameLabel->setText(source->GetName(player).c_str());

  if(typeid(*source) == typeid(Fleet)) {
    sourceFuelWidget->setChangeable(true);
    sourceFuelWidget->setCargoColor(Qt::red);
    sourceFuelWidget->setUnit(tr("mg"));
    sourceFuelWidget->setCurrentCargo(dynamic_cast<Fleet*>(source)->GetFuel());
    sourceFuelWidget->setMaxCargo(dynamic_cast<Fleet*>(source)->GetFuelCapacity());
    sourceFuelWidget->show();
  }
  else {
    sourceFuelWidget->hide();
    sourceFuelLabel->setText("");
  }

  if(typeid(*source) == typeid(Fleet)) {
    sourceCargoHoldWidget->setUnit(tr("kT"));
    sourceCargoHoldWidget->setCargoHolder(source);
    sourceCargoHoldWidget->show();
  }
  else {
    sourceCargoHoldWidget->hide();
    sourceCargoHoldLabel->setText("");
  }

  sourceIroniumWidget->setChangeable(true);
  sourceIroniumWidget->setCargoColor(Qt::blue);
  sourceIroniumWidget->setCurrentCargo(source->GetContain(0));
  sourceIroniumWidget->setUnit(tr("kT"));

  connect(sourceIroniumWidget, SIGNAL(changed(int, int)), this, SLOT(sourceIroniumChanged(int, int)));

  sourceBoraniumWidget->setChangeable(true);
  sourceBoraniumWidget->setCargoColor(Qt::green);
  sourceBoraniumWidget->setCurrentCargo(source->GetContain(1));
  sourceBoraniumWidget->setUnit(tr("kT"));

  connect(sourceBoraniumWidget, SIGNAL(changed(int, int)), this, SLOT(sourceBoraniumChanged(int, int)));

  sourceGermaniumWidget->setChangeable(true);
  sourceGermaniumWidget->setCargoColor(Qt::yellow);
  sourceGermaniumWidget->setCurrentCargo(source->GetContain(2));
  sourceGermaniumWidget->setUnit(tr("kT"));

  connect(sourceGermaniumWidget, SIGNAL(changed(int, int)), this, SLOT(sourceGermaniumChanged(int, int)));

  sourceColonistsWidget->setChangeable(true);
  sourceColonistsWidget->setCargoColor(Qt::blue);
  sourceColonistsWidget->setCurrentCargo(source->GetContain(POPULATION) / Rules::PopEQ1kT);
  sourceColonistsWidget->setUnit(tr("kT"));

  connect(sourceColonistsWidget, SIGNAL(changed(int, int)), this, SLOT(sourceColonistsChanged(int, int)));

  // Destination
  destNameLabel->setText(dest->GetName(player).c_str());

  if(typeid(*dest) == typeid(Fleet)) {
    destFuelWidget->setChangeable(true);
    destFuelWidget->setCargoColor(Qt::red);
    destFuelWidget->setUnit(tr("mg"));
    destFuelWidget->setCurrentCargo(dynamic_cast<Fleet*>(dest)->GetFuel());
    destFuelWidget->setMaxCargo(dynamic_cast<Fleet*>(dest)->GetFuelCapacity());
    destFuelWidget->show();
  }
  else {
    destFuelWidget->hide();
  }

  destCargoHoldWidget->setUnit(tr("kT"));
  destCargoHoldWidget->setCargoHolder(dest);

  destIroniumWidget->setChangeable(true);
  destIroniumWidget->setCargoColor(Qt::blue);
  destIroniumWidget->setUnit(tr("kT"));
  destIroniumWidget->setCurrentCargo(dest->GetContain(0));
  destIroniumWidget->setMaxCargo(dest->GetCargoCapacity());

  connect(destIroniumWidget, SIGNAL(changed(int, int)), this, SLOT(destIroniumChanged(int, int)));

  destBoraniumWidget->setChangeable(true);
  destBoraniumWidget->setCargoColor(Qt::green);
  destBoraniumWidget->setUnit(tr("kT"));
  destBoraniumWidget->setCurrentCargo(dest->GetContain(1));
  destBoraniumWidget->setMaxCargo(dest->GetCargoCapacity());

  connect(destBoraniumWidget, SIGNAL(changed(int, int)), this, SLOT(destBoraniumChanged(int, int)));

  destGermaniumWidget->setChangeable(true);
  destGermaniumWidget->setCargoColor(Qt::yellow);
  destGermaniumWidget->setUnit(tr("kT"));
  destGermaniumWidget->setCurrentCargo(dest->GetContain(2));
  destGermaniumWidget->setMaxCargo(dest->GetCargoCapacity());

  connect(destGermaniumWidget, SIGNAL(changed(int, int)), this, SLOT(destGermaniumChanged(int, int)));

  destColonistsWidget->setChangeable(true);
  destColonistsWidget->setCargoColor(Qt::white);
  destColonistsWidget->setUnit(tr("kT"));
  destColonistsWidget->setCurrentCargo(dest->GetContain(POPULATION) / Rules::PopEQ1kT);
  destColonistsWidget->setMaxCargo(dest->GetCargoCapacity());

  connect(destColonistsWidget, SIGNAL(changed(int, int)), this, SLOT(destColonistsChanged(int, int)));

  sourceIroniumWidget->setMaxAvailableCargo(dest->GetContain(0));
  destIroniumWidget->setMaxAvailableCargo(source->GetContain(0));

  sourceBoraniumWidget->setMaxAvailableCargo(dest->GetContain(1));
  destBoraniumWidget->setMaxAvailableCargo(source->GetContain(1));

  sourceGermaniumWidget->setMaxAvailableCargo(dest->GetContain(2));
  destGermaniumWidget->setMaxAvailableCargo(source->GetContain(2));

  sourceColonistsWidget->setMaxAvailableCargo(dest->GetContain(POPULATION) / Rules::PopEQ1kT);
  destColonistsWidget->setMaxAvailableCargo(source->GetContain(POPULATION) / Rules::PopEQ1kT);
}

void CargoTransferDialog::sourceIroniumChanged(int _old, int _new)
{
  destIroniumWidget->setNewCargo(destIroniumWidget->currentCargo() + _old - _new);
}

void CargoTransferDialog::sourceBoraniumChanged(int _old, int _new)
{
  destBoraniumWidget->setNewCargo(destBoraniumWidget->currentCargo() + _old - _new);
}

void CargoTransferDialog::sourceGermaniumChanged(int _old, int _new)
{
  destGermaniumWidget->setNewCargo(destGermaniumWidget->currentCargo() + _old - _new);
}

void CargoTransferDialog::sourceColonistsChanged(int _old, int _new)
{
  destColonistsWidget->setNewCargo(destColonistsWidget->currentCargo() + _old - _new);
}

void CargoTransferDialog::destIroniumChanged(int _old, int _new)
{
  sourceIroniumWidget->setNewCargo(sourceIroniumWidget->currentCargo() + _old - _new);
}

void CargoTransferDialog::destBoraniumChanged(int _old, int _new)
{
  sourceBoraniumWidget->setNewCargo(sourceBoraniumWidget->currentCargo() + _old - _new);
}

void CargoTransferDialog::destGermaniumChanged(int _old, int _new)
{
  sourceGermaniumWidget->setNewCargo(sourceGermaniumWidget->currentCargo() + _old - _new);
}

void CargoTransferDialog::destColonistsChanged(int _old, int _new)
{
  sourceColonistsWidget->setNewCargo(sourceColonistsWidget->currentCargo() + _old - _new);
}

void CargoTransferDialog::accept()
{
    deque<long> cargo;

    long pop = (destColonistsWidget->newCargo() - destColonistsWidget->currentCargo()) * Rules::PopEQ1kT;

    cargo.push_back(destIroniumWidget->newCargo() - destIroniumWidget->currentCargo());
    cargo.push_back(destBoraniumWidget->newCargo() - destBoraniumWidget->currentCargo());
    cargo.push_back(destGermaniumWidget->newCargo() - destGermaniumWidget->currentCargo());

    player->TransferCargo(source, dest, pop, 0, cargo);

    QDialog::accept();
}

};
