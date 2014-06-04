
#ifndef _ADVANCED_NEW_GAME_WIZARD_H_
#define _ADVANCED_NEW_GAME_WIZARD_H_

#include <QDialog>
#include <QStackedWidget>

#include "FSServer.h"

namespace FreeStars {

class AdvancedNewGameWizard : public QDialog {
    Q_OBJECT

public:
    AdvancedNewGameWizard(QWidget *parent = 0);

private slots:
    void helpClicked(bool);
    void cancelClicked(bool);
    void backClicked(bool);
    void nextClicked(bool);
    void finishClicked(bool);

private:
    void pageChanged();

private:
    QPushButton *nextButton;
    QPushButton *backButton;
    QStackedWidget *pagesWidget;
};

};

#endif
