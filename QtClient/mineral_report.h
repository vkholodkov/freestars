/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _MINERAL_REPORT_H_
#define _MINERAL_REPORT_H_

#include <QWidget>

class MineralReport : public QWidget {
    Q_OBJECT

    Q_PROPERTY(unsigned ironium READ ironium WRITE setIronium)
    Q_PROPERTY(unsigned boranium READ boranium WRITE setBoranium)
    Q_PROPERTY(unsigned germanium READ germanium WRITE setGermanium)

    Q_PROPERTY(unsigned ironiumVelocity READ ironiumVelocity WRITE setIroniumVelocity)
    Q_PROPERTY(unsigned boraniumVelocity READ boraniumVelocity WRITE setBoraniumVelocity)
    Q_PROPERTY(unsigned germaniumVelocity READ germaniumVelocity WRITE setGermaniumVelocity)

    Q_PROPERTY(unsigned ironiumConc READ ironiumConc WRITE setIroniumConc)
    Q_PROPERTY(unsigned boraniumConc READ boraniumConc WRITE setBoraniumConc)
    Q_PROPERTY(unsigned germaniumConc READ germaniumConc WRITE setGermaniumConc)

public:
    MineralReport(QWidget *parent = 0)
        : QWidget(parent)
        , m_ironium(0), m_boranium(0), m_germanium(0)
        , m_ironiumVelocity(0), m_boraniumVelocity(0), m_germaniumVelocity(0)
        , m_ironiumConc(0), m_boraniumConc(0), m_germaniumConc(0)
    {
    }

    unsigned ironium() const { return m_ironium; }
    void setIronium(unsigned _ironium) { m_ironium = _ironium; }

    unsigned boranium() const { return m_boranium; }
    void setBoranium(unsigned _boranium) { m_boranium = _boranium; }

    unsigned germanium() const { return m_germanium; }
    void setGermanium(unsigned _germanium) { m_germanium = _germanium; }

    unsigned ironiumVelocity() const { return m_ironiumVelocity; }
    void setIroniumVelocity(unsigned _ironiumVelocity) { m_ironiumVelocity = _ironiumVelocity; }

    unsigned boraniumVelocity() const { return m_boraniumVelocity; }
    void setBoraniumVelocity(unsigned _boraniumVelocity) { m_boraniumVelocity = _boraniumVelocity; }

    unsigned germaniumVelocity() const { return m_germaniumVelocity; }
    void setGermaniumVelocity(unsigned _germaniumVelocity) { m_germaniumVelocity = _germaniumVelocity; }

    unsigned ironiumConc() const { return m_ironiumConc; }
    void setIroniumConc(unsigned _ironiumConc) { m_ironiumConc = _ironiumConc; }

    unsigned boraniumConc() const { return m_boraniumConc; }
    void setBoraniumConc(unsigned _boraniumConc) { m_boraniumConc = _boraniumConc; }

    unsigned germaniumConc() const { return m_germaniumConc; }
    void setGermaniumConc(unsigned _germaniumConc) { m_germaniumConc = _germaniumConc; }

protected:
    void paintEvent(QPaintEvent *event);    

    unsigned m_ironium, m_boranium, m_germanium;
    unsigned m_ironiumVelocity, m_boraniumVelocity, m_germaniumVelocity;
    unsigned m_ironiumConc, m_boraniumConc, m_germaniumConc;

protected:
    void drawMineralBar(QPainter&, unsigned, unsigned, int, int, const QColor&, const QColor&) const;
    void drawConcentrationSign(QPainter&, unsigned, int, int, const QColor&) const;
};

#endif
