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

#include <stdlib.h>
#include <cmath>
#include "Hull.h"
#include "RacialTrait.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {
deque<string> Component::Subtypes;

long Component::MaxID = 1;
Cost * Component::mDefenseCost = NULL;
Cost * Component::mScannerCost = NULL;

Component::Component()
//:	mCost()
{
	init();
}

void Component::init()
{
	ID = MaxID++;
	Type = 0;		// Base type, can also have other capabilities
	Mass = 0;
	MTGift = false;
	Tech.insert(Tech.begin(), Rules::MaxTechType, -1);
	mHullType = HC_ALL;

	// specific stuff, most will be defaults
	// armor and shield
	Armor = 0;		// armor DP
	Shield = 0;		// shield DP

	// weapons
	WeaponType = -1;	// type of weapon
	Power = 0;		// damage power
	Range = 0;		// weapon range
	Initiative = 0;	// weapon initiative
	Accuracy = 0.0;	// base accuracy

	// bombs
	BombType = 0;
	KillPercentage = 0.0;	// Bomb kill percentage
	KillMin = 0;	// Bomb min killed
	KillInstallation = 0;	// Bomb damage to instalations

	// electrical
	Cloaking = 0;		// cloaking units
	ComputerPower = 1.0;	// affect on inaccuracy of your missiles 1 for no affect, .8 is +20%
	Jamming = 1.0;		// affect on accuracy of incoming missiles 1 for no affect, .8 is 20% jamming
	InitiativeAdjustment = 0;	// adjustment to initiative, and hull base inititive
	Capacitors = 1.0;	// beam damage multiplier - 1.1 and 1.2 for capacitors
	Deflection = 1.0;	// beam damage multiplier - .9 for deflectors
	Tachyon = 0.0;

	// mechanical
	Colonize = false;	// true if it allows colonization
	CargoCapacity = 0;	// cargo capacity
	FuelCapacity = 0;	// Fuel capacity
	FuelGeneration = 0;	// Fuel generation ability
	RepairRate = 0.0;	// Repair Rate
	SpeedBonus = 0.0;	// bonus (.25 for Maneuver Jet)
	Dampener = 0.0;	// Dampener effect (slows all ships in combat if this item is present)

	// engines
	Radiation = -1;	// hab type it radiates in, killing pop traveling with it (rad-ram)
	SafeSpeed = 9;	// normally 9, some engines allow warp 10
	MaxSpeed = 10;	// For expantion
	BattleSpeed	= 0;	// battle speed of the engine, normaly dependant on FuelUsage
	FreeSpeed = 1;

	// Mine layers
	MineType = 0;	// 1 standards, 2 Heavies, 3 speed traps
	MineAmount = 0;	// number laid per year

	// remote mining
	Mines = 0;		// Number of remote mines
	RemoteTerraform = 0;	// Remote terraform: 0x01 - deterra only, 0x02 improve only, 0x03 both deterra: 0x10 to initial, 0x20 terra tech (OAs are therefore 0x23, bombs are 0x11)
	TerraPower = 0;	// power of the remote terraformer

	// Orbitals
	GateMass = 0;	// -1 for infinite
	GateRange = 0;	// -1 for infinite
	DriverSpeed = 0;	// mass driver speed
	JumpGate = false;

	// Scanners
	ScanSpace = 0;	// deep space (red) scanning coverage
	ScanPenetrating = -1;	// orbital (yellow) scanning coverage - every scanner can see a world it is in orbit of but non scanners cannot see details
	StealShip = false;		// can steal cargo from ships
	StealPlanet = false;	// can steal cargo from planets

	// Terraforming
	TerraType = 0;	// -1 for any
	TerraLimit = 0;	// max amount of terraforming at this tech
	DefensePower = 0.0;	// Defensive power at this tech

	// Station attributes
	Refuel = false;	// can this station refuel
	DockBuildCapacity = 0;	// max ship size built here -1 for infinite
	ARMaxPop = 0;	// Max pop for AR races, in cargo units (100 pop a unit)

	// Specials
	PlanetReset = false;	// building this resets the planet
}

