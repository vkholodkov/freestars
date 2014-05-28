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
 **@file RacialTrait.h
 **@ingroup Server
 **@brief Racial Traits.
 **/

#if !defined(FreeStars_RacialTrait_h)
#define FreeStars_RacialTrait_h

#include <string>
#include <deque>
using namespace std;

#include "FSTypes.h"
#include "Component.h"
class TiXmlNode;

namespace FreeStars {
class Player;
class Hull;
class Race;

/**
 * Racial Trait.
 * A Racial Trait is a special ability of a race.
 * It modifies game behavior, giving special bonuses
 * or penalties.
 * @ingroup Server
 */
class RacialTrait {
public:
	RacialTrait();
	virtual ~RacialTrait();
	static void CleanUp();
	static RacialTrait * ParseNode(const TiXmlNode * node, bool interaction = false);
	static bool LoadRacialTraits(const TiXmlNode * node);
	static const RacialTrait * ParsePRT(const char * str);
	static const RacialTrait * ParseLRT(const char * str);
	static void AddInteractions(deque<const RacialTrait *> & mLRTs, const RacialTrait * mPRT);

	string Name() const							{ return mName; }
	long PointCost() const						{ return mPointCost; }
	double GroundAttackFactor() const			{ return mGroundAttackFactor; }
	double GroundDefenseFactor() const			{ return mGroundDefenseFactor; }
	double GrowthRateFactor() const				{ return mGrowthRateFactor; }
	double PopulationFactor() const				{ return mPopulationFactor; }
	HullType InherentCloakHull() const			{ return mInherentCloakHull; }
	long InherentCloakAmount() const			{ return mInherentCloakAmount; }
	bool CloakCargo() const						{ return mCloakCargo; }
	long MineSpeedBonus() const					{ return mMineSpeedBonus; }
	double SpyTechBonus() const					{ return mSpyTechBonus; }
	double BattleSpeedBonus() const				{ return mBattleSpeedBonus; }
	double ComponentCostFactor(ComponentType ct) const;
	double PermaformOdds() const				{ return mPermaformOdds; }
	long PermaformPopAdj() const				{ return mPermaformPopAdj; }
	bool CanSeeHabSettings() const				{ return mCanSeeHabSettings; }
	bool TemporaryTerraform() const				{ return mTemporaryTerraform; }
	bool ScanDesign() const						{ return mScanDesign; }
	double RepairFactor() const					{ return mRepairFactor; }
	double FreighterReproduction() const		{ return mFreighterReproduction; }
	bool MineFieldScanning() const				{ return mMineFieldScanning; }
	bool CanRemoteDetonate() const				{ return mCanRemoteDetonate; }
	double MineDecayFactor() const				{ return mMineDecayFactor; }
	bool MoveAndLayMines() const				{ return mMoveAndLayMines; }

	double PacketTerraformOdds() const			{ return mPacketTerraformOdds; }
	long PacketTerraformMass() const			{ return mPacketTerraformMass; }
	double PacketPermaformOdds() const			{ return mPacketPermaformOdds; }
	long PacketPermaformMass() const			{ return mPacketPermaformMass; }
	bool PacketScanning() const					{ return mPacketScanning; }
	double PacketCostMinFactor() const			{ return mPacketCostMinFactor; }
	long PacketSizeOneMin() const				{ return mPacketSizeOneMin; }
	long PacketSizeMixed() const				{ return mPacketSizeMixed; }
	long PacketCostResources() const			{ return mPacketCostResources; }
	double PacketDecayFactor() const			{ return mPacketDecayFactor; }
	double PacketCatchFactor() const			{ return mPacketCatchFactor; }
	long PacketDecayPenalty() const				{ return mPacketDecayPenalty; }

	long StartingTech(TechType tt) const		{ return mStartingTech[tt]; }
	long StartAtBonus() const					{ return mStartAtBonus; }
	bool HasSecondPlanet() const				{ return mSecondPlanet; }
	bool GateCargo() const						{ return mGateCargo; }
	double OvergateLossFactor() const			{ return mOvergateLossFactor; }
	bool GateScanning() const					{ return mGateScanning; }
	long ARTechType() const						{ return mARTechType; }
	long ARDivisor() const						{ return mARDivisor; }
	double FuelFactor() const					{ return mFuelFactor; }
	double MainResearch() const					{ return mMainResearch; }
	double OtherResearch() const				{ return mOtherResearch; }
	bool UltimateRecycle() const				{ return mUltimateRecycle; }
	double EngineFailure(long speed) const		{ return (speed > mEngineFailureSpeed) ? mEngineFailureRate : 0.0; }
	double SpaceScanFactor() const				{ return mSpaceScanFactor; }
	double PenScanFactor() const				{ return mPenScanFactor; }
	double StartingPopFactor() const			{ return mStartingPopFactor; }
	double MaxMiniturize() const				{ return mMaxMiniturize; }
	double MiniturizeRate() const				{ return mMiniturizeRate; }
	double ZeroTechCost() const					{ return mZeroTechCost; }
	double ShieldFactor() const					{ return mShieldFactor; }
	double ArmorFactor() const					{ return mArmorFactor; }
	double ShieldRegenRate() const				{ return mShieldRegenRate; }
	long TerraformFactor0() const				{ return mTerraformFactor0; }
	long TerraformFactor1() const				{ return mTerraformFactor1; }
	long TerraformFactor2() const				{ return mTerraformFactor2; }
	long FactoryProductionCost() const			{ return mFactoryProductionCost; }

