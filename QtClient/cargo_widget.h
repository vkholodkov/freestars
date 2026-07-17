
#ifndef _CARGO_WIDGET_H_
#define _CARGO_WIDGET_H_

#include <QMouseEvent>
#include <QWidget>

class CargoWidget : public QWidget {
    Q_OBJECT

    Q_PROPERTY(bool changeable READ changeable WRITE setChangeable)
    Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly)
    Q_PROPERTY(bool showMaxCargo READ showMaxCargo WRITE setShowMaxCargo)
    Q_PROPERTY(QColor cargoColor READ cargoColor WRITE setCargoColor)
    Q_PROPERTY(long cargo READ cargo WRITE setCargo)
    Q_PROPERTY(long maxCargo READ maxCargo WRITE setMaxCargo)
    Q_PROPERTY(QString unit READ unit WRITE setUnit)

signals:
    void clicked();
    void changed(long);

public:
    CargoWidget(QWidget *parent = 0);

    bool changeable() const { return m_changeable; }
    void setChangeable(bool _changeable) { m_changeable = _changeable; }

    bool readOnly() const { return m_readOnly; }
    void setReadOnly(bool _readOnly) { m_readOnly = _readOnly; }

    bool showMaxCargo() const { return m_showMaxCargo; }
    void setShowMaxCargo(bool _showMaxCargo) { m_showMaxCargo = _showMaxCargo; }

    const QColor &cargoColor() const { return m_cargoColor; }
    void setCargoColor(const QColor &_cargoColor) { m_cargoColor = _cargoColor; }

    int cargo() const { return m_cargo; }
    void setCargo(long _cargo) { if(m_cargo != _cargo) { m_cargo = _cargo; update(contentsRect()); } }

    int maxCargo() const { return m_maxCargo; }
    void setMaxCargo(long _maxCargo) { m_maxCargo = _maxCargo; update(contentsRect()); }

    const QString &unit() const { return m_unit; }
    void setUnit(const QString &_unit) { m_unit = _unit; }

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);    
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);

private:
    bool m_changeable, m_readOnly, m_showMaxCargo;
    QColor m_cargoColor;
    long m_cargo, m_maxCargo;
    QString m_unit;
    bool m_mousein;
};

#endif
