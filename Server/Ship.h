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
 **@file Ship.h
 **@ingroup Server
 **@brief Ships.
 **/

#if !defined(FreeStars_Ship_h)
#define FreeStars_Ship_h

#include <string>
#include <list>

#include "Slot.h"
#include "Cost.h"

namespace FreeStars {
class Component;
class Hull;
class Player;

/**
 * A ship.
 * Each stack has a Ship associated with it.
 * A ship is the object representing a certain design of ship. (a hull, modules, graphics, name, etc.)
 * @ingroup Server
 */
class Ship {
public:
	Ship();
	Ship(const Hull*);
	virtual ~Ship();
	static void Cleanup();

	bool ParseNode(const TiXmlNode * node, Player * player, bool other);
	void WriteNode(TiXmlNode * node, bool Host, bool Owner, bool SeeDesign) const;
	/**
	 *Check if current ship design is valid.
	 *This function checks the ship design follows the rules (no weapons in the engine slot, etc..)
	 *@retval true Design is valid.
	 *@retval false Design is invalid.
	 */
	bool IsValidDesign() const;
	/**
	 *Check if the ship is buildable by a player.
	 *This function checks whether the specified player can build the ship, taking into consideration
	 *PRTs, LRTs, and tech levels.
	 *@param player The player to check against.
	 *@retval true Design is buildable by player.
	 *@retval false Design is not buildable by player.
	 */
	bool IsValidDesign(const Player * player) const;
	
	long TechLevel(TechType tech) const;
	const Cost & GetCost(const Player * owner, const Ship * from =NULL, const Planet * planet =NULL) const;
	
	string GetName() const					{ return mName; }
	bool IsGift() const						{ return mGift; }
	bool CanStealShip() const;
	bool CanStealPlanet() const;
	long GetScanPenetrating(const Player * player, long pop = 0) const;
	long GetScanSpace(const Player * player, long pop = 0) const;
	long GetCloaking() const;
	long GetFuelCapacity() const;
	long GetFuelGeneration() const;
	long GetMass() const;
	double GetTachyon() const;
	long GetCargoCapacity() const;
	long GetShield(const Player * p) const;
	long GetArmor(const Player * p) const;
	long GetSweeping() const;
	long GetInitiativeAdjustment() const;
	double GetSpeedBonus() const;
	long GetMines() const;
	long GetARMaxPop() const;
	/**
	 *Get the capacity of a space dock.
	 *This will return the capacity of the space dock (ship building area) on a ship.
	 *For the most part, this is only applicable for starbases, although ships capable of building
	 *ships is certainly a possibility.
	 *@retval -1 Infinate space dock -- no limitation.
	 *@retval 0 No space dock -- not capable of building ships.
	 *@returns The capacity of the space dock.
	 */
	long GetDockBuildCapacity() const;
	double GetComputerPower() const;
	double GetJamming() const;
	double GetCapacitors() const;
	double GetDeflection() const;
	double GetDampener() const;
	double GetRepairRate() const;
	bool DoesRadiate(HabType ht) const;
	/**
	 * Can Colonize?
	 * Checks if the ship is capable of acting as a colonization ship.
	 * Colonization turns the ship into an initial habitat. The ship is destroyed during
	 * this process.
	 */
	bool CanColonize() const;
	bool CanRefuel() const;
	long GetDriverSpeed() const;
	long CountDrivers() const;

	long GetTerraPower(long type) const;
	long GetMineAmount(long type) const;
	double GetFuelUsage(long speed) const;
	long GetSafeSpeed() const;
	long GetMaxSpeed() const;
	long GetBattleSpeed() const;
	long GetFreeSpeed() const;
	long GetRating() const;
	bool CanShoot() const;
	bool CanBomb() const {return CanNormalBomb() || CanTerraBomb() || CanSmartBomb();}
	bool CanNormalBomb() const;
	bool CanTerraBomb() const;
	bool CanSmartBomb() const;
	double GetNormalKillPercentage() const;
	//long GetSmartKillPercentage() const;
	long GetKillMin() const;
	long GetKillInstallation() const;
	long GetNetSpeed() const;
	long GetNetInitiative() const;
	bool CanJumpGate() const;
	const Component * GetGate() const;
	long GetEngines() const; ///< Get number of engines for this model.

