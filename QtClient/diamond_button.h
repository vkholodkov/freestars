/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _DIAMOND_BUTTON_H_
#define _DIAMOND_BUTTON_H_

#include <QAbstractButton>

namespace FreeStars {

class DiamondButton : public QAbstractButton {
public:
    DiamondButton(QWidget *parent = 0);

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent*);

private:
    static void drawDiamond(QPainter&, const QRect&, const QColor&);

    bool pressed;
};

};

#endif
