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

/**
 **@file TempFleet.h
 **@ingroup Server
 **@brief Temp fleet.
 **/

#if !defined(FreeStars_TempFleet_h)
#define FreeStars_TempFleet_h

#include <string>
#include <deque>
using namespace std;

#include "CargoHolder.h"

namespace FreeStars {
class Player;

/**
 * TempFleet.
 * @ingroup Server
 */
class TempFleet : public CargoHolder, public Chaser {
public:
	TempFleet(Galaxy*, int fleetID, Player * owner);
	TempFleet(const CargoHolder * ch, CargoHolder * ch2) : CargoHolder(*ch), mCargoHolder(ch2) { assert(ch2 == NULL || ch->IsWith(*ch2)); }
	virtual ~TempFleet();

	virtual long GetContain(CargoType ct) const	{ return mCargoHolder && mCargoHolder->CanLoadBy(GetOwner()) && ct != POPULATION ? mCargoHolder->GetContain(ct) : 0; }
	virtual long GetCargoCapacity() const	{ return mCargoHolder ? mCargoHolder->GetCargoCapacity() : -1; }
	virtual bool CanLoadBy(const Player * p) const		{  return mCargoHolder ? mCargoHolder->CanLoadBy(p) : true;  }
	virtual void ChaseeGone(SpaceObject * chasee);
	virtual int GetOwnerID() const;

	// virtual functions, they should never be called
	virtual long SeenBy(const Player *) const			{ assert(false); return 0; }
	virtual long SeenBy(unsigned long) const			{ assert(false); return 0; }
	virtual void SetSeenBy(long, long)					{ assert(false); }
	virtual long GetCloak(const Player *, bool) const	{ assert(false); return 0; }
	virtual double GetMaxTachyon() const				{ assert(false); return 0.0; }
	virtual long GetScanPenetrating() const				{ assert(false); return -1; }
	virtual long GetScanSpace() const					{ assert(false); return -1; }
	virtual void ResetSeen()							{ assert(false); }
	virtual bool CanStealShip() const					{ assert(false); return false; }
	virtual bool CanStealPlanet() const					{ assert(false); return false; }
	virtual void SetCanLoadBy(const Player *)			{ assert(false); }

	CargoHolder * GetRealCH()	{ return mCargoHolder; }
	long GetFuel() const		{ return mFuel; }
	void AdjustFuel(long fuel)	{ mFuel += fuel; }

private:
	CargoHolder * mCargoHolder;
	long mFuel;
};
}
#endif // !defined(FreeStars_TempFleet_h)
