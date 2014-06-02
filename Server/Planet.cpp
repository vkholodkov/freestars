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

#include "Creation.h"
#include "Order.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

Planet::Planet()
    : CargoHolder()
    , mProductionQ()
{
	Init();
}

void Planet::Init()
{
	mMinConc.insert(mMinConc.begin(), Rules::MaxMinType, 1);
	mMinMined.insert(mMinMined.begin(), Rules::MaxMinType, 0);
	mCanLoadBy.clear();
	mCanLoadBy.insert(mCanLoadBy.begin(), TheGame->NumberPlayers(), false);
	mHabTerra.insert(mHabTerra.begin(), Rules::MaxHabType, 50);
	mHabStart.insert(mHabStart.begin(), Rules::MaxHabType, 50);
	mHomeWorld = false;
	mScanner = false;
	mArtifactType = ARTI_NONE;
	mArtifactAmount = 0;
	mScrapRes = 0;
	mBaseDesign = -1;
	mFactories = 0;
	mMines = 0;
	mDefenses = 0;
	mPayTax = false;
	mBaseDamage = 0;
	mPacketSpeed = 0;
	mPacketDest = NULL;
	mRouteTo = NULL;
	mDisplayPop = 0;
	mName.erase();
	mHadBattle = false;
}

Planet::~Planet()
{
	deque<ProdOrder *>::iterator iter;
	for (iter = mProductionQ.begin(); iter != mProductionQ.end(); ++iter)
		delete *iter;
}

bool Planet::ParseNode(const TiXmlNode * node)
{
	if (!CargoHolder::ParseNode(node))
		return false;

	const TiXmlNode * child1;
	const char * ptr;
	int i;

	ptr = GetString(node->FirstChild("Name"));
	if (ptr != NULL)
		mName = ptr;
	if (mName.empty() && TheGame->GetCreation())
		mName = TheGame->GetCreation()->GetNextName();
	if (mName.empty()) {
		Message * mess = TheGame->AddMessage("Error: Invalid value on Planet");
		mess->AddItem("", "Missing Name");
		return false;
	}
	if (TheGame->GetCreation() && TheGalaxy->GetPlanet(mName.c_str()) != NULL) {
		Message * mess = TheGame->AddMessage("Error: Invalid value on Planet");
		mess->AddItem("Duplicate Name", mName);
		return false;
	}

	mHomeWorld = GetBool(node->FirstChild("HomeWorld"));

	bool bSecond = GetBool(node->FirstChild("SecondWorld"));
	if (bSecond && GetOwner() != NULL)
		TheGame->GetCreation()->AddSecond(NCGetOwner(), this);

	if (mHomeWorld && bSecond) {
		return false;
	}

	child1 = node->FirstChild("StartingHab");
	if (child1 != NULL)
		Rules::ParseArray(child1, mHabStart, HABS);
	else if (TheGame->GetCreation()) {
		for (i = 0; i < Rules::MaxHabType; ++i) {
			if (mHomeWorld && GetOwner() != NULL && GetOwner()->HabCenter(i) >= 0)	// leave immunes random
				mHabStart[i] = GetOwner()->HabCenter(i);
			else if (bSecond && GetOwner() != NULL)
				mHabStart[i] = Rules::GetSecondHab(i, GetOwner());
			else
				mHabStart[i] = Rules::RandomHab(i);
		}
	}

	Rules::ParseArrayBool(node->FirstChild("CanLoadBy"), "Race", "Number", mCanLoadBy);

	child1 = node->FirstChild("Concentrations");
	if (child1 != NULL)
		Rules::ParseArray(child1, mMinConc, MINERALS);
	else if (TheGame->GetCreation()) {
		if (mHomeWorld) {
			for (i = 0; i < Rules::MaxMinType; ++i) {
				mMinConc[i] = Rules::GetHWMC(i);
				mContains[i] = Rules::GetHWStartMinerals(i);
			}
		} else {
			for (i = 0; i < Rules::MaxHabType; ++i)
				mMinConc[i] = Random(Rules::MinMC(i), Rules::MaxMC(i));
		}
	}

	Rules::ParseArray(node->FirstChild("MineProgress"), mMinMined, MINERALS);

	child1 = node->FirstChild("Factories");
	mFactories = GetLong(child1);
	if (mFactories < 0) {
		Message * mess = TheGame->AddMessage("Error: Invalid value on Planet");
		mess->AddItem("", this);
		mess->AddLong("Factories", mFactories);
		return false;
	} else if (child1 == NULL && mHomeWorld && TheGame->GetCreation())
		mFactories = TheGame->GetCreation()->mHWFactories;
	else if (child1 == NULL && bSecond && TheGame->GetCreation())
		mFactories = TheGame->GetCreation()->mSWFactories;

	child1 = node->FirstChild("Mines");
	mMines = GetLong(child1);
	if (mMines < 0) {
		Message * mess = TheGame->AddMessage("Error: Invalid value on Planet");
		mess->AddItem("", this);
		mess->AddLong("Mines", mMines);
		return false;
	} else if (child1 == NULL && mHomeWorld && TheGame->GetCreation())
		mMines = TheGame->GetCreation()->mHWMines;
	else if (child1 == NULL && bSecond && TheGame->GetCreation())
		mMines = TheGame->GetCreation()->mSWMines;

	child1 = node->FirstChild("CurrentHab");
	if (child1 != NULL)
		Rules::ParseArray(child1, mHabTerra, HABS);
	else if (TheGame->GetCreation()) {
		for (i = 0; i < Rules::MaxMinType; ++i)
			mHabTerra[i] = mHabStart[i];
	}

	if (GetOwner() != NULL) {
		child1 = node->FirstChild("BaseDesign");
		if (child1) {
			mBaseDesign = GetLong(child1) - 1;
			if (mBaseDesign < -1 || mBaseDesign > Rules::GetConstant("MaxBaseDesigns")) {
				Message * mess = TheGame->AddMessage("Error: Invalid value on Planet");
				mess->AddItem("", this);
				mess->AddLong("BaseDesign", mBaseDesign);
				return false;
			}
		} else if ((mHomeWorld || bSecond) && TheGame->GetCreation()) {
			mBaseDesign = -2;	// special value to allow placement later
		} else
			mBaseDesign = -1;

		mBaseDamage = GetLong(node->FirstChild("BaseDamage"));
		if (	mBaseDamage < 0 ||
				(mBaseDesign == -1 && mBaseDamage != 0) ||
				(mBaseDamage > 0 && mBaseDamage > GetBaseDesign()->GetArmor(GetOwner()))
			)
		{
			Message * mess = TheGame->AddMessage("Error: Invalid value on Planet");
			mess->AddItem("", this);
			mess->AddLong("BaseDamage", mBaseDamage);
			return false;
		}

		child1 = node->FirstChild("Scanner");
		if (child1)
			mScanner = GetBool(child1);
		else if (TheGame->GetCreation() && (mHomeWorld || bSecond) && GetOwner())
			mScanner = GetOwner()->GetScanSpace() > 0;

		child1 = node->FirstChild("Defenses");
		if (child1)
			mDefenses = GetLong(node->FirstChild("Defenses"));
		else if (mHomeWorld && TheGame->GetCreation())
			mDefenses = TheGame->GetCreation()->mHWDefenses;

		if (mDefenses < 0 || mDefenses > Rules::GetConstant("MaxDefenses")) {
			Message * mess = TheGame->AddMessage("Error: Invalid value on Planet");
			mess->AddItem("", this);
			mess->AddLong("Defenses", mDefenses);
			return false;
		}

		Planet * pdest;
		child1 = node->FirstChild("PacketDestination");
		if (child1) {
			pdest = TheGalaxy->GetPlanet(GetString(child1));
			if (!pdest) {
				Message * mess = NCGetOwner()->AddMessage("Error: Invalid planet");
				mess->AddItem("", GetString(child1));
				mess->AddItem("Where", "driver destination to");
				return false;
			}
			SetPacketDest(pdest);
		}
		child1 = node->FirstChild("PacketDestination");
		if (child1) {
			pdest = TheGalaxy->GetPlanet(GetString(child1));
			if (!pdest) {
				Message * mess = NCGetOwner()->AddMessage("Error: Invalid planet");
				mess->AddItem("", GetString(child1));
				mess->AddItem("Where", "route destination to");
				return false;
			}
			SetRoute(pdest);
		}
		SetPacketSpeed(GetLong(node->FirstChild("PacketSpeed")));
		ParseProduction(node->FirstChild("ProductionQueue"));
		mPayTax = GetBool(node->FirstChild("PayTax"));
		mArtifactType = ARTI_NONE;
		mArtifactAmount = 0;
	} else {
		mBaseDesign = -1;
		mBaseDamage = 0;
		mScanner = false;
		mDefenses = 0;
		mRouteName = "";
		mPacketName = "";
		mPacketSpeed = 0;
		mPayTax = false;
		DeleteProdQ();
		mArtifactType = GetLong(node->FirstChild("ArtifactType"), ARTI_NONE);
		mArtifactAmount = GetLong(node->FirstChild("ArtifactAmount"));
	}

	// if we are creating a game, and it has an owner, add starting ships
	if (TheGame->GetCreation() && mHomeWorld) {
		if (GetOwner())
			NCGetOwner()->PlaceHW(this);
		else
			TheGame->GetCreation()->AddHW(this);
	}
	if (TheGame->GetCreation() && bSecond) {
		if (GetOwner())
			NCGetOwner()->PlaceSW(this, NULL);	// HW isn't used if owner is already set.
	}

	return true;
}

