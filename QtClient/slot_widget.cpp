
#include <QUrl>
#include <QApplication>

#include "slot_widget.h"

namespace FreeStars {

struct CompCategory {
    const char *title;
    long mask;
};

static CompCategory slotCategories[] = {
    { "Armor", CT_ARMOR },
    { "Shield", CT_SHIELD },
    { "Weapon", CT_WEAPON },
    { "Bomb", CT_BOMB },
    { "Elect", CT_ELEC },
    { "Engine", CT_ENGINE },
    { "Mine layer", CT_MINELAY },
    { "Mining", CT_MINER },
    { "Scanner", CT_SCANNER },
    { "Mech", CT_MECH },
    { "Orbital", CT_ORBITAL },
    { NULL, 0 }
};

void HullSlotWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QFont bold(font());
    bold.setBold(true);
    QFont smaller(font());
    smaller.setPointSize(font().pointSize() - 1);
    QFontMetrics fm(bold);

    painter.fillRect(contentsRect(), QBrush(Qt::gray));
    painter.setPen(Qt::black);
    painter.drawRect(contentsRect());

    QPoint base(contentsRect().center().x(), contentsRect().bottom() - 3);

    QString description(describe());
    QString text(tr(hullSlot.IsAllowed(CT_ENGINE) ? "needs %0" : "up to %0").arg(hullSlot.GetCount()));

    int width = fm.width(text);

    base.rx() -= (width / 2);

    painter.setFont(smaller);
    painter.drawText(contentsRect().adjusted(0, 2, 0, -fm.height()),
        Qt::TextWordWrap | Qt::AlignCenter | Qt::AlignBottom, description);
    painter.setFont(bold);
    painter.drawText(base, text);
}

QString HullSlotWidget::describe() const
{
    QStringList typeList;
    struct CompCategory *category = slotCategories;
    int count = 0;

    while (category->title != NULL) {
        if(hullSlot.IsAllowed(category->mask)) {
            typeList << tr(category->title);
            count++;
        }

        category++;
    }

    return count == 1 ? typeList.at(0) : 
        count == 2 ? typeList.join("\nor\n") :
        count == 3 ? typeList.join("\n") :
        tr("General\nPurpose");
}

void SlotWidget::paintEvent(QPaintEvent *event)
{
    if (shipSlot.GetComp() == NULL || shipSlot.GetCount() == 0) {
        HullSlotWidget::paintEvent(event);
        return;
    }

    QPainter painter(this);

    drawComponent(painter, contentsRect());
}

void SlotWidget::drawComponent(QPainter &painter, const QRect &rect)
{
    QFont bold(font());
    bold.setBold(true);
    QFontMetrics fm(bold);

    painter.drawImage(rect.topLeft(), plateImage);

    const Component *comp = shipSlot.GetComp();
    const QIcon *icon = graphicsArray->GetGraphics(comp->GetName());

    if(icon != NULL) {
        painter.drawPixmap(rect.topLeft() + QPoint(6, 0), icon->pixmap(52, 52));
    }

    painter.setFont(font());

    QPoint base(rect.center().x(), rect.bottom() - 3);

    QString text(tr("%0 of %1")
        .arg(shipSlot.GetCount())
        .arg(hullSlot.GetCount()));

    int width = fm.width(text);

    base.rx() -= (width / 2);

    painter.setFont(bold);
    painter.drawText(base, text);
}

Component *EditableSlotWidget::extractComponent(QDropEvent *e)
{
    static const char *mimeType = "application/x-qabstractitemmodeldatalist";

    if(!e->mimeData()->hasFormat(mimeType)) {
        return NULL;
    }

    QStringList componentNames;
    QByteArray encoded = e->mimeData()->data(mimeType);
    QDataStream stream(&encoded, QIODevice::ReadOnly);

    while (!stream.atEnd())
    {
        int row, col;
        QMap<int,  QVariant> roleDataMap;
        stream >> row >> col >> roleDataMap;
        componentNames << roleDataMap[0].toString();
    }

    if(componentNames.empty()) {
        return NULL;
    }

    const std::deque<Component*> &components = TheGame->GetComponents();

    for (std::deque<Component*>::const_iterator i = components.begin() ; i != components.end() ; i++) {
        if(componentNames.at(0) == QString((*i)->GetName().c_str())) {
            return *i;
        }
    } 

    return NULL;
}

int EditableSlotWidget::getAmount()
{
    Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();

    return (modifiers & Qt::ShiftModifier) ? 10 :
        (modifiers & Qt::ControlModifier) ? 100 : 1;
}

void EditableSlotWidget::dragEnterEvent(QDragEnterEvent *e)
{
    Component *comp = extractComponent(e);

    if(comp != NULL && hullSlot.IsAllowed(comp->GetType())) {
        if(shipSlot.GetComp() == NULL || (comp == shipSlot.GetComp()
            && shipSlot.GetCount() < hullSlot.GetCount()))
        {
            e->acceptProposedAction();
            return;
        }
    }

    QWidget::dragEnterEvent(e);
}

void EditableSlotWidget::dropEvent(QDropEvent *e)
{
    Component *comp = extractComponent(e);

    if(comp != NULL && hullSlot.IsAllowed(comp->GetType())) {
        int amount = getAmount();

        if(shipSlot.GetComp() == NULL && amount <= hullSlot.GetCount()) {
            shipSlot.SetComp(comp);
            shipSlot.SetCount(amount);
            update(contentsRect());
            e->acceptProposedAction();
            return;
        }
        else if((comp == shipSlot.GetComp() && shipSlot.GetCount() + amount <= hullSlot.GetCount()))
        {
            shipSlot.SetCount(shipSlot.GetCount() + amount);
            update(contentsRect());
            e->acceptProposedAction();
            return;
        }
    }

    QWidget::dropEvent(e);
}

void EditableSlotWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        dragStartPos = e->pos();
        e->accept();
        return;
    }

    QWidget::mousePressEvent(e);
}

void EditableSlotWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (!(e->buttons() & Qt::LeftButton))
        return;

    if ((e->pos() - dragStartPos).manhattanLength()
        < QApplication::startDragDistance())
        return;

    QPixmap pixmap(contentsRect().size());

    {
        QPainter painter(&pixmap);
        drawComponent(painter, QRect(QPoint(0, 0), contentsRect().size()));
    }

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    QString mimeType("text/plain");
    mimeData->setData(mimeType, QString("test").toAscii());
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);

    Qt::DropAction dropAction = drag->exec(Qt::MoveAction);

    std::cout << "start drag" << std::endl;
}

void CargoWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    painter.fillRect(contentsRect(), QBrush(Qt::gray, Qt::Dense7Pattern));
    painter.setPen(Qt::black);
    painter.drawRect(contentsRect());
}

};

