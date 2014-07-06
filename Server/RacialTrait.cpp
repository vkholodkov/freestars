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

#include "FSServer.h"


//#pragma warning(disable : 4786)	// identifier was truncated to '255' characters in the debug information
#include "RacialTrait.h"

#include "Hull.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

deque<RacialTrait *> RacialTrait::mPRTs;
deque<RacialTrait *> RacialTrait::mLRTs;
deque<RacialTrait *> RacialTrait::mInteractions;

RacialTrait::RacialTrait(Game *game)
	: mGame(game)
{
	mPointCost = 0;
	mGroundAttackFactor = 1.0;
	mGroundDefenseFactor = 1.0;
	mGrowthRateFactor = 1.0;
	mPopulationFactor = 1.0;
	mInherentCloakHull = HC_NONE;
	mInherentCloakAmount = 0;
	mCloakCargo = false;
	mMineSpeedBonus = 0;
	mSpyTechBonus = 0.0;
	mBattleSpeedBonus = 0.0;
	mPermaformOdds = 0.0;
	mPermaformPopAdj = 200000;
	mCanSeeHabSettings = false;
	mTemporaryTerraform = false;
	mScanDesign = false;
	mRepairFactor = 1.0;
	mFreighterReproduction = 0.0;
	mMineFieldScanning = false;
	mCanRemoteDetonate = false;
	mMineDecayFactor = 1.0;
	mMoveAndLayMines = false;

	mPacketTerraformOdds = 0.0;
	mPacketTerraformMass = 100;
	mPacketPermaformOdds = 0.0;
	mPacketPermaformMass = 1000;
	mPacketScanning = false;
	mPacketCostMinFactor = 1.1;	// Default 10% penalty to minerals in packets
	mPacketSizeOneMin = 100;
	mPacketSizeMixed = 40;
	mPacketCostResources = 10;
	mPacketDecayFactor = 1.0;
	mPacketCatchFactor = 1.0;

	mStartAtBonus = 3;
	mSecondPlanet = false;
	mGateCargo = false;
	mOvergateLossFactor = 1.0;
	mGateScanning = false;
	mARTechType = -1;
	mARDivisor = 10;
	mFuelFactor = 1.0;
	mMainResearch = 1.0;
	mOtherResearch = 0.0;
	mUltimateRecycle = false;
	mEngineFailureSpeed = Rules::GetConstant("MaxSpeed");
	mEngineFailureRate = 0.0;
	mSpaceScanFactor = 1.0;
	mPenScanFactor = 1.0;	// mostly not used, normally only applies to AR builtin scanning
	mStartingPopFactor = 1.0;
	mMaxMiniturize = 0.75;
	mMiniturizeRate = 0.04;
	mZeroTechCost = 1.0;
	mShieldFactor = 1.0;
	mArmorFactor = 1.0;
	mShieldRegenRate = 0.0;
	mTerraformFactor0 = 0;
	mTerraformFactor1 = 5;
	mTerraformFactor2 = 15;
	mFactoryProductionCost = 2;
	mStartingTech.insert(mStartingTech.begin(), Rules::MaxTechType, 0);

	mBISHull = HC_NONE;
	mBISHullPen = HC_NONE;
	mBISTech = -1;
	mBISMultiplier = 0;
	mBISCheckPenFactor = false;
	mBISPopFactor = 0.0;

	mDefenseFactor = 1.0;
}

RacialTrait::~RacialTrait()
{
	int i;
	for (i = 0; i < mShipDesigns.size(); ++i)
		delete mShipDesigns[i];
}

void RacialTrait::CleanUp()
{
	deque<RacialTrait *>::iterator i2;
	for (i2 = mPRTs.begin(); i2 != mPRTs.end(); ++i2)
		delete *i2;
	mPRTs.clear();

	for (i2 = mLRTs.begin(); i2 != mLRTs.end(); ++i2)
		delete *i2;
	mLRTs.clear();

	for (i2 = mInteractions.begin(); i2 != mInteractions.end(); ++i2)
		delete *i2;
	mInteractions.clear();
}