TiXmlNode * Planet::WriteNode(TiXmlNode * node, const Player * viewer) const
{
	if (viewer != NULL && !SeenBy(viewer))
		return NULL;

	AddString(node, "Name", mName.c_str());
	CargoHolder::WriteNode(node, viewer);

	if (viewer == NULL)
		node->LinkEndChild(Rules::WriteArrayBool("CanLoadBy", "Race", "Number", mCanLoadBy));

	if (SeenBy(viewer) & SEEN_PLANETMC) {
		AddString(node, "HomeWorld", mHomeWorld ? "true" : "false");
		node->LinkEndChild(Rules::WriteArray("Concentrations", mMinConc, MINERALS));
		if (viewer == NULL)
			node->LinkEndChild(Rules::WriteArray("MineProgress", mMinMined, MINERALS));
	}

	if (SeenBy(viewer) & SEEN_PLANETHAB) {
		node->LinkEndChild(Rules::WriteArray("StartingHab", mHabStart, HABS));
		node->LinkEndChild(Rules::WriteArray("CurrentHab", mHabTerra, HABS));
		if (viewer && GetOwner() && viewer != GetOwner() && viewer->CanSeeHabSettings())
			NCGetOwner()->SetSeenHab(viewer);
	}

	if (SeenBy(viewer) & SEEN_INSTALLATIONS) {
		AddLong(node, "Factories", mFactories);
		AddLong(node, "Mines", mMines);
	}

	if (GetOwner() != NULL) {
		if (SeenBy(viewer) & SEEN_INSTALLATIONS) {
			AddString(node, "Scanner", mScanner ? "true" : "false");
			AddLong(node, "Defenses", mDefenses);
		}

		if (SeenBy(viewer) & SEEN_HULL)
			AddLong(node, "BaseDesign", mBaseDesign+1);

		if (SeenBy(viewer) & SEEN_ORDERS) {
			AddLong(node, "BaseDamage", mBaseDamage);
			if (mRouteTo != NULL)
				AddString(node, "RouteDestination", mRouteTo->GetName().c_str());

			if (mPacketDest != NULL)
				AddString(node, "PacketDestination", mPacketDest->GetName().c_str());

			AddLong(node, "PacketSpeed", mPacketSpeed);
			ProdOrder::WriteNode(node, mProductionQ);
			AddBool(node, "PayTax", mPayTax);
		}

		if (viewer != NULL && viewer != GetOwner() && (SeenBy(viewer) & SEEN_PLANETPOP)) {
			AddDouble(node, "DefenseCoverage", GetDisplayDef());
			TiXmlNode * cargo = node->FirstChild("Contains");
			if (cargo == NULL) {
				cargo = new TiXmlElement("Contains");
				node->LinkEndChild(cargo);
			}

			TiXmlNode * pop = cargo->FirstChild("Population");
			if (pop != NULL)
				cargo->RemoveChild(pop);

			AddLong(cargo, "Population", GetDisplayPop());
		}
	}

	if (viewer == NULL) {
		AddLong(node, "ArtifactType", mArtifactType);
		AddLong(node, "ArtifactAmount", mArtifactAmount);
	}

	return node;
}

long Planet::GetDisplayPop() const
{
	if (mDisplayPop == 0 && mPopulation > 0) {
		// change this once so the same value gets returned for every call
		long lmin, lmax;
		lmin = long(mPopulation * 0.8 / Rules::PopEQ1kT);
		if (lmin < 4)
			lmin = 4;
		lmax = long(mPopulation * 1.2 / Rules::PopEQ1kT);
		if (lmax < lmin)
			lmax = lmin;

		const_cast<Planet *>(this)->mDisplayPop = Random(lmin, lmax) * Rules::PopEQ1kT;
	}

	return mDisplayPop;
}

void Planet::SetDestinations()
{
	mRouteTo = TheGalaxy->GetPlanet(mRouteName.c_str());
	mRouteName.erase();
	mPacketDest = TheGalaxy->GetPlanet(mPacketName.c_str());
	mPacketName.erase();
}

