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
 **@file SpaceObject.h
 **@ingroup Server
 **@brief Space object.
 **/

#if !defined(FreeStars_SpaceObject_h)
#define FreeStars_SpaceObject_h

#include <deque>
using namespace std;

#include "FSTypes.h"
#include "Rules.h"
#include "Location.h"

namespace FreeStars {
class Player;
class Chaser;
class Planet;

const long SEEN_NONE			= 0x00000000;
const long SEEN_OWNER			= 0x00000001;	///< who owns it
const long SEEN_HULL			= 0x00000002;	///< hull of ship or base for planets
const long SEEN_DESIGN			= 0x00000004;	///< full design of ship or base, additional special check for certain races (WM)
const long SEEN_PLANETHAB		= 0x00000008;	///< current and starting planetary hab, owners hab is special case for certain races (CA)
const long SEEN_PLANETMC		= 0x00000010;	///< mineral concentrations
const long SEEN_PLANETPOP		= 0x00000020;	///< display planet pop (actual shown to host and owner)
const long SEEN_INSTALLATIONS	= 0x00000040;	///< stuff on the ground
const long SEEN_ORDERS			= 0x00000080;	///< orders
const long SEEN_SCANNER			= SEEN_OWNER | SEEN_HULL;
const long SEEN_PENSCAN			= SEEN_OWNER | SEEN_PLANETHAB | SEEN_PLANETMC | SEEN_PLANETPOP;
const long SEEN_BYOWNER			= 0x0000FFFF;	///< what the Owner sees
const long SEEN_HOST			= 0xFFFFFFFF;	///< what the Host sees

/**
 * An object in space.
 * @ingroup Server
 */
class SpaceObject : public Location {
public:
	SpaceObject(Game *game) : Location(), mGame(game), mOwner(NULL), mAlsoHere(NULL) { Init(); }
	SpaceObject(const SpaceObject &source) : Location(source), mGame(source.mGame), mOwner(source.mOwner), mAlsoHere(NULL) { Init(); }
//	SpaceObject(int x, int y, Player * owner) : Location(x, y), mOwner(owner) { Init(); }
	virtual ~SpaceObject();
	void Init();
	bool ParseNode(const TiXmlNode * node, Player * player);
	virtual bool ParseNode(const TiXmlNode * node);
	virtual TiXmlNode * WriteNode(TiXmlNode * node, const Player * viewer) const; ///< Save.
	TiXmlNode * WriteTransport(TiXmlNode * node) const; ///< Save transport.

    const Game *GetGame() const { return mGame; }
    Game *GetGame() { return mGame; }

	long GetID() const		{ return mID; } ///< Get ID.
	void SetID(long id)		{ mID = id; } ///< Set ID.

	const Player * GetOwner() const	{ return mOwner; } ///< Get owner of object.
	Player * NCGetOwner() const		{ return mOwner; } ///< Get owner of object.

	virtual const string GetName(const Player *) const	{ string s; return s; }
	virtual long SeenBy(const Player * p) const;
	virtual long SeenBy(unsigned long p) const;
	virtual void SetSeenBy(long p, long seen)		{ mSeenBy[p] |= seen; }
	virtual long GetCloak(const Player * player, bool pen) const = 0; ///< Get cloaking.
	virtual double GetMaxTachyon() const = 0; ///< Tachyon detectors reduce effectiveness of other players' cloaks.
	virtual long GetScanPenetrating() const = 0; ///< Get penetrating scanner radius.
	virtual long GetScanSpace() const = 0; ///< Get scanner radius.
	virtual void ResetSeen();
	virtual bool CanStealShip() const	{ return false; }
	virtual bool CanStealPlanet() const	{ return false; }

	void SetAlsoHere(deque<SpaceObject *> * locs)	{ mAlsoHere = locs; }
	deque<SpaceObject *> * GetAlsoHere()			{ return mAlsoHere; } ///< Get list of objects at this location.
	const deque<SpaceObject *> * GetAlsoHere() const	{ return mAlsoHere; } ///< Get list of objects at this location.
	void AddChaser(Chaser * c)	{ if (find(mChasers.begin(), mChasers.end(), c) == mChasers.end()) mChasers.push_back(c); }
	void CheckSeenByChasers();
	void GetChasers(deque<Chaser *> * chasers, int owner);

	virtual Planet * InOrbit(); ///< Get planet this object is in orbit of, or NULL if none.
	virtual const Planet * InOrbit() const; ///< Get planet this object is in orbit of, or NULL if none.

protected:
    Game *mGame;
	deque<long> mSeenBy;
	deque<Chaser *> mChasers; ///< Chasers following this SpaceObject.
	Player * mOwner; ///< Owner of this SpaceObject.
	long mID; ///< ID of this SpaceObject.
	deque<SpaceObject *> * mAlsoHere; ///< Other SpaceObjects located here.
	void GoingAwayNotifyChasers(); ///< Inform chasers that this SpaceObject is going away.
};
}
#endif // !defined(FreeStars_SpaceObject_h)
