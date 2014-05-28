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
 **@file Stack.h
 **@ingroup Server
 **@brief Stacks of ships
 **/

#if !defined(FreeStars_Stack_h)
#define FreeStars_Stack_h

#include <deque>
using namespace std;

namespace FreeStars {
class Ship;

/**
 * A stack of ships.
 * Fleets are made up of stacks of ships.
 * @ingroup Server
 */
class Stack {
public:
	Stack() : mShip(NULL), mShips(0), mDamage(0), mDamaged(0), mFleetIn(NULL), bIsBase(false), bRandInitiative(0) {}
	~Stack();
	bool ParseNode(const TiXmlNode * node, Player * owner);
	void WriteNode(TiXmlNode * node, const Player * owner, const Player * viewer) const;
	void WriteNode(TiXmlNode * node) const;		// for battle reports

	const Ship * GetDesign() const		{ return mShip; }
	long GetCount() const			{ return mShips; }
	long GetDamaged() const			{ return mDamaged; }
	int GetDamage() const			{ return mDamage; }

	void SetDesign(const Ship * s)		{ mShip = s; }
	void SetCount(long count)		{ mShips = count; }
	void SetDamaged(long d)			{ mDamaged = d; }
	void SetDamage(int d)			{ mDamage = d; }

	long DamageAllShips(long damage);
	bool KillShips(long count, bool salvage);
//	long ApplyDamage(long damage, int type);	// returns number of ships destroyed
	void AddFromFleet(long fleet, long ships, long damaged);
	const Fleet * GetFleetIn() const	{ return mFleetIn; }
	Fleet * GetFleetIn()				{ return mFleetIn; }
	void SetFleetIn(Fleet * fin)		{ mFleetIn = fin; }
	bool operator ==(const Stack& s) const {return mShip == s.GetDesign() && mFleetIn == s.GetFleetIn();}

private:
	const Ship * mShip;	// Design
	long mShips;	// number of ships in the stack
	long mDamage;	// Amount of damage per ship
	long mDamaged;	// Number of damaged ships
	Fleet * mFleetIn;	// Fleet this stack is in

	class Origin {
	public:
		long fleet;		// fleet of origin
		long ships;		// number of ships transfered to this fleet
		long damaged;	// number of those that are damaged
	};

	deque<Origin> mOrigins;

	friend class Fleet;
	friend class Battle;
	friend class Bombing;

	// While in the battle board
	long bArmor;		// total armor remaining of the stack
	long bShield;	// total shield of the stack
	int bSpeed;		// Speed (taking into account ship design and other factors -- WM, ED, etc)
	long bx, by;
	long bMass;		// Mass (adjusted by +-15% for battles)
	long bShips;	// Current ship count in battle
	bool bIsBase;	// bases in combat use become a stack too
	long bRandInitiative;	// used to resolve initiative ties
	long bPlan;		// battle plan, may change
	long bFlee;		// moves spent toward fleeing
	const Stack * bMAT;	// Most Attractive Target

	bool IsBase() const	{ return bIsBase; }
	void SetupShips(const Player * owner, long cargo);
	void SetupBase(const Planet * planet);
	long DP() const		{ return bArmor + bShield; }

public:
	static inline bool StackLighter(Stack * s1, Stack * s2)
	{
		if (s1->bMass < s2->bMass)
			return true;
		else if (s1->bMass == s2->bMass)
			return Random(2) == 0 ? true : false;
		else
			return false;
	}
};
}
#endif // !defined(FreeStars_Stack_h)
