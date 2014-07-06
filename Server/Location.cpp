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

#include <stdlib.h>
#include <math.h>

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

Location::~Location()
{
}

double Location::Distance(const Location * other) const
{
	return sqrt(double((posX - other->posX) * (posX - other->posX) + (posY - other->posY) * (posY - other->posY)));
}

double Location::Distance(double px, double py) const
{
	return sqrt((double(posX) - px) * (double(posX) - px) + (double(posY) - py) * (double(posY) - py));
}

bool Location::ParseNode(const TiXmlNode * node, Game *game)
{
	const TiXmlNode * child = node->FirstChild("XCoord");
	if (child) {
		posX = GetLong(child);
		posY = GetLong(node->FirstChild("YCoord"));

		if (posX <= 0 || posY <= 0) {
			Message * mess = game->AddMessage("Error: Invalid Location");
			mess->AddItem("", this);
			return false;
		}
	} else {
		Planet * p = game->GetGalaxy()->GetPlanet(GetString(node->FirstChild("Planet")));
		if (p == NULL) {
			Message * mess = game->AddMessage("Error: Invalid Location");
			mess->AddItem("", this);
			return false;
		}

		posX = p->GetPosX();
		posY = p->GetPosY();
	}

	return true;
}

/*
	        <Location>
	            <XCoord>1012</XCoord>
	            <YCoord>1045</YCoord>
	        </Location>
*/

TiXmlNode * Location::WriteLocation(TiXmlNode * node) const
{
	TiXmlElement *loc = new TiXmlElement("Location");
//	if (mAlsoHere && mAlsoHere->at(0) != this && dynamic_cast<Planet *>(mAlsoHere->at(0)) != NULL) {
//		AddString(loc, "Planet", mAlsoHere->at(0)->GetName(NULL).c_str());
//	} else {
		AddLong(loc, "XCoord", posX);
		AddLong(loc, "YCoord", posY);
//	}

	node->LinkEndChild(loc);
	return loc;
}

void Location::MoveToward(const Location * start, const Location * dest, double * px, double * py, long distance)
{
	double totdist = start->Distance(dest);
	double travdist = double(distance) + 1.0 - epsilon;

	if (totdist < travdist) {
		*px = dest->posX;
		*py = dest->posY;
	} else {
		*px = start->posX + (dest->posX - start->posX) * travdist / totdist;
		*py = start->posY + (dest->posY - start->posY) * travdist / totdist;
	}
/*
		// adjust by up to 1ly to get some additional distance (allow 100.99ly at warp 10)
		long signx = dest->posX == start->posX ? 0 : dest->posX > start->posX ? 1 : -1;
		long signy = dest->posY == start->posY ? 0 : dest->posY > start->posY ? 1 : -1;
		*px += signx;
		if (long(sqrt((*px - start->posX)**2 + (*py - start->posY)**2)) > distance)
			point->posX -= signx;
		else {
			point->posY += signy;
			if (long(start->Distance(point)) > distance)
				point->posY -= signy;
		}
*/
}

}
