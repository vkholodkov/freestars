/*
 * Copyright (C) 2014 Valery Kholodkov
 */
#include <QMenu>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItemModel>

#include "race_wizard.h"

#include "ui_rw_page1.h"
#include "ui_rw_page2.h"
#include "ui_rw_page3.h"
#include "ui_rw_page4.h"
#include "ui_rw_page5.h"
#include "ui_rw_page6.h"

namespace FreeStars {

class Page5Model : public QAbstractItemModel {
public:
    Page5Model(Race *_race, QObject *parent = 0)
        : QAbstractItemModel(parent)
        , race(_race)
    {
    }

    QModelIndex index(int, int, const QModelIndex&) const;
    QModelIndex parent(const QModelIndex&) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    Race *race;
};

QModelIndex Page5Model::index(int row, int col, const QModelIndex&) const
{
    return createIndex(row, col);
}

QModelIndex Page5Model::parent(const QModelIndex&) const
{
    return QModelIndex();
}

int Page5Model::rowCount(const QModelIndex &parent) const
{
    return 1;
}

int Page5Model::columnCount(const QModelIndex &parent) const
{
    return 8;
}

QVariant Page5Model::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::TextAlignmentRole) {
        return int(Qt::AlignRight | Qt::AlignVCenter);
    }
    else if (role == Qt::EditRole) {
        switch(index.column()) {
            case 0: return QVariant((int)race->PopEfficiency());
            case 1: return QVariant((int)race->FactoryRate());
            case 2: return QVariant((int)race->FactoryCost().GetResources());
            case 3: return QVariant((int)race->FactoriesRun());
            case 4: return QVariant((int)race->FactoryCost()[2] < 4);
            case 5: return QVariant((int)race->MineRate());
            case 6: return QVariant((int)race->MineCost().GetResources());
            default: return QVariant((int)race->MinesRun());
        }
    }
    return QVariant();
}

RaceWizard::RaceWizard(Race *_race, QWidget *parent)
    : QDialog(parent)
    , page5DataModel(new Page5Model(_race, this))
{
    pagesWidget = new QStackedWidget;
    pagesWidget->setContentsMargins(QMargins(0, 0, 0, 0));

    QWidget *page1Widget = new QWidget;
    Ui_RaceWizardPage1 page1;
    page1.setupUi(page1Widget);
    pagesWidget->addWidget(page1Widget);

    createPage2();
    createPage3();

    QWidget *page4Widget = new QWidget;
    Ui_RaceWizardPage4 page4;
    page4.setupUi(page4Widget);
    pagesWidget->addWidget(page4Widget);

    createPage5();

    QWidget *page6Widget = new QWidget;
    Ui_RaceWizardPage6 page6;
    page6.setupUi(page6Widget);
    pagesWidget->addWidget(page6Widget);

    QWidget *buttonsPane = new QWidget;
    buttonsPane->setContentsMargins(QMargins(0, 0, 0, 0));

    QPushButton *helpButton = new QPushButton(tr("&Help"));
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    backButton = new QPushButton(tr("< &Back"));
    nextButton = new QPushButton(tr("&Next >"));
    QPushButton *finishButton = new QPushButton(tr("&Finish"));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(helpButton);
    layout->addWidget(cancelButton);
    layout->addWidget(backButton);
    layout->addWidget(nextButton);
    layout->addWidget(finishButton);
    buttonsPane->setLayout(layout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(pagesWidget);
    mainLayout->addWidget(buttonsPane);
    setLayout(mainLayout);

    pageChanged();

    connect(helpButton, SIGNAL(clicked(bool)), this, SLOT(helpClicked(bool)));
    connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancelClicked(bool)));
    connect(backButton, SIGNAL(clicked(bool)), this, SLOT(backClicked(bool)));
    connect(nextButton, SIGNAL(clicked(bool)), this, SLOT(nextClicked(bool)));
    connect(finishButton, SIGNAL(clicked(bool)), this, SLOT(finishClicked(bool)));
}

