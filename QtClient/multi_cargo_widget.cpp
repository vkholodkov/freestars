
#include <QPainter>

#include "multi_cargo_widget.h"

namespace FreeStars {

MultiCargoWidget::MultiCargoWidget(QWidget *parent)
    : QWidget(parent)
    , m_unit("")
    , m_mousein(false)
    , m_cargoHolder(NULL)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

int MultiCargoWidget::getCargo() const {
  if(m_cargoHolder != NULL) {
    auto ironium = m_cargoHolder->GetContain(0);
    auto boranium = m_cargoHolder->GetContain(1);
    auto germanium = m_cargoHolder->GetContain(2);
    auto colonists = m_cargoHolder->GetContain(POPULATION) / Rules::PopEQ1kT;
    return ironium + boranium + germanium + colonists;
  }
  else {
    return 0;
  }
}

QSize MultiCargoWidget::sizeHint() const
{
    QFontMetrics fm(this->font());

    QString text(tr("%0 of %2%3")
        .arg(this->getCargo())
        .arg(m_cargoHolder != NULL ? m_cargoHolder->GetCargoCapacity() : 0)
        .arg(m_unit));

    auto s = fm.size(Qt::TextSingleLine, text);

    return QSize(s.width(), s.height() + 2);
}

void MultiCargoWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRect rect(contentsRect());
    rect.adjust(0, 0, -1, -1);

    if(m_cargoHolder != NULL) {
      auto maxCargo = m_cargoHolder->GetCargoCapacity();

      if(maxCargo > 0) {

        auto ironium = m_cargoHolder->GetContain(0);
        auto boranium = m_cargoHolder->GetContain(1);
        auto germanium = m_cargoHolder->GetContain(2);
        auto colonists = m_cargoHolder->GetContain(POPULATION) / Rules::PopEQ1kT;

        QRect ri(rect);

        ri.setWidth(rect.width() * ironium / maxCargo);
     
        painter.fillRect(ri, Qt::blue);

        QRect rb(rect);

        rb.setLeft(ri.bottomRight().x());
        rb.setWidth(rect.width() * boranium / maxCargo);
     
        painter.fillRect(ri, Qt::green);

        QRect rg(rect);

        rg.setLeft(rb.bottomRight().x());
        rg.setWidth(rect.width() * germanium / maxCargo);
     
        painter.fillRect(ri, Qt::yellow);

        QRect rc(rect);

        rc.setLeft(rg.bottomRight().x());
        rc.setWidth(rect.width() * colonists / maxCargo);
     
        painter.fillRect(ri, Qt::yellow);
      }
    }

    painter.setPen(Qt::black);
    painter.drawRect(rect);

    QFontMetrics fm(this->font());

    QString text(
        tr("%0 of %2%3")
        .arg(this->getCargo())
        .arg(m_cargoHolder != NULL ? m_cargoHolder->GetCargoCapacity() : 0)
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

void MultiCargoWidget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
      m_mousein = true;
    }

    QWidget::mousePressEvent(e);
}

void MultiCargoWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
      m_mousein = false;
      
      if(contentsRect().contains(e->pos())) {
        emit clicked();
      }
    }

    QWidget::mouseReleaseEvent(e);
}

};
