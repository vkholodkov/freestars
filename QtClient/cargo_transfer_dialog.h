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
    //void accept();

private:
    Game *game;
    CargoHolder *source, *dest;
    Player *player;
};

};

#endif
