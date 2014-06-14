/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _SHIP_DESIGN_DIALOG_H_
#define _SHIP_DESIGN_DIALOG_H_

#include <memory>

#include <QDialog>
#include <QButtonGroup>
#include <QDragEnterEvent>
#include <QDragMoveEvent>

#include "FSServer.h"

#include "graphics_array.h"
#include "slot_widget.h"

#include "ui_ship_design_dialog.h"

namespace FreeStars {

struct CompCategory;

class ShipDesignDialog : public QDialog, private Ui_ShipDesignDialog {
    Q_OBJECT

    typedef enum {
        SDDDM_NONE, SDDDM_SHIPS, SDDDM_STARBASES
    } design_mode_t;

    typedef enum {
        SDDVM_NONE, SDDVM_EXISTING, SDDVM_AVAILABLE, SDDVM_ENEMY, SDDVM_COMPONENTS
    } view_mode_t;

public:
    ShipDesignDialog(Player*, const GraphicsArray*, QWidget *parent = 0);
    virtual ~ShipDesignDialog();

protected:
    void paintEvent(QPaintEvent*);

private slots:
    void setDesignMode(int);
    void setViewMode(int);
    void setComponentCategory(QComboBox*, QListWidget*, int);
    void setComponentCategory1(int);
    void setComponentCategory2(int);
    void setShipDesign(int);
    void setHull(int);
    void copyDesign();
    void editDesign();
    void deleteDesign();

    void saveDesign();
    void abandonDesign();

private:
    void switchMode(int, int, int, int);
    void populateComponentCategoryList();
    void populateExistingDesigns(int);
    void populateAvailableHullTypes(int);
    void clearProperties();

    void enterEditMode();
    void leaveEditMode();

    void drawShip(const Ship*);
    QPoint getWireframeOrigin(const QRect&) const;
    void collectSlotDimensions(const Hull*, std::vector<QRect>&, QRect&);
    void createHullWidgets(const Hull*);
    void deleteFloatingWidgets();
    
    void drawSlot(QPainter&, const Slot&, const QRect&);
    void drawComponent(QPainter&, const Slot&, const Slot&, const QRect&);

    static QString describeSlot(const Slot&);

private:
    bool editing;
    QImage plateImage;
    const GraphicsArray *graphicsArray;
    Player *player;
    int currentDesignMode, currentViewMode;
    Ship *currentShip;
    Hull *currentHull;
    std::auto_ptr<Ship> shipBeingEdited;
    std::vector<QWidget*> slotWidgets;
};

};

#endif
