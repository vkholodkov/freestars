/*
 * Copyright (C) 2014 Valery Kholodkov
 */
#include <cmath>

#include <QSignalMapper>
#include <QPainter>
#include <QMessageBox>
#include <QPropertyAnimation>

#include "FSServer.h"
#include "Hull.h"

#include "ship_describer.h"

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

void (ShipDesignDialog::*ShipDesignDialog::propertyFiller[])(QFormLayout*) = {
    NULL
};

ShipDesignDialog::ShipDesignDialog(Player *_player, const GraphicsArray *_graphicsArray, QWidget *parent)
    : QDialog(parent)
    , graphicsArray(_graphicsArray)
    , game(_player->GetGame())
    , player(_player)
    , currentDesignMode(SDDDM_SHIPS)
    , currentViewMode(SDDVM_EXISTING)
    , shipBeingEdited(0)
    , plateImage(":/images/plate.png")
{
    setupUi(this);

    shipAvatarWidget1->setGraphicsArray(_graphicsArray);
    shipAvatarWidget2->setGraphicsArray(_graphicsArray);

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

    populateExistingDesigns(SDDDM_SHIPS, true);

    QPropertyAnimation *animation = new QPropertyAnimation(chooseDesignBox, "currentIndex");
    animation->setDuration(500);
    animation->setStartValue(-1);
    animation->setEndValue(0);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

ShipDesignDialog::~ShipDesignDialog()
{
    deleteFloatingWidgets();
}

void ShipDesignDialog::resizeEvent(QResizeEvent *e)
{
    QDialog::resizeEvent(e);
    updateFloatingWidgetsGeometry();
}

void ShipDesignDialog::updateFloatingWidgetsGeometry()
{
    QRect boundaries;

    collectFloatingWidgetBoundaries(boundaries);

    unsigned int numSlots = slotWidgets.size();

    QPoint origin(getWireframeOrigin(boundaries));
    QPoint difference(origin - boundaries.topLeft());

    for (int i = 0; i != numSlots; i++) {
        if(slotWidgets[i] != NULL) {
            QRect geometry(slotWidgets[i]->geometry());

            geometry.translate(difference);

            slotWidgets[i]->setGeometry(geometry);
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
        componentListWidget1->disconnect();

        if (newViewMode != SDDVM_COMPONENTS) {
            copyDesignButton->setEnabled(true);
            editDesignButton->setEnabled(true);
            deleteDesignButton->setEnabled(true);
        }
    }
    else {
        chooseDesignBox->clear();
        chooseDesignBox->disconnect();

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
        connect(componentListWidget1, SIGNAL(currentRowChanged(int)), this, SLOT(setComponent1(int)));

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

    const std::deque<Component*> &components = game->GetComponents();

    for (std::deque<Component*>::const_iterator i = components.begin() ; i != components.end() ; i++) {
        if ((*i)->IsBuildable(player) && ((*i)->GetType() & mask)) {
            const QIcon *icon = graphicsArray->GetComponentIcon((*i)->GetName());

            QListWidgetItem *item = (icon != NULL) ? new QListWidgetItem(*icon, QString((*i)->GetName().c_str()))
                : new QListWidgetItem((*i)->GetName().c_str());

            item->setData(Qt::UserRole, QVariant(reinterpret_cast<qlonglong>(*i)));

            componentListWidget->addItem(item);
        }
    }
}

void ShipDesignDialog::setComponentCategory1(int index)
{
    setComponentCategory(chooseComponentBox1, componentListWidget1, index);
    clearProperties();
}

void ShipDesignDialog::setComponentCategory2(int index)
{
    setComponentCategory(chooseComponentBox2, componentListWidget2, index);
}

void ShipDesignDialog::setComponent1(int index)
{
    clearProperties();

    if(index < 0) {
        return;
    }

    QListWidgetItem *item = componentListWidget1->item(index);

    QVariant userData = item->data(Qt::UserRole);

    Component *component = reinterpret_cast<Component*>(userData.toULongLong());

    if(component != NULL) {
        costTitleLabel3->setText(tr("Cost of one %0").arg(component->GetName().c_str()));

        ComponentDescriber componentDescriber(player, this);
        componentDescriber.describe(component, dynamic_cast<QFormLayout*>(propertiesWidget31->layout()),
            dynamic_cast<QFormLayout*>(propertiesWidget32->layout()));
    }
}

void ShipDesignDialog::populateExistingDesigns(int designMode, bool initial)
{
    int start = designMode == SDDDM_SHIPS ? 1 : 0;
    int max = designMode == SDDDM_SHIPS ? Rules::GetConstant("MaxShipDesigns") 
        : Rules::GetConstant("MaxBaseDesigns") + 1;

    for (long i = start ; i != max ; i++) {
        const Ship *ship = designMode == SDDDM_SHIPS ? player->GetShipDesign(i) : player->GetBaseDesign(i);

        if (ship != NULL) {
            chooseDesignBox->addItem(QString(ship->GetName().c_str()),
                QVariant(reinterpret_cast<qlonglong>(ship)));
        }
    }

    connect(chooseDesignBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setShipDesign(int)));

    if(!initial) {
        setShipDesign(chooseDesignBox->currentIndex());
    }
}

void ShipDesignDialog::populateAvailableHullTypes(int designMode)
{
    const std::deque<Component*> &components = game->GetComponents();

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
    while (QWidget* w = propertiesWidget2->findChild<QWidget*>())
        delete w;
    while (QWidget* w = propertiesWidget31->findChild<QWidget*>())
        delete w;
    while (QWidget* w = propertiesWidget32->findChild<QWidget*>())
        delete w;
    costTitleLabel3->setText("");
}

void ShipDesignDialog::enterEditMode(bool addNew)
{
    deleteFloatingWidgets();

    stackedWidget1->setCurrentIndex(0);

    shipAvatarWidget2->setHullName(shipBeingEdited->GetHull()->GetName().c_str());

    updateDesignProperties();

    createShipWidgets(shipBeingEdited.get(), false);

    populateComponentCategoryList();

    if(addNew) {
        shipNameEdit->setText((shipBeingEdited->GetName() + " (2)").c_str());
    }
    else {
        shipNameEdit->setText(shipBeingEdited->GetName().c_str());
    }

    connect(chooseComponentBox2, SIGNAL(activated(int)), this, SLOT(setComponentCategory2(int)));
    setComponentCategory2(0);

    connect(okButton, SIGNAL(clicked()), this, addNew ? SLOT(addNewDesign()) : SLOT(saveDesign()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(abandonDesign()));
}

void ShipDesignDialog::leaveEditMode()
{
    deleteFloatingWidgets();

    stackedWidget1->setCurrentIndex(1);

    shipAvatarWidget2->setHullName("");

    chooseComponentBox2->disconnect();

    okButton->disconnect();
    cancelButton->disconnect();
}

void ShipDesignDialog::setShipDesign(int index)
{
    clearProperties();
    deleteFloatingWidgets();

    QVariant userData = chooseDesignBox->itemData(index);

    Ship *ship = reinterpret_cast<Ship*>(userData.toULongLong());

    if(ship == NULL)
        return;

    shipAvatarWidget1->setHullName(ship->GetHull()->GetName().c_str());

    Cost cost(ship->GetCost(player));

    costTitleLabel1->setText(tr("Cost of one %0").arg(ship->GetName().c_str()));
    ironiumLabel1->setText(tr("%0kt").arg(cost[0]));
    boraniumLabel1->setText(tr("%0kt").arg(cost[1]));
    germaniumLabel1->setText(tr("%0kt").arg(cost[2]));
    resourcesLabel1->setText(QString::number(cost.GetResources()));
    massLabel1->setText(tr("%0kt").arg(ship->GetMass()));

    QFormLayout *formLayout = dynamic_cast<QFormLayout*>(propertiesWidget1->layout());

    if (formLayout != 0) {
        ShipDescriber shipDescriber(player, this);
        shipDescriber.describe(ship, NULL, formLayout);
    }

    createShipWidgets(ship);
}

void ShipDesignDialog::setHull(int index)
{
    clearProperties();
    deleteFloatingWidgets();

    QVariant userData = chooseDesignBox->itemData(index);

    Hull *hull = reinterpret_cast<Hull*>(userData.toULongLong());

    if(hull == NULL)
        return;

    shipAvatarWidget1->setHullName(hull->GetName().c_str());

    Cost cost(hull->GetCost(player));

    costTitleLabel1->setText(tr("Cost of one %0").arg(hull->GetName().c_str()));
    ironiumLabel1->setText(tr("%0kt").arg(cost[0]));
    boraniumLabel1->setText(tr("%0kt").arg(cost[1]));
    germaniumLabel1->setText(tr("%0kt").arg(cost[2]));
    resourcesLabel1->setText(QString::number(cost.GetResources()));
    massLabel1->setText(tr("%0kt").arg(hull->GetMass()));

    QFormLayout *formLayout = dynamic_cast<QFormLayout*>(propertiesWidget1->layout());

    if (formLayout != 0) {
        HullDescriber hullDescriber(this);
        hullDescriber.describe(hull, NULL, formLayout);
    }

    createHullWidgets(hull);
}

void ShipDesignDialog::updateDesignProperties()
{
    clearProperties();

    Ship *ship = shipBeingEdited.get();

    ship->SlotsUpdated();

    Cost cost(ship->GetCost(player));

    costTitleLabel2->setText(tr("Cost of one %0").arg(ship->GetName().c_str()));
    ironiumLabel2->setText(tr("%0kt").arg(cost[0]));
    boraniumLabel2->setText(tr("%0kt").arg(cost[1]));
    germaniumLabel2->setText(tr("%0kt").arg(cost[2]));
    resourcesLabel2->setText(QString::number(cost.GetResources()));
    massLabel2->setText(tr("%0kt").arg(ship->GetMass()));

    QFormLayout *formLayout = dynamic_cast<QFormLayout*>(propertiesWidget2->layout());

    if (formLayout != 0) {
        ShipDescriber shipDescriber(player, this);
        shipDescriber.describe(ship, NULL, formLayout);
    }
}

void ShipDesignDialog::copyDesign(const Hull *hull)
{
    shipBeingEdited.reset(new Ship(player->GetGame(), hull));
    enterEditMode(true);
}

void ShipDesignDialog::copyDesign(const Ship *ship)
{
    shipBeingEdited.reset(new Ship(player->GetGame()));
    shipBeingEdited->CopyDesign(ship, false);
    enterEditMode(true);
}

void ShipDesignDialog::editDesign(const Ship *ship)
{
    shipBeingEdited.reset(new Ship(player->GetGame()));
    shipBeingEdited->CopyDesign(ship, false);
    enterEditMode(false);
}

void ShipDesignDialog::copyDesign()
{
    QVariant userData = chooseDesignBox->itemData(chooseDesignBox->currentIndex());

    if (currentViewMode == SDDVM_EXISTING) {
        Ship *ship = reinterpret_cast<Ship*>(userData.toULongLong());

        if(ship != NULL) {
            copyDesign(ship);
        }
    }
    else if(currentViewMode == SDDVM_AVAILABLE) {
        Hull *hull = reinterpret_cast<Hull*>(userData.toULongLong());

        if(hull != NULL) {
            copyDesign(hull);
        }
    }
}

void ShipDesignDialog::editDesign()
{
    if (currentViewMode != SDDVM_EXISTING)
        return;

    QVariant userData = chooseDesignBox->itemData(chooseDesignBox->currentIndex());

    Ship *ship = reinterpret_cast<Ship*>(userData.toULongLong());

    if(ship != NULL) {
        if(ship->GetNumberBuilt() != 0) {
            copyDesign(ship);
        }
        else {
            editDesign(ship);
        }
    }
}

void ShipDesignDialog::deleteDesign()
{
    if (currentViewMode != SDDVM_EXISTING)
        return;

    QVariant userData = chooseDesignBox->itemData(chooseDesignBox->currentIndex());

    Ship *ship = reinterpret_cast<Ship*>(userData.toULongLong());

    if(ship != NULL) {
        int built = ship->GetNumberBuilt();

        if(built == 0 || QMessageBox::question(this, tr("Delete Design"), tr("You currently " \
                "have %0 %1s. If you delete this design these ships will be destroyed. " \
                "Are you sure?").arg(built).arg(ship->GetName().c_str()), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
        {
            std::cout << "delete design " << ship->GetName() << std::endl;
        }
    }
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

void ShipDesignDialog::collectFloatingWidgetBoundaries(QRect &boundaries)
{
    boundaries = QRect();

    for(std::vector<QWidget*>::const_iterator i = slotWidgets.begin() ;
        i != slotWidgets.end() ; i++)
    {
        if(*i != NULL) {
            boundaries = boundaries.united((*i)->geometry());
        }
    }
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

    drawOrder[numSlots] = slotWidgets[numSlots] = new CargoWidget(hull->GetCargoCapacity(), this);
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
            : new EditableSlotWidget(game, graphicsArray, shipSlot, hullSlot);
        slotWidgets[i]->setGeometry(dimensions[i]);
    }

    dimensions[numSlots].translate(origin);

    drawOrder[numSlots] = slotWidgets[numSlots] = new CargoWidget(hull->GetCargoCapacity(), this);
    slotWidgets[numSlots]->setGeometry(dimensions[numSlots]);
    slotWidgets[numSlots]->show();

    std::sort(drawOrder.begin(), drawOrder.end(), zsort);

    for(std::vector<QWidget*>::const_iterator i = drawOrder.begin() ;
        i != drawOrder.end() ; i++)
    {
        (*i)->setParent(this);
        (*i)->show();

        EditableSlotWidget *editable = dynamic_cast<EditableSlotWidget*>(*i);

        if(editable != NULL) {
            connect(editable, SIGNAL(slotChanged()), this, SLOT(updateDesignProperties()));
        }
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
    Ship *ship = shipBeingEdited.get();

    QString name(shipNameEdit->text());

    ship->SetName(name.toStdString());

    int index = chooseDesignBox->currentIndex();

    if(currentDesignMode == SDDDM_SHIPS) {
        player->SetShipDesign(index + 1, ship);
    }
    else {
        player->SetBaseDesign(index, ship);
    }

    shipBeingEdited.release();

    leaveEditMode();
    setViewMode(SDDVM_EXISTING);
    existingDesignsButton->setChecked(true);

    index = chooseDesignBox->findText(name);

    if(index != -1) {
        chooseDesignBox->setCurrentIndex(index);
        setShipDesign(index);
    }
}

void ShipDesignDialog::addNewDesign()
{
    Ship *ship = shipBeingEdited.get();

    QString name(shipNameEdit->text());

    if(chooseDesignBox->findText(name) != -1) {
        QMessageBox::critical(this, tr("Error"), tr("A design with such name already exists! Choose another name."));
        return;
    }

    ship->SetName(name.toStdString());

    long slot = currentDesignMode == SDDDM_SHIPS ? player->GetFreeShipDesignSlot()
        : player->GetFreeBaseDesignSlot();

    if(slot == -1) {
        QMessageBox::critical(this, tr("Error"), tr("There are no empty slots left for this design"));
        return;
    }

    if(currentDesignMode == SDDDM_SHIPS) {
        player->SetShipDesign(slot, ship);
    }
    else {
        player->SetBaseDesign(slot, ship);
    }

    shipBeingEdited.release();

    leaveEditMode();
    setViewMode(SDDVM_EXISTING);
    existingDesignsButton->setChecked(true);

    int index = chooseDesignBox->findText(name);

    if(index != -1) {
        chooseDesignBox->setCurrentIndex(index);
        setShipDesign(index);
    }
}

void ShipDesignDialog::abandonDesign()
{
    leaveEditMode();
    shipBeingEdited.reset(0);
    switchMode(currentDesignMode, currentDesignMode, currentViewMode, currentViewMode);
}

};
