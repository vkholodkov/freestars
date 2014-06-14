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

ShipDesignDialog::ShipDesignDialog(Player *_player, const GraphicsArray *_graphicsArray, QWidget *parent)
    : QDialog(parent)
    , graphicsArray(_graphicsArray)
    , player(_player)
    , currentDesignMode(SDDDM_SHIPS)
    , currentViewMode(SDDVM_EXISTING)
    , currentShip(0)
    , currentHull(0)
    , shipBeingEdited(0)
    , plateImage(":/images/plate.png")
    , editing(false)
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

ShipDesignDialog::~ShipDesignDialog()
{
    deleteFloatingWidgets();
}

void ShipDesignDialog::resizeEvent(QResizeEvent *e)
{
    QDialog::resizeEvent(e);

    const Hull *hull = currentHull != NULL ? currentHull :
        currentShip != NULL ? currentShip->GetHull() : NULL;

    if(hull != NULL) {
        std::vector<QRect> dimensions;
        QRect boundaries;

        collectSlotDimensions(hull, dimensions, boundaries);

        unsigned int numSlots = slotWidgets.size();

        if(dimensions.size() == numSlots) { 
            QPoint origin(getWireframeOrigin(boundaries));

            for (int i = 0; i != numSlots; i++) {
                dimensions[i].translate(origin);

                if(slotWidgets[i] != NULL) {
                    slotWidgets[i]->setGeometry(dimensions[i]);
                }
            }
        }
    }
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
        deleteFloatingWidgets();

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

        connect(chooseComponentBox1, SIGNAL(activated(int)), this, SLOT(setComponentCategory1(int)));
        setComponentCategory1(0);

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

        editDesignButton->setEnabled(newViewMode == SDDVM_EXISTING && chooseDesignBox->count() != 0);

        if (newViewMode == SDDVM_AVAILABLE || newViewMode == SDDVM_ENEMY) {
            deleteDesignButton->setEnabled(false);
        }
        else {
            deleteDesignButton->setEnabled(chooseDesignBox->count() != 0);
        }
    }
}

void ShipDesignDialog::setComponentCategory(QComboBox *chooseComponentBox, QListWidget *componentListWidget, int index)
{
    QVariant userData = chooseComponentBox->itemData(index);

    long mask = userData.toULongLong();

    componentListWidget->clear();

    const std::deque<Component*> &components = TheGame->GetComponents();

    for (std::deque<Component*>::const_iterator i = components.begin() ; i != components.end() ; i++) {
        if ((*i)->IsBuildable(player) && ((*i)->GetType() & mask)) {
            const QIcon *icon = graphicsArray->GetGraphics((*i)->GetName());

            if(icon != NULL) {
                componentListWidget->addItem(new QListWidgetItem(*icon, QString((*i)->GetName().c_str())));
            }
            else {
                componentListWidget->addItem(new QListWidgetItem((*i)->GetName().c_str()));
            }
        }
    }
}

void ShipDesignDialog::setComponentCategory1(int index)
{
    setComponentCategory(chooseComponentBox1, componentListWidget1, index);
}

void ShipDesignDialog::setComponentCategory2(int index)
{
    setComponentCategory(chooseComponentBox2, componentListWidget2, index);
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

void ShipDesignDialog::populateComponentCategoryList()
{
    struct CompCategory *categories = currentDesignMode == SDDDM_SHIPS ? shipCategories : starbaseCategories;

    while (categories->title != NULL) {
        chooseComponentBox2->addItem(tr(categories->title), QVariant((qlonglong)categories->mask));
        categories++;
    }

    connect(chooseComponentBox2, SIGNAL(activated(int)), this, SLOT(setComponentCategory2(int)));
    setComponentCategory2(0);
}

void ShipDesignDialog::clearProperties()
{
    while (QWidget* w = propertiesWidget1->findChild<QWidget*>())
        delete w;
}

void ShipDesignDialog::enterEditMode()
{
    deleteFloatingWidgets();

    editing = true;
    stackedWidget1->setCurrentIndex(0);

    createShipWidgets(shipBeingEdited.get(), false);

    populateComponentCategoryList();
    shipNameEdit->setText(shipBeingEdited->GetName().c_str());

    connect(chooseComponentBox2, SIGNAL(activated(int)), this, SLOT(setComponentCategory2(int)));
    setComponentCategory2(0);

    connect(okButton, SIGNAL(clicked()), this, SLOT(saveDesign()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(abandonDesign()));
}

void ShipDesignDialog::leaveEditMode()
{
    deleteFloatingWidgets();

    editing = false;
    stackedWidget1->setCurrentIndex(1);

    okButton->disconnect();
    cancelButton->disconnect();

    switchMode(currentDesignMode, currentDesignMode, currentViewMode, currentViewMode);
}

void ShipDesignDialog::setShipDesign(int index)
{
    clearProperties();
    deleteFloatingWidgets();

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

            createShipWidgets(currentShip);
        }
    }
}

void ShipDesignDialog::setHull(int index)
{
    clearProperties();
    deleteFloatingWidgets();

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

        createHullWidgets(currentHull);
    }
}