void Planet::Invade(Player * invader, long amount)
{
	deque<Invasion>::iterator iter;
	for (iter = mInvasions.begin(); iter != mInvasions.end(); ++iter) {
		if (invader == iter->player) {
			iter->amount += amount;
			return;
		}
	}

	mInvasions.push_back(Invasion(invader, amount));
	TheGalaxy->AddInvasion(this);
}

double Planet::GetDefenseValue() const
{
	if (GetOwner() == NULL)
		return 0L;

	if (GetDefenses() == 0)
		return 0L;

	return 1.0 - pow(double(1.0) - GetOwner()->GetDefenseValue(), int(GetDefenses()));
}

double Planet::GetSmartDefenseValue() const
{
	if (GetOwner() == NULL)
		return 0L;

	if (GetDefenses() == 0)
		return 0L;

	return 1.0 - pow(double(1.0) - GetOwner()->GetDefenseValue() / 2, int(GetDefenses()));
}

double Planet::GetDisplayDef() const
{
	double Result = 0;
	if (GetOwner() != NULL) {
		// int((x-1.5)/6)*6+9
		// int((x-150)/600)*600+900
		Result = GetDefenseValue();
		if (Result > 0) {
			Result = double(int((Result*100 - 1.5) / 6) * 6 + 9) / 100.0;
			Result = max(Result, 0.09);
			Result = min(Result, 0.93);
		}
	}

	return Result;
}

void Planet::ResolveInvasion()
{
	if (mInvasions.empty())
		return;

	long AttackStr = 0;
	Invasion * MaxAttack = NULL;
	Invasion * MaxAttack2 = NULL;
	long DefenseValue = 0;
	if (GetOwner()) {
		DefenseValue = long(GetPopulation() * GetOwner()->GroundDefenseFactor());
		DefenseValue = long(DefenseValue / (1.0 - GetDefenseValue() * 3.0 / 4.0));
	}

	Message * omess = NULL;
	if (GetOwner() != NULL) {
		omess = NCGetOwner()->AddMessage("World invasion", this);
		omess->AddLong("Starting population", GetPopulation());
	}

	deque<Invasion>::iterator iter;
	for (iter = mInvasions.begin(); iter != mInvasions.end(); ++iter) {
		long str = long(iter->amount * (1.0 + Rules::GetFloat("GCAttackerBonus")) * iter->player->GroundAttackFactor());
		iter->Strength = str;
		iter->InitStr = str;
		AttackStr += str;

		// find biggest and 2nd biggest attackers
		if (MaxAttack == NULL) {
			MaxAttack = &*iter;
		} else if (MaxAttack->Strength < str) {
			MaxAttack2 = MaxAttack;
			MaxAttack = &*iter;
		} else if (MaxAttack2 && MaxAttack2->Strength < str) {
			MaxAttack2 = &*iter;
		}

		iter->mess = iter->player->AddMessage("World invasion", this);
		iter->mess->AddLong("Starting population", GetPopulation());
		if (omess) {
			omess->AddItem("Invader", iter->player);
			omess->AddLong("Invasion amount", iter->amount);
		}
	}

	// add everyones troop count to everyones message
	deque<Invasion>::iterator i2;
	for (iter = mInvasions.begin(); iter != mInvasions.end(); ++iter) {
		for (i2 = mInvasions.begin(); i2 != mInvasions.end(); ++i2) {
			iter->mess->AddItem("Other invader", i2->player);
			iter->mess->AddLong("Invasion Amount", i2->amount);
		}
	}

	double ratio = double(DefenseValue) / double(AttackStr);
	if (ratio <= 1.00) {
		// attackers win
		// First take losses from fight with defenders
		MaxAttack->Strength = long(MaxAttack->Strength * (1.0-ratio));
		if (MaxAttack2) {
			MaxAttack2->Strength = long(MaxAttack2->Strength * (1.0-ratio));
			// Max now fights Max2
			MaxAttack->Strength -= MaxAttack2->Strength;
		}

		// calculate survivors
		MaxAttack->amount = long(MaxAttack->amount * double(MaxAttack->Strength) / double(MaxAttack->InitStr));

		// Send message to everyone
		for (iter = mInvasions.begin(); iter != mInvasions.end(); ++iter) {
			if (MaxAttack->amount > 100) {
				iter->mess->AddItem("Captured by", MaxAttack->player);
				iter->mess->AddLong("Survivors", MaxAttack->amount);
			} else
				iter->mess->AddItem("No survivors", (const Player *)NULL);
		}
		if (GetOwner() != NULL) {
			if (MaxAttack->amount > 100) {
				omess->AddItem("Captured by", MaxAttack->player);
				omess->AddLong("Survivors", MaxAttack->amount);
			} else
				omess->AddItem("No survivors", (const Player *)NULL);
		}
	} else {	// if (ration > 1.0)
		// defenders win
		// calculate survivors
		int survivors = long(GetPopulation() * (1.0 - double(AttackStr)/DefenseValue));
		mPopulation = survivors;
		// Send message to everyone
		for (iter = mInvasions.begin(); iter != mInvasions.end(); ++iter) {
			if (survivors > Rules::PopEQ1kT) {
				iter->mess->AddItem("Defender holds", GetOwner());
				iter->mess->AddLong("Survivors", survivors);
			} else
				iter->mess->AddItem("No survivors", (const Player *)NULL);
		}
		if (survivors > Rules::PopEQ1kT) {
			omess->AddItem("Defender holds", GetOwner());
			omess->AddLong("Survivors", survivors);
		} else if (omess)
			omess->AddItem("No survivors", (const Player *)NULL);
	}

	if (MaxAttack->amount > Rules::PopEQ1kT)
		TakePlanet(const_cast<Player *>(MaxAttack->player), MaxAttack->amount);
	else {
		TakePlanet(NULL, 0);
	}

	mInvasions.clear();	// Done, delete everything about invasions on this planet
}

void Planet::TakePlanet(Player * invader, long amount)
{
	if (invader != NULL) {
		// get tech
		if (GetOwner() == NULL) {	// colonized
			if (amount < 1000)	// small colonizations don't get full benifit from artifacts
				mArtifactAmount = mArtifactAmount * amount / 1000;

			TechType tt;
			int i;
			switch (mArtifactType) {
			case ARTI_NONE:
				break;
			case ARTI_ALL:
				for (tt = 0; tt < Rules::MaxTechType; ++tt)
					invader->GainTech(mArtifactAmount, tt);
				break;
			case ARTI_RANDTYPE:
				invader->GainTech(mArtifactAmount, Random(Rules::MaxTechType));
				break;
			case ARTI_RANDSPLIT:
				for (i = 0; i < Rules::MaxTechType * 2; ++i)
					invader->GainTech(mArtifactAmount / Rules::MaxTechType / 2, Random(Rules::MaxTechType));
			default:
				invader->GainTech(mArtifactAmount, mArtifactType);
				break;
			}

			mArtifactType = ARTI_NONE;
			mArtifactAmount = 0;
		} else {	// invaded
			if (GetContain(POPULATION) > 0) {
				TechType TechGot = Rules::TechInvasion(invader, GetOwner());
				if (TechGot > TECH_NONE) {
					NCGetOwner()->SetGotTech(true);
					NCGetOwner()->GainTech(TechGot, GetOwner()->TechCost(TechGot));
				}
			}
		}
	}

	Kill();
	mOwner = invader;
	mPopulation = amount;
	if (invader != NULL) {
		CopyProdQ(invader->GetDefaultQ());
		mPayTax = invader->GetDefaultPayTax();
	}
}

