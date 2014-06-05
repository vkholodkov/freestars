/*
 * Copyright (C) 2014 Valery Kholodkov
 */
#include <iostream>

#include <QtGui>

#include "vertical_flow_layout.h"

VerticalFlowLayout::VerticalFlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing)
    : QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing)
{
    setContentsMargins(margin, margin, margin, margin);
}

VerticalFlowLayout::VerticalFlowLayout(int margin, int hSpacing, int vSpacing)
    : m_hSpace(hSpacing), m_vSpace(vSpacing)
{
    setContentsMargins(margin, margin, margin, margin);
}

VerticalFlowLayout::~VerticalFlowLayout()
{
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}

void VerticalFlowLayout::addItem(QLayoutItem *item)
{
    itemList.append(item);
}

int VerticalFlowLayout::horizontalSpacing() const
{
    if (m_hSpace >= 0) {
        return m_hSpace;
    } else {
        return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
    }
}

int VerticalFlowLayout::verticalSpacing() const
{
    if (m_vSpace >= 0) {
        return m_vSpace;
    } else {
        return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
    }
}

int VerticalFlowLayout::count() const
{
    return itemList.size();
}

QLayoutItem *VerticalFlowLayout::itemAt(int index) const
{
    return itemList.value(index);
}

QLayoutItem *VerticalFlowLayout::takeAt(int index)
{
    if (index >= 0 && index < itemList.size())
        return itemList.takeAt(index);
    else
        return 0;
}

Qt::Orientations VerticalFlowLayout::expandingDirections() const
{
    return 0;
}

void VerticalFlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize VerticalFlowLayout::sizeHint() const
{
    return minimumSize();
}

QSize VerticalFlowLayout::minimumSize() const
{
    QSize size;
    QLayoutItem *item;
    foreach (item, itemList)
        size = size.expandedTo(item->minimumSize());

    size += QSize(2*margin(), 2*margin());
    return size;
}

void VerticalFlowLayout::doLayout(const QRect &rect, bool testOnly) const
{
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);

    QRect column(rect);

    column.setWidth(rect.width() / 2);

    doLayoutForColumn(column.adjusted(+left, +top, -right, -bottom),
        testOnly, QString("_column1"));

    column.moveLeft(rect.width() / 2);

    doLayoutForColumn(column.adjusted(+left, +top, -right, -bottom),
        testOnly, QString("_column2"));
}

void VerticalFlowLayout::doLayoutForColumn(const QRect &effectiveRect, bool testOnly, const QString &suffix) const {
    int x = effectiveRect.x();
    int y = effectiveRect.y();

    QLayoutItem *item;
    foreach (item, itemList) {
        QWidget *wid = item->widget();

        if(wid->objectName().right(suffix.length()) == suffix) {
            int spaceX = horizontalSpacing();
            if (spaceX == -1)
                spaceX = wid->style()->layoutSpacing(
                    QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
            int spaceY = verticalSpacing();
            if (spaceY == -1)
                spaceY = wid->style()->layoutSpacing(
                    QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);
            int nextY = y + item->sizeHint().height() + spaceY;

            if (!testOnly) {
                QRect geometry(QPoint(x, y), QSize(effectiveRect.width(), item->sizeHint().height()));
                item->setGeometry(geometry);
            }

            y = nextY;
        }
    }
}

int VerticalFlowLayout::smartSpacing(QStyle::PixelMetric pm) const
{
    QObject *parent = this->parent();
    if (!parent) {
        return -1;
    } else if (parent->isWidgetType()) {
        QWidget *pw = static_cast<QWidget *>(parent);
        return pw->style()->pixelMetric(pm, 0, pw);
    } else {
        return static_cast<QLayout *>(parent)->spacing();
    }
}
