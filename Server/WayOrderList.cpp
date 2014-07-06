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

#include "FSServer.h"

#include "WayOrderList.h"
#include "TempFleet.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

WayOrderList::~WayOrderList()
{
	if (!mNoDelete) {
		int i;
		for (i = 0; i < orders.size(); ++i)
			delete orders[i];
	}
}

/*
	<Waypoints>
		<Fleet>123</Fleet>
		<Repeat>true</Repeat>
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
						<Type>1</Type> <!-- 0-4 for Fuel, Ironium, Boranium, Germanium, Population in that order (if we're going by Stars! UI) -->
						<Action>Load All</Action> <!-- Load All, Unload All, Load Exactly, Unload Exactly, Fill To Percent, Wait For Percent, Load Optimal/Load Dunnage, Set Amount To, Set Waypoint To, Set To Percent, Drop And Load -->
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
	</Waypoints>
*/

bool WayOrderList::ParseNode(const TiXmlNode * node, Player * player, Game *game)
{
	const TiXmlNode * child1;
	const TiXmlNode * child2;

	WayOrder * wo;
	Location * loc;
	bool fmo;
	Player * player2;
	long speed;

	nPlayer = player->GetID();
	// don't verify the fleet, it might not yet exist
	if (nFleet <= 0 || nFleet > Rules::MaxFleets) {
		Message * mess = player->AddMessage("Error: invalid fleet number");
		mess->AddLong("Waypoint order", nFleet);
		nFleet = 0;
		return true;
	}

	for (child1 = node->FirstChild("Waypoint"); child1; child1 = child1->NextSibling("Waypoint")) {
		wo = NULL;
		loc = NULL;
		fmo = false;
		speed = 0;
		unsigned long pnum = GetLong(child1->FirstChild("Player"));
		if (pnum < 0 || pnum > game->NumberPlayers()) {
			Message * mess = player->AddMessage("Error: invalid player number");
			mess->AddLong("", pnum);
			mess->AddItem("Where", "Fleet destination");
			continue;
		}
		player2 = game->NCGetPlayer(pnum);

		for (child2 = child1->FirstChild(); child2; child2 = child2->NextSibling()) {
			if (child2->Type() == TiXmlNode::COMMENT)
				continue;

			if (stricmp(child2->Value(), "Location") == 0) {
				if (loc != NULL)
					continue;

				Location * nl = new Location();
				if (!nl->ParseNode(child2, game)) {
					delete nl;
					continue;
				}
				loc = nl;
				fmo = true;
			} else if (stricmp(child2->Value(), "Planet") == 0) {
				if (loc != NULL)
					continue;

				Planet * planet = game->GetGalaxy()->GetPlanet(GetString(child2));
				if (!planet) {
					Message * mess = player->AddMessage("Error: invalid planet");
					mess->AddItem("", GetString(child2));
					mess->AddItem("Where", "Waypoint order");
					continue;
				}
				loc = planet;
			} else if (stricmp(child2->Value(), "Minefield") == 0) {
				if (loc != NULL)
					continue;

				///@todo add after mines are added
			} else if (stricmp(child2->Value(), "Scrap") == 0) {
				if (loc != NULL)
					continue;

				long l = GetLong(child2);
				Salvage * salvage = game->GetGalaxy()->GetSalvage(l);
				if (!salvage || !salvage->SeenBy(player)) {
					Message * mess = player->AddMessage("Error: invalid salvage pile");
					mess->AddLong("Waypoint order", l);
					continue;
				}
				loc = salvage;
			} else if (stricmp(child2->Value(), "Packet") == 0) {
				if (loc != NULL)
					continue;

				///@todo add after packets are added
			} else if (stricmp(child2->Value(), "Trader") == 0) {
				if (loc != NULL)
					continue;

				///@todo add after Mystery Traders are added
			} else if (stricmp(child2->Value(), "Fleet") == 0) {
				if (loc != NULL)
					continue;

				if (player2 == player) {
					loc = new TempFleet(game, GetLong(child2), player);
					fmo = true;
				} else {
					Fleet * f2 = player2->NCGetFleet(GetLong(child2));
					if (!f2 || !f2->SeenBy(player)) {
						Message * mess = player->AddMessage("Error: invalid fleet number");
						mess->AddLong("Waypoint order", GetLong(child2));
						continue;
					}

					loc = f2;
//					f2->AddChaser(nPlayer, nFleet);
//					chasing.push_back(f2);
				}
			} else if (stricmp(child2->Value(), "Speed") == 0) {
				speed = GetLong(child2);
				if (speed < -1 || speed > Rules::GetConstant("MaxSpeed")) {
					Message * mess = player->AddMessage("Error: invalid speed");
					mess->AddLong("Waypoint order", speed);
					continue;
				}
			} else if (stricmp(child2->Value(), "Order") == 0) {
				if (loc == NULL)
					continue;

				const TiXmlNode * child3;
				child3 = child2->FirstChild();
				while (child3->Type() == TiXmlNode::COMMENT)
					child3 = child3->NextSibling();

				if (strnicmp(child3->Value(), "No Task", 7) == 0) {
					wo = new WayOrder(loc, fmo);
					wo->mOrder = OT_NONE;
				} else if (strnicmp(child3->Value(), "Colonize", 8) == 0) {
					wo = new WayOrder(loc, fmo);
					wo->mOrder = OT_COLONIZE;
				} else if (strnicmp(child3->Value(), "Remote Mine", 11) == 0) {
					wo = new WayOrder(loc, fmo);
					wo->mOrder = OT_REMOTEMINE;
				} else if (strnicmp(child3->Value(), "Scrap", 5) == 0) {
					wo = new WayOrder(loc, fmo);
					wo->mOrder = OT_SCRAP;
				} else if (strnicmp(child3->Value(), "Route", 5) == 0) {
					wo = new WayOrder(loc, fmo);
					wo->mOrder = OT_ROUTE;
				} else if (strnicmp(child3->Value(), "Merge", 5) == 0) {
					unsigned long num = (unsigned long)GetLong(child3);
					if (num > Rules::MaxFleets) {
						Message * mess = player->AddMessage("Error: invalid fleet number");
						mess->AddLong("Waypoint order", num);
						continue;
					}

					wo = new WayOrderNumber(num, loc, fmo);
					wo->mOrder = OT_MERGE;
				} else if (strnicmp(child3->Value(), "LayMine", 7) == 0) {
					long num = GetLong(child3);
					if (num < -1 || num == 0) {
						Message * mess = player->AddMessage("Error: invalid number of years");
						mess->AddLong("Waypoint order", num);
						continue;
					}

					wo = new WayOrderNumber(num, loc, fmo);
					wo->mOrder = OT_LAYMINE;
				} else if (strnicmp(child3->Value(), "Transfer", 8) == 0) {
					unsigned long num = GetLong(child3);
					if (num < 0 || num > game->NumberPlayers()) {
						Message * mess = player->AddMessage("Error: invalid player number");
						mess->AddLong("", num);
						mess->AddItem("Where", "Waypoint order");
						continue;
					}

					wo = new WayOrderNumber(num, loc, fmo);
					wo->mOrder = OT_TRANSFER;
				} else if (strnicmp(child3->Value(), "Patrol", 6) == 0) {
					long s = GetLong(child3->FirstChild("Speed"));
					if (s < 0 || s > Rules::GetConstant("MaxSpeed")) {
						Message * mess = player->AddMessage("Error: invalid speed");
						mess->AddLong("Waypoint order", s);
						continue;
					}

					long r = GetLong(child3->FirstChild("Range"));
					if (r <= 0) {
						Message * mess = player->AddMessage("Error: invalid range");
						mess->AddLong("Waypoint order", r);
						continue;
					}

					wo = new WayOrderPatrol(s, r, loc, fmo);
					wo->mOrder = OT_PATROL;
				} else if (strnicmp(child3->Value(), "Transport", 9) == 0) {
					WayOrderTransport * wot;
					wot = new WayOrderTransport(loc, fmo);
					if (!wot->ParseNode(child3, player))
						continue;

					wo = wot;
					wo->mOrder = OT_TRANSPORT;
				}
			} else {
				Message * mess = player->AddMessage("Warning: Unknown type");
				mess->AddItem("Waypoint order", child2->Value());
				continue;
			}
		}

		if (wo != NULL) {
			wo->mSpeed = speed;
			orders.push_back(wo);
		} else if (fmo && loc) {
			delete loc;
		}
	}

	return true;
}

}
/*
			<Order>
				Colonize
				Remote Mine
				Scrap
				Route
				<Merge>123</Merge>
				<LayMine>5</LayMine> <!-- Lay mines for this many years.  -1 for indefinately. -->
				<Transfer>5</Transfer> <!-- Transfers to this player -->
				<Patrol>
					<Range>50</Range>
					<Speed>0</Speed>
				</Patrol>
				<Transport>
					<Cargo>
						<Type>1</Type> <!-- 0-4 for Fuel, Ironium, Boranium, Germanium, Population in that order (if we're going by Stars! UI) -->
						<Action>Load All</Action> <!-- Load All, Unload All, Load Exactly, Unload Exactly, Fill To Percent, Wait For Percent, Load Optimal/Load Dunnage, Set Amount To, Set Waypoint To, Set To Percent, Drop And Load -->
						<Value>1000</Value>
					</Cargo>
				</Transport>
			</Order>
*/
