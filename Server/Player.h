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
 **@file Player.h
 **@ingroup Server
 **@brief Player object.
 **/

#if !defined(FreeStars_Player_h)
#define FreeStars_Player_h

#include <string>
using namespace std;

#include "BattlePlan.h"
#include "Ship.h"
#include "ProdOrder.h"
#include "Fleet.h"
#include "Race.h"
#include "MineField.h"

namespace FreeStars {
class Message;
class Battle;
class Bombing;
class Order;
class BattlePlanOrder;
class RelationsOrder;
class ProductionOrder;
class MultipleOrder;

/**
 * Player.
 * @ingroup Server
 */
class Player : public Race {
public:
	Player(int id);
	virtual ~Player();

	void Remove()	{ delete this; }

	bool ParseNode(const TiXmlNode * node, bool other);
	bool ParseCommon(const TiXmlNode * node);
	long CreateFromFile(const char * file);
	void SetupRelations();
	bool WriteNode(TiXmlNode * node, const Player * viewer) const;
	void ParseFleet(const TiXmlNode * node, bool other);
	void ParseOrders(const TiXmlNode * node);
	CargoHolder * ParseTransport(const TiXmlNode * node, const CargoHolder * owned);
	void TransferCargo(CargoHolder * from, CargoHolder * to, long pop, long fuel, deque<long> & cargo, bool write = true);
	void ParseMessages(const TiXmlNode * node);
	void ResetSeen();
	void SetSeenDesign(long p, long design, bool base);
	void SetSeenHull(long p, long design, bool base);
	void SetSeenHab(const Player * viewer)	{ mSeenHab[viewer->GetID() - 1] = true; }

	Message * AddMessage(string type, const Location * loc);
	Message * AddMessage(string type);
	void WriteMessages(TiXmlNode * node, const char * type);
	void StoreMessageLocation(const Location * loc);

	// tech functions
	bool HasGotTech() const					{ return mGotTech; }
	void SetGotTech(bool val)				{ mGotTech = val; }
	long GainTech(long TechGain, TechType tech);
	long GainTech(long TechGain)			{ return GainTech(TechGain, mResearchField); }
	bool GainTechLevel(TechType tech);
	void GainSpyTech(long TechGain, TechType tech);
	void CheckTechGain();
	long TechCost(TechType tech) const;
	long TechProgress(TechType tech) const { return mTechProgress[tech]; };
	long GetTechLevel(TechType tech) const	{ return mTechLevel[tech]; }
	void AddProductionTech();
	long GetLastTechGainPhase() const		{ return mLastTechGainPhase; }

	void SetResearchTax(double tax);
	void SetResearchField(long current);
	void SetResearchNext(long next);
	double GetResearchTax() const			{ return mResearchTax; }
	long GetResearchField() const			{ return mResearchField; }
	long GetResearchNext() const			{ return mResearchNext; }

	long GetScanSpace() const				{ return mScanSpace; }
	long GetScanPenetrating() const			{ return mScanPenetrating; }
	const deque<ProdOrder *> & GetDefaultQ() const	{ return mDefaultQ; }
	bool GetDefaultPayTax() const			{ return mDefaultPayTax; }
	void SetDefaultPayTax(bool pt)			{ mDefaultPayTax = pt; }
	const Fleet * GetFleet(long n) const	{ return const_cast<Player *>(this)->NCGetFleet(n); }
	Fleet * NCGetFleet(unsigned long n);
	Fleet * GetFleetCreate(unsigned long n, const CargoHolder &loc);
	const Ship * GetShipDesign(unsigned long n) const;
	const Ship * GetBaseDesign(unsigned long n) const;
	void SetShipDesign(unsigned long, Ship*);
	void SetBaseDesign(unsigned long, Ship*);
	unsigned long GetBaseDesignNumber(const Ship * design) const
		{ return find(mBaseDesigns.begin(), mBaseDesigns.end(), design) - mBaseDesigns.begin(); }
	long GetShipNumber(const Ship * design) const
		{ return find(mShipDesigns.begin(), mShipDesigns.end(), design) - mShipDesigns.begin(); }
	Ship * GetExistingDesign(const Ship * check) const;
	Ship * GetExistingBaseDesign(const Ship * check) const;
	long GetFreeShipDesignSlot() const;
	long GetFreeBaseDesignSlot() const;

