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
 **@file Race.h
 **@ingroup Server
 **@brief Races.
 **/

#if !defined(FreeStars_Race_h)
#define FreeStars_Race_h

#include <string>
#include <deque>
#include <algorithm>
using namespace std;

#include "Cost.h"
#include "FSTypes.h"
#include "Component.h"
#include "MessageSink.h"
class TiXmlNode;

namespace FreeStars {
class Player;
class Hull;
class RacialTrait;
class TestPlanet;

const long LOPB_MINERALS		= 1;
const long LOPB_CONCENTRATIONS	= 2;
const long LOPB_MINES			= 3;
const long LOPB_FACTORIES		= 4;
const long LOPB_DEFENSES		= 5;

/**
 * Race.
 * @ingroup Server
 */
class Race {
public:
	Race();
	virtual ~Race();
	void ResetDefaults();
	bool ParseNode(const TiXmlNode * node, bool other, MessageSink& messageSink = NullMessageSink::Instance());
	void WriteNode(TiXmlNode * node) const;
	void WriteHabs(TiXmlNode * node) const;

	const RacialTrait * GetPRT() const	{ return mPRT; }
	bool HasLRT(const RacialTrait * lrt) const		{ return find(mLRTs.begin(), mLRTs.end(), lrt) != mLRTs.end(); }

	string GetSingleName() const		{ return mSingularName; }
	string GetPluralName() const		{ return mPluralName; }

	// Racial Trait abilities
	long PointCost() const;
	double GroundAttackFactor() const;
	double GroundDefenseFactor() const;
	double GrowthRateFactor() const;
	double PopulationFactor() const;
	long InherentCloaking(HullType hc) const;
	bool CloakCargo() const;
	long MineSpeedBonus() const;
	double SpyTechBonus() const;
	double BattleSpeedBonus() const;
	double ComponentCostFactor(ComponentType ct) const;
	double PermaformOdds() const;
	long PermaformPopAdj() const;
	bool CanSeeHabSettings() const;
	bool TemporaryTerraform() const;
	bool ScanDesign() const;
	double RepairFactor() const;
	double FreighterReproduction() const;
	bool MineFieldScanning() const;
	bool CanRemoteDetonate() const;
	double MineDecayFactor() const;
	bool MoveAndLayMines() const;

	long PacketTerraform(CargoType ct) const;
	double PacketTerraformOdds() const;
	long PacketTerraformMass() const;
	double PacketPermaformOdds() const;
	long PacketPermaformMass() const;

	bool PacketScanning() const;
	double PacketCostMinFactor() const;
	long PacketSizeOneMin() const;
	long PacketSizeMixed() const;
	long PacketCostResources() const;
	double PacketDecayFactor() const;
	double PacketCatchFactor() const;
	long PacketDecayPenalty() const;

	long PRTStartTech(TechType tt) const;
	long LRTStartTech(TechType tt) const;

	long StartAtBonus() const;
	bool HasSecondPlanet() const;
	bool GateCargo() const;
	double OvergateLossFactor() const;
	bool GateScanning() const;
	long ARTechType() const;
	long ARDivisor() const;
	double FuelFactor() const;
	double MainResearch() const;
	double OtherResearch() const;
	bool UltimateRecycle() const;
	double EngineFailure(long speed) const;
	double SpaceScanFactor() const;
	double PenScanFactor() const;
	double StartingPopFactor() const;
	double Miniturize(long diff) const
		{ return (diff == 0) ? ZeroTechCost() : min(MaxMiniturize(), MiniturizeRate() * diff); }
	double ZeroTechCost() const;
	double MaxMiniturize() const;
	double MiniturizeRate() const;
	double ShieldFactor() const;
	double ArmorFactor() const;
	double ShieldRegenRate() const;

	long TerraformFactor0() const;
	long TerraformFactor1() const;
	long TerraformFactor2() const;
	long FactoryProductionCost() const;

	double DefenseFactor() const;

	long BuiltinScan(const Player * player, HullType hc, bool PenScan, long pop = 0) const;


	// Race settings (everything after the LRT selection)
	double RadDamage(HabType ht) const;
//	double HabFactor(HabType ht, long tick) const;	// gives how habitable this is abs(tick - midpoint)/width or negative if outside range
	long HabFactor(const Planet * p) const;			// gives how habitable this planet is
	long HabCenter(HabType ht) const;
	long HabWidth(HabType ht) const;
	double GrowthRate() const				{ return mGrowthRate * GrowthRateFactor(); }
	long PopEfficiency() const				{ return mPopEfficiency; }
	long FactoryRate() const				{ return mFactoryRate; }
	const Cost & FactoryCost() const		{ return mFactoryCost; }
	long FactoriesRun() const				{ return mFactoriesRun; }
	long MineRate() const					{ return mMineRate; }
	const Cost & MineCost() const			{ return mMineCost; }
	long MinesRun() const					{ return mMinesRun; }
	double TechCostFactor(TechType tt) const;
	bool StartAt() const					{ return mStartAt; }

	const RacialTrait * GetLRT(int i) const	{ return mLRTs[i]; }
	long GetLRTCount() const				{ return mLRTs.size(); }

