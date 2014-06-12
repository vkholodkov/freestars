/*
 * Copyright (C) 2014 Valery Kholodkov
 */
#include <cmath>

#include <QSignalMapper>
#include <QPainter>

#include "FSServer.h"
#include "Hull.h"

#include "ship_design_dialog.h"

namespace FreeStars {

struct CompCategory {
    const char *title;
    long mask;
};

static CompCategory starbaseCategories[] = {
    { "All", CT_ARMOR | CT_SHIELD | CT_WEAPON | CT_ELEC | CT_MECH | CT_ORBITAL },
    { "Armor", CT_ARMOR },
    { "Shields", CT_SHIELD },
    { "Weapons", CT_WEAPON },
    { "Electrical", CT_ELEC },
    { "Mechanical", CT_MECH },
    { "Orbital", CT_ORBITAL },
    { NULL, 0 }
};

static CompCategory shipCategories[] = {
    { "All", CT_ARMOR | CT_SHIELD | CT_WEAPON | CT_BOMB | CT_ELEC | CT_ENGINE | CT_MINELAY 
        | CT_MINER | CT_SCANNER | CT_MECH },
    { "Armor", CT_ARMOR },
    { "Shields", CT_SHIELD },
    { "Weapons", CT_WEAPON },
    { "Bombs", CT_BOMB },
    { "Electrical", CT_ELEC },
    { "Engines", CT_ENGINE },
    { "Mine layers", CT_MINELAY },
    { "Mining Robots", CT_MINER },
    { "Scanner", CT_SCANNER },
    { "Mechanical", CT_MECH },
    { NULL, 0 }
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

ShipDesignDialog::ShipDesignDialog(Player *_player, const GraphicsArray *_graphicsArray, QWidget *parent)
    : QDialog(parent)
    , graphicsArray(_graphicsArray)
    , player(_player)
    , currentDesignMode(SDDDM_SHIPS)
    , currentViewMode(SDDVM_EXISTING)
    , currentShip(0)
    , currentHull(0)
    , plateImage(":/images/plate.png")
{
    setupUi(this);

    QSignalMapper *designModeMapper = new QSignalMapper(this);

    designModeMapper->setMapping(shipDesignsButton, SDDDM_SHIPS);
    designModeMapper->setMapping(starbaseDesignsButton, SDDDM_STARBASES);

    connect(shipDesignsButton, SIGNAL(clicked(bool)), designModeMapper, SLOT(map()));
    connect(starbaseDesignsButton, SIGNAL(clicked(bool)), designModeMapper, SLOT(map()));

    QSignalMapper *viewModeMapper = new QSignalMapper(this);

    viewModeMapper->setMapping(existingDesignsButton, SDDVM_EXISTING);
    viewModeMapper->setMapping(availableHullTypesButton, SDDVM_AVAILABLE);
    viewModeMapper->setMapping(enemyHullsButton, SDDVM_ENEMY);
    viewModeMapper->setMapping(componentsButton, SDDVM_COMPONENTS);

    connect(existingDesignsButton, SIGNAL(clicked(bool)), viewModeMapper, SLOT(map()));
    connect(availableHullTypesButton, SIGNAL(clicked(bool)), viewModeMapper, SLOT(map()));
    connect(enemyHullsButton, SIGNAL(clicked(bool)), viewModeMapper, SLOT(map()));
    connect(componentsButton, SIGNAL(clicked(bool)), viewModeMapper, SLOT(map()));

    connect(designModeMapper, SIGNAL(mapped(int)), this, SLOT(setDesignMode(int)));
    connect(viewModeMapper, SIGNAL(mapped(int)), this, SLOT(setViewMode(int)));

    connect(copyDesignButton, SIGNAL(clicked(bool)), this, SLOT(copyDesign()));
    connect(editDesignButton, SIGNAL(clicked(bool)), this, SLOT(editDesign()));
    connect(deleteDesignButton, SIGNAL(clicked(bool)), this, SLOT(deleteDesign()));

    connect(doneButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

    populateExistingDesigns(SDDDM_SHIPS);
}

void ShipDesignDialog::setDesignMode(int mode)
{
    switchMode(currentDesignMode, mode, currentViewMode, currentViewMode);
    currentDesignMode = mode;
}

void ShipDesignDialog::setViewMode(int mode)
{
    switchMode(currentDesignMode, currentDesignMode, currentViewMode, mode);
    currentViewMode = mode;
}

void ShipDesignDialog::switchMode(int oldDesignMode, int newDesignMode, int oldViewMode, int newViewMode)
{
    if (oldViewMode == SDDVM_COMPONENTS) {
        chooseComponentBox1->clear();
        chooseComponentBox1->disconnect();

        if (newViewMode != SDDVM_COMPONENTS) {
            copyDesignButton->setEnabled(true);
            editDesignButton->setEnabled(true);
            deleteDesignButton->setEnabled(true);
        }
    }
    else {
        chooseDesignBox->clear();
        chooseDesignBox->disconnect();

        currentShip = NULL;
        currentHull = NULL;        
        update(contentsRect());

        if (oldViewMode == SDDVM_AVAILABLE || oldViewMode == SDDVM_ENEMY) {
            deleteDesignButton->setEnabled(true);
        }
    }

    stackedWidget2->setCurrentIndex(newViewMode == SDDVM_COMPONENTS ? 1 : 0);

    if (newViewMode == SDDVM_COMPONENTS) {
        struct CompCategory *categories = newDesignMode == SDDDM_SHIPS ? shipCategories : starbaseCategories;

        while (categories->title != NULL) {
            chooseComponentBox1->addItem(tr(categories->title), QVariant((qlonglong)categories->mask));
            categories++;
        }

        connect(chooseComponentBox1, SIGNAL(activated(int)), this, SLOT(setComponentCategory(int)));
        setComponentCategory(0);

        if (oldViewMode != SDDVM_COMPONENTS) {
            copyDesignButton->setEnabled(false);
            editDesignButton->setEnabled(false);
            deleteDesignButton->setEnabled(false);
        }
    }
    else {
        if (newViewMode == SDDVM_EXISTING) {
            populateExistingDesigns(newDesignMode);
        }
        else if (newViewMode == SDDVM_AVAILABLE) {
            populateAvailableHullTypes(newDesignMode);
        }

        if (newViewMode == SDDVM_AVAILABLE || newViewMode == SDDVM_ENEMY) {
            deleteDesignButton->setEnabled(false);
        }
    }
}

void ShipDesignDialog::setComponentCategory(int index)
{
    QVariant userData = chooseComponentBox1->itemData(index);

    long mask = userData.toULongLong();

    componentListWidget1->clear();

    const std::deque<Component*> &components = TheGame->GetComponents();

    for (std::deque<Component*>::const_iterator i = components.begin() ; i != components.end() ; i++) {
        if ((*i)->IsBuildable(player) && ((*i)->GetType() & mask)) {
            const QIcon *icon = graphicsArray->GetGraphics((*i)->GetName());

            if(icon != NULL) {
                componentListWidget1->addItem(new QListWidgetItem(*icon, QString((*i)->GetName().c_str())));
            }
            else {
                componentListWidget1->addItem(new QListWidgetItem((*i)->GetName().c_str()));
            }
        }
    }
}

void ShipDesignDialog::populateExistingDesigns(int designMode)
{
    int max = designMode == SDDDM_SHIPS ? Rules::GetConstant("MaxShipDesigns") 
        : Rules::GetConstant("MaxBaseDesigns");

    for (long i = 0 ; i != max ; i++) {
        const Ship *ship = designMode == SDDDM_SHIPS ? player->GetShipDesign(i) : player->GetBaseDesign(i);

        if (ship != NULL) {
            chooseDesignBox->addItem(QString(ship->GetName().c_str()),
                QVariant(reinterpret_cast<qlonglong>(ship)));
        }
    }

    connect(chooseDesignBox, SIGNAL(activated(int)), this, SLOT(setShipDesign(int)));
    setShipDesign(chooseDesignBox->currentIndex());
}

void ShipDesignDialog::populateAvailableHullTypes(int designMode)
{
    const std::deque<Component*> &components = TheGame->GetComponents();

    for (std::deque<Component*>::const_iterator i = components.begin(); i != components.end(); i++) {
        if ((*i)->IsBuildable(player) && ((*i)->IsType(designMode == SDDDM_SHIPS ? CT_HULL : CT_BASE))) {
            Hull *hull = reinterpret_cast<Hull*>(*i);

            if (hull != NULL) {
                chooseDesignBox->addItem(QString((*i)->GetName().c_str()),
                    QVariant(reinterpret_cast<qlonglong>(hull)));
            }
        }
    }

    connect(chooseDesignBox, SIGNAL(activated(int)), this, SLOT(setHull(int)));
    setHull(chooseDesignBox->currentIndex());
}

void ShipDesignDialog::clearProperties()
{
    while (QWidget* w = propertiesWidget1->findChild<QWidget*>())
        delete w;
}

void ShipDesignDialog::setShipDesign(int index)
{
    clearProperties();

    QVariant userData = chooseDesignBox->itemData(index);

    currentShip = reinterpret_cast<Ship*>(userData.toULongLong());

    if (currentShip != NULL) {
        Cost cost(currentShip->GetCost(player));

        costTitleLabel1->setText(tr("Cost of one %0").arg(currentShip->GetName().c_str()));
        ironiumLabel1->setText(tr("%0kt").arg(cost[0]));
        boraniumLabel1->setText(tr("%0kt").arg(cost[1]));
        germaniumLabel1->setText(tr("%0kt").arg(cost[2]));
        resourcesLabel1->setText(QString::number(cost.GetResources()));
        massLabel1->setText(tr("%0kt").arg(currentShip->GetMass()));

        QFormLayout *formLayout = dynamic_cast<QFormLayout*>(propertiesWidget1->layout());

        if (formLayout != 0) {
            if (currentShip->GetFuelCapacity() != 0) {
                QLabel *label = new QLabel(tr("Max Fuel:"));
                label->setStyleSheet("QLabel { font-weight: bold; }");
                QLabel *text = new QLabel(tr("%0mg").arg(currentShip->GetFuelCapacity()));
                text->setAlignment(Qt::AlignRight);
                formLayout->addRow(label, text);
            }

            if (currentShip->GetArmor(player) != 0) {
                QLabel *label = new QLabel(tr("Armor:"));
                label->setStyleSheet("QLabel { font-weight: bold; }");
                QLabel *text = new QLabel(tr("%0dp").arg(currentShip->GetArmor(player)));
                text->setAlignment(Qt::AlignRight);
                formLayout->addRow(label, text);
            }

            if (currentShip->GetShield(player) != 0) {
                QLabel *label = new QLabel(tr("Shield:"));
                label->setStyleSheet("QLabel { font-weight: bold; }");
                QLabel *text = new QLabel(tr("%0dp").arg(currentShip->GetShield(player)));
                text->setAlignment(Qt::AlignRight);
                formLayout->addRow(label, text);
            }

            if (currentShip->GetRating() != 0) {
                QLabel *label = new QLabel(tr("Rating:"));
                label->setStyleSheet("QLabel { font-weight: bold; }");
                QLabel *text = new QLabel(QString::number(currentShip->GetRating()));
                text->setAlignment(Qt::AlignRight);
                formLayout->addRow(label, text);
            }

            if (currentShip->GetCloaking() != 0 || currentShip->GetJamming() != 0) {
                QLabel *label = new QLabel(tr("Cloak/Jam:"));
                label->setStyleSheet("QLabel { font-weight: bold; }");
                QLabel *text = new QLabel(tr("%0%/%1%").arg(currentShip->GetCloaking()).arg(currentShip->GetJamming() * 100));
                text->setAlignment(Qt::AlignRight);
                formLayout->addRow(label, text);
            }

            QLabel *label = new QLabel(tr("Initiative/Moves:"));
            label->setStyleSheet("QLabel { font-weight: bold; }");
            QLabel *text = new QLabel(tr("%0 / %1").arg(currentShip->GetNetInitiative()).arg(currentShip->GetNetSpeed()));
            text->setAlignment(Qt::AlignRight);
            formLayout->addRow(label, text);
        }
    }

    update(contentsRect());
}

void ShipDesignDialog::setHull(int index)
{
    clearProperties();

    QVariant userData = chooseDesignBox->itemData(index);

    currentHull = reinterpret_cast<Hull*>(userData.toULongLong());

    if (currentHull != NULL) {
        Cost cost(currentHull->GetCost(player));

        costTitleLabel1->setText(tr("Cost of one %0").arg(currentHull->GetName().c_str()));
        ironiumLabel1->setText(tr("%0kt").arg(cost[0]));
        boraniumLabel1->setText(tr("%0kt").arg(cost[1]));
        germaniumLabel1->setText(tr("%0kt").arg(cost[2]));
        resourcesLabel1->setText(QString::number(cost.GetResources()));
        massLabel1->setText(tr("%0kt").arg(currentHull->GetMass()));
    }

    update(contentsRect());
}

void ShipDesignDialog::copyDesign()
{
}

void ShipDesignDialog::editDesign()
{
}

void ShipDesignDialog::deleteDesign()
{
}

void ShipDesignDialog::paintEvent(QPaintEvent *event)
{
    QDialog::paintEvent(event);

    if(currentShip != NULL) {
        drawShip(currentShip);
    }
    else if (currentHull != NULL) {
        drawHull(currentHull);
    }
}

void ShipDesignDialog::drawShip(const Ship *ship)
{
    QPainter painter(this);
    std::vector<QRect> dimensions;
    QRect boundaries;

    collectSlotDimensions(ship->GetHull(), dimensions, boundaries);

    QRect shipDisplayRect(shipDisplayWidget->contentsRect());
    QRect rect(shipDisplayWidget->mapTo(this, shipDisplayRect.topLeft()),
        shipDisplayRect.size());

    rect.setTop(chooseDesignBox->mapTo(this, chooseDesignBox->rect().bottomRight()).y());

    QPoint origin(rect.center());

    origin.rx() -= (boundaries.width() / 2);
    origin.ry() -= 32; /* half a cell */

    const Hull *hull = ship->GetHull();
    unsigned int numSlots = hull->GetNumberSlots();

    for (int i = 0; i != numSlots; i++) {
        const Slot &slot = currentShip->GetSlot(i);

        dimensions[i].translate(origin);

        if (slot.GetComp() != NULL) {
            drawComponent(painter, slot.GetComp(), hull->GetSlot(i), dimensions[i]);
        }
        else {
            drawSlot(painter, hull->GetSlot(i), dimensions[i]);
        }        
    }

    dimensions[numSlots].translate(origin);

    painter.fillRect(dimensions[numSlots], QBrush(Qt::gray, Qt::Dense7Pattern));
    painter.setPen(Qt::black);
    painter.drawRect(dimensions[numSlots]);
}

void ShipDesignDialog::drawHull(const Hull *hull)
{
    QPainter painter(this);
    std::vector<QRect> dimensions;
    QRect boundaries;

    collectSlotDimensions(hull, dimensions, boundaries);

    QRect shipDisplayRect(shipDisplayWidget->contentsRect());
    QRect rect(shipDisplayWidget->mapTo(this, shipDisplayRect.topLeft()),
        shipDisplayRect.size());

    rect.setTop(chooseDesignBox->mapTo(this, chooseDesignBox->rect().bottomRight()).y());

    QPoint origin(rect.center());

    origin.rx() -= boundaries.width() / 2;
    origin.ry() -= 32; /* half a cell */

    unsigned int numSlots = hull->GetNumberSlots();

    for (int i = 0; i != numSlots; i++) {
        dimensions[i].translate(origin);
        drawSlot(painter, hull->GetSlot(i), dimensions[i]);
    }

    dimensions[numSlots].translate(origin);

    painter.fillRect(dimensions[numSlots], QBrush(Qt::gray, Qt::Dense7Pattern));
    painter.setPen(Qt::black);
    painter.drawRect(dimensions[numSlots]);
}

void ShipDesignDialog::collectSlotDimensions(const Hull *hull, std::vector<QRect> &dimensions, QRect &boundaries)
{
    unsigned int numSlots = hull->GetNumberSlots();

    dimensions.resize(numSlots + 1, QRect());

    for (int i = 0; i != numSlots; i++) {
        const Slot &slot = hull->GetSlot(i);

        QPoint slotOrigin(slot.GetLeft(), slot.GetTop());

        dimensions[i].setTopLeft(slotOrigin);
        dimensions[i].setSize(QSize(64, 64));

        boundaries = boundaries.united(dimensions[i]);
    }

    dimensions[numSlots].setTopLeft(QPoint(hull->GetCargoLeft(), hull->GetCargoTop()));
    dimensions[numSlots].setSize(QSize(hull->GetCargoWidth(), hull->GetCargoHeight()));

    boundaries = boundaries.united(dimensions[numSlots]);
}

QString ShipDesignDialog::describeSlot(const Slot &slot) 
{
    QStringList typeList;
    struct CompCategory *category = slotCategories;
    int count = 0;

    while (category->title != NULL) {
        if(slot.IsAllowed(category->mask)) {
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

void ShipDesignDialog::drawSlot(QPainter &painter, const Slot &slot, const QRect &dimensions)
{
    QFont bold(font());
    bold.setBold(true);
    QFont smaller(font());
    smaller.setPointSize(font().pointSize() - 1);
    QFontMetrics fm(bold);

    painter.fillRect(dimensions, QBrush(Qt::gray));
    painter.setPen(Qt::black);
    painter.drawRect(dimensions);

    QPoint base(dimensions.center().x(), dimensions.bottom() - 3);

    QString description(describeSlot(slot));
    QString text(tr(slot.IsAllowed(CT_ENGINE) ? "needs %0" : "up to %0").arg(slot.GetCount()));

    int width = fm.width(text);

    base.rx() -= (width / 2);

    painter.setFont(smaller);
    painter.drawText(dimensions.adjusted(0, 2, 0, -fm.height()),
        Qt::TextWordWrap | Qt::AlignCenter | Qt::AlignBottom, description);
    painter.setFont(bold);
    painter.drawText(base, text);
}

void ShipDesignDialog::drawComponent(QPainter &painter, const Component *comp, const Slot &hullSlot, const QRect &dimensions)
{
    QFont bold(font());
    bold.setBold(true);
    QFontMetrics fm(bold);

    painter.drawImage(dimensions.topLeft(), plateImage);

    const QIcon *icon = graphicsArray->GetGraphics(comp->GetName());

    if(icon != NULL) {
        painter.drawPixmap(dimensions.topLeft() + QPoint(6, 0), icon->pixmap(52, 52));
    }

    painter.setFont(font());

    QPoint base(dimensions.center().x(), dimensions.bottom() - 3);

    QString text(tr("up to %0").arg(hullSlot.GetCount()));

    int width = fm.width(text);

    base.rx() -= (width / 2);

    painter.setFont(bold);
    painter.drawText(base, text);
}

};
