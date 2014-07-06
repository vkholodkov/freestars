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

#include "diamond_button.h"

#include "ui_angw_page1.h"
#include "ui_angw_page3.h"

#include "advanced_new_game_wizard.h"

namespace FreeStars {

AdvancedNewGameWizard::AdvancedNewGameWizard(QWidget *parent)
    : QDialog(parent)
    , playerButtonsMapper(new QSignalMapper(this))
    , victoryConditions(new VictoryConditions)
    , victoryConditionsModel(new QStandardItemModel(1, 17, this))
    , messages()
    , races(16, (Race*)0)
    , raceLabels(16, (QLabel*)0)
    , game(new Game)
    , currentPlayer(0)
{
    newRaceAction = new QAction(tr("New..."), this);
    openRaceAction = new QAction(tr("Open..."), this);
    editRaceAction = new QAction(tr("Edit Race..."), this);

    pagesWidget = new QStackedWidget;
    pagesWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    QWidget *page1Widget = new QWidget;
    Ui_AdvancedNewGameWizardPage1 page1;
    page1.setupUi(page1Widget);
    pagesWidget->addWidget(page1Widget);

    QWidget *page2 = new QWidget;
    QLayout *page2Layout = new QVBoxLayout;

    page2Layout->setContentsMargins(0, 0, 0, 0);
    page2Layout->setSpacing(0);

    for(int i = 0 ; i != 16 ; i++) {
        QWidget *w = new QWidget;
        QHBoxLayout *wLayout = new QHBoxLayout;
        wLayout->setContentsMargins(0, 0, 0, 0);
        wLayout->setSpacing(3);

        DiamondButton *b = new DiamondButton;
        wLayout->addWidget(new QLabel(tr("Player #%0").arg(i + 1)));
        wLayout->addWidget(b);

        QLabel *raceLabel = new QLabel(tr("--- No Player ---"));
        wLayout->addWidget(raceLabel);
        wLayout->addStretch();

        w->setLayout(wLayout);

        page2Layout->addWidget(w);

        playerButtonsMapper->setMapping(b, i);
        raceLabels[i] = raceLabel;

        connect(b, SIGNAL(clicked(bool)), playerButtonsMapper, SLOT(map()));
    }

    page2->setLayout(page2Layout);
    pagesWidget->addWidget(page2);

    QWidget *page3Widget = new QWidget;
    Ui_AdvancedNewGameWizardPage3 page3;
    page3.setupUi(page3Widget);
    pagesWidget->addWidget(page3Widget);

    victoryConditionsMapper = new QDataWidgetMapper(this);
    victoryConditionsMapper->setModel(victoryConditionsModel);

    victoryConditionsMapper->addMapping(page3.worldsCheck, 0);
    victoryConditionsMapper->addMapping(page3.techCheck, 1);
    victoryConditionsMapper->addMapping(page3.scoreCheck, 2);
    victoryConditionsMapper->addMapping(page3.times2ndCheck, 3);
    victoryConditionsMapper->addMapping(page3.resourcesCheck, 4);
    victoryConditionsMapper->addMapping(page3.capShipsCheck, 5);
    victoryConditionsMapper->addMapping(page3.highScoreAtCheck, 6);

    victoryConditionsMapper->addMapping(page3.worldsBox, 0);
    victoryConditionsMapper->addMapping(page3.techLevelBox, 1);
    victoryConditionsMapper->addMapping(page3.techCountBox, 2);
    victoryConditionsMapper->addMapping(page3.scoreBox, 3);
    victoryConditionsMapper->addMapping(page3.times2ndBox, 4);
    victoryConditionsMapper->addMapping(page3.resourcesBox, 5);
    victoryConditionsMapper->addMapping(page3.capShipsBox, 6);
    victoryConditionsMapper->addMapping(page3.highScoreAtBox, 7);
    victoryConditionsMapper->addMapping(page3.countBox, 8);
    victoryConditionsMapper->addMapping(page3.starsBox, 9);

    QWidget *buttonsPane = new QWidget;

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

    connect(playerButtonsMapper, SIGNAL(mapped(int)), this, SLOT(selectPlayer(int)));

    connect(newRaceAction, SIGNAL(triggered()), this, SLOT(newRace()));
    connect(openRaceAction, SIGNAL(triggered()), this, SLOT(openRace()));
    connect(editRaceAction, SIGNAL(triggered()), this, SLOT(editRace()));
}

AdvancedNewGameWizard::~AdvancedNewGameWizard()
{
    for(std::list<Message*>::const_iterator i = messages.begin() ; i != messages.end() ; i++) {
        delete *i;
    }

    for(std::vector<Race*>::const_iterator i = races.begin() ; i != races.end() ; i++) {
        delete *i;
    }
}

void AdvancedNewGameWizard::helpClicked(bool)
{
}

void AdvancedNewGameWizard::cancelClicked(bool)
{
    reject();
}

void AdvancedNewGameWizard::backClicked(bool)
{
    if(pagesWidget->currentIndex() > 0) {
        pagesWidget->setCurrentIndex(pagesWidget->currentIndex() - 1);
        pageChanged();
    }
}

void AdvancedNewGameWizard::nextClicked(bool)
{
    if(pagesWidget->currentIndex() < 2) {
        pagesWidget->setCurrentIndex(pagesWidget->currentIndex() + 1);
        pageChanged();
    }
}

void AdvancedNewGameWizard::finishClicked(bool)
{
    victoryConditionsMapper->submit();

    accept();
}

void AdvancedNewGameWizard::pageChanged()
{
    backButton->setEnabled(pagesWidget->currentIndex() > 0);
    nextButton->setEnabled(pagesWidget->currentIndex() < 2);
    setWindowTitle(tr("Advanced New Game Wizard -- Step %0 of %1")
        .arg(pagesWidget->currentIndex() + 1)
        .arg(pagesWidget->count()));
}

void AdvancedNewGameWizard::selectPlayer(int player)
{
    QWidget *widget = dynamic_cast<QWidget*>(playerButtonsMapper->mapping(player));

    if(widget == NULL) {
        return;
    }

    currentPlayer = player;

    QMenu menu(this);

#if 0
    QMenu *predefinedRaceMenu = menu.addMenu("Predefined Race");
#endif
    QMenu *customRaceMenu = menu.addMenu(tr("Custom Race"));
    customRaceMenu->addAction(newRaceAction);
    customRaceMenu->addAction(openRaceAction);
    menu.addAction(editRaceAction);
    menu.addSeparator();
    QMenu *computerPlayerMenu = menu.addMenu(tr("Computer Player"));
    menu.addSeparator();
    QAction *noPlayerAction = menu.addAction(tr("No Player"));

    QAction *clicked = menu.exec(widget->mapToGlobal(QPoint(0, 0)));
}

void AdvancedNewGameWizard::newRace()
{
}

void AdvancedNewGameWizard::openRace()
{
    QString gameDir(game->GetFileLoc().c_str());

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                     gameDir,
                                                     tr("Race Files (*.r*)"));