void Planet::DeadCheck()
{
	if (GetOwner() != NULL && GetContain(POPULATION) < Rules::PopEQ1kT) {
		Kill();
	}
}

void Planet::Kill()
{
	DeleteProdQ();
	mPopulation = 0;
	mScrapRes = 0;
	mBaseDesign = -1;
	mScanner = false;
	mDefenses = 0;
	mRouteTo = NULL;
	mPacketDest = NULL;
	mPacketSpeed = 0;
	mBaseDamage = 0;
	if (mOwner != NULL && mOwner->TemporaryTerraform())
		mHabTerra = mHabStart;

	mPayTax = false;
	mDisplayPop = 0;
	mOwner = NULL;
}

long Planet::GetScanPenetrating() const
{
	long Scan = 0;
	if (GetOwner()) {
		if (mScanner)
			Scan = GetOwner()->GetScanPenetrating();

		const Ship * base = GetBaseDesign();
		if (base)
			Scan = max(Scan, base->GetScanPenetrating(GetOwner(), GetContain(POPULATION)));
	}

	return Scan;
}

long Planet::GetScanSpace() const
{
	long Scan = 0;
	if (GetOwner()) {
		if (mScanner)
			Scan = GetOwner()->GetScanSpace();

		const Ship * base = GetBaseDesign();
		if (base)
			Scan = max(Scan, base->GetScanSpace(GetOwner(), GetContain(POPULATION)));

		Scan = long(Scan * GetOwner()->SpaceScanFactor());
	}

	return Scan;
}

double Planet::GetMaxTachyon() const
{
	if (GetOwner()) {
		const Ship * base = GetBaseDesign();
		if (base)
			return base->GetTachyon();
	}

	return 0.0;
}

void Planet::SetBaseNumber(long n)
{
	mBaseDesign = n;
	if (mBaseDesign >= 0) {
		mBaseDamage = min(mBaseDamage, GetBaseDesign()->GetArmor(GetOwner()) - 1L);
		NCGetOwner()->IncrementBaseBuilt(n);
	}
}

void Planet::DeleteProdQ()
{
	deque<ProdOrder *>::iterator iter;
	for (iter = mProductionQ.begin(); iter != mProductionQ.end(); ++iter)
		delete *iter;

	mProductionQ.erase(mProductionQ.begin(), mProductionQ.end());
}

void Planet::CopyProdQ(const deque<ProdOrder *> &prod)
{
	deque<ProdOrder *>::iterator iter;
	for (iter = mProductionQ.begin(); iter != mProductionQ.end(); ++iter)
		delete *iter;

	deque<ProdOrder *>::const_iterator it2;
	for (it2 = prod.begin(); it2 != prod.end(); ++it2) {
		mProductionQ.push_back((*it2)->Copy());
	}
}

void Planet::AdjustDefenses(long amount)
{
	mDefenses += amount;
	if (mDefenses < 0)
		mDefenses = 0;
	if (mDefenses > Rules::GetConstant("MaxDefenses"))
		mDefenses = Rules::GetConstant("MaxDefenses");
}

void Planet::AdjustMines(long amount)
{
	mMines += amount;
	if (mMines < 0)
		mMines = 0;
	}

void Planet::AdjustFactories(long amount)
{
	mDefenses += amount;
	if (mMines < 0)
		mMines = 0;

}

long Planet::CanTerraform(const Component * comp) const
{
	if (comp->GetTerraType() != -1)
		return CanTerraform(comp->GetTerraType(), comp->GetTerraLimit());
	else {	// comp->GetTerraType() == -1
		long maxT = 0;
		for (long i = 0; i < Rules::MaxHabType; ++i) {
			maxT = max(maxT, CanTerraform(i, comp->GetTerraLimit()));
		}

		return maxT;
	}
}

long Planet::CanTerraform(HabType ht, long max) const
{
	if (GetOwner()->HabCenter(ht) == -1)
		return 0;

	long direction = 0;
	if (mHabTerra[ht] < GetOwner()->HabCenter(ht))
		direction = 1;
	else if (mHabTerra[ht] > GetOwner()->HabCenter(ht))
		direction = -1;
	else
		return 0;

	// sanity check, should never happen
	if (direction > 0 && mHabTerra[ht] + direction > Rules::GetConstant("MaxHabValue"))
		return 0;
	else if (direction < 0 && mHabTerra[ht] + direction < Rules::GetConstant("MinHabValue"))
		return 0;

	// Check terra limits
	if (direction > 0 && mHabTerra[ht] + direction > mHabStart[ht] + max)
		return 0;
	else if (direction < 0 && mHabTerra[ht] + direction < mHabStart[ht] - max)
		return 0;

		// OK we know which direction to move it, and know that it's valid
	long before = GetOwner()->HabFactor(this);
	const_cast<Planet *>(this)->mHabTerra[ht] += direction;		// move it temporarily
	long after = GetOwner()->HabFactor(this);
	const_cast<Planet *>(this)->mHabTerra[ht] -= direction;		// move it back

	return after - before;
}

long Planet::GetMaxPop() const
{
	if (GetOwner() == NULL)
		return 0L;

	if (GetOwner()->ARTechType() >= 0) {
		if (GetBaseDesign() == NULL)
			return 0L;
		else
			return long(GetBaseDesign()->GetARMaxPop() * GetOwner()->PopulationFactor());
	} else {
		long hab = max(GetOwner()->HabFactor(this), 5L);
		return long(Rules::GetConstant("MaxPlanetPop") * hab / 100L * GetOwner()->PopulationFactor());
	}
}

long Planet::GetMiningVelocity(long mineral) const {
	if (GetOwner() == NULL)
		return 0;

	long mines;
	if (GetOwner()->ARTechType() >= 0) {
		mines = long(sqrt((double)min(GetMaxPop(), GetContain(POPULATION)) * GetOwner()->MinesRun()));
	} else {
		mines = min(GetMaxPop(), GetContain(POPULATION)) * GetOwner()->MinesRun() / 100;
		mines = min(mines, mMines);
	}

	return this->GetMiningVelocity(mines, GetOwner(), mineral);
}

