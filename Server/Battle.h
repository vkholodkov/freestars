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
 **@file Battle.h
 **@ingroup Server
 **@brief Battle.
 **/

#if !defined(FreeStars_Battle_h)
#define FreeStars_Battle_h

namespace FreeStars {

class Game;

/**
 * Battle.
 * A battle happens when two or more hostile players
 * (or ships with hostile orders)
 * are in the same location.
 * @ingroup Server
 */
class Battle : public Location {
public:
	Battle(Game*, const Location & loc);
	virtual ~Battle();

	void AddFleet(Fleet * fleet);
	void SetPlanet(Planet * planet)	{ mBPlanet = planet; }
	void AddFleets();
	void Resolve();
	static void SetStartPos(const Game*);
	static void Cleanup();

protected:
	long GetRange(const Stack * s1, const Stack * s2, int dx = 0, int dy = 0) const;
	bool FightRound();
	long GetSpeed(long speed, long round);
	bool MoveStack(long j);

	/// Gatling flag: false-> return damage from all weapons, including gatlings, true-> return just gatling damage
	long PotentialDamage(const Stack * shoot, const Stack * target, long Range, const Slot * slot, bool Gatling = false) const;
	const Stack * GetTarget(const Stack * hunter, HullType hc, long Range, const Slot * slot) const;

	long mRound;

    Game *game;
	deque<Fleet *> mThere;
	deque<Stack *> mFighting;
	deque<Slot *> mFireOrder;
	Planet * mBPlanet;
	bool mBaseFight;
	TiXmlElement * bReport;

	static long **StartX;
	static long **StartY;
    static long NumberOfPlayers;

	class bSlot {
	public:
		bSlot(const Slot * sl, Stack * st) : slot(sl), stack(st) { initiative = stack->GetDesign()->GetNetInitiative() + slot->GetComp()->GetInitiative(); }
		~bSlot() {}

		const Slot * slot;
		Stack * stack;
		long initiative;
	};

	static bool HighInitiative(const bSlot& s1, const bSlot& s2);
	deque<bSlot> mSlots;
	void ShootSlot(bSlot & shooter);
	static deque<TiXmlElement *> sReports;
};
}
#endif // !defined(FreeStars_Battle_h)
