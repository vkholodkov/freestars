/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _MINERAL_REPORT_H_
#define _MINERAL_REPORT_H_

#include <QWidget>

class MineralReport : public QWidget {
    Q_OBJECT

    Q_PROPERTY(int ironium READ ironium WRITE setIronium)
    Q_PROPERTY(int boranium READ boranium WRITE setBoranium)
    Q_PROPERTY(int germanium READ germanium WRITE setGermanium)

    Q_PROPERTY(int ironiumVelocity READ ironiumVelocity WRITE setIroniumVelocity)
    Q_PROPERTY(int boraniumVelocity READ boraniumVelocity WRITE setBoraniumVelocity)
    Q_PROPERTY(int germaniumVelocity READ germaniumVelocity WRITE setGermaniumVelocity)

    Q_PROPERTY(int ironiumConc READ ironiumConc WRITE setIroniumConc)
    Q_PROPERTY(int boraniumConc READ boraniumConc WRITE setBoraniumConc)
    Q_PROPERTY(int germaniumConc READ germaniumConc WRITE setGermaniumConc)

public:
    MineralReport(QWidget *parent = 0)
        : QWidget(parent)
        , m_ironium(-1), m_boranium(-1), m_germanium(-1)
        , m_ironiumVelocity(-1), m_boraniumVelocity(-1), m_germaniumVelocity(-1)
        , m_ironiumConc(-1), m_boraniumConc(-1), m_germaniumConc(-1)
    {
    }

    int ironium() const { return m_ironium; }
    void setIronium(int _ironium) { m_ironium = _ironium; }

    int boranium() const { return m_boranium; }
    void setBoranium(int _boranium) { m_boranium = _boranium; }

    int germanium() const { return m_germanium; }
    void setGermanium(int _germanium) { m_germanium = _germanium; }

    int ironiumVelocity() const { return m_ironiumVelocity; }
    void setIroniumVelocity(int _ironiumVelocity) { m_ironiumVelocity = _ironiumVelocity; }

    int boraniumVelocity() const { return m_boraniumVelocity; }
    void setBoraniumVelocity(int _boraniumVelocity) { m_boraniumVelocity = _boraniumVelocity; }

    int germaniumVelocity() const { return m_germaniumVelocity; }
    void setGermaniumVelocity(int _germaniumVelocity) { m_germaniumVelocity = _germaniumVelocity; }

    int ironiumConc() const { return m_ironiumConc; }
    void setIroniumConc(int _ironiumConc) { m_ironiumConc = _ironiumConc; }

    int boraniumConc() const { return m_boraniumConc; }
    void setBoraniumConc(int _boraniumConc) { m_boraniumConc = _boraniumConc; }

    int germaniumConc() const { return m_germaniumConc; }
    void setGermaniumConc(int _germaniumConc) { m_germaniumConc = _germaniumConc; }

protected:
    void paintEvent(QPaintEvent *event);    

    int m_ironium, m_boranium, m_germanium;
    int m_ironiumVelocity, m_boraniumVelocity, m_germaniumVelocity;
    int m_ironiumConc, m_boraniumConc, m_germaniumConc;

protected:
    void drawMineralBar(QPainter&, int, int, int, int, const QColor&, const QColor&) const;
    void drawConcentrationSign(QPainter&, int, int, int, const QColor&) const;
};

#endif
