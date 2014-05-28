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
#include "TempFleet.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

/*
		<Waypoint>
			<Fleet>123</Fleet>
			<Mine>123</Mine>
			<Packet>123</Packet>
			<Player>5</Player>
			<Scrap>123</Scrap>
			<Space>123 123</Space>
            <Trader>1</Trader>
			<Planet>Cosine</Planet>
			<Speed>0</Speed><!-- -1 for gating -->
			<Order>
				Colonize
				Remote Mine
				Scrap
				Route
				<Transport>
					<Cargo>
						<Type>Population</Type> <!-- Fuel, Population, or minerals (normally Ironium, Boranium, or Germanium) -->
						<Action>LoadAll</Action> <!-- LoadAll, UnloadAll, LoadExactly, UnloadExactly, FillToPercent, WaitForPercent, LoadOptimal, SetAmountTo, SetWaypointTo -->
						<Value>1000</Value>
					</Cargo>
				</Transport>
				<Merge>123</Merge>
				<LayMine>5</LayMine> <!-- Lay mines for this many years.  -1 for indefinately. -->
				<Patrol>
					<Range>50</Range>
					<Speed>0</Speed>
				</Patrol>
				<Transfer>5</Transfer> <!-- Transfers to this player -->
			</Order>
		</Waypoint>
*/

