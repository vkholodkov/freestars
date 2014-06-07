/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include "research_dialog.h"

namespace FreeStars {

ResearchDialog::ResearchDialog(const Player *_player, QWidget *parent)
    : QDialog(parent)
    , player(_player)
    , researchFieldGroup(new QButtonGroup(this))
{
    long researchField = _player->GetResearchField();

    setupUi(this);

    researchFieldGroup->addButton(energyButton, 0);
    researchFieldGroup->addButton(weaponsButton, 1);
    researchFieldGroup->addButton(propulsionButton, 2);
    researchFieldGroup->addButton(constructionButton, 3);
    researchFieldGroup->addButton(electronicsButton, 4);
    researchFieldGroup->addButton(biotechnologyButton, 5);

    researchFieldGroup->setExclusive(true);

    if(researchField >= 0 && researchField < Rules::MaxTechType) {
        QAbstractButton *button = researchFieldGroup->button(researchField);
        button->setChecked(true);
        setResearchField(researchField);
    }

    energyLevelLabel->setText(QString::number(_player->GetTechLevel(0)));
    weaponsLevelLabel->setText(QString::number(_player->GetTechLevel(1)));
    propulsionLevelLabel->setText(QString::number(_player->GetTechLevel(2)));
    constructionLevelLabel->setText(QString::number(_player->GetTechLevel(3)));
    electronicsLevelLabel->setText(QString::number(_player->GetTechLevel(4)));
    biotechnologyLevelLabel->setText(QString::number(_player->GetTechLevel(5)));

    connect(researchFieldGroup, SIGNAL(buttonClicked(int)), this, SLOT(setResearchField(int)));
    connect(doneButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void ResearchDialog::setResearchField(int researchField)
{
    currentFieldNameLabel->setText(tr(Rules::GetTechName(researchField).c_str()) + QString(","));
    currentTechLevelLabel->setText(researchField < Rules::MaxTechLevel
        ? tr("Texh Level %0").arg(player->GetTechLevel(researchField) + 1)
        : tr("Maxed out"));
}

};
