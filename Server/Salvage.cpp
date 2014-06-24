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

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

Salvage::Salvage(Galaxy *galaxy)
    : CargoHolder(galaxy)
    , TurnCreated(TheGame->GetTurn())
    , MaxSize(0)
{
	SetID(0);
}

Salvage::Salvage(const CargoHolder &source)
:	CargoHolder(source),
	TurnCreated(TheGame->GetTurn()),
	MaxSize(0)
{
	SetID(mGalaxy->GetSalvageID());
}

Salvage::~Salvage()
{
}

void Salvage::Decay()
{
	if (TheGame->GetTurn() > TurnCreated + 1) {
		for (CargoType ct = 0; ct < Rules::MaxMinType; ++ct) {
			long amt = GetContain(ct);
			if (amt > 100)
				amt /= 10;
			else
				amt = min(amt, 10L);

			AdjustAmounts(ct, -amt);
			MaxSize -= amt;
		}
	}
}

bool Salvage::ParseNode(const TiXmlNode * node)
{
	if (!CargoHolder::ParseNode(node))
		return false;

	TurnCreated = GetLong(node->FirstChild("TurnCreated"));
	if (TurnCreated < 1 || TurnCreated >= TheGame->GetTurn()) {
		Message * mess = TheGame->AddMessage("Error: Wrong year number in turn file");
		mess->AddLong("Salvage created", TurnCreated);
		return false;
	}

	TheGame->AddAlsoHere(this);

	return true;
}

TiXmlNode * Salvage::WriteNode(TiXmlNode * node, const Player * viewer) const
{
	if (viewer != NULL && !SeenBy(viewer))
		return NULL;

	CargoHolder::WriteNode(node, viewer);
	if (viewer == NULL)
		AddLong(node, "TurnCreated", TurnCreated);

	return node;
}

}