void ShipDesignDialog::copyDesign()
{
    if (currentShip != NULL) {
        shipBeingEdited.reset(new Ship);
        shipBeingEdited->CopyDesign(currentShip, false);
    }
    else if(currentHull != NULL) {
        shipBeingEdited.reset(new Ship(currentHull));
    }

    enterEditMode();
}

void ShipDesignDialog::editDesign()
{
    if (currentShip != NULL) {
        shipBeingEdited.reset(new Ship);
        shipBeingEdited->CopyDesign(currentShip, false);
        enterEditMode();
    }
}

void ShipDesignDialog::deleteDesign()
{
}

QPoint ShipDesignDialog::getWireframeOrigin(const QRect &boundaries) const
{
    QRect shipDisplayRect(shipDisplayWidget->contentsRect());
    QRect rect(shipDisplayWidget->mapTo((QWidget*)this, shipDisplayRect.topLeft()),
        shipDisplayRect.size());

    rect.setTop(chooseDesignBox->mapTo((QWidget*)this, chooseDesignBox->rect().bottomRight()).y());

    QPoint origin(rect.center());

    origin.rx() -= (boundaries.width() / 2);
    origin.ry() -= 32; /* half a cell */

    return origin;
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

static int distanceFromOrigin(QWidget *a)
{
    QPoint ap1 = a->mapToParent(a->rect().topRight());
    QPoint ap2 = a->mapToParent(a->rect().bottomLeft());

    return ap2.x() - ap2.y() * ((ap2.x() - ap1.x())/(ap2.y() - ap1.y()));
}

static int zsort(QWidget *a, QWidget *b)
{
    return distanceFromOrigin(a) < distanceFromOrigin(b);
}

void ShipDesignDialog::createHullWidgets(const Hull *hull)
{
    std::vector<QRect> dimensions;
    QRect boundaries;

    collectSlotDimensions(hull, dimensions, boundaries);

    QPoint origin(getWireframeOrigin(boundaries));
    unsigned int numSlots = hull->GetNumberSlots();

    /*
     * We add one extra element for cargo
     */
    slotWidgets.resize(numSlots + 1, 0);

    std::vector<QWidget*> drawOrder(numSlots + 1, 0);

    for (int i = 0; i != numSlots; i++) {
        const Slot &hullSlot = hull->GetSlot(i);
        dimensions[i].translate(origin);
        drawOrder[i] = slotWidgets[i] = new HullSlotWidget(hullSlot);
        slotWidgets[i]->setGeometry(dimensions[i]);
    }

    dimensions[numSlots].translate(origin);

    drawOrder[numSlots] = slotWidgets[numSlots] = new CargoWidget(this);
    slotWidgets[numSlots]->setGeometry(dimensions[numSlots]);

    std::sort(drawOrder.begin(), drawOrder.end(), zsort);

    for(std::vector<QWidget*>::const_iterator i = drawOrder.begin() ;
        i != drawOrder.end() ; i++)
    {
        (*i)->setParent(this);
        (*i)->show();
    }
}

void ShipDesignDialog::createShipWidgets(Ship *ship, bool readOnly)
{
    std::vector<QRect> dimensions;
    QRect boundaries;
    const Hull *hull = ship->GetHull();

    collectSlotDimensions(hull, dimensions, boundaries);

    QPoint origin(getWireframeOrigin(boundaries));
    unsigned int numSlots = hull->GetNumberSlots();

    /*
     * We add one extra element for cargo
     */
    slotWidgets.resize(numSlots + 1, 0);

    std::vector<QWidget*> drawOrder(numSlots + 1, 0);

    for (int i = 0; i != numSlots; i++) {
        Slot &shipSlot = ship->GetSlot(i);
        const Slot &hullSlot = hull->GetSlot(i);
        dimensions[i].translate(origin);
        drawOrder[i] = slotWidgets[i] = readOnly ? new SlotWidget(graphicsArray, shipSlot, hullSlot)
            : new EditableSlotWidget(graphicsArray, shipSlot, hullSlot);
        slotWidgets[i]->setGeometry(dimensions[i]);
        slotWidgets[i]->show();
    }

    dimensions[numSlots].translate(origin);

    drawOrder[numSlots] = slotWidgets[numSlots] = new CargoWidget(this);
    slotWidgets[numSlots]->setGeometry(dimensions[numSlots]);
    slotWidgets[numSlots]->show();

    std::sort(drawOrder.begin(), drawOrder.end(), zsort);

    for(std::vector<QWidget*>::const_iterator i = drawOrder.begin() ;
        i != drawOrder.end() ; i++)
    {
        (*i)->setParent(this);
        (*i)->show();
    }
}

void ShipDesignDialog::deleteFloatingWidgets()
{
    for (int i = 0; i != slotWidgets.size(); i++) {
        delete slotWidgets[i];
        slotWidgets[i] = 0;
    }
}

void ShipDesignDialog::saveDesign()
{
    leaveEditMode();

    if(currentShip != NULL) {
        shipBeingEdited.reset(0);
        return;
    }

    //player->AddShipDesign(shipBeingEdited.get());
    currentShip = shipBeingEdited.release();
}

void ShipDesignDialog::abandonDesign()
{
    leaveEditMode();
    shipBeingEdited.reset(0);
}

};
