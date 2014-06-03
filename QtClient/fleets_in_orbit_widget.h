
#ifndef _FLEETS_IN_ORBIT_WIDGET_H_
#define _FLEETS_IN_ORBIT_WIDGET_H_

#include <QWidget>
#include <QComboBox>

#include "folding_widget.h"
#include "cargo_widget.h"

#include "FSServer.h"

namespace FreeStars {

class FleetsInOrbitWidget : public FoldingWidget {
    Q_OBJECT

public:
    FleetsInOrbitWidget(const Planet*, const Player*, QWidget *parent = 0);

public slots:
    void gotoButtonClicked(bool);
    void cargoButtonClicked(bool);

signals:
    void selectObject(const SpaceObject*);
    void exchangeCargo(const Planet*, const Fleet*);

private slots:
    void objectActivated(int);

private:
    QWidget *textWidget;
    QComboBox *fleetsComboBox;
    CargoWidget *fuelWidget, *cargoWidget;
    const Planet *planet;
    const Player *player;
};

};

#endif
