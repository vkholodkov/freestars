/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <QFont>
#include <QBrush>

#include "production_queue_model.h"
#include "translations.h"

namespace FreeStars {

ProductionQueueModel::ProductionQueueModel(const Planet *_planet, const deque<ProdOrder*> &_production_queue, QObject *parent)
    : QAbstractTableModel(parent)
    , production_queue()
    , planet(_planet)
{
    for(auto po : _production_queue) {
      production_queue.push_back(po->Copy());
    }
}

ProductionQueueModel::~ProductionQueueModel()
{
  releaseAll();
}

void ProductionQueueModel::releaseAll()
{
    for(auto po : production_queue) {
      delete po;
    }
}

void ProductionQueueModel::setProductionQueue(const deque<ProdOrder*> &_production_queue)
{
  auto updateSize = std::min(production_queue.size(), _production_queue.size());

  int i = 0;

  if(updateSize != 0) {
    while(i != updateSize) {
      delete production_queue[i];
      production_queue[i] = _production_queue[i]->Copy();
      i++;
    }

    emit dataChanged(index(0, 0), index(i - 1, columnCount() - 1));
  }

  if(production_queue.size() < _production_queue.size()) {
    beginInsertRows(QModelIndex(), i, _production_queue.size() - 1);

    while(i != _production_queue.size()) {
      production_queue.push_back(_production_queue[i]->Copy());
      i++;
    }

    endInsertRows();
  }
  else if(production_queue.size() > _production_queue.size()) {

    beginRemoveRows(QModelIndex(), i, production_queue.size() - 1);

    auto start = production_queue.begin() + i;

    std::for_each(start, production_queue.end(), [](ProdOrder *po) { delete po; });
    production_queue.erase(start, production_queue.end());

    endRemoveRows();
  }
}

int ProductionQueueModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return production_queue.size();
}

int ProductionQueueModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

long ProductionQueueModel::getCompletionYears(int position) const {
    auto leftoverResources = planet->GetResources();
    int years = 1;

    for(auto i = 0 ; i != production_queue.size() ; i++) {

      auto *po = production_queue.at(i);
      auto cost = po->GetCost(planet);
      auto partials = po->GetPartial();
      auto requiredResources = cost.GetResources() - partials.GetResources();

      while(requiredResources > 0) { 
        if(requiredResources < leftoverResources) {
          requiredResources = 0;
          leftoverResources -= requiredResources;
        }
        else {
          leftoverResources += planet->GetResources();
          years++;
        }
      }

      if(i == position) {
        break;
      }
    }

    return years;
}

long ProductionQueueModel::getCompletionPct(int position) const {

    auto *po = production_queue.at(position);
    auto cost = po->GetCost(planet);
    auto partials = po->GetPartial();

    return cost.GetResources() != 0 ? partials.GetResources() * 100 / cost.GetResources() : 0;
}

QVariant ProductionQueueModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::TextAlignmentRole) {
        return QVariant((index.column() == 0 ? Qt::AlignLeft : Qt::AlignRight) | Qt::AlignVCenter);
    }

    if (index.row() >= production_queue.size() || index.row() < 0)
        return QVariant();

    ProdOrder *p = production_queue.at(index.row());

    if(role == Qt::FontRole) {

        QFont font;
        font.setBold(true);

        if(typeid(*p) == typeid(POAuto)) {
          font.setItalic(true);
        }

        auto cost = p->GetCost(planet);

        if(cost.GetResources() == 0) {
          font.setStrikeOut(true);
        }

        return QVariant(font);
    }

    if(role == Qt::ForegroundRole) {
        QBrush brush;

        auto years = getCompletionYears(index.row());

        if(years <= 1) {
          brush.setColor(Qt::darkGreen);
        }
        else if(years < 5) {
          brush.setColor(Qt::darkBlue);
        }

        return QVariant(brush);
    }

    if (role == Qt::DisplayRole) {

        if (index.column() == 0)
            return translate(p, planet);
        else if (index.column() == 1)
            return QString(p->AmountToString().c_str());
    }
    return QVariant();
}

