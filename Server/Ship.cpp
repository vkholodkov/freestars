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

#include "Hull.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

Cost Ship::mUpCost;

Ship::Ship()
{
	ReCost = 0;
	ResetSeen();
	ResetDefaults();

	mCannotBuild = NULL;	// for starting ships
}

Ship::Ship(const Hull *hull)
	: mHull(hull)
{
	mName = hull->GetName();
	mGraphicNumber = 0;
	mGift = false;
	mBuilt = 0;
	ReCost = 0;
	ResetSeen();
	ResetDefaults();

	mCannotBuild = NULL;	// for starting ships

	unsigned int numSlots =  hull->GetNumberSlots();

	for(int pos = 0 ; pos != numSlots ; pos++) {
		mSlots.push_back(Slot(NULL, 0, pos, hull->GetSlot(pos)));
	} 
}

Ship::~Ship()
{
}

void Ship::Cleanup()
{
	mUpCost.Cleanup();
}

void Ship::ResetDefaults()
{
	CVFuelCapacity = -1;
	CVFuelGeneration = -1;
	CVMass = -1;
	CCalcTachyon = false;
	CVCargoCapacity = -1;
	CVArmor = -1;
	CVShield = -1;
	CVInitiativeAdjustment = -1;
	CCalcSpeedBonus = false;
	CVMines = -1;
	CVARMaxPop = -1;
	CVDockBuildCapacity = -1;

	CCalcComputerPower = false;
	CCalcJamming = false;
	CCalcCapacitors = false;
	CCalcDeflection = false;

	CCalcDampener = false;
	CCalcRepairRate = false;
	CVSafeSpeed = -1;
	CVMaxSpeed = -1;
	CVBattleSpeed = -1;
	CVFreeSpeed = -1;
	CVEngines = -1;

	CVColonize = -1;
	CVRefuel = -1;
	CVDriverSpeed = -1;
	CVCountDrivers = -1;

	CVCloaking = -1;
	CVSweeping = -1;
	CVScanSpace = -1;
	CVScanPenetrating = -2;
	CVStealShip = -1;
	CVStealPlanet = -1;

	CVRating = -1;
	CVShoot = -1;
	CVNormalBomb = CVSmartBomb = CVTerraBomb = -1;

	CCalcNormalKillPercentage = false;
	CVKillMin = CVKillInstallation = -1;

	CVMineAmount.erase(CVMineAmount.begin(), CVMineAmount.end());
	CVMineAmount.insert(CVMineAmount.begin(), MT_MAXIMUM, -1);

	CVFuelUsage.erase(CVFuelUsage.begin(), CVFuelUsage.end());
	CVFuelUsage.insert(CVFuelUsage.begin(), Rules::GetConstant("MaxSpeed"), -1);

	CalcedFuelUsage.erase(CalcedFuelUsage.begin(), CalcedFuelUsage.end());
	CalcedFuelUsage.insert(CalcedFuelUsage.begin(), Rules::GetConstant("MaxSpeed"), false);

	CVTechLevel.erase(CVTechLevel.begin(), CVTechLevel.end());
	CVTechLevel.insert(CVTechLevel.begin(), Rules::MaxTechType, -1);

	CVRadiation.erase(CVRadiation.begin(), CVRadiation.end());
	CVRadiation.insert(CVRadiation.begin(), Rules::MaxHabType, -1);
}

void Ship::ResetSeen()
{
	mSeenHull.clear();
	mSeenHull.insert(mSeenHull.begin(), TheGame->NumberPlayers(), false);
	mSeenDesign.clear();
	mSeenDesign.insert(mSeenDesign.begin(), TheGame->NumberPlayers(), false);
}