    QFileInfo fi(fileName);

    TiXmlDocument doc(fileName.toAscii().constData());
    doc.SetCondenseWhiteSpace(false);
    if (!doc.LoadFile()) {
        QMessageBox::critical(this, tr("Error"),
            tr("Cannot open file %0").arg(fileName));
        return;
    }

    const TiXmlNode * node;

    node = doc.FirstChild("RaceDefinition");
    if (!node) {
        QMessageBox::critical(this, tr("Error"),
            tr("File %0 contains no race definition").arg(fileName));
        return;
    }

    if (!game->CheckMetaInfo(node, fileName.toAscii().constData(), RACEFILEVERSION)) {
        QMessageBox::critical(this, tr("Error"),
            tr("File %0 cannot be opened in this version of the game").arg(fileName));
        return;
    }

    std::auto_ptr<Race> race(new Race);

    if (!race->ParseNode(node, false, *this)) {
        QMessageBox::critical(this, tr("Error"),
            tr("Cannot read race information from file %0: invalid file format").arg(fileName));
        return;
    }

    long points = race->GetAdvantagePoints();
    if(points < 0) {
        QMessageBox::question(this, tr("Error"),
            tr("The race from file %0 has negative amount of advantage points. " \
               "You cannot use this race unless the amount of advanatage points is positive. " \
               "Would you like to edit this file?").arg(fileName),
               QMessageBox::Yes | QMessageBox::No);
        return;
    }


    delete races[currentPlayer];

    races[currentPlayer] = race.release();
    raceLabels[currentPlayer]->setText(tr("The %0 (%1)")
        .arg(race->GetSingleName().c_str())
        .arg(fi.fileName()));
}

void AdvancedNewGameWizard::editRace()
{
}

Message *AdvancedNewGameWizard::AddMessage(const std::string &type)
{
    std::auto_ptr<Message> mess(new Message(type));
    messages.push_back(mess.get());
    return mess.release();
}

};
