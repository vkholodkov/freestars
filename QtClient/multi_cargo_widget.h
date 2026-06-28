
#ifndef _MULTI_CARGO_WIDGET_H_
#define _MULTI_CARGO_WIDGET_H_

#include <QMouseEvent>
#include <QWidget>

#include "FSServer.h"

namespace FreeStars {

class MultiCargoWidget : public QWidget {
    Q_OBJECT

    Q_PROPERTY(const CargoHolder* cargoHolder READ cargoHolder WRITE setCargoHolder)
    Q_PROPERTY(QString unit READ unit WRITE setUnit)

signals:
    void clicked();

public:
    MultiCargoWidget(QWidget *parent = 0);

    const CargoHolder *cargoHolder() const { return m_cargoHolder; }
    void setCargoHolder(const CargoHolder* _cargoHolder) { m_cargoHolder = _cargoHolder; update(contentsRect()); }

    const QString &unit() const { return m_unit; }
    void setUnit(const QString &_unit) { m_unit = _unit; }

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);    
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

private:
    int getCargo() const;

    const CargoHolder *m_cargoHolder;
    QString m_unit;
    bool m_mousein;
};

};

#endif