long Planet::GetMiningVelocity(long mines, const Player *miner, long mineral) const {
	if (mines == 0)
		return 0;

	long rate;
	if (GetOwner() == NULL)
		rate = 10;
	else
		rate = GetOwner()->MineRate();

    long conc;
    if (mHomeWorld && miner == GetOwner())
        conc = max(mMinConc[mineral], Rules::GetConstant("MineralHWMinimum"));
    else
        conc = mMinConc[mineral];

    return (conc * mines * rate + 500) / 10 / 100;
}

void Planet::Mine()
{
	if (GetOwner() == NULL)
		return;

	long mines;
	if (GetOwner()->ARTechType() >= 0) {
		mines = long(sqrt((double)min(GetMaxPop(), GetContain(POPULATION)) * GetOwner()->MinesRun()));
	} else {
		mines = min(GetMaxPop(), GetContain(POPULATION)) * GetOwner()->MinesRun() / 100;
		mines = min(mines, mMines);
	}

	Mine(mines, GetOwner());
}


void Planet::Mine(long mines, const Player * miner)
{
	if (mines == 0)
		return;

	long rate;
	if (GetOwner() == NULL)
		rate = 10;
	else
		rate = GetOwner()->MineRate();

	for (long mineral = 0; mineral < Rules::MaxMinType; ++mineral) {
		long conc;
		if (mHomeWorld && miner == GetOwner())
			conc = max(mMinConc[mineral], Rules::GetConstant("MineralHWMinimum"));
		else
			conc = mMinConc[mineral];

		mContains[mineral] += (conc * mines * rate + 500) / 10 / 100;
		mMinMined[mineral] += mines;
		long minesper = Rules::GetConstant("MineralDecayFactor") / mMinConc[mineral] / mMinConc[mineral];
		if (mMinMined[mineral] > minesper) {
			mMinConc[mineral] -= mMinMined[mineral] / minesper;
			if (mMinConc[mineral] < Rules::GetConstant("MineralMinimum"))
				mMinConc[mineral] = Rules::GetConstant("MineralMinimum");

			mMinMined[mineral] %= minesper;
		}
	}
}

long Planet::GetResources() const
{
	if (GetOwner() == NULL)
		return 0;

	long Res = 0;
	long maxpop = GetMaxPop();
	long pop;	// pop up to max pop, then pop /2 up to 3x max, then 0
	pop = min(maxpop, GetContain(POPULATION));
	if (GetContain(POPULATION) > maxpop)
		pop = (min(maxpop * 3L, GetContain(POPULATION)) - maxpop) / 2 + maxpop;

	if (GetOwner()->ARTechType() >= 0) {
		long hab = max(GetOwner()->HabFactor(this), 25L);
		Res = long(sqrt(double(pop * GetOwner()->GetTechLevel(GetOwner()->ARTechType()) / GetOwner()->ARDivisor()) * hab / 100));
	} else {
		long facts;
		facts = min(maxpop, GetContain(POPULATION)) * GetOwner()->FactoriesRun() / 10000;
		facts = min(facts, mFactories);

		Res = facts * GetOwner()->FactoryRate() / 10;
		Res += pop / GetOwner()->PopEfficiency();
	}

	return Res;
}

long Planet::PopGrowth() const
{
	if (GetOwner() == NULL)
		return 0L;

	long currentHab = GetOwner()->HabFactor(this);
	if (currentHab < 0) {
		// 10000 pop on a -10% world will lose 100 pop, so 10000 * -10 = 100000, +500 and /1000 gives 100
		return (GetPopulation() * currentHab + 500) / 1000;
	} else {
		double PopFactor = double(GetPopulation()) / double(GetMaxPop());
		if (PopFactor > 1.0) {
			return long(GetPopulation() * PopFactor * -4 + 0.5);
		} else {
			// 10000 pop on a 100% world with 10% gr will grow by 1000
			double Growth = GetPopulation() * currentHab / 100.0 * GetOwner()->GrowthRate();
			if (PopFactor > 0.25) {
				// 250000 pop on a 100% world with 10% gr will grow by 25000:  2500 * 100 * 1000 / 10000 = 25000
				Growth *= 16.0 / 9.0;
				Growth *= (1.0-PopFactor) * (1.0-PopFactor);	// at 25% this works out to (3/4)^2 which is 9/16
			}
			return long(Growth + .5);
		}
	}
}

void Planet::DoProduction()
{
	Player * owner = NCGetOwner();
	if (owner == NULL)
		return;

	long resources = GetResources();
	bool AutoAlchemy = false;
	mBuiltFactories = 0;
	mBuiltMines = 0;
	mBuiltDefenses = 0;
	mBuiltAlchemy = 0;

	// Add UR resources
	resources += (mScrapRes * resources) / (mScrapRes + resources);

	// first pay research tax
	if (mPayTax && owner->GetResearchField() != RESEARCH_ALCHEMY) {
		long tax = long(resources * owner->GetResearchTax() + .5);
		tax -= owner->GainTech(tax);	// returns unused portion
		resources -= tax;
	}

	for (unsigned int i = 0; i < mProductionQ.size(); ++i) {
		if (mProductionQ[i]->Produce(this, &resources, &AutoAlchemy)) {
			delete mProductionQ[i];
			mProductionQ.erase(mProductionQ.begin() + i);
			--i;
		}

		if (resources <= 0)
			break;
	}

	// Left over resources
	if (resources > 0) {
		// make minerals or research
		if (!AutoAlchemy && owner->GetResearchField() != RESEARCH_ALCHEMY)
			resources = owner->GainTech(resources);	// returns unused portion

		if (resources > 0) {
			double BuildAlchem = 0;
			BuildAlchem = resources / (Rules::GetConstant("AlchemyCost") * owner->ComponentCostFactor(CT_ALCHEMY));

			POPlanetary * alchem = new POPlanetary(POP_ALCHEMY, long(BuildAlchem + 1.0 - epsilon));
			if (!alchem->Produce(this, &resources, &AutoAlchemy)) {
				mProductionQ.push_front(alchem);
			} else {
				delete alchem;
			}
		}
	}

	// Sum built messages
	if (mBuiltFactories > 0) {
		Message * mess = owner->AddMessage("Factories built", this);
		mess->AddLong("Number built", mBuiltFactories);
	}
	if (mBuiltMines > 0) {
		Message * mess = owner->AddMessage("Mines built", this);
		mess->AddLong("Number built", mBuiltMines);
	}
	if (mBuiltDefenses > 0) {
		Message * mess = owner->AddMessage("Defenses built", this);
		mess->AddLong("Number built", mBuiltDefenses);
	}
	if (mBuiltAlchemy > 0) {
		Message * mess = owner->AddMessage("Alchemy built", this);
		mess->AddLong("Number built", mBuiltAlchemy);
	}
}

long Planet::MaxFactories() const
{
	return GetMaxPop() * GetOwner()->FactoriesRun() / 10000;
}

