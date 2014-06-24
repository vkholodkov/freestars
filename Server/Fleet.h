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
 **@file Fleet.h
 **@ingroup Server
 **@brief Fleet.
 **/

#if !defined(FreeStars_Fleet_h)
#define FreeStars_Fleet_h

#include <string>
#include <deque>
using namespace std;

#include "Rules.h"
#include "Stack.h"
#include "Galaxy.h"
#include "CargoHolder.h"
#include "Chaser.h"
class TiXmlNode;

namespace FreeStars {
class Player;
class Planet;
class Ship;
class MineField;
class WaypointOrder;
class WayOrder;
class WayOrderList;

/**
 * A group of stacks.
 */
class Fleet : public CargoHolder, public Chaser {
public:
	Fleet(Galaxy*);
	Fleet(int id, const CargoHolder &loc);
	virtual ~Fleet();

	void Init();
	virtual bool ParseNode(const TiXmlNode * node, Player * player, bool other);
	virtual TiXmlNode * WriteNode(TiXmlNode * node, const Player * viewer) const;

	virtual const string GetName(const Player * viewer) const;
	/// Get amount of fuel stored in fleet.
	long GetFuel() const	{ return mFuel; }
	long GetFuelNeeded() const;	///< Fuel needed to make it to the next waypoint (waypoint 2)
	void AdjustFuel(long amount);	///< Add or remove fuel from the fleet.
	/// Refuel fleet at starbase.
	/// @note should probabaly change return type to bool.
	void Refuel()			{ if (mFuel < GetFuelCapacity()) Refuel(InOrbit()); }
	/// Refuel fleet at specified planet.
	void Refuel(const Planet * p);
	/// Change name of fleet.
	void SetName(const char * n)	{ mName = n; }

	/// Waypoint orders and turn generation stuff
	enum FuncType {
		FTScrap,					///< Scrap fleet order
		FTUnload0,					///< Waypoint zero unload order
		FTColonize0,				///< Waypoint zero colonize.
		FTLoad0,					///< Waypoint zero load order
		FTMove,						///< Fleet travel
		FTRemoteMine,				///< Remote mining order
		FTFreighterReproduction,	///< Coloninist in space growth (IS PRT)
		FTCheckWaypoint,			///< Check waypoint (???)
		FTRefuel,					///< Refuel at local starbase order
		FTUnload1,					///< Waypoint one unload order
		FTColonize1,				///< Waypoint one colonize order
		FTLoad1,					///< Waypoint one load order
		FTMerge,					///< Merge fleets order
		FTClearWaypoint,			///< Clear waypoint (???)
		FTRemoteTerraform,			///< Remote Terraforming (CA ability -- orbital adjuster module)
		FTRepair,					///< Health regeneration
		FTSweepMines,				///< Mine sweeping
		FTLayMines,					///< Lay mines order
		FTTransfer,					///< Transfer fleet to another player (Can't this be a wp0 too?)
	};
	/// Process an order
	bool Process(FuncType func, bool arg);
	/**
	 * Scrap fleet for minerals.
	 * This will scuttle a fleet, recycling it for minerals that are then deposited on the planet the fleet was in orbit of, or create salvage if the order was carried out in space.
	 * @note Expanded rules -- What happens if you scrap a crew-carrying ship in space?
	 * @param colonize Whether to use the standard scrapping rules or the colonization scrapping rules.
	 */
	void Scrap(bool colonize);
	/**
	 * Kill fleet.
	 * This will completely remove a fleet.
	 * The server will send out notification, and everything following it will be told of the
	 * last location the fleet existed at.
	 * No scrap will be generated.
	 * Useful for Mystery Trader leaving the galaxy, etc.
	 */
	void KillFleet();
	void ProcessTransport(bool load, bool dunnage);
	/**
	 * Colonize planet.
	 * This will attempt colonization of the planet the fleet is orbiting.
	 * Upon success, the planet will be colonized, and the fleet will be scrapped.
	 */
	void Colonize();
	/**
	 * Remote mining.
	 * Perform remote mining.
	 * Alternate Reality race does not inhabit their actual planets, so all mining for that race
	 * is remote mining.
	 * @param ARmining Is this an AR race mining? If so, they are allowed to mine their own planets remotely.
	 */
	void RemoteMine(bool ARmining);
	/**
	 * Move fleet.
	 * This will resolve the fleet movement for the turn.
	 * Handles stuff like the actual movement, gating, overgating damage, mine collision, etc.
	 */
	bool Move();
	/// Perform tasks on arrival.
	void MoveArrive();
	/**
	 * Population growth in transit.
	 * This handles the population growth capability of IS.
	 */
	void FreighterReproduction();
	/**
	 * Move toward destination.
	 * This function is called for each fleet for every lightyear of travel during turn generation so the server can check for minefield collisions.
	 */
	bool Move1LY(Location * dest, long speed);
	void CheckWaypoint();
	void ClearWaypoint();
	void CheckMerge();
	void RemoteTerraform(bool bomb);
	void Repair();
	/**
	 * Sweep mines.
	 * Will only sweep mines owned by enemies.
	 */
	void SweepMines();
	/**
	 * Lay a minefield.
	 */
	void LayMines();
	void Transfer();
	/**
	 * Go on patrol.
	 * If a fleet's order is to patrol, it will pursue and engage hostiles within a set range.
	 */
	void Patrol();

