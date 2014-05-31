
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

    int height = fm.height();

    painter.fillRect(rect, m_cargoColor);
    painter.setPen(Qt::black);
    painter.drawRect(contentsRect());

    QPoint baseline(rect.center());
    baseline.setY(rect.bottom() - 3);
    painter.drawText(baseline, text);
}
