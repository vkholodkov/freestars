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
 **@file BattlePlan.h
 **@ingroup Server
 **@brief Tactics.
 **/

#if !defined(FreeStars_BattlePlan_h)
#define FreeStars_BattlePlan_h

#include <limits.h>

class TiXmlNode;

namespace FreeStars {
class Player;

///Battle Plan Tactic.
enum BPTactic
{
	BPT_DISENGAGE	= 0x0001, ///< Disengage.
	BPT_DISIFHIT	= 0x0002, ///< Disengage if challenged.
	BPT_MINDAM		= 0x0004, ///< Minimize damage to self.
	BPT_MAXNET		= 0x0008, ///< Maximize net damage.
	BPT_MAXRATIO	= 0x0010, ///< Maximize damage ratio.
	BPT_MAXDAM		= 0x0020, ///< Maximize damage.
};

/**
 * Battle plans.
 * A Battle plan determines how a ship will behave while
 * in battle.
 * @ingroup Server
 */
class BattlePlan {
public:
	BattlePlan(bool defined);
//	BattlePlan(const BattlePlan & c);	Default should be fine
	virtual ~BattlePlan();
	void SetDefault();

	bool ParseNode(const TiXmlNode * node, Player * owner);
	void WriteNode(TiXmlNode * node) const;
	void WriteNodeBattle(TiXmlNode * node) const;
	bool IsDefined() const	{ return mDefined; }
	void Undefine()			{ mDefined = false; }
	unsigned long GetEnemy() const	{ return mEnemy; }
	HullType GetPrimary() const	{ return mPrimary; }
	HullType GetSecondary() const	{ return mSecondary; }
	BPTactic GetTactic() const		{ return mTactic; }
	/// does 'attacker' want to fight 'defender'
	bool WillFight(const Player * attacker, const Player * defender) const;

protected:
	static HullType GetTarget(const char *ptr);
	static const char * WriteTarget(HullType target);
	string mName;
	HullType mPrimary;
	HullType mSecondary;
	BPTactic mTactic;
	unsigned long mEnemy;
	bool mDumpCargo;
	bool mDefined;	///< is this battle plan real
};

// Battle Plan race to target (enemy for this battle plan) - other numbers are player numbers
const unsigned long BPE_NONE			= ULONG_MAX - 1;
const unsigned long BPE_ENEMY			= ULONG_MAX - 2;
const unsigned long BPE_ENEMYNEUTRAL	= ULONG_MAX - 3;
const unsigned long BPE_ALL				= ULONG_MAX - 4;
}
#endif // !defined(FreeStars_BattlePlan_h)