bool Ship::ParseNode(const TiXmlNode * node, Player * player, bool other)
{
	const TiXmlNode * child1;
	const char * ptr;

	const Component * comp;

	ptr = GetString(node->FirstChild("Name"));
	if (ptr != NULL)
		mName = ptr;

	Message * mess;
	mBuilt = GetLong(node->FirstChild("Built"));
	if (mBuilt < 0) {
		if (player == NULL)
			mess = TheGame->AddMessage("Error: Invalid data");
		else
			mess = player->AddMessage("Error: Invalid data");
		mess->AddLong("Number ships built", mBuilt);
		return false;
	}

	comp = TheGame->ParseComponent(GetString(node->FirstChild("Hull")));
	mHull = dynamic_cast<const Hull *>(comp);
	if (mHull == NULL) {
		if (player == NULL)
			mess = TheGame->AddMessage("Error: Invalid hull for ship design");
		else
			mess = player->AddMessage("Error: Invalid hull for ship design");
		mess->AddItem(mName.c_str(), GetString(node->FirstChild("Hull")));
		return false;
	}

	mGraphicNumber = GetLong(node->FirstChild("GraphicNumber"), -1);
	if (mGraphicNumber < 1) {
		if (player == NULL)
			mess = TheGame->AddMessage("Error: Invalid ship graphic for ship design");
		else
			mess = player->AddMessage("Error: Invalid ship graphic for ship design");
		mess->AddItem(mName.c_str(), GetString(node->FirstChild("GraphicNumber")));
		return false;
	}

	mGift = GetBool(node->FirstChild("Gift"));

	long amount;
	mSlots.erase(mSlots.begin(), mSlots.end());	// in case this is a existing design, shouldn't happen
	for (child1 = node->FirstChild("Slot"); child1; child1 = child1->NextSibling("Slot")) {
		amount = GetLong(child1->FirstChild("Number"));
		comp = TheGame->ParseComponent(GetString(child1->FirstChild("Component")));

		mSlots.push_back(Slot(comp, amount, mSlots.size(), mHull->GetSlot(mSlots.size())));
	}

	if (other)
		return true;
	else
		return IsValidDesign();
}

void Ship::WriteNode(TiXmlNode * node, bool Host, bool Owner, bool SeeDesign) const
{
	if (Host)
		Owner = SeeDesign = true;
	if (Owner)
		SeeDesign = true;

	AddString(node, "Hull", mHull->GetName().c_str());
	AddLong(node, "GraphicNumber", mGraphicNumber);
	if (Owner) {
		AddLong(node, "Built", mBuilt);
		AddString(node, "Gift", mGift ? "true" : "false");
	}

	if (SeeDesign) {
		AddString(node, "Name", mName.c_str());
		deque<Slot>::const_iterator si;
		for (si = mSlots.begin(); si != mSlots.end(); ++si) {
			if (si->GetCount() > 0) {
				TiXmlElement slot("Slot");
				AddLong(&slot, "Number", si->GetCount());
				AddString(&slot, "Component", si->GetComp()->GetName().c_str());
				node->InsertEndChild(slot);
			} else {
				AddString(node, "Slot", "");
			}
		}
	} else if (!(mHull->GetHullType() & HC_BASE))
		AddLong(node, "Mass", GetMass());
}

bool Ship::IsValidDesign(MessageSink &messageSink) const						// is this design valid (no weapons in the engine slot...)
{
	Message * mess;

	if (mHull->GetType() != CT_HULL && mHull->GetType() != CT_BASE) {
		mess = messageSink.AddMessage("Error: Invalid hull type for ship design");
		mess->AddLong(mName.c_str(), mHull->GetType());
		return false;
	}

	if (mSlots.size() > mHull->GetNumberSlots()) {
		mess = messageSink.AddMessage("Error: Invalid number of slots for ship design");
		mess->AddLong(mName.c_str(), mSlots.size());
		return false;
	}

	unsigned int i;
	bool ValidEngine = mHull->GetType() == CT_BASE;

	for (i = 0; i < mSlots.size(); ++i) {
		if (mSlots[i].GetCount() == 0)
			continue;

		// too many things
		if (mSlots[i].GetCount() > mHull->GetSlot(i).GetCount()) {
			mess = messageSink.AddMessage("Error: Too many things in slot for ship design");
			mess->AddItem("Design name", mName.c_str());
			mess->AddLong("Slot number", i);
			mess->AddLong("Number of elements", mSlots[i].GetCount());
			mess->AddLong("Maximum number of elements", mHull->GetSlot(i).GetCount());
			return false;
		}

		if (!mSlots[i].GetComp()) {
			mess = messageSink.AddMessage("Error: wrong design : no component type defined in slot");
			mess->AddItem("Design name", mName.c_str());
			mess->AddLong("Slot number", i);
			return false;
		}

		// is this type of component allowed in this slot?
		if (!mHull->GetSlot(i).IsAllowed(mSlots[i].GetComp()->GetType())) {
			mess = messageSink.AddMessage("Error: wrong design : illegal component in slot");
			mess->AddItem("Design name", mName.c_str());
			mess->AddLong("Slot number", i);
			mess->AddLong("Illegal component type", mSlots[i].GetComp()->GetType());
			return false;
		}

		if (!(mSlots[i].GetComp()->GetHullType() & mHull->GetHullType())) {
			mess = messageSink.AddMessage("Error: wrong design : component not allowed on this type of hull");
			mess->AddItem("Design name", mName.c_str());
			mess->AddLong("Slot number", i);
			mess->AddItem("Component name", mSlots[i].GetComp()->GetName());
			mess->AddItem("Hull name", mHull->GetName());
			return false;
		}

		if (!ValidEngine && mHull->GetSlot(i).IsAllowed(CT_ENGINE) && mSlots[i].GetCount() == mHull->GetSlot(i).GetCount())
			ValidEngine = true;
	}

	return ValidEngine;
}

