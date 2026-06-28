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
{
  setupUi(this);

  // Source
  sourceNameLabel->setText(source->GetName(player).c_str());

  if(typeid(*source) == typeid(Fleet)) {
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
  sourceIroniumWidget->setReadOnly(typeid(*source) != typeid(Fleet));
  sourceIroniumWidget->setCargoColor(Qt::blue);
  sourceIroniumWidget->setCargo(source->GetContain(0));
  sourceIroniumWidget->setUnit(tr("kT"));

  sourceBoraniumWidget->setChangeable(true);
  sourceBoraniumWidget->setReadOnly(typeid(*source) != typeid(Fleet));
  sourceBoraniumWidget->setCargoColor(Qt::green);
  sourceBoraniumWidget->setCargo(source->GetContain(1));
  sourceBoraniumWidget->setUnit(tr("kT"));

  sourceGermaniumWidget->setChangeable(true);
  sourceGermaniumWidget->setReadOnly(typeid(*source) != typeid(Fleet));
  sourceGermaniumWidget->setCargoColor(Qt::yellow);
  sourceGermaniumWidget->setCargo(source->GetContain(2));
  sourceGermaniumWidget->setUnit(tr("kT"));

  sourceColonistsWidget->setChangeable(true);
  sourceColonistsWidget->setReadOnly(typeid(*source) != typeid(Fleet));
  sourceColonistsWidget->setCargoColor(Qt::blue);
  sourceColonistsWidget->setCargo(source->GetContain(POPULATION) / Rules::PopEQ1kT);
  sourceColonistsWidget->setUnit(tr("kT"));

  // Destination
  destNameLabel->setText(dest->GetName(player).c_str());

  if(typeid(*dest) == typeid(Fleet)) {
    if(typeid(*source) == typeid(Fleet)) {
      destFuelWidget->setChangeable(true);
    }

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
  destIroniumWidget->setCargoColor(Qt::blue);
  destIroniumWidget->setUnit(tr("kT"));
  destIroniumWidget->setCargo(dest->GetContain(0));
  destIroniumWidget->setMaxCargo(dest->GetCargoCapacity());

  destBoraniumWidget->setChangeable(true);
  destBoraniumWidget->setCargoColor(Qt::green);
  destBoraniumWidget->setUnit(tr("kT"));
  destBoraniumWidget->setCargo(dest->GetContain(1));
  destBoraniumWidget->setMaxCargo(dest->GetCargoCapacity());

  destGermaniumWidget->setChangeable(true);
  destGermaniumWidget->setCargoColor(Qt::yellow);
  destGermaniumWidget->setUnit(tr("kT"));
  destGermaniumWidget->setCargo(dest->GetContain(2));
  destGermaniumWidget->setMaxCargo(dest->GetCargoCapacity());

  destColonistsWidget->setChangeable(true);
  destColonistsWidget->setCargoColor(Qt::white);
  destColonistsWidget->setUnit(tr("kT"));
  destColonistsWidget->setCargo(dest->GetContain(POPULATION) / Rules::PopEQ1kT);
  destColonistsWidget->setMaxCargo(dest->GetCargoCapacity());
}

};
