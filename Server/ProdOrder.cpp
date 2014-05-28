/*
Copyright 2003 - 2005 Elliott Kleinrock, Dan Neely, Kurt W. Over, Damon Domjan

This file is part of FreeStars, a free clone of the Stars! game.

FreeStars is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

FreeStars is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with FreeStars; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

The full GPL Copyright notice should be in the file COPYING.txt

Contact:
Email Elliott at 9jm0tjj02@sneakemail.com
*/

#include <typeinfo>

#include "FSServer.h"

#include "Packet.h"
#include "Order.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

ProdOrder::ProdOrder()
: Type(0), Amount(0)
{
	init();
}

ProdOrder::ProdOrder(long type, long amount)
: Type(type), Amount(amount)
{
	init();
}

ProdOrder::~ProdOrder()
{
}

void ProdOrder::init()
{
}

ProdOrder * ProdOrder::Copy() const
{
	if (typeid(*this) == typeid(POShip))
		return new POShip(dynamic_cast<const POShip &>(*this));
	else if (typeid(*this) == typeid(POBase))
		return new POBase(dynamic_cast<const POBase &>(*this));
	else if (typeid(*this) == typeid(POPlanetary))
		return new POPlanetary(dynamic_cast<const POPlanetary &>(*this));
	else if (typeid(*this) == typeid(POPacket))
		return new POPacket(dynamic_cast<const POPacket &>(*this));
	else if (typeid(*this) == typeid(POTerraform))
		return new POTerraform(dynamic_cast<const POTerraform &>(*this));
	else if (typeid(*this) == typeid(POAuto))
		return new POAuto(dynamic_cast<const POAuto &>(*this));
	else {
//		assert(false);
		return NULL;
	}
}

deque<ProdOrder *> ProdOrder::ParseNode(const TiXmlNode * node, Planet * planet, Player * player /*= NULL*/, bool TrustPartials /*= false*/)
{
	assert(planet != NULL || player != NULL);
	assert(node != NULL);

	if (player == NULL)
		player = planet->NCGetOwner();

	const TiXmlNode * child;

	deque<ProdOrder *> neword;
	ProdOrder * po = NULL;
	long type;
	long num;

	for (child = node->FirstChild(); child; child = child->NextSibling()) {
		if (child->Type() == TiXmlNode::COMMENT)
			continue;

		po = NULL;
		if (stricmp(child->Value(), "Planet") == 0) {
			continue; // skip
		} else if (stricmp(child->Value(), "Ship") == 0) {
			type = GetLong(child->FirstChild("Design"));
			if (type <= 0 || type > Rules::GetConstant("MaxShipDesigns")) {
				Message * mess = player->AddMessage("Error: Invalid ship design type number");
				mess->AddLong("", type);
				continue;
			}
			num = GetLong(child->FirstChild("Number"));
			po = new POShip(type, num);
			po->CheckPartials(planet, child, TrustPartials);
		} else if (stricmp(child->Value(), "Base") == 0) {
			type = GetLong(child->FirstChild("Design"));
			if (type <= 0 || type > Rules::GetConstant("MaxBaseDesigns")) {
				Message * mess = player->AddMessage("Error: Invalid base design");
				mess->AddLong("", type);
				continue;
			}
			po = new POBase(type);
			po->CheckPartials(planet, child, TrustPartials);
		} else if (stricmp(child->Value(), "Factories") == 0) {
			num = GetLong(child->FirstChild("Number"));
			po = new POPlanetary(POP_FACTS, num);
			po->CheckPartials(planet, child, TrustPartials);
		} else if (stricmp(child->Value(), "Mines") == 0) {
			num = GetLong(child->FirstChild("Number"));
			po = new POPlanetary(POP_MINES, num);
			po->CheckPartials(planet, child, TrustPartials);
		} else if (stricmp(child->Value(), "Defenses") == 0) {
			num = GetLong(child->FirstChild("Number"));
			po = new POPlanetary(POP_DEFS, num);
			po->CheckPartials(planet, child, TrustPartials);
		} else if (stricmp(child->Value(), "Alchemy") == 0) {
			num = GetLong(child->FirstChild("Number"));
			po = new POPlanetary(POP_ALCHEMY, num);
			po->CheckPartials(planet, child, TrustPartials);
		} else if (stricmp(child->Value(), "Scanner") == 0) {
			po = new POPlanetary(POP_SCANNER, 1);
			po->CheckPartials(planet, child, TrustPartials);
		} else if (stricmp(child->Value(), "Packet") == 0) {
			num = GetLong(child->FirstChild("Number"));
			const char * ptr = GetString(child->FirstChild("Type"));
			long t;
			if (stricmp(ptr, "Mixed") == 0)
				t = -1;
			else {
				t = Rules::MineralID(GetString(child->FirstChild("Type")));
				if (t < 0) {
					Message * mess = player->AddMessage("Error: Invalid packet type");
					mess->AddItem("", GetString(child->FirstChild("Type")));
					continue;
				}
			}

			po = new POPacket(t, num);
			po->CheckPartials(planet, child, TrustPartials);
		} else if (stricmp(child->Value(), "Terraform") == 0) {
			num = GetLong(child->FirstChild("Number"), 1);
			po = new POTerraform(POP_TERRAFORM, num);
			po->CheckPartials(planet, child, TrustPartials);
		} else if (stricmp(child->Value(), "AutoMine") == 0) {
			num = GetLong(child);
			po = new POAuto(POP_MINES, num);
		} else if (stricmp(child->Value(), "AutoFactory") == 0) {
			num = GetLong(child);
			po = new POAuto(POP_FACTS, num);
		} else if (stricmp(child->Value(), "AutoDefense") == 0) {
			num = GetLong(child);
			po = new POAuto(POP_DEFS, num);
		} else if (strnicmp(child->Value(), "AutoAlchemy", 11) == 0) {
			po = new POAuto(POP_ALCHEMY, 1);	// on or off, no number required
		} else if (stricmp(child->Value(), "AutoMinTerra") == 0) {
			num = GetLong(child);
			po = new POAuto(POP_MINTERRA, num);
		} else if (stricmp(child->Value(), "AutoMaxTerra") == 0) {
			num = GetLong(child);
			po = new POAuto(POP_MAXTERRA, num);
		} else if (stricmp(child->Value(), "AutoPacket") == 0) {
			num = GetLong(child);
			po = new POAuto(POP_MIXEDPACKET, num);
		} else {
			Message * mess = player->AddMessage("Warning: Unknown production entry");
			mess->AddItem("", child->Value());
			continue;
		}

		neword.push_back(po);
	}

	return neword;
}