Component::~Component()
{
	FuelUsage.clear();
	PRTNeeded.clear();
	PRTDenied.clear();
	LRTNeeded.clear();
	LRTDenied.clear();
}

void Component::Cleanup()
{
	Subtypes.clear();
}

bool Component::LoadComponents(const TiXmlNode * comps, deque<Component *> &CompList, MessageSink &messageSink)
{
	const TiXmlNode * child1;
	const TiXmlNode * child2;
	const char * ptr;
	ArrayParser arrayParser(messageSink);

	Component * temp = NULL;
	for (child1 = comps->FirstChild("Component"); child1; child1 = child1->NextSibling("Component")) {
		temp = NULL;
		ptr = GetString(child1->FirstChild("Type"));
		if (ptr == NULL)
			return false;

		ComponentType ct = ParseCompType(ptr, messageSink);
		if (ct == CT_HULL) {
			temp = new FreeStars::Hull();
			temp->mHullType = HC_NONE;
		} else if (ct == CT_BASE) {
			temp = new Hull();
			temp->mHullType = HC_NONE;
		} else
			temp = new Component;

		temp->Type = ct;

		for (child2 = child1->FirstChild(); child2; child2 = child2->NextSibling()) {
			if (child2->Type() != TiXmlNode::ELEMENT)
				continue;

			if (stricmp(child2->Value(), "Type") == 0) {
				// already done, skip it here
			} else if (stricmp(child2->Value(), "Name") == 0) {
				ptr = GetString(child2);
				if (ptr != NULL)
					temp->Name = ptr;
			} else if (stricmp(child2->Value(), "HullType") == 0) {
				if (temp->mHullType == HC_ALL)
					temp->mHullType = HC_NONE;

				if ((temp->Type == CT_HULL || temp->Type == CT_BASE) && temp->mHullType != HC_NONE) {
					Message * mess = messageSink.AddMessage("Error: More then one HullType specified");
					mess->AddItem("Component", temp->Name);
					continue;
				}
				temp->mHullType |= ParseHullType(GetString(child2), messageSink);
			} else if (stricmp(child2->Value(), "Costs") == 0) {
				temp->mCost.ReadCosts(child2, messageSink);
			} else if (stricmp(child2->Value(), "TechnologyRequirements") == 0) {
				Rules::ParseArray(child2, temp->Tech, TECHS, messageSink);
			} else if (stricmp(child2->Value(), "Mass") == 0) {
				temp->Mass = GetLong(child2);
			} else if (stricmp(child2->Value(), "PRTNeeded") == 0) {
				const RacialTrait * rt;
				ptr = GetString(child2);
				rt = RacialTrait::ParsePRT(ptr);
				if (rt == NULL) {
					Message * mess = messageSink.AddMessage("Error: Unknown PRT");
					mess->AddItem("Component", temp->Name);
					mess->AddItem("PRT", ptr);
					return false;
				}

				temp->PRTNeeded.push_back(rt);
			} else if (stricmp(child2->Value(), "PRTDenied") == 0) {
				const RacialTrait * rt;
				ptr = GetString(child2);
				rt = RacialTrait::ParsePRT(ptr);
				if (rt == NULL) {
					Message * mess = messageSink.AddMessage("Error: Unknown PRT");
					mess->AddItem("Component", temp->Name);
					mess->AddItem("PRT", ptr);
					return false;
				}

				temp->PRTDenied.push_back(rt);
			} else if (stricmp(child2->Value(), "LRTNeeded") == 0) {
				const RacialTrait * rt;
				ptr = GetString(child2);
				rt = RacialTrait::ParseLRT(ptr);
				if (rt == NULL) {
					Message * mess = messageSink.AddMessage("Error: Unknown LRT");
					mess->AddItem("Component", temp->Name);
					mess->AddItem("LRT", ptr);
					return false;
				}

				temp->LRTNeeded.push_back(rt);
			} else if (stricmp(child2->Value(), "LRTDenied") == 0) {
				const RacialTrait * rt;
				ptr = GetString(child2);
				rt = RacialTrait::ParseLRT(ptr);
				if (rt == NULL) {
					Message * mess = messageSink.AddMessage("Error: Unknown LRT");
					mess->AddItem("Component", temp->Name);
					mess->AddItem("LRT", ptr);
					return false;
				}

				temp->LRTDenied.push_back(rt);
			} else if (stricmp(child2->Value(), "Armor") == 0) {
				temp->Armor = GetLong(child2);
			} else if (stricmp(child2->Value(), "Shield") == 0) {
				temp->Shield = GetLong(child2);
			} else if (stricmp(child2->Value(), "Power") == 0) {
				temp->Power = GetLong(child2);
			} else if (stricmp(child2->Value(), "Range") == 0) {
				temp->Range = GetLong(child2);
			} else if (stricmp(child2->Value(), "Initiative") == 0) {
				temp->Initiative = GetLong(child2);
			} else if (stricmp(child2->Value(), "Accuracy") == 0) {
				temp->Accuracy = GetDouble(child2);
			} else if (stricmp(child2->Value(), "KillPercentage") == 0) {
				temp->KillPercentage = GetDouble(child2);
			} else if (stricmp(child2->Value(), "KillMin") == 0) {
				temp->KillMin = GetLong(child2);
			} else if (stricmp(child2->Value(), "KillInstallation") == 0) {
				temp->KillInstallation = GetLong(child2);
			} else if (stricmp(child2->Value(), "Cloak") == 0) {
				temp->Cloaking = GetLong(child2);
			} else if (stricmp(child2->Value(), "InitiativeAdjustment") == 0) {
				temp->InitiativeAdjustment = GetLong(child2);
			} else if (stricmp(child2->Value(), "Tachyon") == 0) {
				temp->Tachyon = GetDouble(child2);
			} else if (stricmp(child2->Value(), "ComputerPower") == 0) {
				temp->ComputerPower = GetDouble(child2);
			} else if (stricmp(child2->Value(), "Jamming") == 0) {
				temp->Jamming = GetDouble(child2);
			} else if (stricmp(child2->Value(), "Capacitors") == 0) {
				temp->Capacitors = GetDouble(child2);
			} else if (stricmp(child2->Value(), "Deflection") == 0) {
				temp->Deflection = GetDouble(child2);
			} else if (stricmp(child2->Value(), "CargoCapacity") == 0) {
				temp->CargoCapacity = GetLong(child2);
			} else if (stricmp(child2->Value(), "CargoLeft") == 0 && temp->Type == CT_HULL) {
                dynamic_cast<Hull *>(temp)->SetCargoLeft(::round(GetDouble(child2) * 64));
			} else if (stricmp(child2->Value(), "CargoTop") == 0 && temp->Type == CT_HULL) {
                dynamic_cast<Hull *>(temp)->SetCargoTop(::round(GetDouble(child2) * 64));
			} else if (stricmp(child2->Value(), "CargoWidth") == 0 && temp->Type == CT_HULL) {
                dynamic_cast<Hull *>(temp)->SetCargoWidth(::round(GetDouble(child2) * 64));
			} else if (stricmp(child2->Value(), "CargoHeight") == 0 && temp->Type == CT_HULL) {
                dynamic_cast<Hull *>(temp)->SetCargoHeight(::round(GetDouble(child2) * 64));
			} else if (stricmp(child2->Value(), "FuelCapacity") == 0) {
				temp->FuelCapacity = GetLong(child2);
			} else if (stricmp(child2->Value(), "FuelGeneration") == 0) {
				temp->FuelGeneration = GetLong(child2);
			} else if (stricmp(child2->Value(), "Colonize") == 0) {
				temp->Colonize = GetBool(child2);
			} else if (stricmp(child2->Value(), "RepairRate") == 0) {
				temp->RepairRate = GetDouble(child2);
			} else if (stricmp(child2->Value(), "SpeedBonus") == 0) {
				temp->SpeedBonus = GetDouble(child2);
			} else if (stricmp(child2->Value(), "Radiation") == 0) {
				temp->Radiation = GetLong(child2, -1);
			} else if (stricmp(child2->Value(), "SafeSpeed") == 0) {
				temp->SafeSpeed = GetLong(child2);
			} else if (stricmp(child2->Value(), "MaxSpeed") == 0) {
				temp->MaxSpeed = GetLong(child2);
				if (temp->MaxSpeed > Rules::GetConstant("MaxSpeed")) {
					temp->MaxSpeed = Rules::GetConstant("MaxSpeed");
					Message * mess = messageSink.AddMessage("Error: Max speed too high");
					mess->AddItem("Component", temp->Name);
					return false;
				}
			} else if (stricmp(child2->Value(), "BattleSpeed") == 0) {
				temp->BattleSpeed = GetLong(child2);
			} else if (stricmp(child2->Value(), "FuelUsage") == 0) {
				if (temp->FuelUsage.size() > 0) {
					Message * mess = messageSink.AddMessage("Warning: Duplicate FuelUsage");
					mess->AddItem("Component", temp->Name);
					return false;
				}
				temp->FuelUsage.insert(temp->FuelUsage.begin(), Rules::GetConstant("MaxSpeed"), 0);
				arrayParser.ParseArrayFloat(child2, "Warp", "Speed", temp->FuelUsage, NULL);
			} else if (stricmp(child2->Value(), "MineAmount") == 0) {
				temp->MineAmount = GetLong(child2);
			} else if (stricmp(child2->Value(), "Mines") == 0) {
				temp->Mines = GetLong(child2);
			} else if (stricmp(child2->Value(), "GateMass") == 0) {
				temp->GateMass = GetLong(child2);
			} else if (stricmp(child2->Value(), "GateRange") == 0) {
				temp->GateRange = GetLong(child2);
			} else if (stricmp(child2->Value(), "DriverSpeed") == 0) {
				temp->DriverSpeed = GetLong(child2);
			} else if (stricmp(child2->Value(), "JumpGate") == 0) {
				temp->JumpGate = GetBool(child2);
			} else if (stricmp(child2->Value(), "ScanSpace") == 0) {
				temp->ScanSpace = GetLong(child2);
			} else if (stricmp(child2->Value(), "ScanPenetrating") == 0) {
				temp->ScanPenetrating = GetLong(child2);
			} else if (stricmp(child2->Value(), "WeaponType") == 0) {
				ptr = GetString(child2);
				if (ptr == NULL)
					return false;

				if (stricmp(ptr, "Beam") == 0)
					temp->WeaponType = WT_BEAM;
				else if (stricmp(ptr, "Gatling") == 0)
					temp->WeaponType = WT_GATLING;
				else if (stricmp(ptr, "Sapper") == 0)
					temp->WeaponType = WT_SAPPER;
				else if (stricmp(ptr, "Torpedo") == 0)
					temp->WeaponType = WT_TORP;
				else if (stricmp(ptr, "Missile") == 0)
					temp->WeaponType = WT_MISSILE;
				else {
					Message * mess = messageSink.AddMessage("Warning: Unkown weapon type");
					mess->AddItem("Component", temp->Name);
					mess->AddItem("Type", ptr);
				}
			} else if (stricmp(child2->Value(), "BombType") == 0) {
				ptr = GetString(child2);
				if (ptr == NULL)
					return false;

				if (stricmp(ptr, "Standard") == 0)
					temp->BombType = BT_NORMAL;
				else if (stricmp(ptr, "Smart") == 0)
					temp->BombType = BT_SMART;
				else if (stricmp(ptr, "Terra") == 0)
					temp->BombType = BT_TERRA;
				else {
					Message * mess = messageSink.AddMessage("Warning: Unknown bomb type");
					mess->AddItem("Component", temp->Name);
					mess->AddItem("Type", ptr);
				}
			} else if (stricmp(child2->Value(), "MineType") == 0) {
				ptr = GetString(child2);
				if (ptr == NULL)
					return false;

				temp->MineType = Rules::MineID(ptr);
				if (temp->MineType < 0) {
					Message * mess = messageSink.AddMessage("Warning: Unknown mine type");
					mess->AddItem("Component", temp->Name);
					mess->AddItem("Type", ptr);
				}
			} else if (stricmp(child2->Value(), "RemoteTerraform") == 0) {
				ptr = GetString(child2);
				if (ptr == NULL)
					return false;

				if (stricmp(ptr, "DeTerraform") == 0)
					temp->RemoteTerraform |= TERRA_DETERRA;
				else if (stricmp(ptr, "Terraforming") == 0)
					temp->RemoteTerraform |= TERRA_POSTERRA;
				else if (stricmp(ptr, "To Initial Value") == 0)
					temp->RemoteTerraform |= TERRA_TOINIT;
				else if (stricmp(ptr, "To Tech Limit") == 0)
					temp->RemoteTerraform |= TERRA_TOTECH;
				else if (stricmp(ptr, "Both") == 0)
					temp->RemoteTerraform |= TERRA_TERRABOTH;
				else if (stricmp(ptr, "Bomb") == 0)
					temp->RemoteTerraform |= TERRA_BOMB;
				else if (stricmp(ptr, "Terraform phase") == 0)
					temp->RemoteTerraform |= TERRA_REMOTE;
				else {
					Message * mess = messageSink.AddMessage("Warning: Unknown terraformer type");
					mess->AddItem("Component", temp->Name);
					mess->AddItem("Type", ptr);
				}
			} else if (stricmp(child2->Value(), "TerraPower") == 0) {
				temp->TerraPower = GetLong(child2);
			} else if (stricmp(child2->Value(), "Thieving") == 0) {
				ptr = GetString(child2);
				if (ptr == NULL)
					return false;

				if (stricmp(ptr, "Ship") == 0)
					temp->StealShip = true;
				else if (stricmp(ptr, "Planet") == 0)
					temp->StealPlanet = true;
				else if (stricmp(ptr, "Both") == 0) {
					temp->StealShip = true;
					temp->StealPlanet = true;
				} else {
					Message * mess = messageSink.AddMessage("Warning: Unknown thieving type");
					mess->AddItem("Component", temp->Name);
					mess->AddItem("Type", ptr);
				}
			} else if (stricmp(child2->Value(), "TerraType") == 0) {
				ptr = GetString(child2);
				if (ptr == NULL)
					return false;

				if (stricmp(ptr, "Any") == 0 || stricmp(ptr, "All") == 0 || stricmp(ptr, "Total") == 0)
					temp->TerraType = -1;
				else
					temp->TerraType = Rules::HabID(ptr);
			} else if (stricmp(child2->Value(), "TerraLimit") == 0) {
				temp->TerraLimit = GetLong(child2);
			} else if (stricmp(child2->Value(), "DefensePower") == 0) {
				temp->DefensePower = GetDouble(child2);
			} else if (stricmp(child2->Value(), "FreeSpeed") == 0) {
				temp->FreeSpeed = GetLong(child2);
			} else if (stricmp(child2->Value(), "MTGift") == 0) {
				temp->MTGift = GetBool(child2);
			} else if (stricmp(child2->Value(), "PlanetReset") == 0) {
				temp->PlanetReset = GetBool(child2);
			} else if (stricmp(child2->Value(), "Dampener") == 0) {
				temp->Dampener = GetDouble(child2, 0.0);
			} else if (stricmp(child2->Value(), "Refuel") == 0) {
				temp->Refuel = GetBool(child2);
			} else if (stricmp(child2->Value(), "DockBuildCapacity") == 0) {
				temp->DockBuildCapacity = GetLong(child2);
			} else if (stricmp(child2->Value(), "ARMaxPop") == 0) {
				temp->ARMaxPop = GetLong(child2);
			} else if (stricmp(child2->Value(), "EngineType") == 0) {
				// For expansion
			} else if (stricmp(child2->Value(), "Slot") == 0) {
				if (!dynamic_cast<Hull *>(temp) || !dynamic_cast<Hull *>(temp)->LoadSlot(child2, messageSink)) {
					Message * mess = messageSink.AddMessage("Error: Slot problem on hull");
					mess->AddItem("Component", temp->Name);
					return false;
				}
			} else if (stricmp(child2->Value(), "Value") == 0) {
				temp->SubTypeIndex.push_back(Component::ParseSubType(GetString(child2->FirstChild("Type")), true));
				temp->SubTypeValue.push_back(GetLong(child2->FirstChild("Score")));
			} else if (stricmp(child2->Value(), "CargoLeft") == 0) {
			} else if (stricmp(child2->Value(), "CargoTop") == 0) {
			} else if (stricmp(child2->Value(), "CargoWidth") == 0) {
			} else if (stricmp(child2->Value(), "CargoHeight") == 0) {
			} else {
				Message * mess = messageSink.AddMessage("Warning: Unknown Section in Component");
				mess->AddItem("Component", temp->Name);
				mess->AddItem("Section", child2->Value());
			}
		}

		if (temp->Type == CT_HULL && ((temp->mHullType & HC_BASE) || temp->mHullType == HC_ALL || temp->mHullType == HC_UNARMED || temp->mHullType == HC_ARMED)) {
			Message * mess = messageSink.AddMessage( "Error: Invalid HullType");
			mess->AddItem("Component", temp->Name);
			continue;
			
		} else if (temp->Type == CT_BASE && !(temp->mHullType & HC_BASE)) {
			Message * mess = messageSink.AddMessage( "Error: Invalid HullType");
			mess->AddItem("Component", temp->Name);
			continue;
		} else if (temp->Type == CT_DEFENSE) {
			mDefenseCost = &temp->mCost;
		} else if (temp->Type == CT_PLANSCAN) {
			mScannerCost = &temp->mCost;
		}

		CompList.push_back(temp);
	}

	return true;
}

