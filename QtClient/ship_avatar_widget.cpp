
#include <QPainter>

#include "ship_avatar_widget.h"

void ShipAvatarWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRect rect(contentsRect());

    painter.fillRect(rect, Qt::black);

    if(graphicsArray != NULL && !m_hullName.isEmpty()) {
        const QIcon *icon = graphicsArray->GetComponentIcon(m_hullName.toStdString());

        if(icon != NULL) {
            painter.drawPixmap(rect.topLeft(), icon->pixmap(rect.size()));
        }
    }
}

void ShipAvatarWidget::setHullName(const QString &_hullName)
{
    m_hullName = _hullName;
    update(contentsRect());
}

