/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _CARGO_TRANSFER_DIALOG_H_
#define _CARGO_TRANSFER_DIALOG_H_

#include <QDialog>
#include <QButtonGroup>

#include "FSServer.h"

#include "ui_cargo_transfer_dialog.h"

namespace FreeStars {

class CargoTransferDialog : public QDialog, private Ui_CargoTransferDialog {
    Q_OBJECT

public:
    CargoTransferDialog(CargoHolder*, CargoHolder*, Player*, QWidget *parent = 0);

private slots:
    void sourceIroniumChanged(long);
    void sourceBoraniumChanged(long);
    void sourceGermaniumChanged(long);
    void sourceColonistsChanged(long);
    void destIroniumChanged(long);
    void destBoraniumChanged(long);
    void destGermaniumChanged(long);
    void destColonistsChanged(long);
    void accept();

private:
    long getSourceIronium() { return source->GetContain(0) - transferCargo[0]; }
    long getSourceBoranium() { return source->GetContain(1) - transferCargo[1]; }
    long getSourceGermanium() { return source->GetContain(2) - transferCargo[2]; }
    long getSourceColonists() { return source->GetContain(POPULATION) / Rules::PopEQ1kT - transferPop / Rules::PopEQ1kT; }

    long getDestIronium() { return dest->GetContain(0) + transferCargo[0]; }
    long getDestBoranium() { return dest->GetContain(1) + transferCargo[1]; }
    long getDestGermanium() { return dest->GetContain(2) + transferCargo[2]; }
    long getDestColonists() { return dest->GetContain(POPULATION) / Rules::PopEQ1kT + transferPop / Rules::PopEQ1kT; }

private:
    Game *game;
    CargoHolder *source, *dest;
    Player *player;
    deque<long> transferCargo;
    long transferPop, transferFuel;
};

};

#endif
