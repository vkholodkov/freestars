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
 **@file FSTypes.h
 **@ingroup Server
 **@brief FreeStars types
 **@todo Make more of this configurable. (at least before 2.0)
 */

#if !defined(FreeStars_FSTypes_h)
#define FreeStars_FSTypes_h

namespace FreeStars {

//--------------- FreeStars Types -----------------------
/**
 * Habibility type.
 * Examples: Radiation, Temperature, Gravity
 */
typedef long HabType;

/**
 * Cargo types.
 * Zero and above are mineral types, negative values are non-minerals.
 * (like people and fuel)
 * These types are also used to determine costs.
 */
typedef long CargoType; ///< Cargo categories


//-------------------------------------------------------

// accumulate with Trinary op, and third argument
template<class FwdIter, class ReturnType, class TrinaryOp, class Third> inline
	ReturnType accumulate(FwdIter First, FwdIter Last, ReturnType Initial, TrinaryOp func, Third arg)
	{for (; First != Last; ++First)
		Initial = func(Initial, *First, arg);
	return (Initial); }

// MS random_shuffle has a bug so do this instead
template <class _RandomAccessIter>
inline void Random_Shuffle(_RandomAccessIter __first,
                           _RandomAccessIter __last) {
  if (__first == __last) return;
  for (_RandomAccessIter __i = __first + 1; __i != __last; ++__i)
    iter_swap(__i, __first + genrand_int32() % (__i - __first + 1));
}

/**
 * @name Version Numbers
 * @{
 */
const double FREESTARSVERSION	= 0.1;		///< overall version
const double HOSTFILEVERSION	= 0.1;		///< .hst (host) file format
const double RULESFILEVERSION	= 0.1;		///< rules file format
const double TURNFILEVERSION	= 0.1;		///< .m (turn file to player) file format
const double ORDERSFILEVERSION	= 0.1;		///< .x (orders file from player) file format
const double XYFILEVERSION		= 0.1;		///< .xy (galaxy defination) file format
const double RACEFILEVERSION	= 0.1;		///< .r (rade defination) file format
//@}

const double epsilon	= 0.00000001;			///< Epsilon test. The difference needed for comparing doubles to be not equal.

const long PR_SELF		= 4;	///< Player Relation -- Self
const long PR_FRIEND	= 3;	///< Player Relation -- Friend
const long PR_NEUTRAL	= 2;	///< Player Relation -- Neutral
const long PR_ENEMY		= 1;	///< Player Relation -- Enemy

/**
 * Technology Type.
 * 0 and above are the types, negative values have special meaning.
 */
typedef long TechType;
/**
 * @name Special TechTypes
 * @{
 */
const TechType NEXT_CHEAP		= -5;		///< Research the field you're closest to getting.
const TechType RESEARCH_ALCHEMY	= -4;		///< Don't do any research, put all leftover points into alchemy.
const TechType NEXT_SAME		= -3;		///< Research the same field.
const TechType NEXT_LOW			= -2;		///< Research the lowest field.
const TechType TECH_NONE		= -1;		///< Don't perform research.
//@}

const long MAX_TECH_LEVEL	= 26;		///< Maximum tech level @todo don't hardcode

/**
 * Transfer type.
 * The type of transfer to perform.
 * @ingroup Server
 */
enum TransferType
{
	TRANSFER_NOORDER		= 0,	///< No transfer order.
	TRANSFER_LOADALL		= 1,	///< Load all available.
	TRANSFER_UNLOADALL		= 2,	///< Unload all.
	TRANSFER_LOADAMT		= 3,	///< Load specified amount.
	TRANSFER_UNLOADAMT		= 4,	///< Unload specified amount.
	TRANSFER_FILLPER		= 5,	///< Fill to percent.
	TRANSFER_WAITPER		= 6,	///< Wait for percent. Affects movement too.
	TRANSFER_LOADDUNN		= 7,	///< Load dunnage. Load optimal for fuel.
	TRANSFER_AMOUNTTO		= 8,	///< Set Amount To.
	TRANSFER_DESTTO			= 9,	///< Set Waypoint To.
	TRANSFER_SETTOPER		= 10,	///< set to a % of fleet capacity
	TRANSFER_DROPNLOAD		= 11,	///< Drop all and then load all. Only really useful for pop drops.
};

/**
 * Component type.
 * @ingroup Server
 */
typedef unsigned long ComponentType;
/**
 * @name Predefined Component Types
 * @{
 */
const ComponentType CT_NONE				= 0x00000000;
const ComponentType CT_ARMOR			= 0x00000001;
const ComponentType CT_SHIELD			= 0x00000002;
const ComponentType CT_WEAPON			= 0x00000004;
const ComponentType CT_BOMB				= 0x00000008;
const ComponentType CT_ELEC				= 0x00000010;
const ComponentType CT_HULL				= 0x00000020;
const ComponentType CT_BASE				= 0x00000040;
const ComponentType CT_PLANSCAN			= 0x00000080;
const ComponentType CT_DEFENSE			= 0x00000100;
const ComponentType CT_TERRAFORM		= 0x00000200;
const ComponentType CT_ENGINE			= 0x00000400;
const ComponentType CT_MINELAY			= 0x00000800;
const ComponentType CT_MINER			= 0x00001000;
const ComponentType CT_SCANNER			= 0x00002000;
const ComponentType CT_MECH				= 0x00004000;
const ComponentType CT_ALCHEMY			= 0x00008000;
const ComponentType CT_PLANET_SPECIAL	= 0x00010000;
const ComponentType CT_GATE				= 0x00020000;
const ComponentType CT_DRIVER			= 0x00040000;
const ComponentType CT_PLANETARY		= CT_PLANSCAN | CT_DEFENSE | CT_TERRAFORM | CT_PLANET_SPECIAL | CT_ALCHEMY;
const ComponentType CT_ORBITAL			= CT_DRIVER | CT_GATE;
const ComponentType CT_ALL				= ~CT_NONE;
const ComponentType CT_GENERAL			= CT_ALL & ~(CT_BOMB | CT_MINER | CT_ENGINE | CT_ORBITAL | CT_PLANETARY | CT_HULL | CT_BASE);
//@}

/**
 * Hull type.
 * @ingroup Server
 */
typedef unsigned long HullType;
/**
 * @name Predefined Hull classes
 * @{
 */
const HullType HC_NONE		= 0x0000;
const HullType HC_SCOUT		= 0x0001;	///< Scout for JOAT scanning
const HullType HC_LAYER		= 0x0002;	///< Mine layer 2x mines
const HullType HC_MINICOL	= 0x0004;	///< Mini colonizer, for SD engine
const HullType HC_WARSHIP	= 0x0008;
const HullType HC_BOMBER	= 0x0010;
const HullType HC_MINER		= 0x0020;	///< Remote miner
const HullType HC_COLONY	= 0x0040;
const HullType HC_FREIGHTER	= 0x0080;
const HullType HC_UTILITY	= 0x0100;
const HullType HC_FUEL		= 0x0200;
const HullType HC_SMALLBASE	= 0x0400;	///< Bases without AR pen scanning
const HullType HC_BIGBASE	= 0x0800;	///< Bases with AR pen scanning
const HullType HC_ARMED		= 0x1000;	///< For battle orders, no hull or component should have this
const HullType HC_UNKNOWN	= 0x2000;	///< For Error Checking, no hull or component should have this
const HullType HC_BASE		= HC_SMALLBASE | HC_BIGBASE;
const HullType HC_UNARMED	= HC_COLONY | HC_MINICOL | HC_FREIGHTER | HC_FUEL | HC_MINER;
const HullType HC_COL		= HC_COLONY | HC_MINICOL;
const HullType HC_ALL		= ~HC_NONE;
//@}

// Random events
const unsigned long RE_NONE		= 0x0000;	///< No random events at all.
const unsigned long RE_MT		= 0x0001;	///< Mystery Trader enable.
const unsigned long RE_ARTIFACT	= 0x0002;	///< Artifacts enable. Note that artifact generation happens only at game creation.
const unsigned long RE_COMET	= 0x0004;	///< Comet strikes enable. Comet strikes can leave minerals, change mineral concentration, 
const unsigned long RE_WORMHOLE	= 0x0008;	///< Wormhole enable. Wormholes are moving entities that act as portals to another location on the galaxy map.
const unsigned long RE_ALL		= ~RE_NONE;	///< Enable all random events.

/**
 * Public Score Categories.
 * Each is tracked in the public scoresheet if public scores are enabled.
 * @ingroup Server
 */
typedef unsigned long PPSType;
/**
 * @name Predefined Public Player Score categories
 * @{
 */
const PPSType PPS_NONE		= 0x0000;		///< No categories.
const PPSType PPS_PLANET	= 0x0001;		///< Planets.
const PPSType PPS_BASES		= 0x0002;		///< Starbases.
const PPSType PPS_UNARM		= 0x0004;		///< Unarmed Ships.
const PPSType PPS_ESCORT	= 0x0008;		///< Escort Ships.
const PPSType PPS_CAPSHIP	= 0x0010;		///< Capital Ships.
const PPSType PPS_TECH		= 0x0020;		///< Tech Levels.
const PPSType PPS_RESOURCE	= 0x0040;		///< Resources.
const PPSType PPS_SCORE		= 0x0080;		///< Score.
const PPSType PPS_RANK		= 0x0100;		///< Rank. score is either on or off from the very beginning, others wait for a start time.
const PPSType PPS_ALL		= ~PPS_NONE;	///< All categories.
//@}

}
#endif // !defined(FreeStars_FSTypes_h)
