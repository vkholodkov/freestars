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
    , transferCargo(Rules::MaxMinType, 0)
    , transferPop(0)
    , transferFuel(0)
{
  setupUi(this);

  // Source
  sourceNameLabel->setText(source->GetName(player).c_str());

  if(typeid(*source) == typeid(Fleet)) {
    sourceFuelWidget->setChangeable(true);
    sourceFuelWidget->setCargoColor(Qt::red);
    sourceFuelWidget->setUnit(tr("mg"));
    sourceFuelWidget->setCargo(dynamic_cast<Fleet*>(source)->GetFuel());
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
  sourceIroniumWidget->setShowMaxCargo(typeid(*source) == typeid(Fleet));
  sourceIroniumWidget->setCargoColor(Qt::blue);
  sourceIroniumWidget->setCargo(source->GetContain(0));
  sourceIroniumWidget->setUnit(tr("kT"));

  connect(sourceIroniumWidget, SIGNAL(changed(long)), this, SLOT(sourceIroniumChanged(long)));

  sourceBoraniumWidget->setChangeable(true);
  sourceBoraniumWidget->setShowMaxCargo(typeid(*source) == typeid(Fleet));
  sourceBoraniumWidget->setCargoColor(Qt::green);
  sourceBoraniumWidget->setCargo(source->GetContain(1));
  sourceBoraniumWidget->setUnit(tr("kT"));

  connect(sourceBoraniumWidget, SIGNAL(changed(long)), this, SLOT(sourceBoraniumChanged(long)));

  sourceGermaniumWidget->setChangeable(true);
  sourceGermaniumWidget->setShowMaxCargo(typeid(*source) == typeid(Fleet));
  sourceGermaniumWidget->setCargoColor(Qt::yellow);
  sourceGermaniumWidget->setCargo(source->GetContain(2));
  sourceGermaniumWidget->setUnit(tr("kT"));

  connect(sourceGermaniumWidget, SIGNAL(changed(long)), this, SLOT(sourceGermaniumChanged(long)));

  sourceColonistsWidget->setChangeable(true);
  sourceColonistsWidget->setShowMaxCargo(typeid(*source) == typeid(Fleet));
  sourceColonistsWidget->setCargoColor(Qt::blue);
  sourceColonistsWidget->setCargo(source->GetContain(POPULATION) / Rules::PopEQ1kT);
  sourceColonistsWidget->setUnit(tr("kT"));

  connect(sourceColonistsWidget, SIGNAL(changed(long)), this, SLOT(sourceColonistsChanged(long)));

  // Destination
  destNameLabel->setText(dest->GetName(player).c_str());

  if(typeid(*dest) == typeid(Fleet)) {
    destFuelWidget->setChangeable(true);
    destFuelWidget->setCargoColor(Qt::red);
    destFuelWidget->setUnit(tr("mg"));
    destFuelWidget->setCargo(dynamic_cast<Fleet*>(dest)->GetFuel());
    destFuelWidget->setMaxCargo(dynamic_cast<Fleet*>(dest)->GetFuelCapacity());
    destFuelWidget->show();
  }
  else {
    destFuelWidget->hide();
  }

  destCargoHoldWidget->setUnit(tr("kT"));
  destCargoHoldWidget->setCargoHolder(dest);

  destIroniumWidget->setChangeable(true);
  destIroniumWidget->setShowMaxCargo(typeid(*dest) == typeid(Fleet));
  destIroniumWidget->setCargoColor(Qt::blue);
  destIroniumWidget->setUnit(tr("kT"));
  destIroniumWidget->setCargo(dest->GetContain(0));
  destIroniumWidget->setMaxCargo(dest->GetCargoCapacity());

  connect(destIroniumWidget, SIGNAL(changed(long)), this, SLOT(destIroniumChanged(long)));

  destBoraniumWidget->setChangeable(true);
  destBoraniumWidget->setShowMaxCargo(typeid(*dest) == typeid(Fleet));
  destBoraniumWidget->setCargoColor(Qt::green);
  destBoraniumWidget->setUnit(tr("kT"));
  destBoraniumWidget->setCargo(dest->GetContain(1));
  destBoraniumWidget->setMaxCargo(dest->GetCargoCapacity());

  connect(destBoraniumWidget, SIGNAL(changed(long)), this, SLOT(destBoraniumChanged(long)));

  destGermaniumWidget->setChangeable(true);
  destGermaniumWidget->setShowMaxCargo(typeid(*dest) == typeid(Fleet));
  destGermaniumWidget->setCargoColor(Qt::yellow);
  destGermaniumWidget->setUnit(tr("kT"));
  destGermaniumWidget->setCargo(dest->GetContain(2));
  destGermaniumWidget->setMaxCargo(dest->GetCargoCapacity());

  connect(destGermaniumWidget, SIGNAL(changed(long)), this, SLOT(destGermaniumChanged(long)));

  destColonistsWidget->setChangeable(true);
  destColonistsWidget->setShowMaxCargo(typeid(*dest) == typeid(Fleet));
  destColonistsWidget->setCargoColor(Qt::white);
  destColonistsWidget->setUnit(tr("kT"));
  destColonistsWidget->setCargo(dest->GetContain(POPULATION) / Rules::PopEQ1kT);
  destColonistsWidget->setMaxCargo(dest->GetCargoCapacity());

  connect(destColonistsWidget, SIGNAL(changed(long)), this, SLOT(destColonistsChanged(long)));
}

void CargoTransferDialog::sourceIroniumChanged(long newSourceIronium)
{
}

void CargoTransferDialog::sourceBoraniumChanged(long _new)
{
}

void CargoTransferDialog::sourceGermaniumChanged(long _new)
{
}

void CargoTransferDialog::sourceColonistsChanged(long _new)
{
}

void CargoTransferDialog::destIroniumChanged(long newDestIronium)
{
  // Check destination cargo holder capacity
  if(newDestIronium + getDestBoranium() + getDestGermanium() + getDestColonists() > dest->GetCargoCapacity()) {
    // Load not possible
    return;
  }

  // Check available cargo
  if(newDestIronium - dest->GetContain(0) >= getSourceIronium()) {
    // Not enough cargo
    return;
  }

  long transfer = newDestIronium - dest->GetContain(0);
  long newSourceIronium = source->GetContain(0) - transfer; 

  if(typeid(*source) == typeid(Fleet)) {
    // Check source cargo holder capacity
    if(newSourceIronium + getSourceBoranium() + getSourceGermanium() + getSourceColonists() >= source->GetCargoCapacity()) {
      // Unload not possible
      return;
    }
  }

  sourceIroniumWidget->setCargo(newSourceIronium);
  destIroniumWidget->setCargo(newDestIronium);

  transferCargo[0] = transfer;
}

void CargoTransferDialog::destBoraniumChanged(long newDestBoranium)
{
  // Check destination cargo holder capacity
  if(getDestIronium() + newDestBoranium + getDestGermanium() + getDestColonists() >= dest->GetCargoCapacity()) {
    // Load not possible
    return;
  }

  // Check available cargo
  if(newDestBoranium - dest->GetContain(1) >= getSourceBoranium()) {
    // Not enough cargo
    return;
  }

  long transfer = newDestBoranium - dest->GetContain(1);
  long newSourceBoranium = source->GetContain(1) - transfer; 

  if(typeid(*source) == typeid(Fleet)) {
    // Check source cargo holder capacity
    if(getSourceIronium() + newSourceBoranium + getSourceGermanium() + getSourceColonists() >= source->GetCargoCapacity()) {
      // Unload not possible
      return;
    }
  }

  sourceBoraniumWidget->setCargo(newSourceBoranium);
  destBoraniumWidget->setCargo(newDestBoranium);

  transferCargo[1] = transfer;
}

void CargoTransferDialog::destGermaniumChanged(long newDestGermanium)
{
  // Check destination cargo holder capacity
  if(getDestIronium() + getDestBoranium() + newDestGermanium + getDestColonists() >= dest->GetCargoCapacity()) {
    // Load not possible
    return;
  }

  // Check available cargo
  if(newDestGermanium - dest->GetContain(2) >= getSourceGermanium()) {
    // Not enough cargo
    return;
  }

  long transfer = newDestGermanium - dest->GetContain(2);
  long newSourceGermanium = source->GetContain(2) - transfer; 

  if(typeid(*source) == typeid(Fleet)) {
    // Check source cargo holder capacity
    if(getSourceIronium() + getSourceBoranium() + newSourceGermanium + getSourceColonists() >= source->GetCargoCapacity()) {
      // Unload not possible
      return;
    }
  }

  sourceGermaniumWidget->setCargo(newSourceGermanium);
  destGermaniumWidget->setCargo(newDestGermanium);

  transferCargo[2] = transfer;
}

void CargoTransferDialog::destColonistsChanged(long newDestColonists)
{
  // Check destination cargo holder capacity
  if(getDestIronium() + getDestBoranium() + getDestGermanium() + newDestColonists >= dest->GetCargoCapacity()) {
    // Load not possible
    return;
  }

  // Check available cargo
  if(newDestColonists - dest->GetContain(POPULATION) / Rules::PopEQ1kT >= getSourceColonists()) {
    // Not enough cargo
    return;
  }

  long transfer = newDestColonists - dest->GetContain(POPULATION) / Rules::PopEQ1kT;
  long newSourceColonists = source->GetContain(POPULATION) / Rules::PopEQ1kT - transfer; 

  if(typeid(*source) == typeid(Fleet)) {
    // Check source cargo holder capacity
    if(getSourceIronium() + getSourceBoranium() + getSourceGermanium() + newSourceColonists >= source->GetCargoCapacity()) {
      // Unload not possible
      return;
    }
  }

  sourceColonistsWidget->setCargo(newSourceColonists);
  destColonistsWidget->setCargo(newDestColonists);

  transferPop = transfer * Rules::PopEQ1kT;
}

void CargoTransferDialog::accept()
{
    player->TransferCargo(source, dest, transferPop, 0, transferCargo);

    QDialog::accept();
}

};
