
#include <QPainter>

#include "cargo_widget.h"

void CargoWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRect rect(contentsRect());

    QFontMetrics fm(this->font());

    QString text(tr("%0 of %2%3")
        .arg(m_cargo)
        .arg(m_maxCargo)
        .arg(m_unit));

    int width = fm.width(text);
    int height = fm.height();

    rect.adjust(0, 0, -1, -1);
 
    painter.fillRect(rect, m_cargoColor);
    painter.setPen(Qt::black);
    painter.drawRect(rect);

    QPoint baseline(rect.center());
    baseline.rx() -= width / 2;
    baseline.setY(rect.bottom() - 2);
    painter.drawText(baseline, text);
}