long Component::ParseSubType(const char * ptr, bool insert)
{
	deque<string>::iterator iter;
	iter = find(Subtypes.begin(), Subtypes.end(), ptr);
	if (iter == Subtypes.end()) {
		if (insert)
			iter = Subtypes.insert(iter, ptr);
		else
			return -1;
	}

	return iter - Subtypes.begin();
}

ComponentType Component::ParseCompType(const char * ptr, MessageSink &messageSink)
{
	if (ptr == NULL || *ptr == '\0') {
		messageSink.AddMessage("Error: Missing Component type");
		return CT_NONE;
	}

	if (stricmp(ptr, "Hull") == 0)
		return CT_HULL;
	else if (stricmp(ptr, "Base") == 0)
		return CT_BASE;
	else if (stricmp(ptr, "Armor") == 0)
		return CT_ARMOR;
	else if (stricmp(ptr, "Shield") == 0)
		return CT_SHIELD;
	else if (stricmp(ptr, "Weapon") == 0)
		return CT_WEAPON;
	else if (stricmp(ptr, "Bomb") == 0)
		return CT_BOMB;
	else if (stricmp(ptr, "Electrical") == 0)
		return CT_ELEC;
	else if (stricmp(ptr, "Planetary Scanner") == 0)
		return CT_PLANSCAN;
	else if (stricmp(ptr, "Planetary") == 0)
		return CT_PLANET_SPECIAL;
	else if (stricmp(ptr, "Defense") == 0)
		return CT_DEFENSE;
	else if (stricmp(ptr, "Terraforming") == 0)
		return CT_TERRAFORM;
	else if (stricmp(ptr, "Engine") == 0)
		return CT_ENGINE;
	else if (stricmp(ptr, "Gate") == 0)
		return CT_GATE;
	else if (stricmp(ptr, "Driver") == 0)
		return CT_DRIVER;
	else if (stricmp(ptr, "Mine Layer") == 0)
		return CT_MINELAY;
	else if (stricmp(ptr, "Remote Miner") == 0)
		return CT_MINER;
	else if (stricmp(ptr, "Mechanical") == 0)
		return CT_MECH;
	else if (stricmp(ptr, "Scanner") == 0)
		return CT_SCANNER;
	else if (stricmp(ptr, "Alchemy") == 0)
		return CT_ALCHEMY;
	else {
		Message* mess = messageSink.AddMessage("Error: Invalid Component type");
		mess->AddItem("Type", ptr);
		return CT_NONE;
	}
}