long Planet::MaxMines() const
{
	return GetMaxPop() * GetOwner()->MinesRun() / 10000;
}

long Planet::MaxDefenses() const
{
	if (GetOwner()->HabFactor(this) < 0)
		return 10;
	else
		return min(Rules::GetConstant("MaxDefenses", 100L), GetMaxPop() / 2500L);
}

Cost Planet::GetPacketCost(long type) const
{
	Cost c;

	c.SetCrew(0);
	c.SetResources(GetOwner()->PacketCostResources());
	if (type == -1) {
		for (long i = 0; i < Rules::MaxMinType; ++i)
			c[i] = long(GetOwner()->PacketSizeMixed() * GetOwner()->PacketCostMinFactor());
	} else
		c[type] = long(GetOwner()->PacketSizeOneMin() * GetOwner()->PacketCostMinFactor());

	return c;
}

void Planet::ResetSeen()
{
	CargoHolder::ResetSeen();

	mCanLoadBy.clear();
	mCanLoadBy.insert(mCanLoadBy.begin(), TheGame->NumberPlayers(), false);
}

void Planet::CreateRandom(Creation * c)
{
	long i;

	mID = TheGalaxy->GetNextPlanetID();
	for (i = 0; i < Rules::MaxMinType; ++i)
		mMinConc[i] = Random(Rules::MinMC(i), Rules::MaxMC(i));

	mCanLoadBy.insert(mCanLoadBy.begin(), TheGame->NumberPlayers(), false);
	for (i = 0; i < Rules::MaxHabType; ++i)
		mHabTerra[i] = mHabStart[i] = Rules::RandomHab(i);

	// preplant artifacts
	if (TheGame->GetRandomEvents() | RE_ARTIFACT && Randodd(Rules::GetFloat("ArtifactOdds"))) {
		mArtifactAmount = Random(Rules::GetConstant("ArtifactMin"), Rules::GetConstant("ArtifactMax"));
		mArtifactType = Random(Rules::MaxTechType);
	}

	mName = c->GetNextName();
}

void Planet::CreateHW(const Player * player)
{
	mPopulation = TheGame->GetCreation()->mHWBasePop;
	if (player->HasSecondPlanet() && TheGame->GetCreation()->mSecondaryWorlds)
		mPopulation = long(mPopulation * TheGame->GetCreation()->mPopMultiplierFor2nd + .5);

	mDefenses = TheGame->GetCreation()->mHWDefenses;
	mFactories = TheGame->GetCreation()->mHWFactories;
	mMines = TheGame->GetCreation()->mHWMines;
	for (int i = 0; i < Rules::MaxMinType; ++i) {
		mMinConc[i] = Rules::GetHWMC(i);
		mContains[i] = Rules::GetHWStartMinerals(i);
	}

	mBaseDesign = -2;	// allow this world to have a base
	mArtifactType = ARTI_NONE;
	mArtifactAmount = 0;
	mHomeWorld = true;
}

void Planet::AdjustHW(Player * player)
{
	mOwner = player;
	long bonusPop;
	bonusPop = long(TheGame->GetCreation()->mHWPopBonus * player->GrowthRate() + .5);
	if (player->HasSecondPlanet() && TheGame->GetCreation()->mSecondaryWorlds)
		bonusPop = long(bonusPop * TheGame->GetCreation()->mPopMultiplierFor2nd + .5);

	mPopulation = long((bonusPop + mPopulation) * player->StartingPopFactor());

	int i;
	for (i = 0; i < Rules::MaxHabType; ++i) {
		if (player->HabCenter(i) >= 0)	// leave immunes random
			mHabTerra[i] = mHabStart[i] = player->HabCenter(i);
	}

	if (player->GetStartConcentrations() > 0) {
		long minMC = 0;
		for (i = 1; i < Rules::MaxMinType; ++i) {
			if (mMinConc[i] < mMinConc[minMC])
				minMC = i;
		}

		for (i = 0; i < Rules::MaxMinType; ++i) {
			if (i == minMC)
				mMinConc[i] += (player->GetStartConcentrations() + 2) * 3 / 4;	// 38 for 50 points
			else
				mMinConc[i] += (player->GetStartConcentrations() + 2) / 4;	// 13 for 50 points
		}
	}

	mMines += player->GetStartMines() / Rules::GetConstant("StartMineCost", 3);
	mFactories += player->GetStartFactories() / Rules::GetConstant("StartFactoryCost", 5);
	mDefenses += player->GetStartDefenses() / Rules::GetConstant("StartDefenseCost", 10);

	if (player->GetStartMinerals() > 0) {
		long minSM = 0;
		for (i = 1; i < Rules::MaxMinType; ++i) {
			if (mContains[i] < mContains[minSM])
				minSM = i;
		}
		mContains[minSM] += player->GetStartMinerals() * 5 / 2;

		for (i = 0; i < Rules::MaxMinType; ++i)
			mContains[i] += player->GetStartMinerals() * 5 / 2;
	}

	CopyProdQ(player->GetDefaultQ());
	mPayTax = player->GetDefaultPayTax();
	if (player->GetScanSpace() > 0)
		mScanner = true;
}

void Planet::CreateSecondWorld(const Planet * HW)
{
	mPopulation = long(HW->GetPopulation() * TheGame->GetCreation()->mSecondaryPop + .5);

	mDefenses = TheGame->GetCreation()->mSWDefenses;
	mFactories = TheGame->GetCreation()->mSWFactories;
	mMines = TheGame->GetCreation()->mSWMines;
	int i;
	for (i = 0; i < Rules::MaxMinType; ++i)
		mContains[i] = Rules::GetSWStartMinerals(i);

	for (i = 0; i < Rules::MaxHabType; ++i)
		mHabStart[i] = Rules::GetSecondHab(i, HW->GetOwner());

	mBaseDesign = -2;	// allow this world to have a base
	mArtifactType = ARTI_NONE;
	mArtifactAmount = 0;
}

void Planet::AdjustSecondWorld(Player * player)
{
	mOwner = player;
	int i;

	for (i = 0; i < Rules::MaxHabType; ++i)
		mHabTerra[i] = mHabStart[i] = Rules::GetSecondHab(i, player);

	CopyProdQ(player->GetDefaultQ());
	mPayTax = player->GetDefaultPayTax();
	if (player->GetScanSpace() > 0)
		mScanner = true;
}

void Planet::ParseProduction(const TiXmlNode * node)
{
	if (node != NULL)
		SetProduction(ProdOrder::ParseNode(node, this));
}

void Planet::SetProduction(const deque<ProdOrder *> & ords)
{
	assert(GetOwner());
	if (GetOwner()->WriteXFile())
		NCGetOwner()->AddOrder(new ProductionOrder(mName.c_str(), &mProductionQ));
	else {
		for (int i = 0; i < mProductionQ.size(); ++i)
			delete mProductionQ[i];
	}

	mProductionQ = ords;
}

