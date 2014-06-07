/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <QPainter>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QMouseEvent>

#include "folding_widget.h"

namespace FreeStars {

FoldButton::FoldButton(QWidget *parent)
    : QFrame(parent)
    , pressed(false), checked(false)
    , fold(":/images/fold.png")
    , unfold(":/images/unfold.png")
{
    setFrameShape(QFrame::Panel);
    setFrameShadow(QFrame::Raised);
}

QSize FoldButton::sizeHint() const {
    QFontMetrics fm(this->font());
    const QImage &image = checked ? unfold : fold;
    return QSize(image.width(), std::max(fm.height(), image.height()));
}

void FoldButton::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);
    QImage &image = checked ? unfold : fold;

    QPainter painter(this);

    QPoint pos(contentsRect().topLeft());

    pos.rx() += contentsRect().width() / 2;
    pos.ry() += contentsRect().height() / 2;

    pos.rx() -= image.width() / 2;
    pos.ry() -= image.height() / 2;

    if(pressed) {
        pos.rx()++;
        pos.ry()++;
    }

    painter.drawImage(pos, image);
}

void FoldButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button() != Qt::LeftButton) {
        return;
    }

    if(contentsRect().contains(e->pos())) {
        if(!pressed) {
            pressed = true;
            setFrameShadow(QFrame::Sunken);
            return;
        }
    }
}

void FoldButton::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() != Qt::LeftButton) {
        return;
    }

    if(pressed) {
        pressed = false;
        setFrameShadow(QFrame::Raised);

        if(contentsRect().contains(e->pos())) {
            checked = !checked;
            emit clicked();
            return;
        }
    }
}

FoldingWidget::FoldingWidget(const QString &_title, QWidget *parent)
    : QFrame(parent)
    , folded(false)
{
    setFrameShadow(QFrame::Raised);
    setFrameShape(QFrame::Panel);

    button = new FoldButton;

    title = new QLabel;
    title->setText(_title);
    title->setMargin(5);
    title->setAlignment(Qt::AlignHCenter);
    title->setFrameShape(QFrame::Panel);
    title->setFrameShadow(QFrame::Raised);
    title->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    QFont font;
    font.setFamily(QString::fromUtf8("Arial"));
    font.setPointSize(12);
    font.setBold(true);
    font.setWeight(75);
    title->setFont(font);

    QWidget *bar = new QWidget;
    QHBoxLayout *barLayout = new QHBoxLayout;
    barLayout->setContentsMargins(QMargins());
    barLayout->setSpacing(0);
    bar->setLayout(barLayout);

    barLayout->addWidget(title);
    barLayout->addWidget(button);

    connect(button, SIGNAL(clicked()), this, SLOT(buttonPressed()));

    stackWidget = new QStackedWidget();

    layout = new QVBoxLayout();
    layout->addWidget(bar, 0, Qt::AlignTop);
    layout->addWidget(stackWidget);
    layout->setContentsMargins(QMargins());
    setLayout(layout);
}

void FoldingWidget::buttonPressed(){

    if(!folded)
    {
        folded = true;

        QSize size = layout->sizeHint();
        int width = size.width();
        int height = size.height();

        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

        stackWidget->hide();
        resize(width, 20);

        updateGeometry();
    }
    else
    {
        folded = false;

        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        stackWidget->show();

        updateGeometry();
    }

    emit foldedChanged(folded);
}

QSize FoldingWidget::sizeHint() const
{
    return QSize(200, 20);
}

void FoldingWidget::addWidget(QWidget *widget)
{
    insertWidget(count(), widget);
}

void FoldingWidget::removeWidget(int index)
{

}

int FoldingWidget::count() const
{
    return stackWidget->count();
}

int FoldingWidget::currentIndex() const
{
    return stackWidget->currentIndex();
}

void FoldingWidget::insertWidget(int index, QWidget *widget)
{
    widget->setParent(stackWidget);
    stackWidget->insertWidget(index, widget);
}

void FoldingWidget::setCurrentIndex(int index)
{
    if (index != currentIndex()) {
        stackWidget->setCurrentIndex(index);
        emit currentIndexChanged(index);
    }
}

QWidget* FoldingWidget::widget(int index)
{
    return stackWidget->widget(index);
}

void FoldingWidget::setFolderTitle(QString const &newTitle)
{
    title->setText(newTitle);
}

QString FoldingWidget::folderTitle() const
{
    return title->text();
}

void FoldingWidget::setFolded(bool flag)
{
    if(flag != folded) buttonPressed();
    else folded = flag;
}

bool FoldingWidget::isFolded() const
{
    return folded;
}

};
