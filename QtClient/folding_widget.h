
#ifndef _FOLDING_WIDGET_H
#define _FOLDING_WIDGET_H

#include <QWidget>
#include <QFrame>

class QPushButton;
class QStackedWidget;
class QVBoxLayout;

class FoldingWidget : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QString folderTitle READ folderTitle WRITE setFolderTitle STORED true)
    Q_PROPERTY(bool isFolded READ isFolded WRITE setFolded STORED true)

public:
    FoldingWidget(const QString&, QWidget *parent = 0);

    QSize sizeHint() const;

    int count() const;
    int currentIndex() const;
    QWidget *widget(int index);

    bool folded;
    QString folderTitle() const;
    bool isFolded() const;

public slots:
    void buttonPressed();
    void setFolderTitle(const QString&);
    void setFolded(bool);

    void addWidget(QWidget*);
    void insertWidget(int, QWidget*);
    void removeWidget(int);
    void setCurrentIndex(int);

signals:
    void currentIndexChanged(int);
    void foldedChanged(bool);

private:
    QPushButton *button;
    QStackedWidget *stackWidget;
    QVBoxLayout *layout;
};

#endif