TiXmlNode * ProdOrder::WriteNode(TiXmlNode * node, const deque<ProdOrder *> & ords)
{
	TiXmlElement * PQ = new TiXmlElement("ProductionQueue");
	for (int i = 0; i < ords.size(); ++i)
		ords[i]->WriteNode(PQ);

	node->LinkEndChild(PQ);
	return PQ;
}

TiXmlNode * ProdOrder::WriteNode(TiXmlNode * node) const
{
	return Partial.WriteCosts(node, "Partial");
}

// <Partial><Resources>4</Resources><Mineral Name="Ironium">2<Mineral>...</Partial>

// <Partial>\([0-9]*\) \([0-9]*\) \([0-9]*\) \([0-9]*\) [0-9]*</Partial>

// <Partial><Resources>\1</Resources><Mineral Name="Ironium">\2</Mineral><Mineral Name="Boranium">\3</Mineral><Mineral Name="Germanium">\4</Mineral></Partial>

void ProdOrder::CheckPartials(Planet * planet, const TiXmlNode * node, bool TrustPartials)
{
	if (node == NULL || planet == NULL)
		return;

	Partial.ReadCosts(node->FirstChild("Partial"));
	if (TrustPartials)
		return;

	deque<ProdOrder *>::iterator iter;
	for (iter = planet->mProductionQ.begin(); iter != planet->mProductionQ.end(); ++iter)
	{
		if (typeid(*iter) == typeid(this) && Type == (*iter)->Type)
		{
			if(Partial == (*iter)->Partial)
			{
				break; // have a match
			}
				/*
			// Check Resources
			if (Partial[RESOURCES] != (*iter)->Partial[RESOURCES])
				continue;	// not equal stop now

			// Check Crew
			if (Partial[POPULATION] != (*iter)->Partial[POPULATION])
				continue;	// not equal stop now


			// Check Minerals
			for (int i = 0; i < Rules::MaxMinType; ++i)
			{
				if (Partial[i] == (*iter)->Partial[i])
					continue; // not equal stop now
			}


			if (i > Rules::MaxMinType)
			// if we didn't find a problem, this is a match
			break;*/
		}
	}

	if (iter == planet->mProductionQ.end()) {
		if (!Partial.IsZero()) {
			// if we don't have a match, use no partial production
			Message * mess = planet->NCGetOwner()->AddMessage("Warning: Incorrect partial values", planet);
			mess->AddItem("Item being built", TypeToString());
			mess->AddLong("Number ordered", Amount);
			Partial.Zero();
		}
	} else {
		// zero remembered partial, so it cannot be duplicated
		(*iter)->Partial.Zero();
	}
}

