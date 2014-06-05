/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef VERTICALFLOWLAYOUT_H
#define VERTICALFLOWLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QWidgetItem>

class VerticalFlowLayout : public QLayout
{
public:
    VerticalFlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    VerticalFlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
    ~VerticalFlowLayout();

    void addItem(QLayoutItem*);

    int horizontalSpacing() const;
    int verticalSpacing() const;
    Qt::Orientations expandingDirections() const;
    int count() const;
    QLayoutItem *itemAt(int index) const;
    QSize minimumSize() const;
    void setGeometry(const QRect &rect);
    QSize sizeHint() const;
    QLayoutItem *takeAt(int index);

private:
    void doLayout(const QRect &rect, bool testOnly) const;
    void doLayoutForColumn(const QRect&, bool, const QString&) const;
    int smartSpacing(QStyle::PixelMetric pm) const;

    QList<QLayoutItem *> itemList;
    int m_hSpace;
    int m_vSpace;
};

#endif
