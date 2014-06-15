
#ifndef _SHIP_AVATAR_WIDGET_H_
#define _SHIP_AVATAR_WIDGET_H_

#include <QWidget>
#include <QPaintEvent>

#include "graphics_array.h"

using namespace FreeStars;

class ShipAvatarWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QString hullName READ hullName WRITE setHullName)

public:
    ShipAvatarWidget(QWidget *parent = 0)
        : QWidget(parent)
        , graphicsArray(0)
    {
    }

    void setGraphicsArray(const GraphicsArray *_graphicsArray) { graphicsArray = _graphicsArray; }

    QString hullName() const { return m_hullName; }
    void setHullName(const QString&);

protected:
    void paintEvent(QPaintEvent*);

private:
    const GraphicsArray *graphicsArray;
    QString m_hullName;
};

#endif
