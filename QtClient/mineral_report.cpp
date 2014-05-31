
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

    drawMineralBar(painter, m_ironium, 0, rect.height() / 3 - 2, Qt::blue);
    drawMineralBar(painter, m_germanium, rect.y() + rect.height() / 3 + 3, 2 * rect.height() / 3 - 2, Qt::red);
    drawMineralBar(painter, m_boranium, 2 * rect.height() / 3 + 3, rect.height() - 3, Qt::green);

    drawConcentrationSign(painter, m_ironiumConc, 0, rect.height() / 3 - 2, Qt::darkBlue);
    drawConcentrationSign(painter, m_germaniumConc, rect.y() + rect.height() / 3 + 3, 2 * rect.height() / 3 - 2, Qt::darkRed);
    drawConcentrationSign(painter, m_boraniumConc, 2 * rect.height() / 3 + 3, rect.height() - 3, Qt::darkGreen);
}

void MineralReport::drawMineralBar(QPainter &painter, unsigned value, int top, int bottom, const QColor &color) const {
    int width = value * contentsRect().width() / 5000;
    painter.fillRect(0, top, width, bottom - top, color);
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
