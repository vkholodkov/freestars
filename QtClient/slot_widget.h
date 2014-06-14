
#ifndef _SLOT_WIDGET_H_
#define _SLOT_WIDGET_H_

#include <QWidget>
#include <QPainter>
#include <QDropEvent>
#include <QDragEnterEvent>

#include "FSServer.h"

#include "graphics_array.h"

namespace FreeStars {

class HullSlotWidget : public QWidget {
    Q_OBJECT

public:
    HullSlotWidget(const Slot &_slot, QWidget *parent = 0)
        : QWidget(parent)
        , hullSlot(_slot)
    {
    }

protected:
    void paintEvent(QPaintEvent*);

    const Slot &hullSlot;

private:
    QString describe() const;
};

class SlotWidget : public HullSlotWidget {
    Q_OBJECT

public:
    SlotWidget(const GraphicsArray *_graphicsArray, Slot &_shipSlot, const Slot &_hullSlot, QWidget *parent = 0)
        : HullSlotWidget(_hullSlot, parent)
        , plateImage(":/images/plate.png")
        , shipSlot(_shipSlot)
        , graphicsArray(_graphicsArray)
    {
        setAcceptDrops(true);
    }

protected:
    void paintEvent(QPaintEvent*);
    void dragEnterEvent(QDragEnterEvent*);
    void dropEvent(QDropEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);

private:
    void drawComponent(QPainter&, const QRect&);
    Component *extractComponent(QDropEvent*);

    static int getAmount();

private:
    QImage plateImage;
    Slot &shipSlot;
    const GraphicsArray *graphicsArray;
    QPoint dragStartPos;
};

class CargoWidget : public QWidget {
    Q_OBJECT

public:
    CargoWidget(QWidget *parent = 0)
        : QWidget(parent)
    {
    }

protected:
    void paintEvent(QPaintEvent*);
};

};

#endif
