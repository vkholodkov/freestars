
#include <QPainter>

#include "cargo_widget.h"

CargoWidget::CargoWidget(QWidget *parent)
    : QWidget(parent)
    , m_changeable(false)
    , m_cargoColor(Qt::black)
    , m_cargo(0)
    , m_maxCargo(0)
    , m_unit("")
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

QSize CargoWidget::sizeHint() const
{
    QFontMetrics fm(this->font());

    QString text(tr("%0 of %2%3")
        .arg(m_cargo)
        .arg(m_maxCargo)
        .arg(m_unit));

    return QSize(fm.width(text), fm.height() + 2);
}

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