	double DefenseFactor() const				{ return mDefenseFactor; }

	void ParseStartShips(const TiXmlNode * node);
	void ParseStartShip(bool base, const TiXmlNode * node);
	long StartShipCount() const					{ return mShipDesigns.size(); }
	const Ship * GetStartDesign(long i) const	{ return mShipDesigns[i]; }
	long GetStartCount(long i) const			{ return mShipCounts[i]; }
	long GetStart2ndCount(long i) const			{ return mShip2ndCounts[i]; }
	
	// To know what traits are possible. -=K=-
	static long PrimaryTraitCount()						{ return mPRTs.size(); }
	static const RacialTrait * GetPrimaryTrait(long i)	{ return mPRTs[i]; }
	static long LesserTraitCount()						{ return mLRTs.size(); }
	static const RacialTrait * GetLesserTrait(long i)	{ return mLRTs[i]; }

	// Builtin scanning -- JOAT and AR in base rules
	long BuiltinScan(const Player * player, HullType hc, bool PenScan, long pop = 0) const;

private:
	string mName;
	long mPointCost;
	double mGroundAttackFactor;
	double mGroundDefenseFactor;
	double mGrowthRateFactor;
	double mPopulationFactor;
	HullType mInherentCloakHull;
	long mInherentCloakAmount;
	bool mCloakCargo;
	long mMineSpeedBonus;
	double mSpyTechBonus;
	double mBattleSpeedBonus;
	deque<ComponentType> mComponentCostType;
	deque<double> mComponentCostFactor;
	double mPermaformOdds;
	long mPermaformPopAdj;
	bool mCanSeeHabSettings;
	bool mTemporaryTerraform;
	bool mScanDesign;
	double mRepairFactor;
	double mFreighterReproduction;
	bool mMineFieldScanning;
	bool mCanRemoteDetonate;
	double mMineDecayFactor;
	bool mMoveAndLayMines;

	double mPacketTerraformOdds;
	long mPacketTerraformMass;
	double mPacketPermaformOdds;
	long mPacketPermaformMass;
	bool mPacketScanning;
	double mPacketCostMinFactor;
	long mPacketSizeOneMin;
	long mPacketSizeMixed;
	long mPacketCostResources;
	double mPacketDecayFactor;
	double mPacketCatchFactor;
	long mPacketDecayPenalty;

	deque<long> mStartingTech;
	long mStartAtBonus;
	bool mSecondPlanet;
	bool mGateCargo;
	double mOvergateLossFactor;
	bool mGateScanning;
	long mARTechType;
	long mARDivisor;
	double mFuelFactor;
	double mMainResearch;
	double mOtherResearch;
	bool mUltimateRecycle;
	long mEngineFailureSpeed;
	double mEngineFailureRate;
	double mSpaceScanFactor;
	double mPenScanFactor;
	double mStartingPopFactor;
	double mMaxMiniturize;
	double mMiniturizeRate;
	double mZeroTechCost;
	double mShieldFactor;
	double mArmorFactor;
	double mShieldRegenRate;

	// terraform factors for determining RW points
	long mTerraformFactor0;
	long mTerraformFactor1;
	long mTerraformFactor2;
	long mFactoryProductionCost;

	double mDefenseFactor;

	HullType mBISHull;
	HullType mBISHullPen;
	TechType mBISTech;
	long mBISMultiplier;
	bool mBISCheckPenFactor;
	double mBISPopFactor;

	deque<Ship *> mShipDesigns;
	deque<long> mShipCounts;
	deque<long> mShip2ndCounts;
	deque<const RacialTrait *> mInteractionTraits;

	static deque<RacialTrait *> mPRTs;
	static deque<RacialTrait *> mLRTs;
	static deque<RacialTrait *> mInteractions;
};

}
#endif // !defined(FreeStars_RacialTrait_h)