	virtual long GetCloak(const Player *, bool) const;
	/**
	 * Merge fleet.
	 * This will merge the entire fleet into another fleet.
	 * @param to Fleet to merge into.
	 */
	void MergeTo(Fleet * to);
	/**
	 * Partial merge.
	 * This will handle merging some ships from this fleet into another.
	 * @param to Target fleet.
	 * @param Ship Type of ship to transfer.
	 * @param number Count to transfer.
	 * @param damaged Number of damaged ships to transfer.
	 */
	void MergeTo(Fleet * to, const Ship * design, long number, long damaged);
	bool IsEmpty() const	{ return mStacks.size() == 0; }
	int GetStacks() const	{ return mStacks.size(); }
	const Stack * GetStack(int i) const	{ return &mStacks[i]; }
	Stack * GetStack(int i)				{ return &mStacks[i]; }
	const deque<WayOrder *> & GetOrders() const		{ return mOrders; }
	deque<WayOrder *> & GetOrders() { return mOrders; }
	const WayOrder* GetFirstOrder() { return mOrders[0]; }
	void SetStartOrders(Planet * planet);
	void SetNextRoute(Planet * planet);
	long GetBestSpeed(const Location * L1, const Location * L2, OrderType ot);

	void ChangeWaypoints(WayOrderList & wol);
	void SetRepeat(bool repeat);
	bool GetRepeat() const	{ return mRepeatOrders; }
	void SetBattlePlan(long bp);

	void TakeMinefieldDamage(const MineField * field);

	void SetSeenDesign(long p, bool seen, bool design);
	virtual void ResetSeen();
	virtual void SetSeenBy(long p, long seen);
	virtual void SetCanLoadBy(const Player * player);
	virtual bool CanLoadBy(const Player * player) const;
	bool HasHull(HullType ht) const;

