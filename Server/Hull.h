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

/**
 **@file Hull.h
 **@ingroup Server
 **@brief Hull.
 **/

#if !defined(FreeStars_Hull_h)
#define FreeStars_Hull_h

#include <deque>
using namespace std;

class TiXmlNode;

#include "Component.h"
#include "Slot.h"

namespace FreeStars {

/**
 * Hull.
 * A hull contains slots for components.
 * Each ship is based on a given hull.
 * @ingroup Server
 */
class Hull : public Component {
public:
	Hull();
	virtual ~Hull();
	bool LoadSlot(const TiXmlNode * node);
	bool CheckSlots();
	const Slot & GetSlot(int i) const	{ return Slots[i]; }
	unsigned int GetNumberSlots() const			{ return Slots.size(); }
	void SetCargoLeft(long v)	{ mCargoLeft = v; }
	void SetCargoTop(long v)	{ mCargoTop = v; }
	void SetCargoWidth(long v)	{ mCargoWidth = v; }
	void SetCargoHeight(long v)	{ mCargoHeight = v; }
	long GetCargoLeft() const	{ return mCargoLeft; }
	long GetCargoTop() const	{ return mCargoTop; }
	long GetCargoWidth() const	{ return mCargoWidth; }
	long GetCargoHeight() const	{ return mCargoHeight; }

protected:
	// display settings
	long mCargoLeft;
	long mCargoTop;
	long mCargoWidth;
	long mCargoHeight;

	deque<Slot> Slots;
};
}
#endif // !defined(FreeStars_Hull_h)
