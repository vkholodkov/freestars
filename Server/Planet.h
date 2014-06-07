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
 **@file Planet.h
 **@ingroup Server
 **@brief Planet stuff.
 **/

#if !defined(FreeStars_Planet_h)
#define FreeStars_Planet_h

#include <string>
#include <deque>
using namespace std;

#include "Player.h"
#include "Rules.h"

#include "CargoHolder.h"
#include "Invasion.h"

namespace FreeStars {
class Ship;
class Message;
class Creation;
class ProductionOrder;
class ProdOrder;

/**
 * Planet.
 * A planet.
 * @ingroup Server
 */
class Planet : public CargoHolder {
public:
	Planet();
//	Planet(int x, int y);
	virtual ~Planet();
	static const char* ELEMENT_NAME()	{return "Planet";}; ///< Name of XML element associated with class.
	virtual bool ParseNode(const TiXmlNode * node);
	virtual TiXmlNode * WriteNode(TiXmlNode * node, const Player * viewer) const;
	/// planets are not technically 'in orbit', but return ourselves if some one asks
	virtual Planet * InOrbit()				{ return this; }
	virtual const Planet * InOrbit() const	{ return this; }

	void Init();
	void SetDestinations();

	void AddScrapRes(int amount)	{ mScrapRes += amount; }
	long GetBaseNumber() const		{ return mBaseDesign; }
	void SetBaseNumber(long n);
	const Ship * GetBaseDesign() const	{ return GetOwner()->GetBaseDesign(mBaseDesign); }
	long GetBaseDamage() const		{ return mBaseDamage; }
	void SetBaseDamage(long bd)		{ mBaseDamage = bd; }
	virtual const string GetName(const Player *) const	{ return mName; }
	const string GetName() const	{ return mName; }
	static const string SGetName(Planet * p)	{ return p->GetName(); }
	static const string SGetName(const Planet * p)	{ return p->GetName(); }
	long GetResources() const;
	virtual long GetCargoCapacity() const		{ return -1; }
	void Invade(Player * invader, long amount);
	void ResolveInvasion();
	void TakePlanet(Player * invader, long amount);
	void DeadCheck();
	void Kill();

	void AdjustDefenses(long amount);
	void AdjustFactories(long amount);
	void AdjustMines(long amount);
	virtual long GetCloak(const Player *, bool) const	{ return 0; }
	virtual double GetMaxTachyon() const;
  /** 
	 * Get pen scanning radius.
	 * This returns the 100% penetrating scanning radius
	 * for the plannet scanner,
	 * or 0 if there is no scanner or the scanner doesn't
	 * have penscan capabilities.
	 */
	virtual long GetScanPenetrating() const;
	/**
	 * Get the scanning radius.
	 * This returns the 100% normal scanning radius
	 * for the plannet scanner,
	 * or 0 if there is no scanner.
	 */
	virtual long GetScanSpace() const;
	const Planet * GetRoute() const		{ return mRouteTo; }
	/**
	 * Get packet driver target.
	 * This will return the planet the packet driver
	 * is locked onto.
	 */
	Planet * GetPacketDest()			{ return mPacketDest; }
	const Planet * GetPacketDest() const      { return mPacketDest; }
	/**
	 * Get packet driver speed.
	 * This will return the speed the packet driver
	 * is set at.
	 */
	int GetPacketSpeed() const				{ return mPacketSpeed; }
	friend bool operator==(const Planet & p1, const Planet & p2)	{ return p1.mName == p2.mName; }
	friend bool operator!=(const Planet & p1, const Planet & p2)	{ return p1.mName != p2.mName; }
	double GetDefenseValue() const;
	double GetSmartDefenseValue() const;
	double GetInstallationDefenseValue() const	{ return GetDefenseValue() / 2; }
	double GetDisplayDef() const;
	long GetDisplayPop() const;
	long GetMaxPop() const;
    long GetMiningVelocity(long) const;
    long GetMiningVelocity(long, const Player*, long) const;
	void Mine();
	void Mine(long mines, const Player * miner);
	void DoProduction();
	long PopGrowth() const;
	void BuildFactories(long number)	{ mFactories += number; mBuiltFactories += number; }
	void BuildMines(long number)		{ mMines += number; mBuiltMines += number; }
	void BuildDefenses(long number)		{ mDefenses += number; mBuiltDefenses += number; }
	void BuildAlchemy(long number)		{ mBuiltAlchemy += number; }
	void BuildScanner()					{ mScanner = true; }
	long GetFactories() const			{ return mFactories; }
	long GetMines() const				{ return mMines; }
	long GetDefenses() const			{ return mDefenses; }
	bool GetScanner() const				{ return mScanner; }
	long GetMinConc(long type) const		{ return type < 0 || type >= Rules::MaxMinType ? 0 : mMinConc[type]; }
	long GetMineProgress(long type) const	{ return type < 0 || type >= Rules::MaxMinType ? 0 : mMinMined[type]; }
	Message* AddMessageOwner(string type) {return NCGetOwner()->AddMessage(type);};	// Add a message to owner of planet
	long MaxFactories() const;
	long MaxMines() const;
	long MaxDefenses() const;
	long CanTerraform(const Component * comp) const;
	long CanTerraform(HabType ht, long max) const;

