/*
 * Copyright (C) 2014 Valery Kholodkov
 */
#ifndef _RACE_WIZARD_H_
#define _RACE_WIZARD_H_

#include <memory>

#include <QLabel>
#include <QDialog>
#include <QStackedWidget>
#include <QAbstractButton>
#include <QAbstractItemModel>
#include <QDataWidgetMapper>

#include "FSServer.h"

namespace FreeStars {

class RaceWizard : public QDialog {
    Q_OBJECT

public:
    RaceWizard(Race*, QWidget *parent = 0);

private slots:
    void helpClicked(bool);
    void cancelClicked(bool);
    void backClicked(bool);
    void nextClicked(bool);
    void finishClicked(bool);

    void PRTChanged(int);
    void LRTChanged(QAbstractButton*);

private:
    void createPage2();
    void createPage3();
    void createPage5();

    void pageChanged();

private:
    QPushButton *nextButton;
    QPushButton *backButton;
    QStackedWidget *pagesWidget;

    QDataWidgetMapper *page5DataMapper;
    QAbstractItemModel *page5DataModel;
};

};

#endif