bool Ship::IsValidDesign(const Player * player) const	// can this player build this ship (right PRT/LRT and tech levels)
{
//	assert(ValidDesign());
	if (!mHull->IsBuildable(player))
		return false;

	deque<Slot>::const_iterator i;
	for (i = mSlots.begin(); i != mSlots.end(); ++i) {
		if (i->GetComp() != NULL && !i->GetComp()->IsBuildable(player))
			return false;
	}

	return true;
}

// Get the gate to use when sending a fleet through this base
// Note that it could easily have determined the 'best' gate to use
// by picking the one that gives the smallest damage.
// However, that may help IT too much.
// This can be changed later if the balance is looked at.
const Component * Ship::GetGate() const
{
	for (unsigned int i = 0; i < mSlots.size(); ++i) {
		if (mSlots[i].GetComp()->GetGateMass() != 0)
			return mSlots[i].GetComp();
	}

	return NULL;
}

const Cost & Ship::GetCost(const Player * owner, const Ship * from /*=NULL*/, const Planet * planet /*=NULL*/) const
{
	bool upgrade = from != NULL;
	bool sameHull = false;
	if (planet == NULL || from == NULL)	upgrade = false;
	if (!(mHull->GetType() | CT_BASE))	upgrade = false;
	if (upgrade && mHull == from->mHull)
		sameHull = true;

	if (ReCost <= owner->GetLastTechGainPhase()) {
		const_cast<Ship *>(this)->ReCost = TheGame->GetTurnPhase();
		if (!sameHull) {
			const_cast<Ship *>(this)->CVCost = mHull->GetCost(owner);
		} else
			mUpCost.Zero();

		// loop through slots on the ship
		int number;
		for (unsigned int i = 0; i < mSlots.size(); ++i) {
			if (mSlots[i].GetComp() != NULL) {
				if (sameHull && mSlots[i].GetComp() == from->mSlots[i].GetComp())
					number = mSlots[i].GetCount() - from->mSlots[i].GetCount();
				else
					number = mSlots[i].GetCount();

				if (number > 0) {
					if (sameHull)
						mUpCost += mSlots[i].GetComp()->GetCost(owner) * number;
					else
						const_cast<Ship *>(this)->CVCost += mSlots[i].GetComp()->GetCost(owner) * number;
				} else if (number < 0) {
					Cost c;
					c = mSlots[i].GetComp()->GetCost(owner);
					ScrapRecover(c, -number, planet);
					mUpCost -= c;
				}

				if (sameHull && mSlots[i].GetComp() != from->mSlots[i].GetComp() && from->mSlots[i].GetComp() != NULL) {
					mUpCost += mSlots[i].GetComp()->GetCost(owner) * mSlots[i].GetCount();

					Cost c;
					c = from->mSlots[i].GetComp()->GetCost(owner);
					ScrapRecover(c, from->mSlots[i].GetCount(), planet);
					mUpCost -= c;
				}
			}
		}

		if (IsGift()) {
			for (CargoType ct = 0; ct < Rules::MaxMinType; ++ct)
				const_cast<Ship *>(this)->CVCost[ct] = long(CVCost[ct] * Rules::GetConstant("GiftPercent") + .5);
		}
	}

	if (upgrade && !sameHull) {
		mUpCost = CVCost;
		Cost fr = from->GetCost(owner);
		ScrapRecover(fr, 1, planet);
		mUpCost -= fr;
		return mUpCost;
	}

	return CVCost;
}

