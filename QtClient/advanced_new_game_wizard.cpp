
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "diamond_button.h"

#include "ui_angw_page1.h"
#include "ui_angw_page3.h"

#include "advanced_new_game_wizard.h"

namespace FreeStars {

AdvancedNewGameWizard::AdvancedNewGameWizard(QWidget *parent)
    : QDialog(parent)
{
    pagesWidget = new QStackedWidget;

    QWidget *page1 = new QWidget;
    Ui_AdvancedNewGameWizardPage1 advancedNewGameWizardPage1;
    advancedNewGameWizardPage1.setupUi(page1);
    pagesWidget->addWidget(page1);

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
        wLayout->addWidget(new QLabel(tr("--- No Player ---")));
        wLayout->addStretch();

        w->setLayout(wLayout);

        page2Layout->addWidget(w);
    }

    page2->setLayout(page2Layout);
    pagesWidget->addWidget(page2);

    QWidget *page3 = new QWidget;
    Ui_AdvancedNewGameWizardPage3 advancedNewGameWizardPage3;
    advancedNewGameWizardPage3.setupUi(page3);
    pagesWidget->addWidget(page3);

    pagesWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

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

};