HullType Component::ParseHullType(const char * ptr, MessageSink &messageSink)
{
	if (ptr == NULL || *ptr == '\0') {
		messageSink.AddMessage("Error: Missing HullType");
		return HC_NONE;
	}

	if (stricmp(ptr, "Scout") == 0)
		return HC_SCOUT;
	else if (stricmp(ptr, "Mine Layer") == 0)
		return HC_LAYER;
	else if (stricmp(ptr, "Mini Colony") == 0)
		return HC_MINICOL;
	else if (stricmp(ptr, "Warship") == 0)
		return HC_WARSHIP;
	else if (stricmp(ptr, "Bomber") == 0)
		return HC_BOMBER;
	else if (stricmp(ptr, "Remote Miner") == 0)
		return HC_MINER;
	else if (stricmp(ptr, "Colony") == 0)
		return HC_COLONY;
	else if (stricmp(ptr, "Freighter") == 0)
		return HC_FREIGHTER;
	else if (stricmp(ptr, "Utility") == 0)
		return HC_UTILITY;
	else if (stricmp(ptr, "Fuel Transport") == 0)
		return HC_FUEL;
	else if (stricmp(ptr, "Station") == 0)
		return HC_SMALLBASE;
	else if (stricmp(ptr, "SuperStation") == 0)
		return HC_BIGBASE;
	else if (stricmp(ptr, "Base") == 0)
		return HC_BASE;
	else if (stricmp(ptr, "Unarmed") == 0)
		return HC_UNARMED;
	else if (stricmp(ptr, "All") == 0)
		return HC_ALL;
	else {
		Message* mess = messageSink.AddMessage("Error: Invalid HullType");
		mess->AddItem("Type", ptr);
		return HC_NONE;
	}
}