	long GetStartMinerals() const			{ return mLeftoverBuys == LOPB_MINERALS ? mLeftoverPoints : 0; }
	long GetStartConcentrations() const		{ return mLeftoverBuys == LOPB_CONCENTRATIONS ? mLeftoverPoints : 0; }
	long GetStartMines() const				{ return mLeftoverBuys == LOPB_MINES ? mLeftoverPoints : 0; }
	long GetStartFactories() const			{ return mLeftoverBuys == LOPB_FACTORIES ? mLeftoverPoints : 0; }
	long GetStartDefenses() const			{ return mLeftoverBuys == LOPB_DEFENSES ? mLeftoverPoints : 0; }

	long GetAdvantagePoints(MessageSink &messageSink = NullMessageSink::Instance()) const;
	long GetHabPoints() const;
	void SetLeftoverPoints(long lop)		{ mLeftoverPoints = lop; }

protected:
	double HabPointsRecursion(int DesireFactor, int TTCorrFactor, int depth, deque<long> &testHabWidth, deque<long> &testHabStart, deque<long> &iterNum, deque<long> &TFDone, TestPlanet * testPlanet) const;
	//double HabPointsRecursion(int DesireFactor, int TTCorrFactor, int depth, deque<long> testHabWidth, deque<long> testHabStart, deque<long> iterNum, deque<long> TFDone, Planet * testPlanet) const;
	long HabFactor(const TestPlanet*) const;		// gives how habitable this test planet is
	const RacialTrait * mPRT;
	deque<const RacialTrait *> mLRTs;
	string mSingularName;
	string mPluralName;
	long mRaceEmblem;
	deque<double> mTechCostFactor;		///< (.5, 1.0, 1.75)

	deque<long> mHabCenter;
	deque<long> mHabWidth;
	double mGrowthRate;
	long mPopEfficiency;
	long mFactoryRate;
	string mPassword;
	Cost mFactoryCost;
	long mFactoriesRun;
	long mMineRate;
	Cost mMineCost;
	long mMinesRun;
	bool mStartAt;

	long mLeftoverPoints;
	long mLeftoverBuys;

	deque<long> mPacketMap;	///< what habs a mineral affects with packet terraforming, empty for most races

	// Calculated Values
	long CVPointCost;
	double CVGroundAttackFactor;
	bool CCalcGroundAttackFactor;
	double CVGroundDefenseFactor;
	bool CCalcGroundDefenseFactor;
	double CVGrowthRateFactor;
	bool CCalcGrowthRateFactor;
	double CVPopulationFactor;
	bool CCalcPopulationFactor;
	long CVCloakCargo;
	long CVMineSpeedBonus;
	double CVSpyTechBonus;
	bool CCalcSpyTechBonus;
	double CVBattleSpeedBonus;
	bool CCalcBattleSpeedBonus;
	double CVPermaformOdds;
	bool CCalcPermaformOdds;
	long CVPermaformPopAdj;
	long CVCanSeeHabSettings;
	long CVTemporaryTerraform;
	long CVScanDesign;
	double CVRepairFactor;
	bool CCalcRepairFactor;
	double CVFreighterReproduction;
	bool CCalcFreighterReproduction;
	long CVMineFieldScanning;
	long CVCanRemoteDetonate;
	double CVMineDecayFactor;
	bool CCalcMineDecayFactor;
	long CVMoveAndLayMines;

	double CVPacketTerraformOdds;
	bool CCalcPacketTerraformOdds;
	long CVPacketTerraformMass;
	double CVPacketPermaformOdds;
	bool CCalcPacketPermaformOdds;
	long CVPacketPermaformMass;
	long CVPacketScanning;
	double CVPacketCostMinFactor;
	bool CCalcPacketCostMinFactor;
	long CVPacketSizeOneMin;
	long CVPacketSizeMixed;
	long CVPacketCostResources;
	double CVPacketDecayFactor;
	bool CCalcPacketDecayFactor;
	double CVPacketCatchFactor;
	bool CCalcPacketCatchFactor;
	long CVPacketDecayPenalty;

	long CVStartAtBonus;
	long CVGateCargo;
	double CVOvergateLossFactor;
	bool CCalcOvergateLossFactor;
	long CVGateScanning;
	long CVARTechType;
	long CVARDivisor;
	double CVFuelFactor;
	bool CCalcFuelFactor;
	bool CCalcMainResearch;
	double CVMainResearch;
	bool CCalcOtherResearch;
	double CVOtherResearch;
	long CVUltimateRecycle;
	double CVSpaceScanFactor;
	bool CCalcSpaceScanFactor;
	double CVPenScanFactor;
	bool CCalcPenScanFactor;
	double CVZeroTechCost;
	bool CCalcZeroTechCost;
	double CVMaxMiniturize;
	bool CCalcMaxMiniturize;
	double CVMiniturizeRate;
	bool CCalcMiniturizeRate;
	double CVStartingPopFactor;
	bool CCalcStartingPopFactor;
	double CVShieldFactor;
	bool CCalcShieldFactor;
	double CVArmorFactor;
	bool CCalcArmorFactor;
	double CVShieldRegenRate;
	bool CCalcShieldRegenRate;
	long CVTerraformFactor0;
	long CVTerraformFactor1;
	long CVTerraformFactor2;
	long CVFactoryProductionCost;
	double CVDefenseFactor;
	bool CCalcDefenseFactor;
	deque<double> CVEngineFailure;
};

}
#endif // !defined(FreeStars_Race_h)
