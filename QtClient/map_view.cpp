/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <limits>

#include <QPainter>
#include <QMouseEvent>
#include <QScrollArea>

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
    setContextMenuPolicy(Qt::CustomContextMenu);
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

void MapView::clearSelection()
{
    if(selection != NULL) {
        QPoint pos(galaxyToScreen(QPoint(selection->GetPosX(), selection->GetPosY())));

        QRect updateRect(pos, QSize(60, 60));

        updateRect.translate(-30, -30);

        QPoint parentPos(mapToParent(updateRect.topLeft()));

        QRect viewportUpdateRect(parentPos, updateRect.size());

        selection = NULL;

        if(viewportUpdateRect.intersects(parentWidget()->contentsRect())) {
            update(updateRect);
            parentWidget()->update(viewportUpdateRect);
        }
    }
}

void MapView::setSelection(const SpaceObject *o) {
    selection = o;

    QPoint pos(galaxyToScreen(QPoint(o->GetPosX(), o->GetPosY())));

    QRect updateRect(pos, QSize(60, 60));

    updateRect.translate(-30, -30);

    QPoint parentPos(mapToParent(updateRect.topLeft()));

    QRect viewportUpdateRect(parentPos, updateRect.size());

    if(viewportUpdateRect.intersects(parentWidget()->contentsRect())) {
        update(updateRect);
        parentWidget()->update(viewportUpdateRect);
    }
}

const SpaceObject *MapView::findSelection(const QPoint &selectionPos) const {
    double min_distance = std::numeric_limits<double>::max();
    const SpaceObject *selection = NULL;

    unsigned num_planets = galaxy->GetPlanetCount();

    for(unsigned n = 1 ; n <= num_planets ; n++) {
        const Planet *planet = galaxy->GetPlanet(n);

        QPoint pos(galaxyToScreen(QPoint(planet->GetPosX(), planet->GetPosY())));

        double dx = abs(selectionPos.x() - pos.x());
        double dy = abs(selectionPos.y() - pos.y());

        double distance = ::sqrt(dx * dx + dy * dy);

        if(distance < min_distance) {
            min_distance = distance;
            selection = planet;
        }
    }

    if(min_distance > 20) {
        selection = NULL;
    }

    return selection;
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

void MapView::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        const SpaceObject *newSelection = findSelection(e->pos());

        if(newSelection != NULL) {
            emit selectionChanged(newSelection);
            return;
        }
    }

    QWidget::mousePressEvent(e);
}

void MapView::mouseReleaseEvent(QMouseEvent *e)
{
}

void MapView::mouseMoveEvent(QMouseEvent *e)
{
}

};
