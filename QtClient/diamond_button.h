/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _DIAMOND_BUTTON_H_
#define _DIAMOND_BUTTON_H_

#include <QAbstractButton>

namespace FreeStars {

class DiamondButton : public QWidget {
    Q_OBJECT
public:
    DiamondButton(QWidget *parent = 0);

    QSize sizeHint() const;

signals:
    void clicked();
    void help();

protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

private:
    static void drawDiamond(QPainter&, const QRect&, const QColor&);

    bool m_mousein;
};

};

#endif
