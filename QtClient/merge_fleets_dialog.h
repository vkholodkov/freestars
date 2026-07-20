/*
 * Copyright (C) 2026 Valery Kholodkov
 */

#ifndef _MERGE_FLEETS_DIALOG_H_
#define _MERGE_FLEETS_DIALOG_H_

#include <QDialog>
#include <QButtonGroup>

#include "FSServer.h"

#include "ui_merge_fleets_dialog.h"

#include "merge_fleets_model.h"

namespace FreeStars {

class MergeFleetsDialog : public QDialog, private Ui_MergeFleetsDialog {
    Q_OBJECT

public:
    MergeFleetsDialog(const Fleet*, Player*, QWidget *parent = 0);
    ~MergeFleetsDialog();

public slots:
    void accept();

private slots:
    void fleetSelectionChanged(const QItemSelection&, const QItemSelection&);
    void selectAll();
    void unselectAll();

private:
    MergeFleetsModel *mergeFleetsModel;
    Player *player;
};

};

#endif