	// stuff from components
	long TechLevel(TechType tech) const;
	const Cost & GetCost() const;
	long GetCloaking() const;
	virtual bool CanStealShip() const;
	virtual bool CanStealPlanet() const;
	long GetCargoCapacity() const;
	long GetScanPenetrating() const;
	long GetScanSpace() const;
	long GetFuelCapacity() const;
	long GetFuelGeneration() const;
	long GetMass() const;
	long GetMaxMass() const;
	double GetMaxTachyon() const;
	long GetSweeping() const;
	long GetMines() const;
	double GetMaxDampener() const;
	double GetMaxRepairRate() const;
	bool DoesRadiate(HabType ht) const;
	double RadDamage() const;
	bool CanColonize() const;
	long GetTerraPower(long type) const;
	long GetMineAmount(long type) const;
	double GetFuelUsage(long speed) const;
	long GetMinSafeSpeed() const;
	long GetMinMaxSpeed() const;
	long GetMinFreeSpeed() const;
	long GetMinBattleSpeed() const;
	bool CanShoot() const;		///< Is any ship of this fleet armed?
	bool CanNormalBomb() const;	///< Can any ship do normal bombing?
	bool CanSmartBomb() const;	///< Can any ship smart bomb?
	bool CanTerraBomb() const;	///< Can any ship terra bomb?
	bool CanJumpGate() const;	///< Can this fleet use a jumpgate?
	bool CanBomb() const{return CanNormalBomb() || CanSmartBomb() || CanTerraBomb();}///< Can any ship bomb planets?
	void AddShips(long Type, long number);
	void AddShips(const Ship * design, long number);
	bool AlreadyFought() const	{ return mAlreadyFought; }
	void SetAlreadyFought()		{ mAlreadyFought = true; }
	const BattlePlan * GetBattlePlan() const;
	void SetInBattle(bool in)	{ mInBattle = in; }
	bool GetInBattle() const	{ return mInBattle; }
	long GetShipCount() const;
	bool KillShips(long i, long count, bool salvage);

	double GetNormalKillPercentage() const;
	//long GetSmartKillPercentage() const;
	long GetKillMin() const;
	long GetKillInstallation() const;

	virtual void ChaseeGone(SpaceObject * chasee);
	virtual int GetOwnerID() const;

private:
	void Merge(Stack &stack, long number, long damaged, long Origin);

	long mFuel;
	string mName;
	deque<Stack> mStacks;  ///< stacks of different ships
	long mRepairRate;	///< how much this fleet will repair this turn - 0: nothing (gated); 1: fleet moved through space; 2: fleet didn't move
	long mBattlePlan;
	bool mAlreadyFought;
	bool mInBattle;
	bool mHasMoved;
	long mDistMoved;	///< distance already moved this turn
	Location mStartPos;	///< where this fleet started the turn
	double mPX;	///< current location while in flight
	double mPY;	///< current location while in flight
	bool mDoneWaypoint;	///< true if we've arrived at our destination and are ready to do waypoint 1 tasks

	deque<WayOrder *> mOrders;
	bool mRepeatOrders;

	friend class WayOrder;
	friend class Battle;
	friend class Bombing;

	const Fleet * mChasing;	///< other fleet this fleet is chasing, at waypoint 1 only

	deque<MineField *> mPossibleMines;
	double ReClosestMinefield(double dist);
	MineField * CheckMineHits(int speed);

	// true if a player # can load from this (can a ship in the same location steal from it, or is a ship remote mining it)
	// set to true or false on the end of movement for thieving capabilities.
	deque<bool> mCanLoadBy;

	// Calculated Values
	Cost CVCost;
	long ReCost;
	void ResetDefaults();	///< reset all Calculated Values to defaults
	long CVScanPenetrating;
	long CVScanSpace;
	long CVCloaking;
	long CVFuelCapacity;
	long CVFuelGeneration;
	long CVMass;
	long CVMaxMass;
	bool CCalcMaxMass;
	double CVMaxTachyon;
	bool CCalcMaxTachyon;
	long CVCargoCapacity;
	long CVMines;
	long CVSweeping;

	double CVMaxDampener;
	bool CCalcMaxDampener;
	double CVMaxRepairRate;
	bool CCalcMaxRepairRate;
	long CVMinSafeSpeed;
	long CVMinMaxSpeed;
	long CVMinFreeSpeed;
	long CVMinBattleSpeed;

	long CVColonize;
	long CVStealShip;
	long CVStealPlanet;
	long CVShoot;
	long CVNormalBomb;
	long CVSmartBomb;
	long CVTerraBomb;

	double CVNormalKillPercentage;
	bool CCalcNormalKillPercentage;
	//long CVSmartKillPercentage;
	long CVKillMin;
	long CVKillInstallation;
	long CVJumpGate;

	deque<long> CVTechLevel;
	deque<long> CVMineAmount;
	deque<long> CVRadiation;
};
}
#endif // !defined(FreeStars_Fleet_h)