	virtual void SetCanLoadBy(const Player * player)	{ mCanLoadBy[player->GetID()-1] = true; }
	virtual bool CanLoadBy(const Player * player) const	{ return mCanLoadBy[player->GetID()-1]; }
	long GetHabValue(HabType ht) const		{ return mHabTerra[ht]; }
	void SetHabValue(HabType ht, long v)	{ mHabTerra[ht] = v; }
//	bool Terraform(const Player * player, bool positive);		// Terraforms a world 1 tick, and returns true if any change was made
	HabType BestHabForTerraform(const Player * owner, const Player * performer, bool positive, long amt, HabType ht, long * tick);
	void Terraform(long totech, long toinit, HabType ht, const Player * performer, const Player * owner, Message * mess);
	void Instaform();
	void RemoteTerraform(Fleet * fleet, bool bomb);
	bool Permaform(const Player * performer, long amount, HabType ht, bool ImmToExtreme);

	Cost GetPacketCost(long type) const;
	virtual void ResetSeen();

	void CreateRandom(Creation * c);
	void CreateHW(const Player * player);
	void AdjustHW(Player * player);
    bool IsHW() const { return mHomeWorld; }
	void CreateSecondWorld(const Planet * HW);
	void AdjustSecondWorld(Player * player);
	void ParseProduction(const TiXmlNode * node);
    const deque<ProdOrder *> GetProduction() const { return mProductionQ; }
	void SetProduction(const deque<ProdOrder *> & ords);
	void SetPayTax(bool paytax);
	void SetInBattle()	{ mHadBattle = true; }
	void RepairBase();

	void SweepMines();
	void SetPacketSpeed(int speed);
	void SetPacketDest(Planet * pdest);
	void SetRoute(const Planet * rdest);

	friend bool operator ==(const Planet &a, const char * b)	{ return a.mName == b; }
	friend bool operator !=(const Planet &a, const char * b)	{ return !(a == b); }

private:
	void DeleteProdQ();
	void CopyProdQ(const deque<ProdOrder *> &prod);
	string mName;
	long mScrapRes;
	long mBaseDesign;
	long mFactories;
	long mMines;
	deque<long> mMinConc;
	deque<long>	mMinMined;	// amount of mines on this concentration
	bool mScanner;
	long mDefenses;
	const Planet * mRouteTo;
	Planet * mPacketDest;
	string mRouteName;	// temp till we read in all planets;
	string mPacketName;	// temp till we read in all planets;
	long mPacketSpeed;
	long mBaseDamage;
	/**
	 * Production queue.
	 */
	deque<ProdOrder *> mProductionQ;
	deque<Invasion> mInvasions;
	deque<long> mHabTerra;
	deque<long> mHabStart;
	long mArtifactType;
	long mArtifactAmount;
	bool mPayTax;
	/**
	 * Is this planet a homeworld?
	 */
	bool mHomeWorld;
	long mBuiltFactories;
	long mBuiltMines;
	long mBuiltDefenses;
	long mBuiltAlchemy;
	long mDisplayPop;
	bool mHadBattle;

	// true if a player # can load from this (can a ship in the same location steal from it, or is a ship remote mining it)
	// set to true or false on the end of movement for thieving capabilities. Set to true when mining planets for remote miners
	deque<bool> mCanLoadBy;

	friend class ProdOrder;
};

const TechType ARTI_NONE		= -1;
const TechType ARTI_ALL			= -2;	/*!< give amount to all fields */
const TechType ARTI_RANDTYPE	= -3;	/*!< give all to a random field */
const TechType ARTI_RANDSPLIT	= -4;	/*!< divide amount into Rules::MaxTechType * 2 parts and assign them randomly */
}
#endif // !defined(FreeStars_Planet_h)