void RaceWizard::createPage2()
{
    QWidget *page2Widget = new QWidget;
    Ui_RaceWizardPage2 page2;
    page2.setupUi(page2Widget);
    pagesWidget->addWidget(page2Widget);

    QVBoxLayout *column1Layout = new QVBoxLayout(page2.PRTcolumn1);
    QVBoxLayout *column2Layout = new QVBoxLayout(page2.PRTcolumn2);

    QButtonGroup *buttonGroup = new QButtonGroup(this);

    unsigned int numPRTs = RacialTrait::PrimaryTraitCount();

    for(unsigned int i = 0 ; i != numPRTs ; i++) {
        const RacialTrait *prt = RacialTrait::GetPrimaryTrait(i);

        QRadioButton *radioButton = new QRadioButton(prt->Name().c_str(), this);

        if(i < (numPRTs / 2)) {
            column1Layout->addWidget(radioButton);
        }
        else {
            column2Layout->addWidget(radioButton);
        }

        buttonGroup->addButton(radioButton, i);
    }

    connect(buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(PRTChanged(int))); 
}

void RaceWizard::createPage3()
{
    QWidget *page3Widget = new QWidget;
    Ui_RaceWizardPage3 page3;
    page3.setupUi(page3Widget);
    pagesWidget->addWidget(page3Widget);

    QVBoxLayout *column1Layout = new QVBoxLayout(page3.LRTcolumn1);
    QVBoxLayout *column2Layout = new QVBoxLayout(page3.LRTcolumn2);

    QButtonGroup *buttonGroup = new QButtonGroup(this);

    unsigned int numLRTs = RacialTrait::LesserTraitCount();

    for(unsigned int i = 0 ; i != numLRTs ; i++) {
        const RacialTrait *lrt = RacialTrait::GetLesserTrait(i);

        QCheckBox *checkBox = new QCheckBox(lrt->Name().c_str(), this);

        if(i < (numLRTs / 2)) {
            column1Layout->addWidget(checkBox);
        }
        else {
            column2Layout->addWidget(checkBox);
        }

        buttonGroup->addButton(checkBox, i);
    }

    buttonGroup->setExclusive(false);

    connect(buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(LRTChanged(QAbstractButton*))); 
}

void RaceWizard::createPage5()
{
    QWidget *page5Widget = new QWidget;
    Ui_RaceWizardPage5 page5;
    page5.setupUi(page5Widget);
    pagesWidget->addWidget(page5Widget);

    page5DataMapper = new QDataWidgetMapper(this);
    page5DataMapper->setModel(page5DataModel);

    page5DataMapper->addMapping(page5.popEfficiencyBox, 0);
    page5DataMapper->addMapping(page5.factoryRateBox, 1);
    page5DataMapper->addMapping(page5.factoryCostResourcesBox, 2);
    page5DataMapper->addMapping(page5.factoriesRunBox, 3);
    page5DataMapper->addMapping(page5.factoryCostDiscountBox, 4);
    page5DataMapper->addMapping(page5.mineRateBox, 5);
    page5DataMapper->addMapping(page5.mineCostResourcesBox, 6);
    page5DataMapper->addMapping(page5.minesRunBox, 7);

    page5DataMapper->toFirst();
}

void RaceWizard::helpClicked(bool)
{
}

void RaceWizard::cancelClicked(bool)
{
    reject();
}

void RaceWizard::backClicked(bool)
{
    if(pagesWidget->currentIndex() > 0) {
        pagesWidget->setCurrentIndex(pagesWidget->currentIndex() - 1);
        pageChanged();
    }
}

void RaceWizard::nextClicked(bool)
{
    if(pagesWidget->currentIndex() < pagesWidget->count()-1) {
        pagesWidget->setCurrentIndex(pagesWidget->currentIndex() + 1);
        pageChanged();
    }
}

void RaceWizard::finishClicked(bool)
{
    accept();
}

void RaceWizard::pageChanged()
{
    backButton->setEnabled(pagesWidget->currentIndex() > 0);
    nextButton->setEnabled(pagesWidget->currentIndex() < pagesWidget->count()-1);
    setWindowTitle(tr("View Race-- Step %0 of %1")
        .arg(pagesWidget->currentIndex() + 1)
        .arg(pagesWidget->count()));
}

void RaceWizard::PRTChanged(int index)
{
    std::cout << "set PRT " << index << std::endl;
}

void RaceWizard::LRTChanged(QAbstractButton *button)
{
    std::cout << "set LRT \"" << button->text().toStdString() << "\" " << button->isChecked() << std::endl;
}

};