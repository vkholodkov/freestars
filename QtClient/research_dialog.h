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
    ResearchDialog(const Player*, QWidget *parent = 0);

private slots:
    void setResearchField(int);

private:
    const Player *player;
    QButtonGroup *researchFieldGroup;
};

};

#endif