/*
Object variables used:
Amount: number of items to build
Partial: work previously done

Parameters:
cost: cost of one item
planet: where it's being built
resources: how many resources are still available, update as used
AutoAlchem: true if the prior item is AutoAlchem, set to false when done
maxbuild: max # of items to build, -1 for auto items (which means use Amount)

locals:
Build: Number to build, including partials
fb: fraction part of Build, for partial completion
lBuilt: count of items built, pass to Built function to actually build them
Auto: true if in an auto item, false otherwise


Build = max(Amount, resources&minerals + Partial / cost)
res&min -= int(Build) * cost - Partial
if Build < Amount
	Partial = fraction(Build) * cost
	res&min -= fraction(Build) * cost

amount=1
cost = 9r 3g
4 res
Build= 4/9
Partial = 4, 1 3/9 -> 4 2

2g
Build= 2/3
Partial = 6 2

cost = 3r 9g
2 res
Build= 2/3
Partial = 2, 6

2g
Build= 2/9
Partial = 0 2 -> which means that no work is done, and the g shouldn't get spent either
4g
Build= 4/9
Partial = 1 4

cost = 5r 2g
1r
Build = 1/5
Partial = 1 1
1g
Build = 1/2
Partial = 2 1
*/

bool ProdOrder::DoProduce(const Cost & cost, Planet * planet, long * resources, bool * AutoAlchemy, long maxbuild /*= -1*/)
{
	// maxbuild should only be provided for auto build items
	assert(maxbuild == -1 || dynamic_cast<POAuto *>(this) != NULL || dynamic_cast<POTerraform *>(this) != NULL);

	Player * owner = planet->NCGetOwner();
	CargoType ct;

	double Build;
	int lBuilt = 0;
	bool Auto = false;
	if (maxbuild > 0)
		Auto = true;
	else
		maxbuild = Amount;

	// Partial completion is limited to cost
	Partial.SetResources(min(cost.GetResources(), Partial.GetResources()));
	Partial.SetCrew(min(cost.GetCrew(), Partial.GetCrew()));
	for (ct = 0; ct < Rules::MaxMinType; ++ct)
		Partial[ct] = min(cost[ct], Partial[ct]);

	do {
		// Add partial to stockpile
		if (Partial.GetResources() > 0) {
			*resources += Partial.GetResources();
			planet->AdjustPopulation(Partial.GetCrew());
			for (ct = 0; ct < Rules::MaxMinType; ++ct)
				planet->AdjustAmounts(ct, Partial[ct]);

			Partial.Zero();
		}

		// How much can we get done?
		// Build = max(maxbuild, resources&minerals + Partial / cost)
		Build = maxbuild;
		if (cost.GetResources() > 0)
			Build = min(Build, double(*resources) / cost.GetResources());

		if (cost.GetCrew() > 0)
			Build = min(Build, double(planet->GetPopulation()) / cost.GetCrew());

		for (ct = 0; ct < Rules::MaxMinType; ++ct)
			if (cost[ct] > 0)
				Build = min(Build, double(planet->GetContain(ct)) / cost[ct]);

		// res&min -= int(Build) * cost - Partial
		if (long(Build) >= 1) {
			*resources -= long(Build) * cost.GetResources();
			planet->AdjustPopulation(-(long(Build) * cost.GetCrew()));
			for (ct = 0; ct < Rules::MaxMinType; ++ct)
				planet->AdjustAmounts(ct, -(long(Build) * cost[ct]));
		}

		lBuilt += long(Build);

		maxbuild -= long(Build);

		if (maxbuild > 0) {
			if (!Auto)
				BuildPartial(cost, planet, resources, Build);

			// Build alchemy
			if ((*AutoAlchemy || (!Auto && owner->GetResearchField() == RESEARCH_ALCHEMY)) && *resources > 0) {
				if (*resources >= long(Rules::GetConstant("AlchemyCost") * owner->ComponentCostFactor(CT_ALCHEMY))) {
					planet->BuildAlchemy(1);
					*resources -= long(Rules::GetConstant("AlchemyCost") * owner->ComponentCostFactor(CT_ALCHEMY));
					for (CargoType ct = 0; ct < Rules::MaxMinType; ++ct)
						planet->AdjustAmounts(ct, 1);
				} else {
					POPlanetary * alchem = new POPlanetary(POP_ALCHEMY, 1);
					alchem->Partial.SetResources(*resources);
					*resources = 0;
					planet->mProductionQ.push_front(alchem);
				}
			}
		}
	} while (maxbuild > 0 && *resources > 0 && (*AutoAlchemy || (!Auto && owner->GetResearchField() == RESEARCH_ALCHEMY)));

	if (maxbuild > 0 && *resources > 0) { // AutoAlchemy is false
		if (Auto) {
			BuildPartial(cost, planet, resources, Build);
			if (Partial.GetResources() > 0) {	// if resources are 0, then no work is done, and don't allocate anything else to it yet
				ProdOrder * item = NULL;
				if (Type >= POP_MIXEDPACKET && Type <= POP_MIXEDPACKET + Rules::MaxMinType) {
					item = new POPacket(Type - POP_MIXEDPACKET - 1, 1);
				} else if (Type == POP_MINTERRA || Type == POP_MAXTERRA)
					item = new POTerraform(POP_TERRAFORM, 1);
				else if (Type == POP_TERRAFORM)
					item = NULL;
				else	// should only be POP_FACTS, POP_MINES, or POP_DEFS
					item = new POPlanetary(Type, 1);

				if (item != NULL) {
					item->Partial = Partial;
					Partial.Zero();
					planet->mProductionQ.push_front(item);
				}
			}
		} else {
			// left over resources go to research
			owner->GainTech(*resources);
			*resources = 0;
		}
	}

	if (!Auto)	Amount = maxbuild;
	Built(planet, lBuilt);	// Actually build them
	*AutoAlchemy = false;	// always turn off auto-alchemy
	return(!Auto && Amount == 0);		// if we're done, delete it
}

