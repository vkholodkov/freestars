
#ifndef _CARGO_WIDGET_H_
#define _CARGO_WIDGET_H_

#include <QMouseEvent>
#include <QWidget>

class CargoWidget : public QWidget {
    Q_OBJECT

    Q_PROPERTY(bool changeable READ changeable WRITE setChangeable)
    Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly)
    Q_PROPERTY(QColor cargoColor READ cargoColor WRITE setCargoColor)
    Q_PROPERTY(int currentCargo READ currentCargo WRITE setCurrentCargo)
    Q_PROPERTY(int newCargo READ newCargo WRITE setNewCargo)
    Q_PROPERTY(int maxAvailableCargo READ maxAvailableCargo WRITE setMaxAvailableCargo)
    Q_PROPERTY(int maxCargo READ maxCargo WRITE setMaxCargo)
    Q_PROPERTY(QString unit READ unit WRITE setUnit)

signals:
    void clicked();
    void changed(int, int);

public:
    CargoWidget(QWidget *parent = 0);

    bool changeable() const { return m_changeable; }
    void setChangeable(bool _changeable) { m_changeable = _changeable; }

    bool readOnly() const { return m_readOnly; }
    void setReadOnly(bool _readOnly) { m_readOnly = _readOnly; }

    const QColor &cargoColor() const { return m_cargoColor; }
    void setCargoColor(const QColor &_cargoColor) { m_cargoColor = _cargoColor; }

    int currentCargo() const { return m_currentCargo; }
    void setCurrentCargo(int _currentCargo) { m_currentCargo = m_newCargo = _currentCargo; update(contentsRect()); }

    int newCargo() const { return m_newCargo; }
    void setNewCargo(int _newCargo) { m_newCargo = _newCargo; update(contentsRect()); }

    int maxCargo() const { return m_maxCargo; }
    void setMaxCargo(int _maxCargo) { m_maxCargo = _maxCargo; update(contentsRect()); }

    int maxAvailableCargo() const { return m_maxAvailableCargo; }
    void setMaxAvailableCargo(int _maxAvailableCargo) {

      if(_maxAvailableCargo < m_newCargo) {
        if(_maxAvailableCargo >= m_currentCargo) {
            m_newCargo = _maxAvailableCargo;
        }
      }

      m_maxAvailableCargo = _maxAvailableCargo;

      update(contentsRect());
    }

    const QString &unit() const { return m_unit; }
    void setUnit(const QString &_unit) { m_unit = _unit; }

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);    
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);

private:
    bool m_changeable, m_readOnly;
    QColor m_cargoColor;
    int m_currentCargo, m_newCargo, m_maxAvailableCargo, m_maxCargo;
    QString m_unit;
    bool m_mousein;
    bool m_changed;
};

#endif
