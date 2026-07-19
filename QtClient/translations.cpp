
#include <QCoreApplication>

#include "translations.h"

namespace FreeStars {

typedef struct {
  const char *name;
  QString translation;
} OrderDescription;

const char *ORDER_DESCRIPTION_SECTION = "OrderDescription";

OrderDescription orderDescriptions[] = {

  { "AutoMinTerra", QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Autoterraform minimal") },
  { "AutoMaxTerra", QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Autoterraform maximal") },
  { "Terraform", QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Terraform") },
  { "Factories", QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Factories") },
  { "Mines", QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Mines") },
  { "Defenses", QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Defenses") },
  { "Alchemy", QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Mineral Alchemy") },
  { "Scanner", QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Planetary Scanner") },
  { "AutoMine", QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Mines (Auto Build)") },
  { "AutoFactory", QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Factories (Auto Build)") },
  { "AutoDefense", QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Defenses (Auto Build)") },
  { "AutoAlchemy", QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Alchemy (Auto Build)") },
  { "AutoPacket", QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Mineral Packets (Auto Build)") },

  { NULL, QString() }
};

string POBaseHumanReadableName(const POBase *po, const Planet *planet)
{
	const Player * owner = planet->GetOwner();
  auto baseDesign = owner->GetBaseDesign(po->GetType());

	// check for invalid designs
	if (!baseDesign || !baseDesign->IsValidDesign(owner)) {
		return po->TypeToString();
	}
  else {
    return baseDesign->GetName();
  }
}

string POShipHumanReadableName(const POShip *po, const Planet *planet)
{
  auto owner = planet->GetOwner();
  auto shipDesign = owner->GetShipDesign(po->GetType());

	if (!shipDesign || !shipDesign->IsValidDesign(owner)) {
    return po->TypeToString();
	}
  else {
    return shipDesign->GetName();
  }
}

QString POPacketTranslation(const POPacket *po)
{
  if(po->GetType() == POP_MIXEDPACKET + 1) {
    return QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Build Ironium Packet");
  }
  else if(po->GetType() == POP_MIXEDPACKET + 2) {
    return QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Build Boranium Packet");
  }
  else if(po->GetType() == POP_MIXEDPACKET + 3) {
    return QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Build Germanium Packet");
  }
  else {
    return QCoreApplication::translate(ORDER_DESCRIPTION_SECTION, "Build Mixed Packet");
  }
}

QString translate(const ProdOrder *p, const Planet *planet)
{
  if(typeid(*p) == typeid(POBase)) {
    return QString(POBaseHumanReadableName(dynamic_cast<const POBase*>(p), planet).c_str());
  }
  else if(typeid(*p) == typeid(POShip)) {
    return QString(POShipHumanReadableName(dynamic_cast<const POShip*>(p), planet).c_str());
  }
  else if(typeid(*p) == typeid(POPacket)) {
    return POPacketTranslation(dynamic_cast<const POPacket*>(p));
  }
  else {
    OrderDescription *od = orderDescriptions;

    while(od->name != NULL) {
      if(p->TypeToString() == od->name) {
        return od->translation;
      }
      od++;
    }
    return QString(p->TypeToString().c_str());
  }
}

};