void ProdOrder::BuildPartial(const Cost & cost, Planet * planet, long * resources, double Build)
{
	// Partial = fraction(Build) * cost
	double dummy;
	double fb = modf(Build, &dummy);

	// Can't spend resources till all needed raw materials are on hand.
	// for example: 3res 9iron item, with 4 iron on hand, since minerals must be spent before resources, you can only do 1r 4i so far
	// another example 5res 4iron with 3 iron you can put in 3res 3iron. If limited to 2res you do 2res 2iron
	Partial.Zero();
	Partial.SetResources(long(fb * cost.GetResources()));
	fb = double(Partial.GetResources()) / cost.GetResources();
	*resources -= Partial.GetResources();
	if (Partial.GetResources() > 0) {	// if resources are 0, then no work is done, and don't allocate anything else to it yet
		// Crew doesn't get on till it's all done.
		for (CargoType ct = 0; ct < Rules::MaxMinType; ++ct) {
			Partial[ct] = long(fb * cost[ct] + 1.0 - epsilon);
			planet->AdjustAmounts(ct, -Partial[ct]);
		}
	}
}

POAuto::~POAuto()
{
}

TiXmlNode * POAuto::WriteNode(TiXmlNode * node) const
{
	string str;
	str = TypeToString();

	if (str.empty())
		TheGame->AddMessage("Error: Invalid auto procution order");
	else
		AddLong(node, str.c_str(), Amount);

	return node;
}

string POAuto::TypeToString() const
{
	string str;
	if (Type == POP_MINES)
		str = "AutoMine";
	else if (Type == POP_FACTS)
		str = "AutoFactory";
	else if (Type == POP_DEFS)
		str = "AutoDefense";
	else if (Type == POP_ALCHEMY)
		str = "AutoAlchemy";
	else if (Type == POP_MIXEDPACKET)
		str = "AutoPacket";
	else if (Type == POP_MINTERRA)
		str = "AutoMinTerra";
	else if (Type == POP_MAXTERRA)
		str = "AutoMaxTerra";
	else
		TheGame->AddMessage("Error: Invalid auto procution order");

	return str;
}

