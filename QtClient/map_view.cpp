/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <iostream>

#include <QPainter>

#include "map_view.h"

namespace FreeStars {

static const char * blarg_xpm[] = {
    "16 7 2 1",
    "* c #000000",
    ". c #ffffff",
    "**..*...........",
    "*.*.*...........",
    "**..*..**.**..**",
    "*.*.*.*.*.*..*.*",
    "**..*..**.*...**",
    "...............*",
    ".............**."
};

MapView::MapView(const Galaxy *_galaxy, const Game *_game, const Player *_player, QWidget *parent)
    : QWidget(parent)
    , galaxy(_galaxy)
    , game(_game)
    , player(_player)
    , selection(0)
    , mapMode(MM_PLANET_VALUE)
{
}

QSize MapView::sizeHint() const {
    QSize hint(galaxy->MaxX() - galaxy->MinX(), galaxy->MaxY() - galaxy->MinY());

    hint *= 5;

    return hint;
}

QPoint MapView::galaxyToScreen(const QPoint &gp) const {
    return QPoint((gp.x() - galaxy->MinX()) * contentsRect().width() / (galaxy->MaxX() - galaxy->MinX()),
        (gp.y() - galaxy->MinY()) * contentsRect().height() / (galaxy->MaxY() - galaxy->MinY()));
}

void MapView::setSelection(const SpaceObject *o) {
    selection = o;

    QPoint pos(galaxyToScreen(QPoint(o->GetPosX(), o->GetPosY())));

    QRect updateRect(pos, QSize(40, 40));

    updateRect.translate(-20, 20);

    update(updateRect);
}

void (MapView::*MapView::planetDrawers[])(QPainter&, const Planet*, const QPoint&) = {
      &MapView::normalPlanetDrawer,
      &MapView::surfaceMineralPlanetDrawer,
      &MapView::mineralConcPlanetDrawer,
      &MapView::valuePlanetDrawer,
      &MapView::populationPlanetDrawer,
      &MapView::noInfoPlanetDrawer
};

void MapView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setRenderHints(QPainter::Antialiasing);

    QRect rect(contentsRect());

    painter.fillRect(rect, Qt::black);

    unsigned num_planets = galaxy->GetPlanetCount();

    for(unsigned n = 1 ; n <= num_planets ; n++) {
        const Planet *planet = galaxy->GetPlanet(n);

        QPoint pos(galaxyToScreen(QPoint(planet->GetPosX(), planet->GetPosY())));

        painter.setPen(Qt::gray);

        painter.drawPoint(pos);
        painter.drawPoint(pos + QPoint(1, 0));
        painter.drawPoint(pos + QPoint(0, 1));
        painter.drawPoint(pos + QPoint(1, 1));

        if(planet->GetOwner() == player) {
            (this->*planetDrawers[mapMode])(painter, planet, pos);
        }
    }

    if(selection != NULL) { 
        drawArrow(painter, galaxyToScreen(QPoint(selection->GetPosX(), selection->GetPosY())));
    }
}

void MapView::normalPlanetDrawer(QPainter&, const Planet*, const QPoint&)
{
}

void MapView::surfaceMineralPlanetDrawer(QPainter&, const Planet*, const QPoint&)
{
}

void MapView::mineralConcPlanetDrawer(QPainter&, const Planet*, const QPoint&)
{
}

void MapView::valuePlanetDrawer(QPainter &painter, const Planet *planet, const QPoint &pos)
{
    const int diameter = 10;
    const int diameter2 = 8;
    int hab = player->HabFactor(planet);

    QPainterPath path, patho;

    if(hab > 0) {
        path.addEllipse(pos, diameter * hab / 100, diameter * hab / 100);
        painter.fillPath(path, QBrush(Qt::darkGreen));

        patho.addEllipse(pos, diameter2 * hab / 100, diameter2 * hab / 100);
        painter.fillPath(patho, QBrush(Qt::green));
    }
    else {
        path.addEllipse(pos, diameter * hab / 100, diameter * hab / 100);
        painter.fillPath(path, QBrush(Qt::darkRed));

        patho.addEllipse(pos, diameter2 * hab / 100, diameter2 * hab / 100);
        painter.fillPath(patho, QBrush(Qt::red));
    }
}

void MapView::populationPlanetDrawer(QPainter&, const Planet*, const QPoint&)
{
}

void MapView::noInfoPlanetDrawer(QPainter&, const Planet*, const QPoint&)
{
}

void MapView::drawArrow(QPainter &painter, const QPoint &p)
{
    QPainterPath path;

    path.moveTo(0, 0);
    path.lineTo(1, 0);
    path.lineTo(7, 15);
    path.lineTo(0, 10);
    path.lineTo(-7, 15);
    path.lineTo(0, 0);

    path.translate(p);
    path.translate(0, 2);

    painter.fillPath(path, QBrush(Qt::yellow));
}

};
