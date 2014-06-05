/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _SCALE_WIDGET_H_
#define _SCALE_WIDGET_H_

#include <QWidget>

class ScaleWidget : public QWidget {
    Q_OBJECT

public:
    ScaleWidget(QWidget *parent = 0)
        : QWidget(parent)
    {
    }

public:
    void setMineralReport(const QWidget *_mineralReport) { mineralReport = _mineralReport; }
    virtual QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);    

private:
    const QWidget *mineralReport;
};

#endif
