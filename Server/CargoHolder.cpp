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

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {
void CargoHolder::Init()
{
	mContains.insert(mContains.begin(), Rules::MaxMinType, 0);
	mPopulation = 0;
	mID = 0;
}

CargoHolder::~CargoHolder()
{
}

bool CargoHolder::ParseNode(const TiXmlNode * node)
{
	if (!SpaceObject::ParseNode(node))
		return false;

	Rules::ReadCargo(node->FirstChild("Contains"), mContains, &mPopulation, *mGame);
	return true;
}

TiXmlNode * CargoHolder::WriteNode(TiXmlNode * node, const Player * viewer) const
{
	if (SpaceObject::WriteNode(node, viewer) == NULL)
		return NULL;

	TiXmlElement * cargo = NULL;
	if (viewer == NULL || viewer == GetOwner())
		cargo = Rules::WriteCargo(node, "Contains", mContains, mPopulation);
	else if (CanLoadBy(viewer))
		cargo = Rules::WriteCargo(node, "Contains", mContains, 0);

	return node;
}

TiXmlNode * CargoHolder::WriteTransport(TiXmlNode * node) const
{
	const Planet * p = dynamic_cast<const Planet *>(this);
	if (p != NULL) {
		AddString(node, "Planet", p->GetName().c_str());
		return node;
	}

	const Fleet * f = dynamic_cast<const Fleet *>(this);
	if (f != NULL) {
		AddLong(node, "Fleet", GetID());
		AddLong(node, "Owner", GetOwner()->GetID());
		return node;
	}

//	const Packet * pac = dynamic_cast<const Packet *>(this);
//	if (pac != NULL) {
//		return node;
//	}

	return node;
}

void CargoHolder::ProcessUnload(CargoHolder * dest, CargoType ct, TransferType tt, long value)	// value usage depends on TransferType
{
	assert(dest->IsWith(*this));
	assert(dest != this);

	if (value < 0) {
		Message * mess = NCGetOwner()->AddMessage("Error: Transfer order is negative", this);
		mess->AddLong("Amount transferred", value);
		return;
	}

	if (value > Rules::GetConstant("MaxTransfer")) {
		Message * mess = NCGetOwner()->AddMessage("Error: Transfer order is over max", this);
		mess->AddLong("Amount transferred", value);
		return;
	}

	long destAmt = dest->GetContain(ct);
	if (GetOwner() != dest->GetOwner() && (ct == POPULATION || !dest->CanLoadBy(GetOwner())))
		destAmt = 0;

	long amount = 0;	// actual amount moved;
	switch (tt) {
	case TRANSFER_LOADALL:
		// processing unloads, skip loads.
		break;

	case TRANSFER_DROPNLOAD:
	case TRANSFER_UNLOADALL:
		amount = TransferAmount(ct, this, dest, GetContain(ct));
		break;

	case TRANSFER_LOADAMT:
		// processing unloads, skip loads.
		break;

	case TRANSFER_UNLOADAMT:
		amount = TransferAmount(ct, this, dest, value);
		break;

	case TRANSFER_FILLPER:
	case TRANSFER_WAITPER:	// affects movement too
		// processing unloads, skip loads.
		break;

	case TRANSFER_LOADDUNN:
		// processing unloads, skip loads.
		break;

	case TRANSFER_SETTOPER:
		if (value > 100) {
			Message * mess = NCGetOwner()->AddMessage("Error: Transfer percent over 100%", this);
			mess->AddLong("Amount transferred", value);
			return;
		}
		value = GetCargoCapacity() * value / 100;
		// drop to a regualr transfer amount
	case TRANSFER_AMOUNTTO:
		if (GetContain(ct) > value)
			amount = TransferAmount(ct, this, dest, GetContain(ct) - value);
		break;

	case TRANSFER_DESTTO:
		// Allow set dest to for pop to do a maximal invade, followed by a setto load
		if (ct == POPULATION && GetOwner() != dest->GetOwner())
			amount = TransferAmount(ct, this, dest, GetContain(ct));
		else if (destAmt < value)
			amount = TransferAmount(ct, this, dest, value - destAmt);
		break;

	default:
		Message * mess = NCGetOwner()->AddMessage("Error: Invalid transfer order", this);
		mess->AddLong("Transfer code", tt);
		return;
	}

	if (amount > 0 && ct == POPULATION && GetOwner() != dest->GetOwner()) {
		// try to drop pop on an uninhabited world
		if (dest->GetOwner() == NULL) {
			Message * mess;
			mess = NCGetOwner()->AddMessage("Warning: Pop drop on unowned world", this);
			return;
		}

		// Dropping pop on a world with a base
		Planet * destP = dynamic_cast<Planet *>(dest);
		if (destP && destP->GetBaseNumber() >= 0) {
			Message * mess;
			mess = NCGetOwner()->AddMessage("Warning: Pop drop with base", this);
			mess->AddItem("", destP);
			return;
		}

		// unloading pop to some one elses fleet
		Fleet * destF = dynamic_cast<Fleet *>(dest);
		if (destF) {
			Message * mess;
			mess = NCGetOwner()->AddMessage("Warning: Transfer pop to unowned fleet", this);
			mess->AddItem("", destF);
			return;
		}

		if (GetOwner()->GroundAttackFactor() <= 0.01) {
			Message * mess;
			mess = NCGetOwner()->AddMessage("Warning: AR trying to invade", this);
			mess->AddItem("", dest);
			return;
		}

		destP->Invade(NCGetOwner(), amount);
	}

	assert(amount >= 0);
	dest->AdjustAmounts(ct, amount);	// add cargo to destination
	AdjustAmounts(ct, -amount);		// remove it from the source
}

void CargoHolder::ProcessLoad(CargoHolder * dest, CargoType ct, TransferType tt, long value, bool dunnage)	// value usage depends on TransferType
{
	assert(dest->IsWith(*this));
	assert(dest != this);

	if (value < 0) {
		Message * mess = NCGetOwner()->AddMessage("Error: Transfer order is negative", this);
		mess->AddLong("Amount transferred", value);
		return;
	}

	if (value > Rules::GetConstant("MaxTransfer")) {
		Message * mess = NCGetOwner()->AddMessage("Error: Transfer order is over max", this);
		mess->AddLong("Amount transferred", value);
		return;
	}

	long destAmt = dest->GetContain(ct);
	if (GetOwner() != dest->GetOwner() && (ct == POPULATION || !dest->CanLoadBy(GetOwner())))
		destAmt = 0;

	long amount = 0;	// actual amount moved;
	switch (tt) {
	case TRANSFER_DROPNLOAD:
	case TRANSFER_LOADALL:
		if (dunnage) break;
		amount = TransferAmount(ct, dest, this, destAmt);
		break;

	case TRANSFER_UNLOADALL:
		break;

	case TRANSFER_LOADAMT:
		if (dunnage) break;
		amount = TransferAmount(ct, dest, this, value);
		break;

	case TRANSFER_UNLOADAMT:
		break;

	case TRANSFER_FILLPER:
	case TRANSFER_WAITPER:	// affects movement too
		if (dunnage) break;
		if (value > 100) {
			Message * mess = NCGetOwner()->AddMessage("Error: Transfer percent over 100%", this);
			mess->AddLong("Amount transferred", value);
			return;
		}
		amount = TransferAmount(ct, dest, this, GetCargoCapacity() * value / 100);
		break;

	case TRANSFER_LOADDUNN:
		if (!dunnage) break;
		// order of loading is different, but this is basicly a loadall -- execpt fuel
		if (ct == FUEL) {
			Fleet * f = dynamic_cast<Fleet *>(this);
			int Need = f->GetFuelNeeded() - f->GetFuel();
			if (Need > 0)
				amount = TransferAmount(ct, dest, this, Need);
			else
				amount = -TransferAmount(ct, this, dest, -Need);
		} else {
			amount = TransferAmount(ct, dest, this, destAmt);
		}
		break;

	case TRANSFER_SETTOPER:
		if (value > 100) {
			Message * mess = NCGetOwner()->AddMessage("Error: Transfer percent over 100%", this);
			mess->AddLong("Amount transferred", value);
			return;
			
			
		}
		value = GetCargoCapacity() * value / 100;
		// drop to a regualr transfer amount
	case TRANSFER_AMOUNTTO:
		if (dunnage) break;
		if (GetContain(ct) < value)
			amount = TransferAmount(ct, dest, this, value - GetContain(ct));
		break;

	case TRANSFER_DESTTO:
		if (dunnage) break;
		if (destAmt > value)
			amount = TransferAmount(ct, dest, this, destAmt - value);
		break;

	default:
		Message * mess = NCGetOwner()->AddMessage("Error: Invalid transfer order", this);
		mess->AddLong("Transfer code", tt);
		return;
	}

	assert(amount >= 0 || tt == TRANSFER_LOADDUNN && ct == FUEL);

	if (amount > 0 && dest->GetOwner() != GetOwner()) {
		if (!dest->CanLoadBy(GetOwner())) {
			Message * mess = NCGetOwner()->AddMessage("Warning: No theiving component at location", this);
			mess->AddItem("", dest);
			return;
		} else if (!Rules::Stealable(ct)) {
			Message * mess = NCGetOwner()->AddMessage("Warning: Cargo cannot be stolen", this);
			mess->AddItem("", dest);
			mess->AddLong("Cargo type", ct);
			return;
		}
	}

	dest->AdjustAmounts(ct, -amount);	// remove cargo from destination
	AdjustAmounts(ct, amount);		// add it to the source
}

long CargoHolder::GetCargoMass() const
{
	long Result = mPopulation / Rules::PopEQ1kT;
	for (int i = 0; i < Rules::MaxMinType; ++i)
		Result += this->mContains[i];

	return Result;
}

long CargoHolder::TransferAmount(CargoType ct, CargoHolder * from, CargoHolder * to, long Request)
{
	assert(from->IsWith(*to));
	assert(from != to);
	assert(Request >= 0);
	assert(FUEL <= ct && ct < Rules::MaxMinType);

	long Result = Request;
	if (Result > from->GetContain(ct))
		Result = from->GetContain(ct);

	long cap = to->GetCargoCapacity() - to->GetCargoMass();
	if (cap >= 0 && Result > cap)
		Result = cap;

	return Result;
}

void CargoHolder::TransferCargo(CargoHolder * dest, CargoType ct, long * amount, Player * player)
{
	if (*amount == 0)
		return;
	assert(*amount > 0);

	if (GetContain(ct) < *amount) {
		Message * mess = player->AddMessage("Warning: Transfer more then carried", this);
		mess->AddLong("Attempted amount", *amount);
		*amount = GetContain(ct);
		mess->AddLong("Actual amount", *amount);
	}

	if (ct == POPULATION) {
		*amount -= *amount % Rules::PopEQ1kT;	// only transfer full groups
		if (dest->GetCargoCapacity() >= 0 && dest->GetCargoCapacity() < dest->GetCargoMass() + (*amount / Rules::PopEQ1kT)) {
			Message * mess = player->AddMessage("Warning: Transfer more then capacity", this);
			mess->AddItem("", dest);
			mess->AddLong("Attempted amount", *amount);
			*amount = (dest->GetCargoCapacity() - dest->GetCargoMass()) * Rules::PopEQ1kT;
			mess->AddLong("Actual amount", *amount);
		}
	} else if (ct == FUEL) {
		Fleet * destf = dynamic_cast<Fleet *>(dest);
		if (destf->GetFuelCapacity() >= 0 && destf->GetFuelCapacity() < destf->GetFuel() + *amount) {
			Message * mess = player->AddMessage("Warning: Transfer more then capacity", this);
			mess->AddItem("", dest);
			mess->AddLong("Attempted amount", *amount);
			*amount = destf->GetFuelCapacity() - destf->GetFuel();
			mess->AddLong("Actual amount", *amount);
		}
	} else {
		if (dest->GetCargoCapacity() >= 0 && dest->GetCargoCapacity() < dest->GetCargoMass() + *amount) {
			Message * mess = player->AddMessage("Warning: Transfer more then capacity", this);
			mess->AddItem("", dest);
			mess->AddLong("Attempted amount", *amount);
			*amount = dest->GetCargoCapacity() - dest->GetCargoMass();
			mess->AddLong("Actual amount", *amount);
		}
	}

	if (ct == POPULATION && GetOwner() != dest->GetOwner()) {
		assert(false);	// should never get here now
		Planet * destp = dynamic_cast<Planet *>(dest);
		if (destp) {
			if (destp->GetBaseNumber() >= 0) {
				player->AddMessage("Warning: Invading world with base", this);
				return;
			} else {
				AdjustAmounts(ct, -*amount);
				destp->Invade(NCGetOwner(), *amount);
				return;
			}
		} else {
			Message * mess = player->AddMessage("Warning: Transfer pop to unowned fleet", this);
			mess->AddItem("", dest);
			return;
		}
	}

	dest->AdjustAmounts(ct, *amount);
	AdjustAmounts(ct, -*amount);
}

long CargoHolder::GetContain(CargoType ct) const
{
	if (ct >= 0)
		return mContains[ct];
	else if (ct == POPULATION)
		return GetPopulation();
	else if (ct == FUEL) {
		const Fleet * f = dynamic_cast<const Fleet *>(this);
		if (f == NULL)
			return 0;
		else
			return f->GetFuel();
	} else {
		assert(false);
		return 0;
	}
}

void CargoHolder::AdjustAmounts(CargoType ct, long amount)
{
	if (amount == 0)
		return;

	if (ct >= 0) {
		mContains[ct] += amount;
	} else if (ct == POPULATION) {
		mPopulation += amount;
	} else if (ct == FUEL) {
		Fleet * f = dynamic_cast<Fleet *>(this);
		if (f == NULL) {
			assert(false);
		} else
			f->AdjustFuel(amount);
	} else {
		assert(false);
	}
}

}
