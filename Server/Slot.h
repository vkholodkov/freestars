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
 **@file Slot.h
 **@ingroup Server
 **@brief Component slots
 **/

#if !defined(FreeStars_Slot_h)
#define FreeStars_Slot_h

#include "Component.h"
class TiXmlNode;

namespace FreeStars {

/**
 * Slot.
 * A slot contains a specific component. Sometimes you can
 * fit more than one component into the slot.
 * However, the components must be identical.
 * @ingroup Server
 */
class Slot {
public:
	Slot(const Component * comp, long amount, long Pos, const Slot & hull) : Allowed(0), component(comp), count(amount), mPosition(Pos), mSlotLeft(hull.mSlotLeft), mSlotTop(hull.mSlotTop) {}
//	Slot(long allowed, long maximum, long Pos) : Allowed(allowed), component(NULL), count(maximum), mPosition(Pos) {}
	Slot(const TiXmlNode * node, long Pos);
	Slot(const Slot & other) : Allowed(other.Allowed), component(other.component), count(other.count), mPosition(other.mPosition), mSlotLeft(other.mSlotLeft), mSlotTop(other.mSlotTop) {}

	const Component * GetComp() const	{ return component; }
	bool IsAllowed(ComponentType type) const		{ return type & Allowed ? true : false; }
	int GetCount() const				{ return count; }
	long GetPosition() const			{ return mPosition; }

	void SetComp(const Component * comp)	{ component = comp; }

	friend bool operator==(const Slot & s1, const Slot & s2);
	friend bool operator!=(const Slot & s1, const Slot & s2)	{ return !(s1 == s2); }

private:
	ComponentType Allowed;	///< what types are allowed in this slot
	const Component * component;	///< what component is here (in a design)
	long count;	///< max allowed, or current count
	long mPosition;		///< where the slot is in the hull
	long mSlotLeft;
	long mSlotTop;
};
}
#endif // !defined(FreeStars_Slot_h)
