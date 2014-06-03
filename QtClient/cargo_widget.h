
#ifndef _CARGO_WIDGET_H_
#define _CARGO_WIDGET_H_

#include <QWidget>

class CargoWidget : public QWidget {
    Q_OBJECT

    Q_PROPERTY(bool changeable READ changeable WRITE setChangeable)
    Q_PROPERTY(QColor cargoColor READ cargoColor WRITE setCargoColor)
    Q_PROPERTY(int cargo READ cargo WRITE setCargo)
    Q_PROPERTY(int maxCargo READ maxCargo WRITE setMaxCargo)
    Q_PROPERTY(QString unit READ unit WRITE setUnit)

public:
    CargoWidget(QWidget *parent = 0)
        : QWidget(parent)
        , m_changeable(false)
        , m_cargoColor(Qt::black)
        , m_cargo(0)
        , m_maxCargo(0)
        , m_unit("")
    {
    }

    bool changeable() const { return m_changeable; }
    void setChangeable(bool _changeable) { m_changeable = _changeable; }

    const QColor &cargoColor() const { return m_cargoColor; }
    void setCargoColor(const QColor &_cargoColor) { m_cargoColor = _cargoColor; }

    int cargo() const { return m_cargo; }
    void setCargo(int _cargo) { m_cargo = _cargo; }

    int maxCargo() const { return m_maxCargo; }
    void setMaxCargo(int _maxCargo) { m_maxCargo = _maxCargo; }

    const QString &unit() const { return m_unit; }
    void setUnit(const QString &_unit) { m_unit = _unit; }

protected:
    void paintEvent(QPaintEvent *event);    

private:
    bool m_changeable;
    QColor m_cargoColor;
    int m_cargo, m_maxCargo;
    QString m_unit;
};

#endif
