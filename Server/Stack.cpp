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
#include "Stack.h"
#include "Hull.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

Stack::~Stack()
{
}

void Stack::AddFromFleet(long fleet, long ships, long damaged)
{
	assert(ships <= mShips);
	assert(damaged <= mDamaged);
	deque<Origin>::iterator iter;
	for (iter = mOrigins.begin(); iter != mOrigins.end(); ++iter) {
		if (iter->fleet == fleet)
			break;
	}

	if (iter == mOrigins.end()) {
		iter = mOrigins.insert(mOrigins.end(), Origin());
		iter->fleet = fleet;
	}

	iter->ships += ships;
	iter->damaged += damaged;
}

bool Stack::ParseNode(const TiXmlNode * node, Player * owner)
{
	long num = GetLong(node->FirstChild("ShipDesign"), -1);
	if (num < 1 || num > Rules::GetConstant("MaxShipDesigns")) {
		Message * mess = owner->AddMessage("Error: Invalid ship design number");
		mess->AddLong("", num);
		return false;
	}

	mShip = owner->GetShipDesign(num);
	mShips = GetLong(node->FirstChild("ShipCount"));
	if (mShips < 1 || mShips > Rules::GetConstant("MaxShips")) {
		Message * mess = owner->AddMessage("Error: Invalid number of ships");
		mess->AddLong(mShip->GetName().c_str(), mShips);
		return false;
	}

	mDamaged = GetLong(node->FirstChild("Damaged"));
	if (mDamaged < 0 || mDamaged > mShips) {
		Message * mess = owner->AddMessage("Error: Invalid number of damaged ships");
		mess->AddLong(mShip->GetName().c_str(), mDamaged);
		return false;
	}

	mDamage = GetLong(node->FirstChild("Damage"));
	// extra check of Damage > 0 so it doesn't have to calc GetArmor if not needed
	if (mDamage < 0 || (mDamage > 0 && mDamage > mShip->GetArmor(owner))) {
		Message * mess = owner->AddMessage("Error: Invalid amount of damage");
		mess->AddLong(mShip->GetName().c_str(), mDamage);
		return false;
	}

	return true;
}

void Stack::WriteNode(TiXmlNode * node, const Player * owner, const Player * viewer) const
{
	AddLong(node, "ShipDesign", owner->GetShipNumber(mShip)+1);
	AddLong(node, "ShipCount", mShips);
	if (mDamaged > 0 && viewer == NULL || viewer == owner) {
		AddLong(node, "Damaged", mDamaged);
		AddLong(node, "Damage", mDamage);
	}
}

void Stack::WriteNode(TiXmlNode * node) const
{
	AddLong(node, "Owner", mFleetIn->GetOwner()->GetID());
	WriteNode(node, mFleetIn->GetOwner(), NULL);

	// add battle stuff
	AddLong(node, "Armor", bArmor);
	AddLong(node, "Shield", bShield);
	AddLong(node, "Speed", bSpeed);
	AddLong(node, "XPos", bx);
	AddLong(node, "YPos", by);
	mFleetIn->GetBattlePlan()->WriteNodeBattle(node, *mFleetIn->GetGame());
}

void Stack::SetupBase(const Planet * planet)
{
	mFleetIn = NULL;
	mDamage = planet->GetBaseDamage();
	if (mDamage > 0)
		mDamaged = 1;
	else
		mDamaged = 0;

	mShip = planet->GetBaseDesign();
	mShips = 1;
	bShips = 1;
	bArmor = mShip->GetArmor(planet->GetOwner()) - mDamage;
	bIsBase = true;
	bMass = 0;	// not strictly true, but base mass isn't needed
	bShield = mShip->GetShield(planet->GetOwner());
	bSpeed = 0;
	bPlan = BPT_MAXDAM;
	bFlee = 0;
}

void Stack::SetupShips(const Player * owner, long cargo)
{
	bShips = mShips;
	bArmor = mShip->GetArmor(owner) * GetCount() - GetDamage() * GetDamaged();
	assert(bArmor > 0);
	bShield = mShip->GetShield(owner) * GetCount();
	bSpeed = mShip->GetNetSpeed();
	bMass = mShip->GetMass();
	bMass += cargo;
	bSpeed -= bMass / (70 * mShip->GetHull()->GetSlot(0).GetCount());
	bSpeed += long(owner->BattleSpeedBonus() * 4 + .1);
	if (bSpeed > 10)
		bSpeed = 10;
	if (bSpeed < 2)
		bSpeed = 2;
	long lmin = long(bMass * 0.85 + .5);
	long lmax = long(bMass * 1.15 + .5);
	bMass = Random(lmin, lmax);
	bPlan = mFleetIn->GetBattlePlan()->GetTactic();
	bFlee = 0;
}

bool Stack::KillShips(long count, bool salvage, Galaxy *galaxy)
{
	if (count <= 0)
		return false;

	if (count > GetCount())
		count = GetCount();

	Salvage * sal = NULL;
	if (salvage)
		sal = galaxy->AddSalvage(*GetFleetIn());

	// lose a portion of the fleets cargo
	double cargolost = double(GetDesign()->GetFuelCapacity() * count) / GetFleetIn()->GetFuelCapacity();
	GetFleetIn()->AdjustFuel(-long(cargolost * GetFleetIn()->GetFuel() +.5));

	if (GetDesign()->GetCargoCapacity() > 0 && GetFleetIn()->GetCargoCapacity() > 0) {
		cargolost = double(GetDesign()->GetCargoCapacity() * count) / GetFleetIn()->GetCargoCapacity();
		GetFleetIn()->AdjustAmounts(POPULATION, -long(cargolost * GetFleetIn()->GetContain(POPULATION) + .5));
		long amount;
		for (int ct = 0; ct < Rules::MaxMinType; ++ct) {
			amount = long(cargolost * GetFleetIn()->GetContain(ct) + .5);
			GetFleetIn()->AdjustAmounts(ct, -amount);
			if (salvage) {
				amount += GetDesign()->GetCost(GetFleetIn()->GetOwner())[ct] / 3;
				sal->AdjustAmounts(ct, amount);
			}
		}
	}

	SetCount(GetCount() - count);
	SetDamaged(GetDamaged() - count);	// kill damaged ships first
	if (GetCount() <= 0)
		return true;
	else
		return false;
}

long Stack::DamageAllShips(long damage)
{
	long Result = 0;
	if (damage > GetDesign()->GetArmor(GetFleetIn()->GetOwner()))
		Result = GetCount();
	else if (GetDamage() + damage > GetDesign()->GetArmor(GetFleetIn()->GetOwner())) {
		Result = GetDamaged();
		SetDamaged(GetCount());
		SetDamage(damage);
	} else {
		damage += long(GetDamage() * GetDamaged() / GetCount() + .5);
		SetDamaged(GetCount());
		SetDamage(damage);
	}

	return Result;
}

}
