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

#include "Hull.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

Hull::Hull() : Component()
{
	// display settings
	mCargoLeft = 0;
	mCargoTop = 0;
	mCargoWidth = 0;
	mCargoHeight = 0;
}

Hull::~Hull()
{
	Slots.clear();
}

bool Hull::LoadSlot(const TiXmlNode * node, MessageSink &messageSink)
{
	deque<Slot>::iterator iter;
	iter = Slots.insert(Slots.end(), Slot(node, Slots.size(), messageSink));
	if (iter->IsAllowed(~CT_NONE) && iter->GetCount() > 0)
		return true;
	else {
		Slots.erase(iter);
		return false;
	}
}

bool Hull::CheckSlots()
{
	deque<Slot>::const_iterator iter;
	const Slot * temp;

	for (iter = Slots.begin(); iter != Slots.end(); ++iter) {
		temp = &*iter;
	}

	return true;
}

}
