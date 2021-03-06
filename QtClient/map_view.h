/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _MAP_VIEW_H_
#define _MAP_VIEW_H_

#include <list>

#include <QWidget>
#include <QCursor>

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
    Q_OBJECT

public:
    typedef std::pair<int, int> loc_t;
    typedef std::pair<loc_t, loc_t> order_t;
    typedef std::pair<QPoint, QPoint> track_t;
    typedef std::pair<QPoint, int> scan_area_t;

public:
    MapView(const Galaxy*, const Game*, const Player*, QWidget *parent = 0);
    ~MapView();

    QSize sizeHint() const;

    QPoint galaxyToScreen(const QPoint&) const;
    int galaxyToScreen(int) const;

    void clearSelection();
    void setSelection(const Location*);
    const SpaceObject *findSelection(const QPoint&) const;

signals:
    void selectionChanged(const SpaceObject*);
    void listObjectsInLocation(const SpaceObject*, const QPoint&);
    void waypointAdded(const Location*);

public slots:
    void setViewMode(int);

/*
    void showScannerRange(bool);
    void showFleetPaths(bool);
    void showNames(bool);
    void showShipCount(bool);
    void dontShowIdleFleets(bool);
*/

protected:
    void paintEvent(QPaintEvent*);    
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void contextMenuEvent(QContextMenuEvent*);
    bool eventFilter(QObject*, QEvent*);

private:
    void setAddWaypointMode(bool);

    void drawArrow(QPainter&, const QPoint&);
    void drawDot(QPainter&, const QPoint&, const QColor&);
    void drawCircle(QPainter&, const QPoint&, int);
    void fillCircle(QPainter&, const QPoint&, int, const QColor&);
    void normalPlanetDrawer(QPainter&, const Planet*, const QPoint&);
    void surfaceMineralPlanetDrawer(QPainter&, const Planet*, const QPoint&);
    void mineralConcPlanetDrawer(QPainter&, const Planet*, const QPoint&);
    void valuePlanetDrawer(QPainter&, const Planet*, const QPoint&);
    void populationPlanetDrawer(QPainter&, const Planet*, const QPoint&);
    void noInfoPlanetDrawer(QPainter&, const Planet*, const QPoint&);
    void collectTracks(std::list<track_t>&);
    void collectScanAreas(std::list<scan_area_t>&);

private:
    static void (MapView::*planetDrawers[])(QPainter&, const Planet*, const QPoint&);
    QCursor arrow_up, cross;
    const Galaxy *galaxy;
    const Game *game;
    const Player *player;
    const Location *selection;
    int mapMode;
    unsigned mapOptions;
    bool addWaypointMode;
};

};

#endif