void Planet::SetPayTax(bool paytax)
{
	assert(GetOwner());
	if (paytax != mPayTax && GetOwner()->WriteXFile())
		NCGetOwner()->AddOrder(new TypedOrder<bool>(&mPayTax, AddBool, "PayTax", "Planet", mName.c_str()));

	mPayTax = paytax;
}

void Planet::SetPacketSpeed(int speed)
{
	if (speed != mPacketSpeed && GetOwner()->WriteXFile())
		NCGetOwner()->AddOrder(new TypedOrder<long>(&mPacketSpeed, AddLong, "PacketSpeed", "Planet", mName.c_str()));

	mPacketSpeed = speed;
}

void Planet::SetPacketDest(Planet * pdest)
{
	if (pdest != mPacketDest && GetOwner()->WriteXFile())
		NCGetOwner()->AddOrder(new TypedOrder<Planet *>(&mPacketDest, "PacketDestination", Planet::SGetName, "Planet", mName.c_str()));

	mPacketDest = pdest;
}

void Planet::SetRoute(const Planet * rdest)
{
	if (rdest != mRouteTo && GetOwner()->WriteXFile())
		NCGetOwner()->AddOrder(new TypedOrder<const Planet *>(&mRouteTo, "RouteDestination", Planet::SGetName, "From", mName.c_str()));

	mRouteTo = rdest;
}

void Planet::RepairBase()
{
	if (mHadBattle || mOwner == NULL || mBaseDesign < 0 || mBaseDamage == 0)
		return;

	assert(mBaseDamage > 0);
	double rate = Rules::GetFloat("StarbaseRepair", .1);
	rate += rate * mOwner->RepairFactor() / 2.0;

	mBaseDamage -= min(1L, long(GetBaseDesign()->GetArmor(mOwner) * rate + .5));
}

void Planet::Instaform()
{
	if (mOwner == NULL)
		return;

	HabType ht;
	double odds = mOwner->PermaformOdds();
	if (odds > epsilon) {
		if (GetPopulation() < mOwner->PermaformPopAdj()) {
			odds *= GetPopulation();
			odds /= mOwner->PermaformPopAdj();
		}

		if (Randodd(odds)) {
			ht = Random(Rules::MaxHabType);
			if (Permaform(mOwner, 1, ht, false)) {
				Message * mess = mOwner->AddMessage("Permaform", this);
				mess->AddItem("Hab improved", Rules::GetHabName(ht).c_str());
			}
		}
	}

	if (mOwner->ComponentCostFactor(CT_TERRAFORM) < epsilon) {
		long startHab = mOwner->HabFactor(this);
		for (ht = 0; ht < Rules::MaxHabType; ++ht) {
			if (GetOwner()->HabCenter(ht) == -1 || mHabTerra[ht] == mOwner->HabCenter(ht))
				continue;

			long maxT, minT;
			maxT = min(mHabStart[ht] + mOwner->TerraLimit(ht), Rules::GetConstant("MaxHabValue"));
			maxT = max(maxT, mHabTerra[ht]);
			minT = max(mHabStart[ht] - mOwner->TerraLimit(ht), Rules::GetConstant("MinHabValue"));
			minT = min(minT, mHabTerra[ht]);

			if (mOwner->HabCenter(ht) > maxT)
				mHabTerra[ht] = maxT;
			else if (mOwner->HabCenter(ht) < minT)
				mHabTerra[ht] = minT;
			else	// between min and max, set to ideal
				mHabTerra[ht] = mOwner->HabCenter(ht);
		}

		long endHab = mOwner->HabFactor(this);
		if (startHab != endHab) {
			Message * mess = mOwner->AddMessage("Instaform", this);
			mess->AddLong("Start Hab", startHab);
			mess->AddLong("End Hab", endHab);
		}
	}
}

HabType Planet::BestHabForTerraform(const Player * owner, const Player * performer, bool positive, long amt, HabType ht, long * tick)
{
	long total = 0;		// check to see if maxamount is sufficient to maximize results - not used for deterraforming
	HabType h;			// current hab being checked
	long tempv;			// value change for current hab
	long temptick;		// tick to terraform this hab to
	long maxtemptick;	// max tick this hab could be terraformed to - not used for deterraforming
	HabType maxh = -1;	// best hab so far
	long maxv = 0;		// value of best hab so far
	long maxtick = 0;	// tick to terraform for best hab so far
	long maxmaxtick = 0;// max tick to terraform to for best hab so far (if maxamount is sufficient to maximize results) - not used for deterraforming
	long tempterra;		// temp storage for current hab value
	bool found = false;

	if (ht > 0)
		*tick = mHabTerra[ht];	// default to current if hab is specified

	for (h = 1; h < Rules::MaxHabType; ++h) {
		if (ht < 0 && ht != h)	// if hab specified, only check that one
			continue;

		// if doing terraforming, and this hab is already ideal, skip it
		if (positive && (mHabTerra[h] == owner->HabCenter(h)))
			continue;

		// if immune skip this hab, unless it's a packet
		if (owner->HabCenter(h) <= 0 && ht < 0)
			continue;

		tempv = 0;
		// figure out the lowest we could terraform this hab:
		long gminT = min(mHabTerra[h], mHabStart[h] - (performer == NULL) ? performer->TerraLimit(h) : 0L);
		gminT = max(gminT, Rules::GetConstant("MinHabValue"));
		long minT = max(gminT, mHabTerra[h] - amt);

		// figure out the highest we could terraform this hab:
		long gmaxT = max(mHabTerra[h], mHabStart[h] + (performer == NULL) ? performer->TerraLimit(h) : 0L);
		gmaxT = min(gmaxT, Rules::GetConstant("MaxHabValue"));
		long maxT = min(gmaxT, mHabTerra[h] + amt);

		if (positive) {
			if (mHabTerra[h] < owner->HabCenter(h)) {
				// if trying to improve and owner wants it increased
				maxtemptick = min(maxT, owner->HabCenter(h));
				total += maxtemptick - mHabTerra[h];
				temptick = min(maxtemptick, mHabTerra[h] + 1L);
			} else if (mHabTerra[h] > owner->HabCenter(h)) {
				// if trying to improve and owner wants it decreased
				maxtemptick = max(minT, owner->HabCenter(h));
				total += mHabTerra[h] - maxtemptick;
				temptick = max(maxtemptick, mHabTerra[h] - 1L);
			} else {
				assert(false);
				maxtemptick = temptick = mHabTerra[h];
			}

			// OPTIMIZE: It's probably possible to figure the total terraforming in one pass instead of looping and doing it one tick at a time

			if (maxtemptick == mHabTerra[h])
				continue;

			if (ht < 0) {
				tempterra = mHabTerra[h];
				mHabTerra[h] = temptick;
				tempv = owner->HabFactor(this);
				mHabTerra[h] = tempterra;
			}

			if (found && maxv < 0 && tempv > 0)
				continue;	// do red habs first

			if (!found || tempv > maxv) {	// if ht >= 0, found will be false
				found = true;
				maxv = tempv;
				maxtick = temptick;
				maxh = h;
				maxmaxtick = maxtemptick;
			}
		} else if (owner->HabCenter(h) <= 0) {
			// if owner is immune and specific hab type called for, deterraform it as much as possible
			found = true;
			long center = (Rules::GetConstant("MinHabValue", 1) + Rules::GetConstant("MaxHabValue", 99)) / 2;
			maxh = h;
			if (center - gminT > gmaxT - center)
				maxtick = minT;
			else if (center - gminT < gmaxT - center)
				maxtick = maxT;
			else
				maxtick = Random(2) ? minT : maxT;
		} else {
			// Testing determined that deterraforming goes for the absolute min, even it that cannot be achieved this turn,
			// which may end up improving a planet they are trying to deterraform, although that scenario is hard to arrange.
			long tempterra = mHabTerra[h];
			mHabTerra[h] = gminT;
			tempv = owner->HabFactor(this);
			if (!found || tempv < maxv) {
				found = true;
				maxv = tempv;
				maxtick = minT;
				maxh = h;
			}
			mHabTerra[h] = gmaxT;
			tempv = owner->HabFactor(this);
			if (!found || tempv < maxv) {
				found = true;
				maxv = tempv;
				maxtick = maxT;
				maxh = h;
			}

			mHabTerra[h] = tempterra;
		}
	}

	if (positive && (total <= amt || ht >= 0))
		*tick = maxmaxtick;
	else
		*tick = maxtick;

	return maxh;
}

