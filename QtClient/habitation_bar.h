/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _HABITATION_BAR_H_
#define _HABITATION_BAR_H_

#include <QFrame>

class HabRange {
public:
    HabRange()
        : center(50)
        , width(1)
    {
    }

    HabRange(unsigned _center, unsigned _width)
        : center(_center)
        , width(_width)
    {
    }

    HabRange(const HabRange &_other)
        : center(_other.center)
        , width(_other.width)
    {
    }

    HabRange &operator=(const HabRange &_other) {
        this->center = _other.center;
        this->width = _other.width;
        return *this;
    }

    unsigned center, width;
};

class HabitationBar : public QFrame {
    Q_OBJECT

    Q_PROPERTY(HabRange gravityRange READ gravityRange WRITE setGravityRange)
    Q_PROPERTY(HabRange tempRange READ tempRange WRITE setTempRange)
    Q_PROPERTY(HabRange radRange READ radRange WRITE setRadRange)

    Q_PROPERTY(unsigned gravityValue READ gravityValue WRITE setGravityValue)
    Q_PROPERTY(unsigned tempValue READ tempValue WRITE setTempValue)
    Q_PROPERTY(unsigned radValue READ radValue WRITE setRadValue)

public:
    HabitationBar(QWidget *parent = 0)
        : QFrame(parent)
    {
    }

    HabRange gravityRange() const { return m_gravityRange; }
    void setGravityRange(const HabRange &_gravityRange) { m_gravityRange = _gravityRange; }

    HabRange tempRange() const { return m_tempRange; }
    void setTempRange(const HabRange &_tempRange) { m_tempRange = _tempRange; }

    HabRange radRange() const { return m_radRange; }
    void setRadRange(const HabRange &_radRange) { m_radRange = _radRange; }

    unsigned gravityValue() const { return m_gravityValue; }
    void setGravityValue(unsigned _gravityValue) { m_gravityValue = _gravityValue; }

    unsigned tempValue() const { return m_tempValue; }
    void setTempValue(unsigned _tempValue) { m_tempValue = _tempValue; }

    unsigned radValue() const { return m_radValue; }
    void setRadValue(unsigned _radValue) { m_radValue = _radValue; }

protected:
    void paintEvent(QPaintEvent *event);    
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

    HabRange m_gravityRange, m_tempRange, m_radRange;
    unsigned m_gravityValue, m_tempValue, m_radValue;

protected:
    void drawHabitationBar(QPainter&, const HabRange&, int, int, const QColor&) const;
    void drawHabitationSign(QPainter&, unsigned, int, int, const QColor&) const;
};

#endif