void Ship::ScrapRecover(Cost & c, int number, const Planet * planet) const
{
	if (number == 0) {
		c.Zero();
	} else {
		c.SetResources(long(c.GetResources() * number * (Rules::ScrapResource(planet) + 1.0) / 2.0));
		c.SetCrew(c.GetCrew() * number);	// 100% crew recovery
		for (CargoType ct = 0; ct < Rules::MaxMinType; ++ct)
			c[ct] = long(c[ct] * number * Rules::ScrapRecover(planet, false));
	}
}

void Ship::CopyDesign(const Ship * copy, bool IsGift)
{
	mName = copy->mName;
	mGraphicNumber = copy->mGraphicNumber;
	mGift = IsGift;
	mHull = copy->mHull;
	mBuilt = 0;
	ReCost = 0;

	mSlots.insert(mSlots.begin(), copy->mSlots.begin(), copy->mSlots.end());

	ResetDefaults();
}

void Ship::Upgrade(const Player * player)
{
	const Component * best;
	for (deque<Slot>::iterator i = mSlots.begin(); i != mSlots.end(); ++i) {
		if (i->GetComp() != NULL) {
			best = TheGame->GetBestComp(player, i->GetComp()->GetValueType(), (mHull->GetHullType() & HC_COL) != 0, mHull->GetHullType());
			if (best != NULL)
				i->SetComp(best);
		}
	}

	ResetDefaults();
}

bool operator==(const Ship & s1, const Ship & s2)
{
	if (s1.mHull != s2.mHull)
		return false;

	if (s1.mGift != s2.mGift)
		return false;

	if (s1.mSlots.size() != s2.mSlots.size())
		return false;

	for (int i = 0; i < s1.mSlots.size(); ++i) {
		if (s1.mSlots[i] != s2.mSlots[i])
			return false;
	}

	return true;
}


// scanning is a bit complex due to how scanners add to each other and because of builtin scanning
double lGetScanPenetrating(double v, const Slot & s)
{
	if (s.GetComp() != NULL)
		return Rules::CalcScanning(v, s.GetComp()->GetScanPenetrating(), s.GetCount());
	else
		return v;
}

long Ship::GetScanPenetrating(const Player * player, long pop /*= 0*/) const
{
	if (CVScanPenetrating == -2) {
		const_cast<Ship *>(this)->CVScanPenetrating = long(std::accumulate(	mSlots.begin(),
									mSlots.end(),
									Rules::CalcScanning(player->BuiltinScan(player, mHull->GetHullType(), true, pop),
														mHull->GetScanPenetrating(),
														1),
									lGetScanPenetrating));
	}
	return CVScanPenetrating;
}

double lGetScanSpace(double v, const Slot & s)
{
	if (s.GetComp() != NULL)
		return Rules::CalcScanning(v, s.GetComp()->GetScanSpace(), s.GetCount());
	else
		return v;
}

long Ship::GetScanSpace(const Player * player, long pop /*= 0*/) const
{
	if (CVScanSpace == -1) {
		const_cast<Ship *>(this)->CVScanSpace = long(std::accumulate(mSlots.begin(),
									mSlots.end(),
									Rules::CalcScanning(player->BuiltinScan(player, mHull->GetHullType(), false, pop),
														mHull->GetScanSpace(),
														1),
									lGetScanSpace));
	}
	return CVScanSpace;
}

long lGetTechLevel(long v, const Slot & s, TechType t)
{
	if (s.GetComp() != NULL) {
		if (t < Rules::MaxTechType)
			return max(v, s.GetComp()->GetTech(t));
		else	// MT Parts, add em up
			return v + (s.GetComp()->GetID() == t - Rules::MaxTechType) ? s.GetCount() : 0;
	} else
		return v;
}