bool POAuto::Produce(Planet * planet, long * resources, bool * AutoAlchemy)
{
	const Player * owner = planet->GetOwner();

	if (Type == POP_MINES) {
		if (owner->ARTechType() >= 0) {
			// AR trying to build planetary stuff, send message and delete from queue
			return true;
		}

		long maxbuild = min(Amount, planet->MaxMines() - planet->GetMines());
		if (maxbuild > 0)
			DoProduce(owner->MineCost(), planet, resources, AutoAlchemy, maxbuild);

		// auto itmes never get deleted fromt the queue
	} else if (Type == POP_FACTS) {
		if (owner->ARTechType() >= 0) {
			// AR trying to build planetary stuff, send message and delete from queue
			return true;
		}

		long maxbuild = min(Amount, planet->MaxFactories() - planet->GetFactories());
		if (maxbuild > 0)
			DoProduce(owner->FactoryCost(), planet, resources, AutoAlchemy, maxbuild);

		// auto itmes never get deleted fromt the queue
	} else if (Type == POP_DEFS) {
		if (owner->ARTechType() >= 0) {
			// AR trying to build planetary stuff, send message and delete from queue
			return true;
		}

		long maxbuild = min(Amount, planet->MaxDefenses() - planet->GetDefenses());
		if (maxbuild > 0) {
			assert(Component::DefenseCost());
			Cost cost;
			cost.SetResources(long(Component::DefenseCost()->GetResources() * owner->ComponentCostFactor(CT_DEFENSE) + .5));
			cost.SetCrew(long(Component::DefenseCost()->GetCrew() * owner->ComponentCostFactor(CT_DEFENSE) + .5));
			for (CargoType ct = 0; ct < Rules::MaxMinType; ++ct)
				cost[ct] = long((*Component::DefenseCost())[ct] * owner->ComponentCostFactor(CT_DEFENSE) + .5);

			DoProduce(cost, planet, resources, AutoAlchemy, maxbuild);
		}

		// auto itmes never get deleted fromt the queue
	} else if (Type == POP_ALCHEMY) {
		*AutoAlchemy = true;
		// auto itmes never get deleted fromt the queue
	} else if (Type == POP_MINTERRA) {
		// auto itmes never get deleted fromt the queue
	} else if (Type == POP_MAXTERRA) {
		// auto itmes never get deleted fromt the queue
	} else if (Type >= POP_MIXEDPACKET && Type <= POP_MIXEDPACKET + Rules::MaxMinType) {
		if (POPacket::CheckPacket(planet))
			DoProduce(planet->GetPacketCost(Type - POP_MIXEDPACKET - 1), planet, resources, AutoAlchemy, Amount);
		// auto itmes never get deleted fromt the queue
	} else {
		Message * mess = planet->NCGetOwner()->AddMessage("Error: Invalid Production type", planet);
		mess->AddLong("Auto", Type);
		return true;	// delete invalid items
	}

	return false;	// auto itmes never get deleted fromt the queue
}


POBase::~POBase()
{
}

TiXmlNode * POBase::WriteNode(TiXmlNode * node) const
{
	TiXmlElement POB("Base");
	AddLong(&POB, "Design", Type);
	ProdOrder::WriteNode(&POB);
	node->InsertEndChild(POB);

	return node;
}

string POBase::TypeToString() const
{
	string str;
	str = "Base design ";
	str += Long2String(Type);
	return str;
}

bool POBase::Produce(Planet * planet, long * resources, bool * AutoAlchemy)
{
	Cost cost;

	const Player * owner = planet->GetOwner();
	if (planet->GetBaseNumber() == Type)	// we've already got one
		return true;

	// check for invalid designs
	if (!owner->GetShipDesign(Type) || !owner->GetShipDesign(Type)->IsValidDesign(owner)) {
		Message * mess = planet->NCGetOwner()->AddMessage("Error: Invalid Production type", planet);
		mess->AddLong("Base", Type);
		return true;
	}

	if (planet->GetBaseNumber() > 0)
		cost = owner->GetBaseDesign(Type)->GetCost(owner, planet->GetBaseDesign(), planet);
	else
		cost = owner->GetBaseDesign(Type)->GetCost(owner);

	return DoProduce(cost, planet, resources, AutoAlchemy);
}

