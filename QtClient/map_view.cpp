/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <limits>
#include <set>

#include <QPainter>
#include <QMouseEvent>
#include <QScrollArea>
#include <QApplication>

#include "map_view.h"

namespace FreeStars {

static const char * arrow_up_xpm[] = {
"16 16 2 1",
"   c None",
".  c #FFFFFF",
"       .        ",
"       .        ",
"      ...       ",
"      ...       ",
"      ...       ",
"      ....      ",
"     .....      ",
"     .....      ",
"     ......     ",
"    .......     ",
"    ........    ",
"    ........    ",
"   ....  ...    ",
"   ..     ...   ",
"  ..       ..   ",
"  .         ..  "};

static const char * cross_xpm[] = {
"16 16 2 1",
" 	c None",
".	c #FFFFFF",
"                ",
"                ",
"                ",
"                ",
"                ",
"       .        ",
"       .        ",
"       .        ",
"    .......     ",
"       .        ",
"       .        ",
"       .        ",
"                ",
"                ",
"                ",
"                "};

MapView::MapView(const Galaxy *_galaxy, const Game *_game, const Player *_player, QWidget *parent)
    : QWidget(parent)
    , arrow_up(QPixmap(arrow_up_xpm), 8, 0)
    , cross(QPixmap(cross_xpm), 7, 8)
    , galaxy(_galaxy)
    , game(_game)
    , player(_player)
    , selection(0)
    , mapMode(MM_NORMAL)
{
    setContextMenuPolicy(Qt::DefaultContextMenu);
    setAddWaypointMode(false);

    qApp->installEventFilter(this);
}

MapView::~MapView()
{
    qApp->removeEventFilter(this);
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

int MapView::galaxyToScreen(int distance) const {
    return (distance * contentsRect().width()) / (galaxy->MaxX() - galaxy->MinX());
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

void MapView::setSelection(const Location *o) {
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

    std::list<track_t> tracks;
    std::list<scan_area_t> scan_areas;

    collectTracks(tracks);
    collectScanAreas(scan_areas);

    for (std::list<scan_area_t>::const_iterator i = scan_areas.begin(); i != scan_areas.end(); i++) {
        QPainterPath path;
        path.addEllipse(i->first, i->second / 10, i->second / 10);
        painter.fillPath(path, QBrush(QColor(143, 29,12)));
    }

    painter.setPen(Qt::blue);

    for (std::list<track_t>::const_iterator i = tracks.begin() ; i != tracks.end() ; i++) {
        painter.drawLine(i->first, i->second);
    }

    if (selection != NULL) {
        drawArrow(painter, galaxyToScreen(QPoint(selection->GetPosX(), selection->GetPosY())));
    }

    unsigned num_planets = galaxy->GetPlanetCount();

    for(unsigned n = 1 ; n <= num_planets ; n++) {
        const Planet *planet = galaxy->GetPlanet(n);

        QPoint pos(galaxyToScreen(QPoint(planet->GetPosX(), planet->GetPosY())));

        (this->*planetDrawers[mapMode])(painter, planet, pos);
    }
}

void MapView::normalPlanetDrawer(QPainter &painter, const Planet *planet, const QPoint &pos)
{
    long seen = planet->SeenBy(player);

    if(seen == 0) {
        noInfoPlanetDrawer(painter, planet, pos);
        return;
    }
    
    const Player *owner = planet->GetOwner();

    int diameter = 3;

    if(seen & SEEN_OWNER) {
        if(owner != NULL) {
            long relation = player->GetRelations(owner);

            if(relation == PR_SELF) {
                if(planet->IsHW()) {
                    diameter += 1;
                }

                fillCircle(painter, pos, diameter, Qt::green);
            }
            else if(relation == PR_FRIEND) {
                fillCircle(painter, pos, diameter, Qt::yellow);
            }
            else {
                fillCircle(painter, pos, diameter, Qt::red);
            }
        }
        else {
            drawDot(painter, pos, Qt::white);
        }
    }
    else {
        drawDot(painter, pos, Qt::white);
    }

    if(seen & SEEN_HULL) {
        const deque<SpaceObject *> *alsoHere = planet->GetAlsoHere();

        if(alsoHere && !alsoHere->empty()) {
            diameter += 3;
            painter.setPen(Qt::white);
            drawCircle(painter, pos, diameter);
        }

        if(planet->GetBaseDesign() != NULL) {
            fillCircle(painter, pos + QPoint(3, -3), 2, Qt::yellow);
        }
    }
}

void MapView::surfaceMineralPlanetDrawer(QPainter &painter, const Planet *planet, const QPoint &pos)
{
    normalPlanetDrawer(painter, planet, pos);
}

void MapView::mineralConcPlanetDrawer(QPainter &painter, const Planet *planet, const QPoint &pos)
{
    normalPlanetDrawer(painter, planet, pos);
}

void MapView::valuePlanetDrawer(QPainter &painter, const Planet *planet, const QPoint &pos)
{
    long seen = planet->SeenBy(player);

    if(seen == 0) {
        noInfoPlanetDrawer(painter, planet, pos);
        return;
    }

    if((seen & SEEN_PLANETHAB) == 0) {
        drawDot(painter, pos, Qt::white);
        return;
    }
    
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

void MapView::populationPlanetDrawer(QPainter &painter, const Planet *planet, const QPoint &pos)
{
    long seen = planet->SeenBy(player);

    if(seen == 0) {
        noInfoPlanetDrawer(painter, planet, pos);
        return;
    }

    if((seen & SEEN_PLANETPOP) == 0) {
        drawDot(painter, pos, Qt::white);
        return;
    }

    int hab = player->HabFactor(planet);

    fillCircle(painter, pos, 2 + (8 * planet->GetDisplayPop()) / planet->GetMaxPop(),
        hab > 0 ? Qt::green : Qt::red);
}

void MapView::noInfoPlanetDrawer(QPainter &painter, const Planet*, const QPoint &pos)
{
    drawDot(painter, pos, Qt::gray);
}

void MapView::drawDot(QPainter &painter, const QPoint &pos, const QColor &color)
{
    painter.setPen(color);

    painter.drawPoint(pos);
    painter.drawPoint(pos + QPoint(1, 0));
    painter.drawPoint(pos + QPoint(0, 1));
    painter.drawPoint(pos + QPoint(1, 1));
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

void MapView::drawCircle(QPainter &painter, const QPoint &center, int diameter)
{
    QPainterPath path, patho;

    path.addEllipse(center, diameter, diameter);
    painter.drawPath(path);
}

void MapView::fillCircle(QPainter &painter, const QPoint &center, int diameter, const QColor &color)
{
    QPainterPath path, patho;

    path.addEllipse(center, diameter, diameter);
    painter.fillPath(path, QBrush(color.darker()));

    patho.addEllipse(center, diameter / 1.1, diameter / 1.1);
    painter.fillPath(patho, QBrush(color));
}

void MapView::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        if(!addWaypointMode) {
            const SpaceObject *newSelection = findSelection(e->pos());

            if(newSelection != NULL) {
                emit selectionChanged(newSelection);
                return;
            }
        }
        else {
            const SpaceObject *newSelection = findSelection(e->pos());

            if(newSelection != NULL) {
                emit waypointAdded(newSelection);
                return;
            }
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

void MapView::contextMenuEvent(QContextMenuEvent *e)
{
    const SpaceObject *selection = findSelection(e->pos());

    if(selection != NULL) {
        emit listObjectsInLocation(selection, mapToGlobal(e->pos()));
    }
}

bool MapView::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if(keyEvent->key() == Qt::Key_Shift) {
            setAddWaypointMode(true);
        }
        else if(keyEvent->key() == Qt::Key_Control) {
        }
        return false;
    }
    else if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if(keyEvent->key() == Qt::Key_Shift) {
            setAddWaypointMode(false);
        }
        else if(keyEvent->key() == Qt::Key_Control) {
        }
        return false;
    }

    return QObject::eventFilter(object, event);
}

void MapView::setViewMode(int _mapMode)
{
    mapMode = _mapMode;

    update(contentsRect());
    parentWidget()->update(parentWidget()->contentsRect());
}

void MapView::setAddWaypointMode(bool enabled)
{
    addWaypointMode = enabled;

    setCursor(enabled ? cross : arrow_up);
}

void MapView::collectTracks(std::list<track_t> &orders)
{
    std::set<order_t> order_set;

    unsigned num_planets = galaxy->GetPlanetCount();

    for (unsigned n = 1; n <= num_planets; n++) {
        const Planet *planet = galaxy->GetPlanet(n);

        const std::deque<SpaceObject *> *alsoHere = planet->GetAlsoHere();

        if (alsoHere != NULL) {
            for (std::deque<SpaceObject *>::const_iterator i = alsoHere->begin(); i != alsoHere->end() ; i++) {
                const Fleet *fleet = dynamic_cast<const Fleet*>(*i);

                if (fleet != NULL && (fleet->SeenBy(player) & SEEN_ORDERS)) {
                    const std::deque<WayOrder *> &orders = fleet->GetOrders();

                    std::deque<QPoint> points;

                    for (std::deque<WayOrder*>::const_iterator oi = orders.begin(); oi != orders.end(); oi++) {
                        points.push_back(galaxyToScreen(QPoint((*oi)->GetLocation()->GetPosX(), (*oi)->GetLocation()->GetPosY())));

                        if (points.size() == 2) {
                            order_set.insert(std::make_pair(
                                std::make_pair(points[0].x(), points[0].y()),
                                std::make_pair(points[1].x(), points[1].y())));
                            points.pop_front();
                        }
                    }
                }
            }
        }
    }

    for (std::set<order_t>::const_iterator i = order_set.begin(); i != order_set.end(); i++) {
        orders.push_back(std::make_pair(QPoint(i->first.first, i->first.second),
            QPoint(i->second.first, i->second.second)));
    }
}

void MapView::collectScanAreas(std::list<scan_area_t> &scan_areas)
{
    unsigned num_planets = galaxy->GetPlanetCount();

    for (unsigned n = 1; n <= num_planets; n++) {
        const Planet *planet = galaxy->GetPlanet(n);

        if (planet->SeenBy(player) & SEEN_INSTALLATIONS) {
            long scanRange = planet->GetScanSpace();

            if (scanRange > 0) {
                scan_areas.push_back(std::make_pair(galaxyToScreen(QPoint(planet->GetPosX(), planet->GetPosY())),
                    galaxyToScreen(scanRange)));
            }
        }
    }
}

};
