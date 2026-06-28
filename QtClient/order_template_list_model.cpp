/*
 * Copyright (C) 2014 Valery Kholodkov
 */
#include <QCoreApplication>
#include <QFont>
#include <QBrush>

#include "order_template_list_model.h"
#include "translations.h"

namespace FreeStars {


OrderTemplateListModel::OrderTemplateListModel(const Planet *_planet, QObject *parent)
    : QAbstractTableModel(parent)
    , planet(_planet)
    , order_template_list()
{
  auto owner = planet->GetOwner();

  const Ship *base = planet->GetBaseDesign();

  if(base != NULL) {
    for(auto n = 0 ; n != Rules::GetConstant("MaxShipDesigns") ; n++) {
      auto shipDesign = owner->GetShipDesign(n + 1);

      if(shipDesign) {
        order_template_list.push_back(new POShip(n + 1, 1));
      }
    }
  }
  else {
    for(auto n = 0 ; n != Rules::GetConstant("MaxBaseDesigns") ; n++) {
      auto baseDesign = owner->GetBaseDesign(n + 1);

      if(baseDesign) {
        order_template_list.push_back(new POBase(n + 1));
      }
    }
  }

  if(owner->ARTechType() < 0) {

    if(!planet->GetScanner()) {
      order_template_list.push_back(new POPlanetary(POP_SCANNER, 1));
    }

    order_template_list.push_back(new POPlanetary(POP_FACTS, 1));
    order_template_list.push_back(new POPlanetary(POP_MINES, 1));
    order_template_list.push_back(new POPlanetary(POP_DEFS, 1));
  }

  order_template_list.push_back(new POPlanetary(POP_ALCHEMY, 1));

  if(owner->ARTechType() < 0) {
    order_template_list.push_back(new POAuto(POP_FACTS, 1));
    order_template_list.push_back(new POAuto(POP_MINES, 1));
    order_template_list.push_back(new POAuto(POP_DEFS, 1));
  }

  order_template_list.push_back(new POAuto(POP_ALCHEMY, 1));

  if(base != NULL && base->GetDriverSpeed() > 0) {
    order_template_list.push_back(new POPacket(-1, 1));
    order_template_list.push_back(new POPacket(0, 1));
    order_template_list.push_back(new POPacket(1, 1));
    order_template_list.push_back(new POPacket(2, 1));
  }
}

OrderTemplateListModel::~OrderTemplateListModel()
{
  releaseAll();
}

void OrderTemplateListModel::releaseAll()
{
    for(auto po : order_template_list) {
      delete po;
    }
}
int OrderTemplateListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return order_template_list.size();
}

int OrderTemplateListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

long OrderTemplateListModel::getCompletionYears(int position) const {

    auto *po = order_template_list.at(position);

    if(typeid(*po) == typeid(POShip) || typeid(*po) == typeid(POBase)
      || (typeid(*po) == typeid(POPlanetary) && po->GetType() == POP_SCANNER)) {
      auto availableResources = planet->GetResources();
      auto cost = po->GetCost(planet);
      auto requiredResources = cost.GetResources();
      return (requiredResources / availableResources) + 1;
    }

    return -1;
}

QVariant OrderTemplateListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::TextAlignmentRole) {
        return QVariant((index.column() == 0 ? Qt::AlignLeft : Qt::AlignRight) | Qt::AlignVCenter);
    }

    if (index.row() >= order_template_list.size() || index.row() < 0)
        return QVariant();

    ProdOrder *p = order_template_list.at(index.row());

    if(role == Qt::FontRole) {

        QFont font;
        font.setBold(true);

        if(typeid(*p) == typeid(POAuto)) {
          font.setItalic(true);
        }

        return QVariant(font);
    }

    if(role == Qt::ForegroundRole) {
        QBrush brush;

        auto years = getCompletionYears(index.row());

        if(years > 0) {
          if(years <= 1) {
            brush.setColor(Qt::darkGreen);
          }
          else if(years < 5) {
            brush.setColor(Qt::darkBlue);
          }
        }

        return QVariant(brush);
    }

    if (role == Qt::DisplayRole) {
        if (index.column() == 0) {
          return translate(p, planet);
        }
    }
    return QVariant();
}

QVariant OrderTemplateListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Name");

            default:
                return QVariant();
        }
    }
    return QVariant();
}

bool OrderTemplateListModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    return false;
}

bool OrderTemplateListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

Qt::ItemFlags OrderTemplateListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}

};