// Builtin scanning -- JOAT and AR in base rules
long RacialTrait::BuiltinScan(const Player * player, HullType hc, bool PenScan, long pop /*= 0*/) const
{
	long Scan = 0;
	if (mBISPopFactor > 0.0) {
		// AR scanning: Dependant on population
		if (hc & mBISHull) {
			Scan = long(sqrt(pop * mBISPopFactor));
			if (PenScan) {
				Scan /= 2;
				if (mBISCheckPenFactor)
					Scan = long(Scan * PenScanFactor());
				if (!(hc & mBISHullPen))
					Scan = 0;
			}
		}
	} else if (mBISTech >= 0) {
		// JOAT scanning: Dependant on tech and multiplier
		if (hc & mBISHull) {
			Scan = player->GetTechLevel(mBISTech) * mBISMultiplier;
			if (PenScan) {
				Scan /= 2;
				if (mBISCheckPenFactor)
					Scan = long(Scan * PenScanFactor());
				if (!(hc & mBISHullPen))
					Scan = 0;
			}
		}
	}

	return Scan;
}

double RacialTrait::ComponentCostFactor(ComponentType ct) const
{
	deque<ComponentType>::const_iterator iter;
	iter = find(mComponentCostType.begin(), mComponentCostType.end(), ct);
	if (iter != mComponentCostType.end()) {
		return mComponentCostFactor[iter - mComponentCostType.begin()];
	} else
		return 1.0;
}

void RacialTrait::ParseStartShips(const TiXmlNode * node)
{
	const TiXmlNode * child1;
	for (child1 = node->FirstChild("BaseDesign"); child1; child1 = child1->NextSibling("BaseDesign"))
		ParseStartShip(true, child1);

	for (child1 = node->FirstChild("ShipDesign"); child1; child1 = child1->NextSibling("ShipDesign"))
		ParseStartShip(false, child1);
}

void RacialTrait::ParseStartShip(bool base, const TiXmlNode * node)
{
	Ship * sd = NULL;
	sd = mGame->ObjectFactory(sd);

	sd->SetCannotBuild(mGame->ParseComponent(GetString(node->FirstChild("IfCannotBuild"))));
	if (sd->ParseNode(node, NULL, false)) {
		mShipDesigns.push_back(sd);
		mShipCounts.push_back(GetLong(node->FirstChild("ShipCount")));
		mShip2ndCounts.push_back(GetLong(node->FirstChild("SecondPlanet")));
	} else
		delete sd;
}

bool RacialTrait::LoadRacialTraits(Game *game, const TiXmlNode * node)
{
	if (!node)
		return false;

	const TiXmlNode * child;
	for (child = node->FirstChild("PrimaryRacialTrait"); child != NULL; child = child->NextSibling("PrimaryRacialTrait")) {
		mPRTs.push_back(RacialTrait::ParseNode(game, child));
	}

	for (child = node->FirstChild("LesserRacialTrait"); child != NULL; child = child->NextSibling("LesserRacialTrait")) {
		mLRTs.push_back(RacialTrait::ParseNode(game, child));
	}

	for (child = node->FirstChild("Interaction"); child != NULL; child = child->NextSibling("Interaction")) {
		mInteractions.push_back(RacialTrait::ParseNode(game, child, true));
	}

	return true;
}

const RacialTrait * RacialTrait::ParsePRT(const char * str)
{
	if (str != NULL && *str != '\0') {
		deque<RacialTrait *>::const_iterator iter;
		for (iter = mPRTs.begin(); iter != mPRTs.end(); ++iter) {
			if (stricmp((*iter)->Name().c_str(), str) == 0)
				return *iter;
		}
	}

	return NULL;
}