	bool IsBattleTarget(HullType hc) const;
	long GetAttractiveCost(const Player * owner) const;
	double GetAccuracy(const Component & torp, const Ship * target) const;

	void ResetSeen();
	bool SeenHull(long p) const				{ return mSeenHull[p]; }
	bool SeenDesign(long p) const			{ return mSeenDesign[p]; }
	void SetSeenHull(long p, bool seen)		{ mSeenHull[p] = seen; }
	void SetSeenDesign(long p, bool seen)	{ mSeenDesign[p] = seen; }
	/**
	 *Add to the number built tally.
	 *This utility function will add to the number built tally "mBuilt."
	 *@param number The number to increment by.
	 */
	void IncrementBuilt(long number)		{ mBuilt += number; }

	const Hull * GetHull() const			{ return mHull; }

	const Slot & GetSlot(int i) const	{ return mSlots[i]; }
	Slot &GetSlot(int i) { return mSlots[i]; }

	void CopyDesign(const Ship * sd, bool IsGift);
	void Upgrade(const Player * player);
	void SetCannotBuild(const Component * comp)	{ mCannotBuild = comp; }
	const Component * GetCannotBuild() const	{ return mCannotBuild; }

	friend bool operator==(const Ship & s1, const Ship & s2);

private:
	void ResetDefaults();
	std::string mName;
	long mGraphicNumber;
	///Was the ship a gift?
	bool mGift;
	deque<Slot> mSlots;
	const Hull * mHull;			///< The hull to use.
	long mBuilt;			///< Number of ships of this design that you've had.
	long ReCost;			///< Turn phase costs were last recalculated.
	void ScrapRecover(Cost & c, int number, const Planet * planet) const;

	deque<bool> mSeenDesign;	///< Who has seen this design this turn.
	deque<bool> mSeenHull;		///< Who has seen this hull this turn.
	
	friend class Bombing;

	const Component * mCannotBuild;	///< for starting ships

	static Cost mUpCost;
	// Calculated values
	Cost CVCost;			///< cost in minerals, resources, and crew(note normal rules don't have a crew cost)

	long CVFuelCapacity;
	long CVFuelGeneration;
	long CVMass;
	double CVTachyon;
	bool CCalcTachyon;
	long CVCargoCapacity;
	long CVArmor;
	long CVShield;
	long CVInitiativeAdjustment;
	double CVSpeedBonus;
	bool CCalcSpeedBonus;
	long CVMines;
	long CVARMaxPop;
	long CVDockBuildCapacity;

	double CVComputerPower;
	bool CCalcComputerPower;
	double CVJamming;
	bool CCalcJamming;
	double CVCapacitors;
	bool CCalcCapacitors;
	double CVDeflection;
	bool CCalcDeflection;

	double CVDampener;
	bool CCalcDampener;
	double CVRepairRate;
	bool CCalcRepairRate;
	long CVSafeSpeed;
	long CVMaxSpeed;
	long CVBattleSpeed;
	long CVFreeSpeed;

	long CVColonize;
	long CVRefuel;
	long CVDriverSpeed;
	long CVCountDrivers;

	long CVStealShip;
	long CVStealPlanet;

	long CVCloaking;
	long CVSweeping;
	long CVScanSpace;
	long CVScanPenetrating;
	long CVRating;
	long CVShoot;
	long CVNormalBomb;
	long CVSmartBomb;
	long CVTerraBomb;
	long CVJumpGate;
	long CVEngines;
	
	// Bomb kill stats
	double CVNormalKillPercentage;
	bool CCalcNormalKillPercentage;
	long CVKillMin;
	long CVKillInstallation;
	
	deque<long> CVTechLevel;		///< Calculated Tech levels needed to build this ship.
	deque<long> CVMineAmount;
	deque<double> CVFuelUsage;
	deque<bool> CalcedFuelUsage;
	deque<long> CVRadiation;
};
}
#endif // !defined(FreeStars_Ship_h)