long Component::GetSweeping(long RangeBonus) const
{
	if (WeaponType == WT_BEAM)
		return Power * (Range + RangeBonus) * (Range + RangeBonus);
	else if (WeaponType == WT_GATLING)
		return Power * (Range + RangeBonus + 2) * (Range + RangeBonus + 2);
	else
		return 0;
}

bool Component::IsBuildable(const Player * player) const	// can this player build this component (right LRT and tech levels)
{
	// tech first
	for (TechType tt = 0; tt < Rules::MaxTechType; ++tt) {
		if (Tech[tt] > player->GetTechLevel(tt))
			return false;
	}

	return CheckPRTLRT(player);
}

bool Component::WouldBeBuildable(const Player *player, long techType, long levels) const
{
	for (TechType tt = 0; tt < Rules::MaxTechType; ++tt) {
		if (Tech[tt] > (player->GetTechLevel(tt) + ((tt == techType) ? levels : 0)))
			return false;
	}

	return CheckPRTLRT(player);
}

bool Component::CheckPRTLRT(const Player *player) const
{
	if (find(PRTDenied.begin(), PRTDenied.end(), player->GetPRT()) != PRTDenied.end())
		return false;

	if (PRTNeeded.size() > 0 && find(PRTNeeded.begin(), PRTNeeded.end(), player->GetPRT()) == PRTNeeded.end())
		return false;

	if (LRTNeeded.size() > 0) {
		deque<const RacialTrait *>::const_iterator i;
		for (i = LRTNeeded.begin(); i != LRTNeeded.end(); ++i) {
			if (player->HasLRT(*i))
				break;
		}

		if (i == LRTNeeded.end())
			return false;
	}

	if (LRTDenied.size() > 0) {
		deque<const RacialTrait *>::const_iterator i;
		for (i = LRTDenied.begin(); i != LRTDenied.end(); ++i) {
			if (player->HasLRT(*i))
				return false;
		}
	}

	return true;
}

long Component::GetScore(const Component * check, long ValueType)
{
	for (unsigned long i = 0; i < check->SubTypeIndex.size(); ++i) {
		if (check->SubTypeIndex[i] == ValueType)
			return check->SubTypeValue[i];
	}

	return 0;
}

Cost Component::GetCost(const Player * owner) const
{
	Cost c;
	double reduc = owner->Miniturize(this);
	for (CargoType ct = RESOURCES; ct < Rules::MaxMinType; ++ct) {
		c[ct] = GetCost()[ct] - long(GetCost()[ct] * reduc + .5);
		if (c[ct] == 0 && GetCost()[ct] > 0)
			c[ct] = 1;
	}

	return c;
}

}