long Ship::TechLevel(TechType tech) const
{
	if (tech >= Rules::MaxTechType || CVTechLevel[tech] == -1) {
		long Result = accumulate(	mSlots.begin(),
									mSlots.end(),
									(tech < Rules::MaxTechType) ? mHull->GetTech(tech) : ((mHull->GetID() == tech - Rules::MaxTechType) ? 1 : 0),
									lGetTechLevel,
									tech);
		if (tech < Rules::MaxTechType)
			const_cast<Ship *>(this)->CVTechLevel[tech] = Result;

		return Result;
	} else
		return CVTechLevel[tech];
}

long lGetSweeping (long v, const Slot & s, long rb)
{
	if (s.GetComp() != NULL)
		return v + s.GetComp()->GetSweeping(rb);
	else
		return v;
}

long Ship::GetSweeping() const
{
	if (CVSweeping == -1)
		const_cast<Ship *>(this)->CVSweeping = accumulate(mSlots.begin(),
										mSlots.end(),
										mHull->GetSweeping((mHull->GetHullType() & HC_BASE) ? 1 : 0),
										lGetSweeping,
										(mHull->GetHullType() & HC_BASE) ? 1 : 0);

	return CVSweeping;
}

// retro is 0x0111
// orbital adjuster is 0x0232
long lGetTerraPower(long v, const Slot & s, long type)
{
	if (s.GetComp() == NULL)
		return v;
	else {
		if (!(s.GetComp()->GetTerraType() & type & 0x000f))			// check limit to start or to tech
			return v;
		else if (!(s.GetComp()->GetTerraType() & type & 0x00f0))	// check terraform or deterraform capability
			return v;
		else if (!(s.GetComp()->GetTerraType() & type & 0x0f00))	// check if bomb or not
			return v;
		else
			return v + s.GetComp()->GetTerraPower() * s.GetCount();
	}
}

//	to ask for +terra: type is: 0x12 to initial (if deterraformed for some reason) 0x22 to tech of ship owner
//	to ask for -terra: type is: 0x11 to initial, 0x21 to tech of ship owner
long Ship::GetTerraPower(long type) const
{
	return accumulate(	mSlots.begin(),
						mSlots.end(),
						(mHull->GetTerraType() & type) ? mHull->GetTerraPower() : 0,
						lGetTerraPower,
						type);
}

long lGetMineAmount(long v, const Slot & s, long type)
{
	if (s.GetComp() != NULL)
		return v + (s.GetComp()->GetMineType() == type) ? s.GetComp()->GetMineAmount() * s.GetCount() : 0;
	else
		return v;
}

long Ship::GetMineAmount(long type) const
{
	if (CVMineAmount[type-1] == -1) {
		long Result = accumulate(mSlots.begin(),
								mSlots.end(),
								(mHull->GetMineType() == type) ? mHull->GetMineAmount() : 0,
								lGetMineAmount,
								type);

		if (mHull->GetHullType() & HC_LAYER)
			Result *= 2;

		const_cast<Ship *>(this)->CVMineAmount[type-1] = Result;
	}
	return CVMineAmount[type-1];
}

long lGetArmor(long v, const Slot & s, const Player * p)
{
	if (s.GetComp() != NULL) {
		if (s.GetComp()->GetType() == CT_ARMOR)
			v += long(s.GetComp()->GetArmor() * s.GetCount() * p->ArmorFactor());
		else
			v += s.GetComp()->GetArmor() * s.GetCount();
	}

	return v;
}

long Ship::GetArmor(const Player * p) const
{
	if (CVArmor == -1) {
		const_cast<Ship *>(this)->CVArmor =
					accumulate(	mSlots.begin(),
								mSlots.end(),
								mHull->GetArmor(),
								lGetArmor,
								p);
	}
	return CVArmor;
}

long lGetShield(long v, const Slot & s, const Player * p)
{
	if (s.GetComp() != NULL)
		v += long(s.GetComp()->GetShield() * s.GetCount() * p->ShieldFactor());

	return v;
}

long Ship::GetShield(const Player * p) const
{
	if (CVShield == -1) {
		const_cast<Ship *>(this)->CVShield =
					accumulate(	mSlots.begin(),
								mSlots.end(),
								mHull->GetShield(),
								lGetShield,
								p);
	}
	return CVShield;
}

double lGetFuelUsage(double v, const Slot & s, long speed)
{
	if (s.GetComp() != NULL && s.GetComp()->GetType() == CT_ENGINE) {
		double Result = s.GetComp()->GetFuelUsage(speed);
		if (Result < 0)
			Result *= s.GetCount();
		return v == 0 ? Result : min(v, Result);
	} else
		return v;
}