void POBase::Built(Planet * planet, long number)
{
	assert(number == 1);
	planet->SetBaseNumber(Type);
	Message * mess = planet->NCGetOwner()->AddMessage("Base built", planet);
	mess->AddLong("BaseDesign", Type);
}


POPacket::~POPacket()
{
}

TiXmlNode * POPacket::WriteNode(TiXmlNode * node) const
{
	TiXmlElement POP("Packet");
	if (Type == POP_MIXEDPACKET)
		AddString(&POP, "Type", "Mixed");
	else
		AddString(&POP, "Type", Rules::GetCargoName(Type - POP_MIXEDPACKET - 1).c_str());
	AddLong(&POP, "Number", Amount);
	ProdOrder::WriteNode(&POP);
	node->InsertEndChild(POP);

	return node;
}

string POPacket::TypeToString() const
{
	string str;
	str = "Packet ";
	str += Long2String(Type - POP_MIXEDPACKET - 1);
	return str;
}

bool POPacket::CheckPacket(Planet * planet)
{
	if (planet->GetBaseNumber() < 0 || planet->GetBaseDesign()->GetDriverSpeed() <= 0) {
		planet->NCGetOwner()->AddMessage("Error: Packet without driver", planet);
		return false;
	} else if (planet->GetPacketSpeed() > planet->GetBaseDesign()->GetDriverSpeed() + Rules::GetConstant("MaxPacketOverFling")) {
		Message * mess = planet->NCGetOwner()->AddMessage("Error: Invalid packet speed", planet);
		mess->AddLong("Driver safe speed", planet->GetBaseDesign()->GetDriverSpeed());
		mess->AddLong("Packet speed", planet->GetPacketSpeed());
		return false;
	} else if (planet->GetPacketDest() == NULL) {
		planet->NCGetOwner()->AddMessage("Error: Packet without destination", planet);
		return false;
	} else
		return true;
}

bool POPacket::Produce(Planet * planet, long * resources, bool * AutoAlchemy)
{
	if (!CheckPacket(planet))
		return true;

	return DoProduce(planet->GetPacketCost(Type - POP_MIXEDPACKET - 1), planet, resources, AutoAlchemy);
}


POPlanetary::~POPlanetary()
{
}

TiXmlNode * POPlanetary::WriteNode(TiXmlNode * node) const
{
	string type = TypeToString();

	if (type.empty())
		return node;

	TiXmlElement POP(type.c_str());
	AddLong(&POP, "Number", Amount);
	ProdOrder::WriteNode(&POP);
	node->InsertEndChild(POP);

	return node;
}

string POPlanetary::TypeToString() const
{
	string str;
	switch (Type) {
	case POP_FACTS:
		str = "Factories";
		break;
	case POP_MINES:
		str = "Mines";
		break;
	case POP_DEFS:
		str = "Defenses";
		break;
	case POP_ALCHEMY:
		str = "Alchemy";
		break;
	case POP_SCANNER:
		str = "Scanner";
		break;
	default:
		TheGame->AddMessage("Error: Invalid planetary procution order");
		break;
	}

	return str;
}

