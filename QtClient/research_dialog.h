/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _RESEARCH_DIALOG_H_
#define _RESEARCH_DIALOG_H_

#include <QDialog>
#include <QButtonGroup>

#include "FSServer.h"

#include "ui_research_dialog.h"

namespace FreeStars {

class ResearchDialog : public QDialog, private Ui_ResearchDialog {
    Q_OBJECT

public:
    ResearchDialog(Player*, QWidget *parent = 0);

private slots:
    void setResearchField(int);
    void setResearchTax(int);
    void accept();

private:
    Player *player;
    QButtonGroup *researchFieldGroup;
    long totalResources;
    long techTypeOffset;
};

};

#endif