double Ship::GetFuelUsage(long speed) const
{
	if (!CalcedFuelUsage[speed-1]) {
		const_cast<Ship *>(this)->CalcedFuelUsage[speed-1] = true;
		const_cast<Ship *>(this)->CVFuelUsage[speed-1] =
				accumulate(	mSlots.begin(),
							mSlots.end(),
							0.0,
							lGetFuelUsage,
							speed);
	}

	return CVFuelUsage[speed-1];
}

long lGetEngines(long v, const Slot & s)
{
	if (s.GetComp() != NULL && s.GetComp()->GetType() == CT_ENGINE)
		return v + s.GetCount();
	else
		return v;
}

long Ship::GetEngines() const
{
	if (CVEngines == -1)
		const_cast<Ship *>(this)->CVEngines =
			std::accumulate(mSlots.begin(),
							mSlots.end(),
							0,
							lGetEngines);

	return CVEngines;
}

long lGetCloaking(long v, const Slot & s)
{
	if (s.GetComp() != NULL)
		return v + s.GetComp()->GetCloaking() * s.GetCount();
	else
		return v;
}

long Ship::GetCloaking() const
{
	if (CVCloaking == -1)
		const_cast<Ship *>(this)->CVCloaking = GetMass() *
			std::accumulate(mSlots.begin(),
						mSlots.end(),
						mHull->GetCloaking(),
						lGetCloaking);
	return CVCloaking;
}

long lCountDrivers(long v, const Slot & s, long speed)
{
	if (s.GetComp() != NULL && s.GetComp()->GetDriverSpeed() == speed)
		v += s.GetCount();

	return v;
}

long Ship::CountDrivers() const
{
	if (CVCountDrivers == -1) {
		const_cast<Ship *>(this)->CVCountDrivers =
				accumulate(	mSlots.begin(),
							mSlots.end(),
							(mHull->GetDriverSpeed() == GetDriverSpeed()) ? 1 : 0,
							lCountDrivers,
							GetDriverSpeed());
	}

	return CVCountDrivers;
}

bool Ship::DoesRadiate(HabType ht) const
{
	if (CVRadiation[ht] != -1)
		return CVRadiation[ht] ? true : false;
	const_cast<Ship *>(this)->CVRadiation[ht] = 1;
	if (mHull->GetRadiation() == ht)
		return true;
	for (deque<Slot>::const_iterator i = mSlots.begin(); i != mSlots.end(); ++i)
		if (i->GetComp() != NULL && i->GetComp()->GetRadiation() == ht)
			return true;
	const_cast<Ship *>(this)->CVRadiation[ht] = 0;
	return false;
}

long Ship::GetRating() const
{
	if (CVRating == -1) {
		const_cast<Ship *>(this)->CVShoot = 0;
		long Rating = 0;
		double value;
		for (deque<Slot>::const_iterator i = mSlots.begin(); i != mSlots.end(); ++i) {
			value = 0.0;
            if (i->GetComp() != NULL) {
                switch (i->GetComp()->GetWeaponType()) {
                case WT_BEAM:
                    value = (i->GetComp()->GetRange() + 3.0) / 4.0 * (GetNetSpeed() / 10.0 + 0.4);
                    value *= GetCapacitors();
                    break;
                case WT_SAPPER:
                    value = (i->GetComp()->GetRange() - 1.0) / 4.0 * (GetNetSpeed() / 10.0 + 0.4);
                    value *= GetCapacitors();
                    break;
                case WT_GATLING:
                    value = (i->GetComp()->GetRange() + 4.0) / 4.0 * (GetNetSpeed() / 10.0 + 0.4);
                    value *= GetCapacitors();
                    break;
                case WT_TORP:
                case WT_MISSILE:
                    value = (i->GetComp()->GetRange() - 2.0) / 2.0;
                    break;
                default:
                    break;
                }
            }

			if (value > 0.0) {
				const_cast<Ship *>(this)->CVShoot = 1;
				Rating += long(i->GetComp()->GetPower() * value);
			}
            if (i->GetComp() != NULL && i->GetComp()->GetBombType() != 0)
			{
				Rating += long(i->GetComp()->GetKillPercentage() * .2) + i->GetComp()->GetKillInstallation() * 2;
			}
		}
		const_cast<Ship *>(this)->CVRating = Rating;
	}

	return CVRating;
}