bool POPlanetary::Produce(Planet * planet, long * resources, bool * AutoAlchemy)
{
	const Player * owner = planet->GetOwner();
	if (Type == POP_FACTS) {
		if (owner->ARTechType() >= 0) {
			// AR trying to build planetary stuff, send message and delete from queue
			return true;
		}

		long max = planet->MaxFactories();
		if (Amount + planet->GetFactories() > max) {
			// send message too
			Amount = max - planet->GetFactories();
		}

		return DoProduce(owner->FactoryCost(), planet, resources, AutoAlchemy);
	} else if (Type == POP_MINES) {
		if (owner->ARTechType() >= 0) {
			// AR trying to build planetary stuff, send message and delete from queue
			return true;
		}

		long max = planet->MaxMines();
		if (Amount + planet->GetMines() > max) {
			// send message too
			Amount = max - planet->GetMines();
		}

		return DoProduce(owner->MineCost(), planet, resources, AutoAlchemy);
	} else if (Type == POP_DEFS) {
		if (owner->ARTechType() >= 0) {
			// AR trying to build planetary stuff, send message and delete from queue
			return true;
		}

		long max = planet->MaxDefenses();
		if (Amount + planet->GetDefenses() > max) {
			// send message too
			Amount = max - planet->GetDefenses();
		}

		assert(Component::DefenseCost());
		Cost cost;
		cost.SetResources(long(Component::DefenseCost()->GetResources() * owner->ComponentCostFactor(CT_DEFENSE) + .5));
		cost.SetCrew(long(Component::DefenseCost()->GetCrew() * owner->ComponentCostFactor(CT_DEFENSE) + .5));
		for (CargoType ct = 0; ct < Rules::MaxMinType; ++ct)
			cost[ct] = long((*Component::DefenseCost())[ct] * owner->ComponentCostFactor(CT_DEFENSE) + .5);

		return DoProduce(cost, planet, resources, AutoAlchemy);
	} else if (Type == POP_ALCHEMY) {
		Cost cost;
		cost.SetResources(long(Rules::GetConstant("AlchemyCost") * owner->ComponentCostFactor(CT_ALCHEMY) + .5));
		cost.SetCrew(0);
		for (CargoType ct = 0; ct < Rules::MaxMinType; ++ct)
			cost[ct] = 0;

		return DoProduce(cost, planet, resources, AutoAlchemy);
	} else if (Type == POP_SCANNER) {
		if (owner->ARTechType() >= 0) {
			// AR trying to build planetary stuff, send message and delete from queue
			return true;
		}

		assert(Amount == 1);
		if (planet->GetScanner()) {
			// send message too
			return true;
		}

		assert(Component::ScannerCost());
		Cost cost;
		cost.SetResources(long(Component::ScannerCost()->GetResources() * owner->ComponentCostFactor(CT_PLANSCAN) + .5));
		cost.SetCrew(long(Component::ScannerCost()->GetCrew() * owner->ComponentCostFactor(CT_PLANSCAN) + .5));
		for (CargoType ct = 0; ct < Rules::MaxMinType; ++ct)
			cost[ct] = long((*Component::ScannerCost())[ct] * owner->ComponentCostFactor(CT_PLANSCAN) + .5);

		return DoProduce(cost, planet, resources, AutoAlchemy);
	} else {
		Message * mess = planet->NCGetOwner()->AddMessage("Error: Invalid Production type", planet);
		mess->AddLong("Planetary", Type);
		return true;
	}
}

void POPlanetary::Built(Planet * planet, long number)
{
	if (Type >= POP_MIXEDPACKET && Type <= POP_MIXEDPACKET + Rules::MaxMinType) {
		Packet * p;
		p = new Packet(*planet, planet->GetPacketSpeed(), planet->GetBaseDesign()->GetDriverSpeed(), planet->GetPacketDest());
		if (Type == POP_MIXEDPACKET) {
			for (long i = 0; i < Rules::MaxMinType; ++i)
				p->AdjustAmounts(i, planet->GetOwner()->PacketSizeMixed() * number);
		} else {
			p->AdjustAmounts(Type - POP_MIXEDPACKET - 1, planet->GetOwner()->PacketSizeOneMin() * number);
		}

		TheGalaxy->AddPacket(p, planet);
	} else if (Type == POP_FACTS) {
		planet->BuildFactories(number);
	} else if (Type == POP_MINES) {
		planet->BuildMines(number);
	} else if (Type == POP_DEFS) {
		planet->BuildDefenses(number);
	} else if (Type == POP_ALCHEMY) {
		planet->BuildAlchemy(number);
		for (CargoType ct = 0; ct < Rules::MaxMinType; ++ct)
			planet->AdjustAmounts(ct, number);
	} else if (Type == POP_SCANNER) {
		planet->NCGetOwner()->AddMessage("Built scanner", planet);
		assert(number == 1);
		planet->BuildScanner();
	}
}


POShip::~POShip()
{
}

TiXmlNode * POShip::WriteNode(TiXmlNode * node) const
{
	TiXmlElement POS("Ship");
	AddLong(&POS, "Design", Type);
	AddLong(&POS, "Number", Amount);
	ProdOrder::WriteNode(&POS);
	node->InsertEndChild(POS);
	return node;
}

string POShip::TypeToString() const
{
	string str;
	str = "Ship design ";
	str += Long2String(Type);
	return str;
}

