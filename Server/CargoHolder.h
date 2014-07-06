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
 **@file CargoHolder.h
 **@ingroup Server
 **@brief Cargo Holder functions.
 **/

#if !defined(FreeStars_CargoHolder_h)
#define FreeStars_CargoHolder_h

#include <deque>
using namespace std;

#include "FSTypes.h"
#include "Rules.h"
#include "SpaceObject.h"

namespace FreeStars {
class Player;

/**
 * Cargo hold functionality.
 * @ingroup Server
 */
class CargoHolder : public SpaceObject {
public:
	CargoHolder(Game *game) : SpaceObject(game) { Init(); }
	CargoHolder(const CargoHolder &source) : SpaceObject(source) { Init(); }
//	CargoHolder(int x, int y, Player * owner) : Location(x, y) { Init(); }
	virtual ~CargoHolder();
	void Init();
	virtual bool ParseNode(const TiXmlNode * node);
	virtual TiXmlNode * WriteNode(TiXmlNode * node, const Player * viewer) const;
	long GetPopulation() const	{ return mPopulation; }
	TiXmlNode * WriteTransport(TiXmlNode * node) const;

	void AdjustPopulation(long amount)	{ this->mPopulation += amount; }
	void AdjustAmounts(long i, long amount);
	void ProcessUnload(CargoHolder * dest, CargoType ct, TransferType tt, long value);	///< value usage depends on TransferType
	void ProcessLoad(CargoHolder * dest, CargoType ct, TransferType tt, long value, bool dunnage);	///< value usage depends on TransferType
	void TransferCargo(CargoHolder * dest, CargoType ct, long * amount, Player * player);
	virtual long GetCargoCapacity() const = 0;
	virtual long GetContain(CargoType ct) const;
	virtual const deque<long> GetContain() const {return mContains;}; // 20060516 bdragon: Added -- This seemed like a useful function to have.
	long GetCargoMass() const;

	virtual bool CanLoadBy(const Player * player) const = 0;
	virtual void SetCanLoadBy(const Player * player) = 0;

protected:
	static long TransferAmount(CargoType ct, CargoHolder * from, CargoHolder * to, long Request);
	long mPopulation;
	deque<long> mContains;
};
}
#endif // !defined(FreeStars_CargoHolder_h)