bool Ship::CanShoot() const
{
	if (CVShoot == -1)
		GetRating();

	return CVShoot == 1 ? true : false;
}

// returns net battle speed where 10 is 2.5 and 2 is .5. 0 is stationary
long Ship::GetNetSpeed() const
{
	if (mHull->GetHullType() | HC_BASE)
		return 0;

	long speed = GetBattleSpeed() - 2 + long(GetSpeedBonus() * 4);
	if (speed < 2)
		speed = 2;
	if (speed > 10)
		speed = 10;

	return speed;
}

long Ship::GetNetInitiative() const
{
	return mHull->GetInitiative() + GetInitiativeAdjustment();
}

bool Ship::IsBattleTarget(HullType hc) const
{
	switch (hc) {
	case HC_NONE:
		return false;
	case HC_ALL:
		return true;
	case HC_UNARMED:
		return !CanShoot();
	case HC_ARMED:
		return CanShoot();
	case HC_BASE:
		return (mHull->GetHullType() | HC_BASE) ? true : false;
	case HC_FUEL:
		return (mHull->GetHullType() | HC_FUEL) ? true : false;
	case HC_FREIGHTER:
		return (mHull->GetHullType() | HC_FREIGHTER) ? true : false;
	case HC_BOMBER:
		return (mHull->GetHullType() | HC_BOMBER | HC_FREIGHTER) ? true : false;	// targeting bombers also gets freighters
	default:
		return false;	// invalid battle order, target nothing
	}
}

long Ship::GetAttractiveCost(const Player * owner) const
{
	long Result = 0;
	Cost c = GetCost(owner);
	Result = c.GetResources();
	for (int i = 0; i < Rules::MaxMinType; ++i) {
		Result += Rules::GetArrayValue("AttractiveMineral", i) * c[i];
	}

	return Result;
}

double Ship::GetAccuracy(const Component & torp, const Ship * target) const
{
	if (torp.GetWeaponType() != WT_TORP && torp.GetWeaponType() != WT_MISSILE)
		return 0;

	double Result = torp.GetAccuracy();
	double temp = (1.0 - GetComputerPower()) - (1.0 - target->GetJamming());
	if (temp > 0.0)	// more comp power then jamming power
		Result += (1.0 - Result) * temp;
	else if (temp < 0.0)	// more jamming then comp power
		Result += Result * temp;

	return Result;
}


//Macros for the rest of the functions:
// define a local function that adds current total, and component capability * count
// make a member function that accumulates all slots using the local function, and initial value of the hull capability

