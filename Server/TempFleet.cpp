/*
Copyright 2005 Elliott Kleinrock, Dan Neely, Kurt W. Over, Damon Domjan

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

#include "TempFleet.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

TempFleet::TempFleet(Galaxy *galaxy, int fleetID, Player * owner)
    : CargoHolder(galaxy)
{
	mID = fleetID;
	mOwner = owner;
}

TempFleet::~TempFleet()
{
}

int TempFleet::GetOwnerID() const
{
	return mOwner->GetID();
}

void TempFleet::ChaseeGone(SpaceObject * chasee)
{
	Planet * planet = chasee->InOrbit();
	Message * mess = NCGetOwner()->AddMessage("Chasee has vanished", chasee);
	if (planet == NULL)
		mess->AddItem("Last location", new Location(*chasee), true);
	else
		mess->AddItem("Last location", planet);

	Fleet * f = dynamic_cast<Fleet *>(GetRealCH());
	if (f != NULL) {
		deque<WayOrder *> const & ords = f->GetOrders();
		int i;
		for (i = 0; i < ords.size(); ++i) {
			if (ords[i]->GetLocation() == chasee) {
				if (planet == NULL)
					ords[i]->SetLocation(new Location(*chasee), true);
				else
					ords[i]->SetLocation(planet);
			}
		}
	}
}

}
