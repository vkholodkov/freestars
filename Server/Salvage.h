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
 **@file Salvage.h
 **@ingroup Server
 **@brief Space wreckage.
 **/

#if !defined(FreeStars_Salvage_h)
#define FreeStars_Salvage_h

#include "Rules.h"
#include "CargoHolder.h"

namespace FreeStars {
class Player;

/**
 * Salvage.
 * Salvage is junk in space.
 * Salvage may be picked up by any player.
 * @ingroup Server
 */
class Salvage : public CargoHolder {
public:
	Salvage(Galaxy*);
	Salvage(const CargoHolder &source);
	virtual ~Salvage();
	static const char* ELEMENT_NAME()	{return "Salvage";}; ///< Name of XML element associated with class.
	virtual bool ParseNode(const TiXmlNode * node);
	virtual TiXmlNode * WriteNode(TiXmlNode * node, const Player * viewer) const;
	/// scrap piles should never be 'in orbit' and it has no effect if they are, so just claim we're never in orbit
	virtual Planet * InOrbit()				{ return NULL; }
	virtual const Planet * InOrbit() const	{ return NULL; }

	bool IsEmpty() const	{ return MaxSize == 0; }
	virtual long GetCargoCapacity() const	{ return MaxSize; }
	virtual long GetCloak(const Player *, bool) const	{ return 0; }
	virtual double GetMaxTachyon() const	{ return 0.0; }
	virtual long GetScanPenetrating() const			{ return -1; }
	virtual long GetScanSpace() const		{ return -1; }
	virtual void SetCanLoadBy(const Player *)	{}
	virtual bool CanLoadBy(const Player *) const	{ return true; }	///< anyone can load from scrap piles
	void Decay();

private:
	long TurnCreated;
	long MaxSize;
};
}
#endif // !defined(FreeStars_Salvage_h)
