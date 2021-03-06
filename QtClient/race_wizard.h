/*
 * Copyright (C) 2014 Valery Kholodkov
 */
#ifndef _RACE_WIZARD_H_
#define _RACE_WIZARD_H_

#include <memory>

#include <QGroupBox>
#include <QDialog>
#include <QStackedWidget>
#include <QAbstractButton>
#include <QAbstractItemModel>
#include <QDataWidgetMapper>

#include "FSServer.h"

namespace FreeStars {

class TechFactorWidget : public QGroupBox {
    Q_OBJECT
    Q_PROPERTY(double techCostFactor READ techCostFactor WRITE setTechCostFactor)

public:
    TechFactorWidget(const QString &_title, QWidget *parent = 0);

    double techCostFactor() const;
    void setTechCostFactor(double);

private:
    QAbstractButton *extra, *normal, *less;
};

class RaceWizard : public QDialog {
    Q_OBJECT

public:
    RaceWizard(Race*, bool readOnly = false, QWidget *parent = 0);

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
    void createPage6();

    void pageChanged();

private:
    Race *race;

    QPushButton *nextButton;
    QPushButton *backButton;
    QStackedWidget *pagesWidget;

    QDataWidgetMapper *page5DataMapper;
    QAbstractItemModel *page5DataModel;
};

};

#endif
