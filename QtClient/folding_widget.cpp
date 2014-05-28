
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLabel>

#include "folding_widget.h"

FoldingWidget::FoldingWidget(const QString &_title, QWidget *parent)
    : QFrame(parent)
    , folded(false)
{
    setFrameShadow(QFrame::Raised);
    setFrameShape(QFrame::Panel);

    button = new QPushButton();
    button->setObjectName("__qt__passive_button");
    button->setIcon(QIcon(":/arrow-expanded.png"));
    button->setFlat(true);
    button->setStyleSheet("text-align: left; font-weight: bold; border: none;");
    button->setMaximumSize(QSize(40, 40));

    QLabel *title = new QLabel;
    title->setText(_title);
    title->setMargin(5);
    title->setAlignment(Qt::AlignHCenter);
    title->setFrameShape(QFrame::Panel);
    title->setFrameShadow(QFrame::Raised);

    QFont font;
    font.setFamily(QString::fromUtf8("Arial"));
    font.setPointSize(12);
    font.setBold(true);
    font.setWeight(75);
    title->setFont(font);

    QWidget *bar = new QWidget;
    QHBoxLayout *barLayout = new QHBoxLayout;
    barLayout->setContentsMargins(QMargins());
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
        button->setIcon(QIcon(":/arrow.png"));

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
        button->setIcon(QIcon(":/arrow-expanded.png"));

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
    button->setText(newTitle);
}

QString FoldingWidget::folderTitle() const
{
    return button->text();
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
