/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _MAP_VIEW_H_
#define _MAP_VIEW_H_

#include <QWidget>

#include "FSServer.h"

namespace FreeStars {

typedef enum {
    MM_NORMAL = 0,
    MM_SURFACE_MIN = 1,
    MM_CONC_MIN = 2,
    MM_PLANET_VALUE = 3,
    MM_POPULATION = 4,
    MM_NO_INFO = 5
} map_mode_t;

typedef enum {
    MO_SCANNER_RANGE, MO_FLEET_PATHS, MO_NAMES, MO_SHIP_COUNT, MO_NO_IDLE_FLEETS
} map_options_t;

class MapView : public QWidget {
public:
    MapView(const Galaxy*, const Game*, const Player*, QWidget *parent = 0);

    QSize sizeHint() const;

    QPoint galaxyToScreen(const QPoint&) const;

    void setSelection(const SpaceObject*);

public slots:
    void setNormalMapMode();
    void setSurfaceMineralsMapMode();
    void setMineralConcMapMode();
    void setPlanetValueMapMode();
    void setPopulationMapMode();
    void setNoInfoMapMode();

    void showScannerRange(bool);
    void showFleetPaths(bool);
    void showNames(bool);
    void showShipCount(bool);
    void dontShowIdleFleets(bool);

protected:
    void paintEvent(QPaintEvent*);    

private:
    void drawArrow(QPainter&, const QPoint&);
    void normalPlanetDrawer(QPainter&, const Planet*, const QPoint&);
    void surfaceMineralPlanetDrawer(QPainter&, const Planet*, const QPoint&);
    void mineralConcPlanetDrawer(QPainter&, const Planet*, const QPoint&);
    void valuePlanetDrawer(QPainter&, const Planet*, const QPoint&);
    void populationPlanetDrawer(QPainter&, const Planet*, const QPoint&);
    void noInfoPlanetDrawer(QPainter&, const Planet*, const QPoint&);

private:
    static void (MapView::*planetDrawers[])(QPainter&, const Planet*, const QPoint&);
    const Galaxy *galaxy;
    const Game *game;
    const Player *player;
    const SpaceObject *selection;
    map_mode_t mapMode;
    unsigned mapOptions;
};

};

#endif
