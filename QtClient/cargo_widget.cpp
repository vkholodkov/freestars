
#include <QPainter>

#include "cargo_widget.h"

CargoWidget::CargoWidget(QWidget *parent)
    : QWidget(parent)
    , m_changeable(false)
    , m_cargoColor(Qt::black)
    , m_cargo(0)
    , m_maxCargo(0)
    , m_unit("")
    , m_mousein(false)
    , m_changed(false)
    , m_readOnly(false)
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

    auto s = fm.size(Qt::TextSingleLine, text);

    return QSize(s.width(), s.height() + 2);
}

void CargoWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRect rect(contentsRect());
    rect.adjust(0, 0, -1, -1);

    if(!m_readOnly) {

      if(m_maxCargo > 0) {

        QRect r(rect);

        r.setWidth(rect.width() * m_cargo / m_maxCargo);
     
        painter.fillRect(r, m_cargoColor);
        painter.setPen(Qt::black);
      }

      painter.drawRect(rect);
    }

    QFontMetrics fm(this->font());

    QString text(m_readOnly ?
        tr("%0%3")
        .arg(m_cargo)
        .arg(m_unit)
        :
        tr("%0 of %2%3")
        .arg(m_cargo)
        .arg(m_maxCargo)
        .arg(m_unit)
    );

    auto s = fm.size(Qt::TextSingleLine, text);
    int width = s.width();
    int height = s.height();

    QPoint baseline(rect.center());
    baseline.rx() -= width / 2;
    baseline.setY(rect.bottom() - 2);
    painter.drawText(baseline, text);
}

void CargoWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
      m_mousein = true;

      if(m_changeable) {
        if(contentsRect().contains(e->pos())) {
          setCargo(e->pos().x() * m_maxCargo / contentsRect().width());
          m_changed = true;
        }
      }
    }

    QWidget::mousePressEvent(e);
}

void CargoWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
      m_mousein = false;
      
      if(!m_changeable) {
        if(contentsRect().contains(e->pos())) {
          emit clicked();
        }
      }
      else {
        if(m_changed) {
          m_changed = false;
          emit changed();
        }
      }
    }

    QWidget::mouseReleaseEvent(e);
}

void CargoWidget::mouseMoveEvent(QMouseEvent *e)
{
  if(m_mousein) {
    if(m_changeable) {
      if(e->pos().x() >= contentsRect().topLeft().x() && e->pos().x() < contentsRect().bottomRight().x()) {
        setCargo(e->pos().x() * m_maxCargo / contentsRect().width());
      }
      else if(e->pos().x() >= contentsRect().bottomRight().x()) {
        setCargo(m_maxCargo);
      }
      m_changed = true;
    }
  }
}
