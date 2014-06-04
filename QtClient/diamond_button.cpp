
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>

#include "diamond_button.h"

namespace FreeStars {

DiamondButton::DiamondButton(QWidget *parent)
    : QAbstractButton(parent)
    , pressed(false)
{
    setCursor(Qt::PointingHandCursor);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

QSize DiamondButton::sizeHint() const {
    return QSize(15, 15);
}

void DiamondButton::paintEvent(QPaintEvent *event)
{
    QRect rect(contentsRect());
    QPainter painter(this);

    drawDiamond(painter, rect, Qt::gray);

    rect.adjust(rect.width() / 4, rect.height() / 4, -rect.width() / 4, -rect.height() / 4);

    drawDiamond(painter, rect, Qt::blue);
}

void DiamondButton::drawDiamond(QPainter &painter, const QRect &rect, const QColor &color)
{
    QPoint center(rect.center());
    
    QPainterPath path;

    path.moveTo(center.x(), rect.top());
    path.lineTo(rect.right(), center.y());
    path.lineTo(center.x(), rect.bottom());
    path.lineTo(rect.left(), center.y());

    painter.fillPath(path, QBrush(color));
}

};
