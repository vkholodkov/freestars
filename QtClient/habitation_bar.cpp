/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <QPainter>
#include <QMouseEvent>
#include <QToolTip>

#include "habitation_bar.h"

void HabitationBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRect rect(contentsRect());

    painter.fillRect(rect, Qt::black);
    painter.drawLine(rect.x(), rect.y() + rect.height() / 3, rect.right(), rect.y() + rect.height() / 3);
    painter.drawLine(rect.x(), rect.y() + 2 * rect.height() / 3, rect.right(), rect.y() + 2 * rect.height() / 3);

    drawHabitationBar(painter, m_gravityRange, 0, rect.height() / 3 - 1, Qt::darkBlue);
    drawHabitationBar(painter, m_tempRange, rect.y() + rect.height() / 3 + 2, 2 * rect.height() / 3 - 1, Qt::darkRed);
    drawHabitationBar(painter, m_radRange, 2 * rect.height() / 3 + 2, rect.height() - 1, Qt::darkGreen);

    drawHabitationSign(painter, m_gravityValue, 0, rect.height() / 3 - 1, Qt::blue);
    drawHabitationSign(painter, m_tempValue, rect.y() + rect.height() / 3 + 2, 2 * rect.height() / 3 - 2, Qt::red);
    drawHabitationSign(painter, m_radValue, 2 * rect.height() / 3 + 2, rect.height() - 2, Qt::green);
}

void HabitationBar::drawHabitationBar(QPainter &painter, const HabRange &range, int top, int bottom, const QColor &color) const {
    QRect rect((range.center - range.width / 2) * contentsRect().width() / 100, top,
        range.width * contentsRect().width() / 100, bottom - top);

    painter.fillRect(rect, color);
}

void HabitationBar::drawHabitationSign(QPainter &painter, int value, int top, int bottom, const QColor &color) const {
    if(value < 0) {
        return;
    }

    int x = value * contentsRect().width() / 100;
    int y = (top + bottom) / 2;
    int width = (bottom - top) / 2 - 3;

    painter.setPen(color);
    painter.drawLine(x - width, y, x + width, y);
    painter.drawLine(x, y - width, x, y + width);
}

void HabitationBar::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        QRect rect(contentsRect());
        QRect gravityRect(rect.x(), rect.y(), rect.width(), rect.height() / 3);
        QRect tempRect(rect.x(), rect.y() + rect.height() / 3, rect.width(), rect.height() / 3);
        QRect radRect(rect.x(), rect.y() + 2 * rect.height() / 3, rect.width(), rect.height() / 3);

        if(gravityRect.contains(event->pos())) {
            QString text("Gravity");
            QToolTip::showText(event->globalPos(), text);
        }

        if(tempRect.contains(event->pos())) {
            QString text("Temperature");
            QToolTip::showText(event->globalPos(), text);
        }

        if(radRect.contains(event->pos())) {
            QString text("Radiation");
            QToolTip::showText(event->globalPos(), text);
        }
    }
}

void HabitationBar::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        QToolTip::hideText();
    }
}