void Planet::Terraform(long totech, long toinit, HabType ht, const Player * performer, const Player * owner, Message * mess)
{
	if (owner == NULL)
		return;

	bool positive = false;	// true if we're doing positive terraforming, false if doing negative terraforming
	if (performer == owner || performer->GetRelations(owner) >= PR_FRIEND)
		positive = true;

	long tick = 0;
	HabType h;
	deque<long> workdone;
	workdone.insert(workdone.begin(), Rules::MaxHabType, 0);

	while (toinit > 0) {
		h = BestHabForTerraform(owner, NULL, positive, toinit, ht, &tick);
		if (h >= 0) {
			workdone[h] += tick - mHabTerra[h];
			toinit -= abs(tick - mHabTerra[h]);
			mHabTerra[h] = tick;
			if (tick == mHabTerra[h])
				break;
		} else
			break;

		if (ht >= 0)	// only one pass when hab specifed (packets)
			break;
	}

	while (totech > 0) {
		h = BestHabForTerraform(owner, performer, positive, totech, ht, &tick);
		if (h >= 0) {
			workdone[h] += tick - mHabTerra[h];
			totech -= abs(tick - mHabTerra[h]);
			mHabTerra[h] = tick;
			if (tick == mHabTerra[h])
				break;
		} else
			break;

		if (ht >= 0)	// only one pass when hab specifed (packets)
			break;
	}

	if (mess != NULL) for (h = 0; h < Rules::MaxHabType; ++h) {
		if (workdone[h] > 0)
			mess->AddLong(Rules::GetHabName(h).c_str(), workdone[h]);
	}
}

bool Planet::Permaform(const Player * performer, long amount, HabType ht, bool ImmToExtreme)
{
	if (amount <= 0 || ht < 0)
		return false;

	bool Result = false;
	if (performer->HabCenter(ht) == -1 && ImmToExtreme) {
		long minhab, maxhab, center;
		minhab = Rules::GetConstant("MinHabValue", 1);
		maxhab = Rules::GetConstant("MaxHabValue", 99);
		center = (minhab + maxhab) / 2;

		if (mHabStart[ht] < center || (mHabStart[ht] == center && Random(2) == 0)) {
			if (mHabStart[ht] != minhab) {
				Result = true;
				mHabStart[ht] = max(minhab, mHabStart[ht] - amount);
			}
		} else {
			if (mHabStart[ht] != maxhab) {
				Result = true;
				mHabStart[ht] = min(maxhab, mHabStart[ht] + amount);
			}
		}

		return Result;
	}

	long diff = mHabStart[ht] - performer->HabCenter(ht);
	if (diff == 0 || performer->HabCenter(ht) <= 0)
		return false;

	if (diff > 0) {
		diff = min(diff, amount);
		mHabStart[ht] -= diff;
		mHabTerra[ht] -= diff;
	} else {
		diff = min(-diff, amount);
		mHabStart[ht] += diff;
		mHabTerra[ht] += diff;
	}

	return true;
}

void Planet::RemoteTerraform(Fleet * fleet, bool bomb)
{
	// At some point, perhaps this should be changed to be the sum of positive and
	// negative terraforming, if there are is competing remote terraforming.
	// Or apply lowest terra tech first if cooperating ones.

	bool positive = fleet->GetOwner()->GetRelations(mOwner) > PR_NEUTRAL;

	long totech, toinit;
	if (positive) {
		totech = fleet->GetTerraPower((bomb ? TERRA_BOMB : TERRA_REMOTE) | TERRA_POSTERRA | TERRA_TOTECH);
		toinit = fleet->GetTerraPower((bomb ? TERRA_BOMB : TERRA_REMOTE) | TERRA_POSTERRA | TERRA_TOINIT);
	} else {
		totech = fleet->GetTerraPower((bomb ? TERRA_BOMB : TERRA_REMOTE) | TERRA_DETERRA | TERRA_TOTECH);
		toinit = fleet->GetTerraPower((bomb ? TERRA_BOMB : TERRA_REMOTE) | TERRA_DETERRA | TERRA_TOINIT);
	}

	if (totech == 0 && toinit == 0)
		return;

	long startHab = mOwner->HabFactor(this);
	Terraform(totech, toinit, -1, fleet->GetOwner(), GetOwner(), NULL);
	long endHab = mOwner->HabFactor(this);

	Message * mess = fleet->NCGetOwner()->AddMessage("Remote Terraform", this);
	mess->AddItem("Terraforming fleet", fleet);
	mess->AddLong("Start Hab", startHab);
	mess->AddLong("End Hab", endHab);

	if (startHab != endHab && mOwner != fleet->GetOwner()) {
		mess = mOwner->AddMessage("Remote Terraform", this);
		mess->AddItem("Terraforming fleet", fleet);
		mess->AddLong("Start Hab", startHab);
		mess->AddLong("End Hab", endHab);
	}
}

void Planet::SweepMines()
{
	if (mOwner == NULL || mBaseDesign < 0)
		return;

	int sweep = GetBaseDesign()->GetSweeping();
	if (sweep == 0)
		return;

	TheGame->SweepMines(this, sweep, mOwner->GetBattlePlan(0));
}

}