// Get the sum of the hull and all components for a Function
#define GET_SHIP_SUM(Function)								\
	long lGet##Function(long v, const Slot & s)				\
	{														\
		if (s.GetComp() != NULL)							\
			return v + s.GetComp()->Get##Function() * s.GetCount();	\
		else												\
			return v;										\
	}														\
															\
	long Ship::Get##Function() const						\
	{														\
		if (CV##Function == -1)								\
			const_cast<Ship *>(this)->CV##Function = std::accumulate(mSlots.begin(), mSlots.end(), mHull->Get##Function(), lGet##Function);	\
		return CV##Function;								\
	}
#define GET_SHIP_SUMD(Function)								\
	double lGet##Function(double v, const Slot & s)			\
	{														\
		if (s.GetComp() != NULL)							\
			return v + s.GetComp()->Get##Function() * s.GetCount();	\
		else												\
			return v;										\
	}														\
															\
	double Ship::Get##Function() const						\
	{														\
		if (!CCalc##Function) {								\
			const_cast<Ship *>(this)->CCalc##Function = true;\
			const_cast<Ship *>(this)->CV##Function = std::accumulate(mSlots.begin(), mSlots.end(), mHull->Get##Function(), lGet##Function);	\
		}													\
		return CV##Function;								\
	}
// Get the product of all components multiplied by each other
#define GET_SHIP_PRODUCT(Function, Min)						\
	double lGet##Function(double v, const Slot & s)			\
	{														\
		if (s.GetComp() != NULL)							\
			return v * pow(double(s.GetComp()->Get##Function()), s.GetCount());	\
		else												\
			return v;										\
	}														\
															\
	double Ship::Get##Function() const						\
	{														\
		if (!CCalc##Function) {								\
			const_cast<Ship *>(this)->CCalc##Function = true;\
			double Result = std::accumulate(mSlots.begin(), mSlots.end(), mHull->Get##Function(), lGet##Function);	\
			if (Result < Min) Result = Min;					\
			const_cast<Ship *>(this)->CV##Function = Result;\
		}													\
		return CV##Function;								\
	}
// Get the max value of all on components on the ship
#define GET_SHIP_MAX(Function)								\
	long lGet##Function(long v, const Slot & s)				\
	{														\
		if (s.GetComp() != NULL)							\
			return max(v, s.GetComp()->Get##Function());	\
		else												\
			return v;										\
	}														\
															\
	long Ship::Get##Function() const						\
	{														\
		if (CV##Function == -1)								\
			const_cast<Ship *>(this)->CV##Function = std::accumulate(mSlots.begin(), mSlots.end(), mHull->Get##Function(), lGet##Function);	\
		return CV##Function;								\
	}
#define GET_SHIP_MAXD(Function)								\
	double lGet##Function(double v, const Slot & s)			\
	{														\
		if (s.GetComp() != NULL)							\
			return max(v, s.GetComp()->Get##Function());	\
		else												\
			return v;										\
	}														\
															\
	double Ship::Get##Function() const						\
	{														\
		if (!CCalc##Function) {								\
			const_cast<Ship *>(this)->CCalc##Function = true;\
			const_cast<Ship *>(this)->CV##Function = std::accumulate(mSlots.begin(), mSlots.end(), mHull->Get##Function(), lGet##Function);	\
		}													\
		return CV##Function;								\
	}
// Get true if the ship has the capability, false otherwise
#define GET_SHIP_HAS(Function)								\
	bool Ship::Can##Function() const						\
	{														\
		if (CV##Function != -1)								\
			return CV##Function ? true : false;				\
		const_cast<Ship *>(this)->CV##Function = 1;			\
		if (mHull->Can##Function())							\
			return true;									\
		for (deque<Slot>::const_iterator i = mSlots.begin(); i != mSlots.end(); ++i)	\
			if (i->GetComp() != NULL && i->GetComp()->Can##Function())	\
				return true;								\
		const_cast<Ship *>(this)->CV##Function = 0;			\
		return false;										\
	}

GET_SHIP_SUM(FuelCapacity)
GET_SHIP_SUM(FuelGeneration)
GET_SHIP_SUM(Mass)
GET_SHIP_SUMD(Tachyon)
GET_SHIP_SUM(CargoCapacity)
GET_SHIP_SUM(InitiativeAdjustment)
GET_SHIP_SUMD(SpeedBonus)
GET_SHIP_SUM(Mines)
GET_SHIP_SUM(ARMaxPop)
GET_SHIP_SUM(DockBuildCapacity)

GET_SHIP_PRODUCT(ComputerPower, 0.05)
GET_SHIP_PRODUCT(Jamming, (mHull->GetHullType() & HC_BASE) ? 0.25 : 0.05)
GET_SHIP_PRODUCT(Capacitors, 0.0)
GET_SHIP_PRODUCT(Deflection, 0.0)

GET_SHIP_MAXD(Dampener)
GET_SHIP_MAXD(RepairRate)
GET_SHIP_MAX(SafeSpeed)
GET_SHIP_MAX(MaxSpeed)
GET_SHIP_MAX(BattleSpeed)
GET_SHIP_MAX(FreeSpeed)
GET_SHIP_MAX(DriverSpeed)

GET_SHIP_HAS(Colonize)
GET_SHIP_HAS(Refuel)
GET_SHIP_HAS(StealShip)
GET_SHIP_HAS(StealPlanet)
GET_SHIP_HAS(SmartBomb)
GET_SHIP_HAS(TerraBomb)
GET_SHIP_HAS(NormalBomb)
GET_SHIP_HAS(JumpGate)

GET_SHIP_SUMD(NormalKillPercentage)
//GET_SHIP_SUM(SmartKillPercentage)
GET_SHIP_SUM(KillMin)
GET_SHIP_SUM(KillInstallation)

}