	void DeleteFleet(Fleet * gone);
	unsigned long GetID() const						{ return mID; }
	void SetRelations(const deque<long> rel);
	long GetRelations(const Player * p2) const		{ return p2 == this ? PR_SELF : mRelations[p2->GetID()-1]; }
	const BattlePlan * GetBattlePlan(unsigned long num) const;
	void ChangeBattlePlan(BattlePlan * bp, int num);
	double GetDefenseValue() const			{ return mDefenseValue * DefenseFactor(); }
	double Miniturize(const Component * comp) const;
	long TerraLimit(HabType ht) const		{ return mTerraLimit[ht]; }
	void BuildShips(Planet * planet, long Type, long number);
	bool TransferFleet(const Fleet * from);
	long ForEachFleet(Fleet::FuncType func, bool arg);

	void ResetTerraLimits();
	void SetTerraLimit(HabType ht, long limit);

	void DoBattles();
	void AddBattleFleets(Battle * bat);
	bool AddBombingFleets(Bombing * bom, const Player* owner,const long t) const; // returns true if it added any fleets
	void ClearBattleEnemies();
	void SetBattleEnemy(long p)			{ mBattleEnemy[p] = true; mInBattle = true; }
	bool GetBattleEnemy(long p) const	{ return mBattleEnemy[p]; }
	bool InThisBattle() const			{ return mInBattle; }
	
	const deque<Message *>& GetMessages() const {return mMessages;}
	long GetMineFieldID() const;
	MineField * GetMineField(long ID) const;
	void DeleteMineField(MineField * mf);
	void DecayMineFields();
	void SweepMines(SpaceObject * so, int sweep);
	void LayMines(const Location * loc, long type, int mines);
	void DetonateMineFields();
	SpaceObject * GetPatrolTarget(const Fleet * persuer, double * range) const;

	void AddStartShips(Planet * planet, bool HomeWorld);
	void AddStartShips(const RacialTrait * rt, int i, Planet * planet, bool HomeWorld);

	bool HasHW()		{ return mHasHW; }
	void PlaceHW(Planet * planet);
	void PlaceSW(Planet * second, Planet * homeworld);

	void IncrementBaseBuilt(long base)	{ mBaseDesigns[base]->IncrementBuilt(1); }
	void LoadFleets();
	void LoadMinefields();

	bool UnsavedChanges() const	{ return mUnsavedChanges != 0; }
	bool OpenOrdersFile();
	bool OpenOrdersFile(const char * file);
	bool SaveXFile();
	bool WriteXFile() const		{ return mWriteXFile; }
	void SetWriteXFile()		{ mWriteXFile = true; }
	void AddOrder(Order * o);
	void TestUndoRedo();
	const Order * GetOrder(int i)		{ return mOrders[i]; }
	bool UndoOrder(int pos = -1);
	bool RedoOrder();
	void StartMultipleOrder();
	void EndMultipleOrder();

	void ParseResearchTax(const TiXmlNode * node);
	long ParseResearchField(const TiXmlNode * node);

	void UndoBattlePlan(BattlePlanOrder * bpo);
	TiXmlNode * WriteBattlePlan(TiXmlNode * node, int num) const;

	void SetProduction(const deque<ProdOrder *> & ords);
	void SetPayTax(bool paytax);
	void ParseWaypoints(const TiXmlNode * node);

	double GetPossibleMines(deque<MineField *> *pm, const Fleet * f, double dist) const;

	virtual Fleet * FleetFactory();
	virtual BattlePlan * BattlePlanFactory();
	virtual MineField * MineFieldFactory();

private:
	double mResearchTax;
	long mResearchField;
	long mResearchNext;
	bool mGotTech;
	deque<long> mTechLevel;
	deque<long> mTempTechLevel;
	deque<long> mTechProgress;
	long mLastTechGainPhase;

	long mScanSpace;
	long mScanPenetrating;
	double mDefenseValue;
	deque<ProdOrder *> mDefaultQ;
	bool mDefaultPayTax;
	deque<Fleet *> mFleets;
	deque<Ship *> mShipDesigns;
	deque<Ship *> mBaseDesigns;
	deque<long> mRelations;
	deque<BattlePlan *> mBattlePlans;
	unsigned long mID;
	deque<long> mTerraLimit;
	deque<MineField *> mMineFields;
	deque<Message *> mMessages;
	deque<bool> mSeenHab;

	bool mWriteXFile;
	long mUnsavedChanges;
	deque<Order *> mOrders;
	MultipleOrder * mMO;

	// Temporary stuff for battles
	deque<bool> mBattleEnemy;	///< Will we fight this race in the current battle.
	bool mInBattle;				///< Are we in this battle at all?

	bool mHasHW;	// for setup only
};
}
#endif // !defined(FreeStars_Player_h)
