
#include <QPainter>

#include "scale_widget.h"

QSize ScaleWidget::sizeHint() const {
    QFontMetrics fm(this->font());
    return fm.size(Qt::TextSingleLine , QString("kT 0 5000"));
}

void ScaleWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QFontMetrics fm(this->font());
    QRect rect(contentsRect());
    QString kt("kT");

    painter.setPen(Qt::black);

    painter.drawText(QPoint(mineralReport->geometry().x() - fm.width(kt) - 10, rect.bottom()), kt);

    int labelSpacing = fm.width(QString("5000")) * 1.5;

    int numLabels = mineralReport->geometry().width() / labelSpacing;
    int value = 0, increment = 5000 / numLabels;

    for(int x = mineralReport->geometry().x() ; x <= mineralReport->geometry().right() ; x += labelSpacing, value += increment) {
        QString label(QString::number(value));
        int labelWidth = fm.width(label);
        painter.drawText(QPoint(x - labelWidth / 2, rect.bottom()), label);
    }
}