namespace FreeStars {

WayOrder::~WayOrder()
{
	if (mForMeOnly)
		delete mLoc;
}

WayOrder::WayOrder(const WayOrder & copy)
	: mSpeed(copy.mSpeed),
	mOrder(copy.mOrder)
{
	if (copy.mForMeOnly) {
		TempFleet * tf = dynamic_cast<TempFleet *>(copy.mLoc);
		if (tf != NULL) {
			assert(false);
			mLoc = new TempFleet(*tf);
		} else {
			mLoc = new Location(*copy.mLoc);
		}
		mForMeOnly = true;
	} else {
		mLoc = copy.mLoc;
		mForMeOnly = false;
	}
}

WayOrder * WayOrder::Copy() const
{
	if (typeid(*this) == typeid(WayOrder)) {
		return new WayOrder(*this);
	} else if (typeid(*this) == typeid(WayOrderNumber)) {
		return new WayOrderNumber(dynamic_cast<const WayOrderNumber &>(*this));
	} else if (typeid(*this) == typeid(WayOrderPatrol)) {
		return new WayOrderPatrol(dynamic_cast<const WayOrderPatrol &>(*this));
	} else if (typeid(*this) == typeid(WayOrderTransport)) {
		return new WayOrderTransport(dynamic_cast<const WayOrderTransport &>(*this));
	} else {
//		assert(false);
		return NULL;
	}
}

void WayOrder::SetLocation(Location * loc, bool fmo/*= false*/)
{
	if (mForMeOnly)
		delete mLoc;
	
	mLoc = loc;
	mForMeOnly = fmo;
}

TiXmlNode * WayOrder::WriteNode(TiXmlNode * node) const
{
	TiXmlElement * wayorder = new TiXmlElement("Waypoint");

	SpaceObject * so = dynamic_cast<SpaceObject *>(mLoc);
	if (so == NULL) {
		mLoc->WriteNode(wayorder);
	} else {
		const type_info & ti = typeid(*mLoc);
		if (typeid(Planet) == ti) {
			AddString(wayorder, "Planet", dynamic_cast<Planet *>(so)->GetName().c_str());
		} else if (typeid(Fleet) == ti) {
			AddLong(wayorder, "Fleet", so->GetID());
			AddLong(wayorder, "Player", so->GetOwner()->GetID());
		} else if (typeid(Packet) == ti) {
			AddLong(wayorder, "Packet", so->GetID());
		} else if (typeid(MineField) == ti) {
			AddLong(wayorder, "Minefield", so->GetID());
		} else if (typeid(Salvage) == ti) {
			AddLong(wayorder, "Scrap", so->GetID());
		}
	}
/*
	const Planet * planet = dynamic_cast<const Planet *>(mLoc);
	if (planet != NULL) {
		AddString(wayorder, "Planet", planet->GetName().c_str());
	} else {
		const Fleet * fleet = dynamic_cast<const Fleet *>(mLoc);
		if (fleet != NULL) {
			AddLong(wayorder, "Fleet", fleet->GetID());
			AddLong(wayorder, "Player", fleet->GetOwner()->GetID());
		} else {
			const Packet * packet = dynamic_cast<const Packet *>(mLoc);
			if (packet != NULL) {
				AddLong(wayorder, "Packet", packet->GetID());
			} else {
				const MineField * mine = dynamic_cast<const MineField *>(mLoc);
				if (mine != NULL) {
					AddLong(wayorder, "Minefield", mine->GetID());
				} else {
					const Salvage * scrap = dynamic_cast<const Salvage *>(mLoc);
					if (scrap != NULL) {
						AddLong(wayorder, "Scrap", scrap->GetID());
					} else {
						mLoc->WriteNode(wayorder);
					}
				}
			}
		}
	}
*/
	AddLong(wayorder, "Speed", mSpeed);
	switch (mOrder) {
	case OT_NONE:
		AddString(wayorder, "Order", "No Task");
		break;
	case OT_COLONIZE:
		AddString(wayorder, "Order", "Colonize");
		break;
	case OT_REMOTEMINE:
		AddString(wayorder, "Order", "Remote Mine");
		break;
	case OT_SCRAP:
		AddString(wayorder, "Order", "Scrap");
		break;
	case OT_ROUTE:
		AddString(wayorder, "Order", "Route");
		break;
	}

	node->LinkEndChild(wayorder);
	return wayorder;
}

WayOrderNumber::~WayOrderNumber()
{
}

TiXmlNode * WayOrderNumber::WriteNode(TiXmlNode * node) const
{
	TiXmlNode * wo = WayOrder::WriteNode(node);
	TiXmlElement order("Order");
	switch (mOrder) {
	case OT_MERGE:
		AddLong(&order, "Merge", mNumber);
		break;
	case OT_LAYMINE:
		AddLong(&order, "LayMine", mNumber);
		break;
	case OT_TRANSFER:
		AddLong(&order, "Transfer", mNumber);
		break;
	}

	wo->InsertEndChild(order);

	return wo;
}

WayOrderPatrol::~WayOrderPatrol()
{
}

TiXmlNode * WayOrderPatrol::WriteNode(TiXmlNode * node) const
{
	TiXmlNode * wo = WayOrder::WriteNode(node);
	TiXmlElement order("Order");
	TiXmlElement patrol("Patrol");
	AddLong(&patrol, "Speed", mPatrolSpeed);
	AddLong(&patrol, "Range", mRange);
	order.InsertEndChild(patrol);
	wo->InsertEndChild(order);

	return wo;
}

WayOrderTransport::WayOrderTransport(Location * loc, bool ForMeOnly /*= false*/)
: WayOrder(loc, ForMeOnly)
{
	actions.insert(actions.begin(), Rules::MaxMinType-FUEL, TRANSFER_NOORDER);
	values.insert(values.begin(), Rules::MaxMinType-FUEL, 0L);
}

WayOrderTransport::WayOrderTransport(const WayOrderTransport & copy) : WayOrder(copy)
{
	actions.insert(actions.begin(), copy.actions.begin(), copy.actions.begin());
	values.insert(values.begin(), copy.values.begin(), copy.values.begin());
}

WayOrderTransport::~WayOrderTransport()
{
}

/*
					<Cargo>
						<Type>Population</Type> <!-- Fuel, Population, or minerals (normally Ironium, Boranium, or Germanium) -->
						<Action>Load All</Action> <!-- Load All, Unload All, Load Exactly, Unload Exactly, Fill To Percent, Wait For Percent, Load Optimal/Load Dunnage, Set Amount To, Set Waypoint To, Set To Percent, Drop And Load -->
						<Value>1000</Value>
					</Cargo>
*/
bool WayOrderTransport::ParseNode(const TiXmlNode * node, Player * player)
{
	const TiXmlNode * child1;
	const char * ptr;

	for (child1 = node->FirstChild("Cargo"); child1; child1 = child1->NextSibling("Cargo")) {
		long type = Rules::GetCargoType(GetString(child1->FirstChild("Type")));
		if (type < FUEL || type >= Rules::MaxMinType) {
			Message * mess = player->AddMessage("Error: Invalid cargo type");
			mess->AddItem("", GetString(child1->FirstChild("Type")));
			return false;
		}

		type -= FUEL;	// make it 0 indexed
		ptr = GetString(child1->FirstChild("Action"));
		if (stricmp(ptr, "Load All") == 0)
			actions[type] = TRANSFER_LOADALL;
		else if (stricmp(ptr, "Unload All") == 0)
			actions[type] = TRANSFER_UNLOADALL;
		else if (stricmp(ptr, "Load Exactly") == 0)
			actions[type] = TRANSFER_LOADAMT;
		else if (stricmp(ptr, "Unload Exactly") == 0)
			actions[type] = TRANSFER_UNLOADAMT;
		else if (stricmp(ptr, "Fill To Percent") == 0)
			actions[type] = TRANSFER_FILLPER;
		else if (stricmp(ptr, "Wait For Percent") == 0)
			actions[type] = TRANSFER_WAITPER;		// Affects movement too
		else if (stricmp(ptr, "Load Optimal") == 0 || stricmp(ptr, "Load Dunnage") == 0)
			actions[type] = TRANSFER_LOADDUNN;		// Load optimal for fuel
		else if (stricmp(ptr, "Set Amount To") == 0)
			actions[type] = TRANSFER_AMOUNTTO;
		else if (stricmp(ptr, "Set Waypoint To") == 0)
			actions[type] = TRANSFER_DESTTO;
		else if (stricmp(ptr, "Set To Percent") == 0)
			actions[type] = TRANSFER_SETTOPER;		// set to a % of fleet capacity
		else if (stricmp(ptr, "Drop And Load") == 0)
			actions[type] = TRANSFER_DROPNLOAD;		// drop all and then load all. only really useful for pop drops
		else if (stricmp(ptr, "No Orders") == 0)
			actions[type] = TRANSFER_NOORDER;
		else {
			actions[type] = TRANSFER_NOORDER;
			Message * mess = player->AddMessage("Error: Invalid transfer order");
			mess->AddItem("", ptr);
		}
		values[type] = GetLong(child1->FirstChild("Value"));
	}

	return true;
}

TiXmlNode * WayOrderTransport::WriteNode(TiXmlNode * node) const
{
	TiXmlNode * wo = WayOrder::WriteNode(node);
	TiXmlElement order("Order");
	TiXmlElement transport("Transport");

	for (CargoType i = 0; i < Rules::MaxMinType-FUEL; ++i) {
		if (actions[i] != TRANSFER_NOORDER) {
			TiXmlElement cargo("Cargo");
			AddString(&cargo, "Type", Rules::GetCargoName(i+FUEL).c_str());

			switch (actions[i]) {
			case TRANSFER_LOADALL:
				AddString(&cargo, "Action", "Load All");
				break;
			case TRANSFER_UNLOADALL:
				AddString(&cargo, "Action", "Unload All");
				break;
			case TRANSFER_LOADAMT:
				AddString(&cargo, "Action", "Load Exactly");
				AddLong(&cargo, "Value", values[i]);
				break;
			case TRANSFER_UNLOADAMT:
				AddString(&cargo, "Action", "Unload Exactly");
				AddLong(&cargo, "Value", values[i]);
				break;
			case TRANSFER_FILLPER:
				AddString(&cargo, "Action", "Fill To Percent");
				AddLong(&cargo, "Value", values[i]);
				break;
			case TRANSFER_WAITPER:
				AddString(&cargo, "Action", "Wait For Percent");
				AddLong(&cargo, "Value", values[i]);
				break;
			case TRANSFER_LOADDUNN:
				AddString(&cargo, "Action", "Load Optimal");
				break;
			case TRANSFER_AMOUNTTO:
				AddString(&cargo, "Action", "Set Amount To");
				AddLong(&cargo, "Value", values[i]);
				break;
			case TRANSFER_DESTTO:
				AddString(&cargo, "Action", "Set Waypoint To");
				AddLong(&cargo, "Value", values[i]);
				break;
			case TRANSFER_SETTOPER:
				AddString(&cargo, "Action", "Set To Percent");
				AddLong(&cargo, "Value", values[i]);
				break;
			case TRANSFER_DROPNLOAD:
				AddString(&cargo, "Action", "Drop And Load");
				break;
			}

			transport.InsertEndChild(cargo);
		}
	}

	order.InsertEndChild(transport);
	wo->InsertEndChild(order);

	return wo;
}

}