bool POShip::Produce(Planet * planet, long * resources, bool * AutoAlchemy)
{
	const Player * owner = planet->GetOwner();

	// check for invalid designs
	if (!owner->GetShipDesign(Type) || !owner->GetShipDesign(Type)->IsValidDesign(owner)) {
		Message * mess = planet->NCGetOwner()->AddMessage("Error: Invalid Production type", planet);
		mess->AddLong("Ship", Type);
		return true;
	}

	if (planet->GetBaseNumber() < 0 ||
		(planet->GetBaseDesign()->GetDockBuildCapacity() >= 0 &&
			owner->GetShipDesign(Type)->GetMass() > planet->GetBaseDesign()->GetDockBuildCapacity()))
	{
		Message * mess = planet->NCGetOwner()->AddMessage("Warning: Ship too big for dock", planet);
		mess->AddItem("Ship design", owner->GetShipDesign(Type)->GetName());
		mess->AddLong("dock capacity", planet->GetBaseDesign()->GetDockBuildCapacity());
		return true;
	}

	return DoProduce(owner->GetShipDesign(Type)->GetCost(owner), planet, resources, AutoAlchemy);
}

void POShip::Built(Planet * planet, long number)
{
	Player * player = planet->NCGetOwner();
	player->BuildShips(planet, Type, number);
}


POTerraform::~POTerraform()
{
}

TiXmlNode * POTerraform::WriteNode(TiXmlNode * node) const
{
	if (Type == POP_MINTERRA)
		AddLong(node, "AutoMinTerra", Amount);
	else if (Type == POP_MAXTERRA)
		AddLong(node, "AutoMaxTerra", Amount);
	else {
		TiXmlElement POT("Terraform");
		AddLong(&POT, "Number", Amount);
		ProdOrder::WriteNode(&POT);
		node->InsertEndChild(POT);
	}

	return node;
}

string POTerraform::TypeToString() const
{
	string str;
	if (Type == POP_MINTERRA)
		str = "AutoMinTerra";
	else if (Type == POP_MAXTERRA)
		str = "AutoMaxTerra";
	else
		str = "Terraform";

	return str;
}

bool POTerraform::Produce(Planet * planet, long * resources, bool * AutoAlchemy)
{
	Player * owner = planet->NCGetOwner();
	double cf = owner->ComponentCostFactor(CT_TERRAFORM);
	if (cf <= epsilon)
		return true;	// CA's can't buy terraforming

	Cost c;

	long tempAmt = Amount;
	while (Amount > 0 && *resources > 0) {
		if (Type == POP_MINTERRA && planet->GetPopulation() <= planet->GetMaxPop() && planet->GetOwner()->HabFactor(planet) >= 0)
			return false;

		c.Zero();
		owner->ResetTerraLimits();
		for (unsigned long i = 0; i < TheGame->GetComponents().size(); ++i) {
			// Is it terraforming? can we build it? Will it help? Is it cheaper?
			if (TheGame->GetComponents()[i]->GetType() == CT_TERRAFORM &&
				TheGame->GetComponents()[i]->IsBuildable(owner) &&
				planet->CanTerraform(TheGame->GetComponents()[i]) &&
				(mResCost == 0 || mResCost >= TheGame->GetComponents()[i]->GetCost().GetResources()))
			{
				owner->SetTerraLimit(TheGame->GetComponents()[i]->GetTerraType(), TheGame->GetComponents()[i]->GetTerraLimit());
				c = TheGame->GetComponents()[i]->GetCost();
			}
		}

		if (c.GetResources() == 0) {
			if (Type == POP_TERRAFORM)
				return true;	// this player cannot terraform yet
			else
				break;
		}

		c *= cf;

		if (DoProduce(c, planet, resources, AutoAlchemy) == true && Type == POP_TERRAFORM)
			return true;
	}

	if (Type == POP_TERRAFORM)
		return Amount == 0;
	else {
		Amount = tempAmt;
		return false;
	}
}

void POTerraform::Built(Planet * planet, long number)
{
	if (number > 0) {
		Message * mess = planet->NCGetOwner()->AddMessage("Terraform built", planet);
		planet->Terraform(number, 0, -1, planet->GetOwner(), planet->GetOwner(), mess);
		Amount -= number;
	}
}

}