const RacialTrait * RacialTrait::ParseLRT(const char * str)
{
	if (str != NULL && *str != '\0') {
		deque<RacialTrait *>::const_iterator iter;
		for (iter = mLRTs.begin(); iter != mLRTs.end(); ++iter) {
			if (stricmp((*iter)->Name().c_str(), str) == 0)
				return *iter;
		}
	}

	return NULL;
}

void RacialTrait::AddInteractions(deque<const RacialTrait *> & mLRTs, const RacialTrait * mPRT)
{
	// this is a mess!
	int startLRTsize = mLRTs.size();
	for (int i = 0; i < mInteractions.size(); ++i) {
		int j=0;
		for (j = 0; j < mInteractions[i]->mInteractionTraits.size(); ++j) {
			if (mInteractions[i]->mInteractionTraits[j] == mPRT)
				continue;

			int k=0;
			for (k = 0; k < startLRTsize; ++k) {
				if (mInteractions[i]->mInteractionTraits[j] == mLRTs[k])
					break;
			}

			if (k >= startLRTsize) {	// not found
				break;
			}
		}
		if (j >= mInteractions[i]->mInteractionTraits.size()) {	// all found
			mLRTs.push_back(mInteractions[i]);
		}
	}
}

RacialTrait * RacialTrait::ParseNode(Game *game, const TiXmlNode * node, bool interaction /*= false*/)
{
	RacialTrait * rt = NULL;
	rt = game->ObjectFactory(rt);

	const TiXmlNode * child1;
	const TiXmlNode * child2;
	const char * ptr;
	ArrayParser arrayParser(*game);

	for (child1 = node->FirstChild(); child1; child1 = child1->NextSibling()) {
		if (child1->Type() == TiXmlNode::COMMENT)
			continue;

		if (stricmp(child1->Value(), "Name") == 0) {
			ptr = GetString(child1);
			if (interaction) {
				const RacialTrait * rti;
				rti = ParsePRT(ptr);
				if (rti == NULL)
					rti = ParseLRT(ptr);

				if (rti != NULL)
					rt->mInteractionTraits.push_back(rti);
				else {
					Message * mess = game->AddMessage("Warning: Invalid Racial Trait");
					mess->AddItem("Racial Interactions", child1->Value());
				}
			} else {
				if (ptr != NULL)
					rt->mName = ptr;
			}
		} else if (stricmp(child1->Value(), "PointCost") == 0) {
			rt->mPointCost = GetLong(child1);
		} else if (stricmp(child1->Value(), "ShipDesign") == 0) {
			// Ship designs are read after components are loaded, so skip for now
		} else if (stricmp(child1->Value(), "GroundAttackFactor") == 0) {
			rt->mGroundAttackFactor = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "GroundDefenseFactor") == 0) {
			rt->mGroundDefenseFactor = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "GrowthRateFactor") == 0) {
			rt->mGrowthRateFactor = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "PopulationFactor") == 0) {
			rt->mPopulationFactor = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "InherentCloaking") == 0) {
			child2 = child1->FirstChild("HullType");
			rt->mInherentCloakHull = Component::ParseHullType(GetString(child2), *game);
			child2 = child1->FirstChild("Cloaking");
			rt->mInherentCloakAmount = GetLong(child2);
		} else if (stricmp(child1->Value(), "CloakCargo") == 0) {
			rt->mCloakCargo = GetBool(child1);
		} else if (stricmp(child1->Value(), "MineSpeedBonus") == 0) {
			rt->mMineSpeedBonus = GetLong(child1);
		} else if (stricmp(child1->Value(), "SpyTechBonus") == 0) {
			rt->mSpyTechBonus = GetDouble(child1, 0.0);
		} else if (stricmp(child1->Value(), "BattleSpeedBonus") == 0) {
			rt->mBattleSpeedBonus = GetDouble(child1, 0.0);
		} else if (stricmp(child1->Value(), "ComponentCostFactor") == 0) {
			child2 = child1->FirstChild("ComponentType");
			rt->mComponentCostType.insert(rt->mComponentCostType.end(), Component::ParseCompType(GetString(child2), *game));
			child2 = child1->FirstChild("CostFactor");
			rt->mComponentCostFactor.insert(rt->mComponentCostFactor.end(), GetDouble(child2, 1.0));
		} else if (stricmp(child1->Value(), "Permaform") == 0) {
			rt->mPermaformOdds = GetDouble(child1->FirstChild("Chance"), 0.01);
			rt->mPermaformPopAdj = GetLong(child1->FirstChild("PopAdjustment"), 200000);
		} else if (stricmp(child1->Value(), "CanSeeHabSettings") == 0) {
			rt->mCanSeeHabSettings = GetBool(child1);
		} else if (stricmp(child1->Value(), "TemporaryTerraform") == 0) {
			rt->mTemporaryTerraform = GetBool(child1);
		} else if (stricmp(child1->Value(), "ScanDesign") == 0) {
			rt->mScanDesign = GetBool(child1);
		} else if (stricmp(child1->Value(), "RepairFactor") == 0) {
			rt->mRepairFactor = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "FreighterReproduction") == 0) {
			rt->mFreighterReproduction = GetDouble(child1, 0.0);
		} else if (stricmp(child1->Value(), "MineFieldScanning") == 0) {
			rt->mMineFieldScanning = GetBool(child1);
		} else if (stricmp(child1->Value(), "CanRemoteDetonate") == 0) {
			rt->mCanRemoteDetonate = GetBool(child1);
		} else if (stricmp(child1->Value(), "MineDecayFactor") == 0) {
			rt->mMineDecayFactor = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "MoveAndLayMines") == 0) {
			rt->mMoveAndLayMines = GetBool(child1);
		} else if (stricmp(child1->Value(), "PacketTerraform") == 0) {
			rt->mPacketTerraformOdds = GetDouble(child1->FirstChild("TerraformChance"), .50);
			rt->mPacketTerraformMass = GetLong(child1->FirstChild("TerraformMass"), 70);
			rt->mPacketPermaformOdds = GetDouble(child1->FirstChild("PermaformChance"), .001);
			rt->mPacketPermaformMass = GetLong(child1->FirstChild("PermaformMass"), 70);
		} else if (stricmp(child1->Value(), "PacketScanning") == 0) {
			rt->mPacketScanning = GetBool(child1);
		} else if (stricmp(child1->Value(), "PacketCostMinFactor") == 0) {
			rt->mPacketCostMinFactor = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "PacketSizeOneMin") == 0) {
			rt->mPacketSizeOneMin = GetLong(child1, 100);
		} else if (stricmp(child1->Value(), "PacketSizeMixed") == 0) {
			rt->mPacketSizeMixed = GetLong(child1, 40);
		} else if (stricmp(child1->Value(), "PacketCostResources") == 0) {
			rt->mPacketCostResources = GetLong(child1, 10);
		} else if (stricmp(child1->Value(), "PacketDecayFactor") == 0) {
			rt->mPacketDecayFactor = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "PacketCatchFactor") == 0) {
			rt->mPacketCatchFactor = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "PacketDecayPenalty") == 0) {
			rt->mPacketDecayPenalty = GetLong(child1);

		} else if (stricmp(child1->Value(), "StartingTech") == 0) {
			Rules::ParseArray(child1, rt->mStartingTech, TECHS, *game);
		} else if (stricmp(child1->Value(), "StartAtBonus") == 0) {
			rt->mStartAtBonus = GetLong(child1, 3);
		} else if (stricmp(child1->Value(), "SecondPlanet") == 0) {
			rt->mSecondPlanet = GetBool(child1);
		} else if (stricmp(child1->Value(), "GateCargo") == 0) {
			rt->mGateCargo = GetBool(child1);
		} else if (stricmp(child1->Value(), "OvergateLossFactor") == 0) {
			rt->mOvergateLossFactor = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "GateScanning") == 0) {
			rt->mGateScanning = GetBool(child1);
		} else if (stricmp(child1->Value(), "ARTechType") == 0) {
			rt->mARTechType = Rules::TechID(GetString(child1));
			if (rt->mARTechType < 0) {
				rt->mARTechType = 0;
				game->AddMessage("Invalid AR Tech Type");
			}
		} else if (stricmp(child1->Value(), "ARDivisor") == 0) {
			rt->mARDivisor = GetLong(child1, 10);
		} else if (stricmp(child1->Value(), "FuelFactor") == 0) {
			rt->mFuelFactor = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "MainResearch") == 0) {
			rt->mMainResearch = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "OtherResearch") == 0) {
			rt->mOtherResearch = GetDouble(child1, 0.0);
		} else if (stricmp(child1->Value(), "UltimateRecycle") == 0) {
			rt->mUltimateRecycle = GetBool(child1);
		} else if (stricmp(child1->Value(), "EngineFailureSpeed") == 0) {
			rt->mEngineFailureSpeed = GetLong(child1, 0);
		} else if (stricmp(child1->Value(), "EngineFailureRate") == 0) {
			rt->mEngineFailureRate = GetDouble(child1, 0.0);
		} else if (stricmp(child1->Value(), "SpaceScanFactor") == 0) {
			rt->mSpaceScanFactor = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "PenScanFactor") == 0) {
			rt->mPenScanFactor = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "StartingPopFactor") == 0) {
			rt->mStartingPopFactor = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "MaxMiniturize") == 0) {
			rt->mMaxMiniturize = GetDouble(child1, 0.75);
		} else if (stricmp(child1->Value(), "MiniturizeRate") == 0) {
			rt->mMiniturizeRate = GetDouble(child1, 0.04);
		} else if (stricmp(child1->Value(), "ZeroTechCost") == 0) {
			rt->mZeroTechCost = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "ShieldFactor") == 0) {
			rt->mShieldFactor = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "ArmorFactor") == 0) {
			rt->mArmorFactor = GetDouble(child1, 1.0);
		} else if (stricmp(child1->Value(), "ShieldRegenRate") == 0) {
			rt->mShieldRegenRate = GetDouble(child1, 0.0);
		} else if (stricmp(child1->Value(), "TerraformFactor0") == 0) {
			rt->mTerraformFactor0 = GetLong(child1, 0);
		} else if (stricmp(child1->Value(), "TerraformFactor1") == 0) {
			rt->mTerraformFactor1 = GetLong(child1, 5);
		} else if (stricmp(child1->Value(), "TerraformFactor2") == 0) {
			rt->mTerraformFactor2 = GetLong(child1, 15);
		} else if (stricmp(child1->Value(), "FactoryProductionCost") == 0) {
			rt->mFactoryProductionCost = GetLong(child1, 2);
		} else if (stricmp(child1->Value(), "BuiltInScan") == 0) {
			child2 = child1->FirstChild("HullType");
			rt->mBISHull = Component::ParseHullType(GetString(child2), *game);
			child2 = child1->FirstChild("PenScanHullType");
			rt->mBISHullPen = Component::ParseHullType(GetString(child2), *game);
			child2 = child1->FirstChild("TechType");
			rt->mBISTech = GetLong(child2, -1);
			child2 = child1->FirstChild("Multiplier");
			rt->mBISMultiplier = GetLong(child2, 20);
			child2 = child1->FirstChild("CheckPenFactor");
			rt->mBISCheckPenFactor = GetBool(child2);
			child2 = child1->FirstChild("PopFactor");
			rt->mBISPopFactor = GetDouble(child2, 0.0);
		} else if (stricmp(child1->Value(), "DefenseFactor") == 0) {
			rt->mDefenseFactor = GetDouble(child1, 1.0);
		} else {
			Message * mess = game->AddMessage("Warning: Unknown section");
			mess->AddItem("Racial trait", rt->mName);
			mess->AddItem("Section", child1->Value());
			continue;
		}
	}

	return rt;
}

}
