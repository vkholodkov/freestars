
#include <QPainter>

#include "mineral_report.h"

void MineralReport::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRect rect(contentsRect());

    painter.fillRect(rect, Qt::black);

    painter.setPen(Qt::white);

    QFontMetrics fm(this->font());

    int labelSpacing = fm.width(QString("5000")) * 1.5;

    painter.setPen(Qt::white);

    for(int x = labelSpacing ; x <= rect.width() ; x += labelSpacing) {
        painter.drawLine(x, 0, x, rect.height());
    }

    drawMineralBar(painter, m_ironium, m_ironiumVelocity, 0, rect.height() / 3 - 2, Qt::blue, Qt::darkBlue);
    drawMineralBar(painter, m_boranium, m_boraniumVelocity, rect.y() + rect.height() / 3 + 3, 2 * rect.height() / 3 - 2, Qt::green, Qt::darkGreen);
    drawMineralBar(painter, m_germanium, m_germaniumVelocity, 2 * rect.height() / 3 + 3, rect.height() - 3, Qt::yellow, Qt::darkYellow);

    drawConcentrationSign(painter, m_ironiumConc, 0, rect.height() / 3 - 2, Qt::darkBlue);
    drawConcentrationSign(painter, m_boraniumConc, rect.y() + rect.height() / 3 + 3, 2 * rect.height() / 3 - 2, Qt::darkGreen);
    drawConcentrationSign(painter, m_germaniumConc, 2 * rect.height() / 3 + 3, rect.height() - 3, Qt::darkYellow);
}

void MineralReport::drawMineralBar(QPainter &painter, unsigned value, unsigned velocity, int top, int bottom, const QColor &color, const QColor &velColor) const {
    int barWidth = value * contentsRect().width() / 5000;

    if(barWidth != 0) {
        painter.fillRect(0, top, barWidth, bottom - top, color);
    }

    int velocityBarWidth = velocity * contentsRect().width() / 5000;

    if(velocityBarWidth != 0) {
        painter.fillRect(barWidth, top, barWidth + velocityBarWidth, bottom - top, velColor);
    }
}

void MineralReport::drawConcentrationSign(QPainter &painter, unsigned value, int top, int bottom, const QColor &color) const {
    int width = (bottom - top) / 2 - 3;
    QRect rect(contentsRect());

    rect.adjust(width / 2, 0, width / 2, 0);

    int x = width + value * rect.width() / 120;

    QPainterPath path;

    path.moveTo(x, top);
    path.lineTo(x + width, (bottom + top) / 2);
    path.lineTo(x, bottom);
    path.lineTo(x - width, (bottom + top) / 2);
    path.lineTo(x, top);

    painter.fillPath(path, QBrush(color));
}
