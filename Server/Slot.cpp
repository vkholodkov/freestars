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

#include <cmath>

#include "FSServer.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

Slot::Slot(const TiXmlNode * node, long Pos)
{
	const TiXmlNode * child1;
	const char * ptr;

	mPosition = Pos;
	Allowed = CT_NONE;
	count = 0;
	component = NULL;
	mSlotTop = 0;
	mSlotLeft = 0;
	for (child1 = node->FirstChild(); child1; child1 = child1->NextSibling()) {
		if (child1->Type() != TiXmlNode::ELEMENT)
			continue;

		if (stricmp(child1->Value(), "Type") == 0) {
			ptr = GetString(child1);
			if (ptr == NULL)
				break;

			if (stricmp(ptr, "Armor") == 0)
				Allowed |= CT_ARMOR;
			else if (stricmp(ptr, "Shield") == 0)
				Allowed |= CT_SHIELD;
			else if (stricmp(ptr, "Weapon") == 0)
				Allowed |= CT_WEAPON;
			else if (stricmp(ptr, "Bomb") == 0)
				Allowed |= CT_BOMB;
			else if (stricmp(ptr, "Electrical") == 0)
				Allowed |= CT_ELEC;
			else if (stricmp(ptr, "Engine") == 0)
				Allowed |= CT_ENGINE;
			else if (stricmp(ptr, "Orbital") == 0)
				Allowed |= CT_ORBITAL;
			else if (stricmp(ptr, "Mine Layer") == 0)
				Allowed |= CT_MINELAY;
			else if (stricmp(ptr, "Remote Miner") == 0)
				Allowed |= CT_MINER;
			else if (stricmp(ptr, "Mechanical") == 0)
				Allowed |= CT_MECH;
			else if (stricmp(ptr, "Scanner") == 0)
				Allowed |= CT_SCANNER;
			else if (stricmp(ptr, "General") == 0)
				Allowed |= CT_GENERAL;
			else {
				Message * mess = TheGame->AddMessage("Warning: Unknown slot type");
				mess->AddItem("", ptr);
			}
		} else if (stricmp(child1->Value(), "Number") == 0) {
			ptr = GetString(child1);
			if (ptr == NULL)
				break;

			count = atol(ptr);
		} else if (stricmp(child1->Value(), "SlotLeft") == 0) {
			mSlotLeft = ::round(GetDouble(child1) * 64);
		} else if (stricmp(child1->Value(), "SlotTop") == 0) {
			mSlotTop = ::round(GetDouble(child1) * 64);
		} else {
			Message * mess = TheGame->AddMessage("Warning: Unknown section");
			mess->AddItem("Slot", child1->Value());
		}
	}
}

bool operator==(const Slot & s1, const Slot & s2)
{
	if (s1.Allowed != s2.Allowed)
		return false;

	if (s1.component != s2.component)
		return false;

	if (s1.count != s2.count)
		return false;

	return true;
}

}