QVariant ProductionQueueModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Name");

            case 1:
                return tr("Qty");

            default:
                return QVariant();
        }
    }
    return QVariant();
}

bool ProductionQueueModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    return false;
}

bool ProductionQueueModel::removeRows(int position, int rows, const QModelIndex &index)
{
    if(position < 0 || position > production_queue.size() || position + rows > production_queue.size()) {
      return false;
    }

    beginRemoveRows(QModelIndex(), position, position+rows-1);

    auto start = production_queue.begin() + position;
    auto end = start + rows;

    std::for_each(start, end, [](ProdOrder *po) { delete po; });

    production_queue.erase(start, end);

    endRemoveRows();
    return true;
}

bool ProductionQueueModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    int sourceFirst = sourceRow;
    int sourceLast = sourceRow + count - 1;

    if(sourceFirst < 0 || sourceLast >= rowCount(sourceParent) || destinationChild < 0) {
      return false;
    }

    beginMoveRows(sourceParent, sourceFirst, sourceLast, destinationParent, destinationChild);

    std::list<ProdOrder*> temp;

    auto start = production_queue.begin() + sourceFirst;
    auto end = start + count;

    std::copy(start, end, std::back_inserter(temp));

    production_queue.erase(start, end);

    if(destinationChild < sourceRow) {
      std::copy(temp.begin(), temp.end(), std::inserter(production_queue, production_queue.begin() + destinationChild));
    }
    else {
      std::copy(temp.begin(), temp.end(), std::inserter(production_queue, production_queue.begin() + destinationChild - count));
    }

    endMoveRows();
    return true;
}

bool ProductionQueueModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

Qt::ItemFlags ProductionQueueModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index);
}

bool ProductionQueueModel::addOrderFromTemplate(int position, const ProdOrder *orderTemplate, int amount)
{
    if(position < 0) {
      return false;
    }

    if(position < rowCount()) {
      auto order = production_queue.at(position);

      if(typeid(*order) == typeid(POShip) && typeid(*orderTemplate) == typeid(POShip)
        || typeid(*order) == typeid(POBase) && typeid(*orderTemplate) == typeid(POBase))
      {
        if(order->GetType() == orderTemplate->GetType()) {
          return mergeOrderWithTemplate(position, order, orderTemplate, amount);
        }
      }
      else if(typeid(*order) == typeid(POPlanetary) && typeid(*orderTemplate) == typeid(POPlanetary)) {
        if(order->GetType() == orderTemplate->GetType()) {
          return mergeOrderWithTemplate(position, order, orderTemplate, amount);
        }
      }
      else if(typeid(*order) == typeid(POAuto) && typeid(*orderTemplate) == typeid(POAuto)) {
        if(order->GetType() == orderTemplate->GetType()) {
          return mergeOrderWithTemplate(position, order, orderTemplate, amount);
        }
      }
    }

    return insertOrderFromTemplate(position, orderTemplate, amount);
}

bool ProductionQueueModel::removeOrder(int position, int amount)
{
    if(position < 0 || position >= rowCount()) {
      return false;
    }

    auto order = production_queue.at(position);

    if(order->GetAmount() > amount) {
      order->SetAmount(order->GetAmount() - amount);
      emit dataChanged(index(position, 0), index(position, columnCount()));
      return true;
    }
    else {
      return removeRows(position, 1);
    }
}

bool ProductionQueueModel::insertOrderFromTemplate(int position, const ProdOrder *orderTemplate, int amount)
{
    beginInsertRows(QModelIndex(), position, position);

    std::unique_ptr<ProdOrder> order(orderTemplate->Copy());

    order->SetAmount(amount);

    if(position == rowCount()) {
      production_queue.push_back(order.get());
    }
    else {
      auto i = std::next(production_queue.begin() + position);
      production_queue.insert(i, order.get());
    }

    order.release();

    endInsertRows();
    return true;
}

bool ProductionQueueModel::mergeOrderWithTemplate(int position, ProdOrder *order, const ProdOrder *orderTemplate, int amount)
{
    order->SetAmount(order->GetAmount() + amount);
    emit dataChanged(index(position, 0), index(position, columnCount()));
    return true;
}

};
