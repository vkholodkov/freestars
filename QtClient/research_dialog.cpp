/*
 * Copyright (C) 2014 Valery Kholodkov
 */
#include <cmath>

#include "research_dialog.h"

namespace FreeStars {

ResearchDialog::ResearchDialog(Player *_player, QWidget *parent)
    : QDialog(parent)
    , player(_player)
    , game(_player->GetGame())
    , researchFieldGroup(new QButtonGroup(this))
    , totalResources(0)
    , techTypeOffset(0)
{
    long researchField = _player->GetResearchField();

    unsigned num_planets = game->GetGalaxy()->GetPlanetCount();

    for(unsigned n = 1 ; n <= num_planets ; n++) {
        Planet *p = game->GetGalaxy()->GetPlanet(n);

        if(p->GetOwner() == player && p->GetPayTax()) {
            totalResources += p->GetResources();
        }
    }

    setupUi(this);

    researchFieldGroup->addButton(energyButton, 0);
    researchFieldGroup->addButton(weaponsButton, 1);
    researchFieldGroup->addButton(propulsionButton, 2);
    researchFieldGroup->addButton(constructionButton, 3);
    researchFieldGroup->addButton(electronicsButton, 4);
    researchFieldGroup->addButton(biotechnologyButton, 5);

    researchFieldGroup->setExclusive(true);

    energyLevelLabel->setText(QString::number(_player->GetTechLevel(0)));
    weaponsLevelLabel->setText(QString::number(_player->GetTechLevel(1)));
    propulsionLevelLabel->setText(QString::number(_player->GetTechLevel(2)));
    constructionLevelLabel->setText(QString::number(_player->GetTechLevel(3)));
    electronicsLevelLabel->setText(QString::number(_player->GetTechLevel(4)));
    biotechnologyLevelLabel->setText(QString::number(_player->GetTechLevel(5)));

    researchTaxBox->setValue(::round(_player->GetResearchTax() * 100.0f));

    nextFieldBox->addItem(tr("<Cheapest field>"));
    nextFieldBox->addItem(tr("<Alchemy>"));
    nextFieldBox->addItem(tr("<Same field>"));
    nextFieldBox->addItem(tr("<Lowest field>"));
    nextFieldBox->addItem(tr("<Do not research>"));

    techTypeOffset = nextFieldBox->count();

    for(long tt = 0 ; tt != Rules::MaxTechType ; tt++) {
        nextFieldBox->addItem(tr(Rules::GetTechName(tt).c_str()));
    }

    nextFieldBox->setCurrentIndex(player->GetResearchNext() + techTypeOffset);

    if(researchField >= 0 && researchField < Rules::MaxTechType) {
        QAbstractButton *button = researchFieldGroup->button(researchField);
        button->setChecked(true);
        setResearchField(researchField);
    }

    connect(researchFieldGroup, SIGNAL(buttonClicked(int)), this, SLOT(setResearchField(int)));
    connect(researchTaxBox, SIGNAL(valueChanged(int)), this, SLOT(setResearchTax(int)));
    connect(doneButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void ResearchDialog::setResearchField(int researchField)
{
    currentFieldNameLabel->setText(tr(Rules::GetTechName(researchField).c_str()) + QString(","));
    currentTechLevelLabel->setText(researchField < Rules::MaxTechLevel
        ? tr("Tech Level %0").arg(player->GetTechLevel(researchField) + 1)
        : tr("Maxed out"));

    researchBenefitsList->clear();

    const std::deque<Component *> &components = game->GetComponents();
    std::list<Component*> available1, available24, available10;
    size_t available_count = 0;

    for(std::deque<Component *>::const_iterator i = components.begin() ;
        i != components.end() && (available_count < 5) ; i++)
    {
        Component *c = *i;
        if(!c->IsBuildable(player)) {
            if(c->WouldBeBuildable(player, researchField, 1)) {
                available1.push_back(c);
                available_count++;
            }
            else if(c->WouldBeBuildable(player, researchField, 4)) {
                available24.push_back(c);
                available_count++;
            }
            else if(c->WouldBeBuildable(player, researchField, 10)) {
                available10.push_back(c);
                available_count++;
            }
        }
    }

    for(std::list<Component*>::const_iterator i = available1.begin() ; i != available1.end() ; i++) {
        Component *c = *i;
        QListWidgetItem *item = new QListWidgetItem(c->GetName().c_str(),
            researchBenefitsList);
        item->setForeground(Qt::darkGreen);
        researchBenefitsList->addItem(item);
    }

    for(std::list<Component*>::const_iterator i = available24.begin() ; i != available24.end() ; i++) {
        Component *c = *i;
        QListWidgetItem *item = new QListWidgetItem(c->GetName().c_str(),
            researchBenefitsList);
        item->setForeground(Qt::darkBlue);
        researchBenefitsList->addItem(item);
    }

    for(std::list<Component*>::const_iterator i = available10.begin() ; i != available10.end() ; i++) {
        Component *c = *i;
        QListWidgetItem *item = new QListWidgetItem(c->GetName().c_str(),
            researchBenefitsList);
        researchBenefitsList->addItem(item);
    }

    long resourcesNeeded = player->TechCost(researchField) - player->TechProgress(researchField);
    long nextYearBudget = (totalResources * researchTaxBox->value()) / 100;

    resourcesNeededLabel->setText(QString::number(resourcesNeeded));
    annualResourcesLabel->setText(QString::number(totalResources));
    nextYearBudgetLabel->setText(QString::number(nextYearBudget));

    if(nextYearBudget > 0) {
        long years = ::ceil(resourcesNeeded / nextYearBudget);
        estTimeLabel->setText(QString::number(years));
    }
    else {
        estTimeLabel->setText(tr("Never"));
    }

    lastYearResourcesLabel->setText(tr("N/A"));
}

void ResearchDialog::setResearchTax(int tax)
{
    setResearchField(researchFieldGroup->checkedId());
}

void ResearchDialog::accept()
{
    player->SetResearchField(researchFieldGroup->checkedId());    
    player->SetResearchTax((double)researchTaxBox->value() / 100.0f);
    player->SetResearchNext(nextFieldBox->currentIndex() - techTypeOffset);

    QDialog::accept();
}

};
