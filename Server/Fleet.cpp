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
#include "TempFleet.h"
#include "Hull.h"
#include "Order.h"
#include "Wormhole.h"
#include "MineFieldType.h"

#include <stdlib.h>
#include <algorithm>
#include <functional>

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

Fleet::Fleet(int id, const CargoHolder &loc) : CargoHolder(loc), mStartPos(loc)
{
	mID = id;
	Init();
}

Fleet::Fleet(Game *game)
    : CargoHolder(game)
{
	Init();
}

void Fleet::Init()
{
	mName.erase();
	ResetDefaults();	// Set all calculated values to defaults
	mCanLoadBy.clear();
	mCanLoadBy.insert(mCanLoadBy.begin(), mGame->NumberPlayers(), false);
	mRepeatOrders = false;

	mRepairRate = 2;	// how much this fleet will repair this turn
	mBattlePlan = 0;
	mRepeatOrders = false;
	mAlreadyFought = false;
	mHasMoved = false;
	mChasing = NULL;
	mDistMoved = 0;
	mFuel = 0;	// unknown
}

Fleet::~Fleet()
{
	mStacks.clear();
	int i;
	for (i = 0; i < mOrders.size(); ++i)
		delete mOrders[i];
}

void Fleet::ResetDefaults()
{
	// set all Calculated Values to defaults
	CVCost.Zero();
	ReCost = 0;
	CVScanPenetrating = -2;
	CVScanSpace = -1;
	CVMass = -1;

	CVCloaking = -1;
	CVFuelCapacity = -1;
	CVFuelGeneration = -1;
	CVMass = -1;
	CCalcMaxMass = false;
	CCalcMaxTachyon = false;
	CVCargoCapacity = -1;
	CVMines = -1;
	CVSweeping = -1;

	CCalcMaxDampener = false;
	CCalcMaxRepairRate = false;
	CVMinSafeSpeed = -1;
	CVMinMaxSpeed = -1;
	CVMinFreeSpeed = -1;
	CVMinBattleSpeed = -1;

	CVColonize = -1;
	CVStealShip = -1;
	CVStealPlanet = -1;
	CVShoot = -1;

	CCalcNormalKillPercentage = false;
	CVKillMin = CVKillInstallation = -1;
	CVJumpGate = -1;

	CVNormalBomb = CVSmartBomb = CVTerraBomb = -1;

	CVMineAmount.erase(CVMineAmount.begin(), CVMineAmount.end());
	CVMineAmount.insert(CVMineAmount.begin(), MT_MAXIMUM, -1);

	CVTechLevel.erase(CVTechLevel.begin(), CVTechLevel.end());
	CVTechLevel.insert(CVTechLevel.begin(), Rules::MaxTechType, -1);

	CVRadiation.erase(CVRadiation.begin(), CVRadiation.end());
	CVRadiation.insert(CVRadiation.begin(), Rules::MaxHabType, -1);
}

bool Fleet::ParseNode(const TiXmlNode * node, Player * player, bool other)
{
	if (!CargoHolder::ParseNode(node))
		return false;

	mStartPos = *this;
	const TiXmlNode * child1;
	const char * ptr;
    ArrayParser arrayParser(*player->GetGame());

	if (mID < 1 || mID > (int)Rules::MaxFleets)
		return false;

	ptr = GetString(node->FirstChild("Name"));
	if (ptr != NULL)
		mName = ptr;

	child1 = node->FirstChild("Contains");
	if (child1 != NULL)
		mFuel = GetLong(child1->FirstChild("Fuel"));

	if (!other) {
		mBattlePlan = GetLong(node->FirstChild("BattlePlan"));
		if (mBattlePlan < 0 || mBattlePlan >= Rules::GetConstant("MaxBattlePlans"))
			return false;

		mRepeatOrders = GetBool(node->FirstChild("Repeat"));

		arrayParser.ParseArrayBool(node->FirstChild("CanLoadBy"), "Race", "Number", mCanLoadBy);
	}

	for (child1 = node->FirstChild("Stack"); child1; child1 = child1->NextSibling("Stack")) {
		Stack a;
		unsigned long c = GetLong(child1->FirstChild("ShipCount"));
		a.SetCount(c);
		unsigned long d = GetLong(child1->FirstChild("ShipDesign"));
		const Ship * s = player->GetShipDesign(d);
		if (s == NULL) {
			return false;
		}
		a.SetDesign(s);
		mStacks.push_back(a);
		mStacks.back().ParseNode(child1, player);
		mStacks.back().SetFleetIn(this);
	}

	if (!other) {
		WayOrderList ords;
		ords.SetFleet(GetID());
		ords.ParseNode(node, player, mGame);
		ChangeWaypoints(ords);
		mBattlePlan = GetLong(node->FirstChild("BattlePlan"));
	}

	return true;
}

TiXmlNode * Fleet::WriteNode(TiXmlNode * node, const Player * viewer) const
{
	CargoHolder::WriteNode(node, viewer);

	if (viewer == NULL || viewer == GetOwner()) {
		TiXmlNode * child1;
		child1 = node->FirstChild("Contains");
		if (child1 == NULL) {
			child1 = new TiXmlElement("Contains");
			node->LinkEndChild(child1);
		}
		AddLong(child1, "Fuel", mFuel);

		AddString(node, "Name", mName.c_str());
		AddLong(node, "BattlePlan", mBattlePlan);
		AddBool(node, "Repeat", mRepeatOrders);
		for (int i = 0; i < mOrders.size(); ++i)
			mOrders[i]->WriteNode(node);
	} else {
		// For enemy fleets, write the destination and speed of first waypoint
		// if in space.
		if (mOrders.size() > 1 && dynamic_cast<Planet *>(mAlsoHere->at(0)) == NULL) {
			Location * loc = mOrders[0]->NCGetLocation();
			WayOrder wonew = WayOrder(mOrders[0]->NCGetLocation(), false);
			wonew.WriteNode(node);
			wonew.SetLocation(mOrders[1]->NCGetLocation(), false);
			wonew.SetSpeed(mOrders[1]->GetSpeed());
			wonew.WriteNode(node);
		}
	}
	if (viewer == NULL)
		node->LinkEndChild(Rules::WriteArrayBool("CanLoadBy", "Race", "Number", mCanLoadBy));

	AddLong(node, "Mass", GetMass());
	deque<Stack>::const_iterator si;
	for (si = mStacks.begin(); si != mStacks.end(); ++si) {
		TiXmlElement stack("Stack");
		si->WriteNode(&stack, GetOwner(), viewer);
		node->InsertEndChild(stack);
	}

	return node;
}

const string Fleet::GetName(const Player * viewer) const
{
	if (viewer == GetOwner() && !mName.empty())
		return mName;
	else
	{
		string str;
		const Ship * MostShips = NULL;
		long count = 0;

		// loop through ships
		deque<Stack>::const_iterator iter;
		for (iter = mStacks.begin(); iter != mStacks.end(); ++iter) {
			if (iter->GetCount() > count) {
				count = iter->GetCount();
				MostShips = iter->GetDesign();
			}
		}

		str = MostShips->GetName();
		if (str.empty()) {
			str = MostShips->GetHull()->GetName();
			for (int i = 0; i < Rules::GetConstant("MaxShipDesigns"); ++i) {
				if (GetOwner()->GetShipDesign(i) == MostShips) {
					str += "(" + Long2String(i) + ")";
					break;
				}
			}
		}

		if (viewer != GetOwner())
			str = GetOwner()->GetSingleName() + " " + str;

		if (mStacks.size() > 1)
			str += '+';
		str += " #" + Long2String(mID);
		return str;
	}
}

const BattlePlan * Fleet::GetBattlePlan() const
{
	return mOwner->GetBattlePlan(mBattlePlan);
}

bool Fleet::Process(FuncType func, bool arg)
{
	switch (func) {
	case FTScrap:
		if (mOrders.size() > 0 && GetFirstOrder()->GetType() == OT_SCRAP)
			Scrap(false);
		return false;
	case FTRemoteMine:
		RemoteMine(arg);
		return false;
	case FTUnload0:
		ProcessTransport(false, arg);
		return false;
	case FTColonize0:
		Colonize();
		return false;
	case FTLoad0:
		ProcessTransport(true, arg);
		return false;
	case FTMove:
		if (!mHasMoved)
			return Move();
		else
			return false;
	case FTFreighterReproduction:
		FreighterReproduction();
		return false;
	case FTRefuel:
		Refuel();
		return false;
	case FTCheckWaypoint:
		CheckWaypoint();
		return false;
	case FTUnload1:
		if (mDoneWaypoint)
			ProcessTransport(false, arg);
		return false;
	case FTColonize1:
		if (mDoneWaypoint)
			Colonize();
		return false;
	case FTLoad1:
		if (mDoneWaypoint)
			ProcessTransport(true, arg);
		return false;
	case FTMerge:
		CheckMerge();
		return false;
	case FTClearWaypoint:
		ClearWaypoint();
		return false;
	case FTRemoteTerraform:
		RemoteTerraform(false);
		return false;
	case FTRepair:
		Repair();
		return false;
	case FTSweepMines:
		SweepMines();
		return false;
	case FTTransfer:
		Transfer();
		return false;
	default:
		return false;
	}
}

void Fleet::CheckWaypoint()
{
	if (mOrders.size() >= 2 && *this == *mOrders[1]->GetLocation()) {
		delete mOrders[0];
		mOrders.pop_front();
		mDoneWaypoint = true;
	} else
		mDoneWaypoint = false;

	// set current order location to current location
	// but, don't reset location if location is a fleet (so you can continue following some one)
	Fleet * f = dynamic_cast<Fleet *>(mOrders[0]->NCGetLocation());
	if (f != NULL) {
		if (!IsWith(*f)) {
			mOrders[0]->SetLocation(new Location(*this), true);
			WayOrder * wo = new WayOrder(f);
			wo->SetType(OT_NONE);
			wo->SetSpeed(GetBestSpeed(this, f, OT_PATROL));
			mOrders.push_back(wo);
		}
	} else {
		if (dynamic_cast<Planet *>(mAlsoHere->at(0)) != NULL)
			mOrders[0]->SetLocation(mAlsoHere->at(0), false);
		else
			mOrders[0]->SetLocation(new Location(*this), true);
	}
}

void Fleet::ClearWaypoint()
{
	switch (mOrders[0]->GetType()) {
	case OT_NONE:
	case OT_COLONIZE:
	case OT_MERGE:
	case OT_TRANSFER:
	case OT_TRANSPORT:
		mOrders[0]->SetType(OT_NONE);
		break;
	case OT_REMOTEMINE:
		break;
	case OT_SCRAP:
		break;
	case OT_ROUTE:
		if (mOrders.size() < 2)
			SetNextRoute(InOrbit());

		mOrders[0]->SetType(OT_NONE);
		break;
	case OT_LAYMINE:
		// decrement # of years
		break;
	case OT_PATROL:
		Patrol();
		break;
	default:
		break;
	}
}

void Fleet::RemoteTerraform(bool bomb)
{
	Planet * planet = InOrbit();
	if (planet != NULL &&
		planet->GetOwner() != NULL &&
		(planet->GetOwner()->GetRelations(GetOwner()) > PR_NEUTRAL || planet->GetBaseDesign() < 0))
	{
		planet->RemoteTerraform(this, bomb);
	}
}

void Fleet::Repair()
{
	if (mAlreadyFought || mRepairRate == 0)
		return;

	double rate = Rules::GetFloat("RepairMoved", 0.01);	// rate for moving
	if (mRepairRate == 2) {
		rate = Rules::GetFloat("RepairStill", 0.02);	// sitting still
		Planet * p = InOrbit();
		if (p != NULL) {
			rate = Rules::GetFloat("RepairOrbit", 0.03);	// while in orbit
			if (GetOwner() == p->GetOwner()) {
				rate = Rules::GetFloat("RepairOrbitOwned", 0.05);	// of your own planet
				if (p->GetBaseDesign() != NULL)
					rate = p->GetBaseDesign()->GetRepairRate();	// with a base
			}
		}
	}

	// Apply IS bonus
	rate *= GetOwner()->RepairFactor();

	// Add Fuel transport bonus
	rate += GetMaxRepairRate();

	long repair;
	for (int i = 0; i < mStacks.size(); ++i) {
		if (mStacks[i].GetDamage() > 0) {
			repair = max(1L, long(mStacks[i].GetDesign()->GetArmor(GetOwner()) * rate + .5));
			mStacks[i].SetDamage(max(0L, mStacks[i].GetDamage() - repair));
		}
	}
}

void Fleet::SweepMines()
{
	int sweep = GetSweeping();

	if (sweep <= 0)
		return;

	mGame->SweepMines(this, sweep, mOwner->GetBattlePlan(mBattlePlan));
}

void Fleet::LayMines()
{
	const WayOrder * wo = GetFirstOrder();
	if (wo == NULL || wo->GetType() != OT_LAYMINE)
		return;

	bool doublemines = GetOwner()->MoveAndLayMines();
	if (mStartPos != *this) {
		if (doublemines)
			doublemines = false;
		else
			return;
	}

	int mines;
	int i;
	for (i = 0; i < Rules::MaxMineType; ++i) {
		mines = GetMineAmount(i);
		if (mines == 0)
			continue;

		if (doublemines)
			mines *= 2;

		NCGetOwner()->LayMines(this, i, mines);
	}
}

void Fleet::Transfer()
{
	const WayOrderNumber * wo = dynamic_cast<const WayOrderNumber *>(GetFirstOrder());
	if (wo == NULL || wo->GetType() != OT_TRANSFER)
		return;
}

void Fleet::Patrol()
{
	const WayOrderPatrol * wo = dynamic_cast<const WayOrderPatrol *>(GetFirstOrder());
	if (wo == NULL || wo->GetType() != OT_PATROL)
		return;

	if (mOrders.size() > 1)	// don't go on patrol if other orders still pending
		return;

	SpaceObject * target = mGame->GetPatrolTarget(this, wo->GetPatrolRange());
	if (target == NULL)
		return;

	long speed = wo->GetPatrolSpeed();
	if (speed < 1)
		speed = this->GetBestSpeed(this, target, OT_PATROL);

	WayOrder * wonew = new WayOrderPatrol(speed, wo->GetPatrolRange(), target);
	wonew->SetType(OT_PATROL);
	wonew->SetSpeed(speed);
	mOrders.push_back(wonew);

	if (target != NULL)
		target->AddChaser(this);

	if (mRepeatOrders) {
		if (InOrbit())
			wonew = new WayOrderPatrol(speed, wo->GetPatrolRange(), InOrbit());
		else
			wonew = new WayOrderPatrol(speed, wo->GetPatrolRange(), new Location(*this), true);

		wonew->SetType(OT_PATROL);
		wonew->SetSpeed(speed);
		mOrders.push_back(wonew);
	}
}

void Fleet::ProcessTransport(bool load, bool dunnage)
{
	if (mOrders.size() == 0 || mOrders[0]->GetType() != OT_TRANSPORT)
		return;

	WayOrderTransport * order = dynamic_cast<WayOrderTransport *>(mOrders[0]);
	if (!IsWith(*order->GetLocation()))
		return;

	CargoHolder * dest = dynamic_cast<CargoHolder *>(order->NCGetLocation());
	if (dest == NULL)
		return;

	for (int i = FUEL; i < Rules::MaxMinType; ++i) {
		if (load)
			ProcessLoad(dest, i, order->GetAction(i), order->GetValue(i), dunnage);
		else
			ProcessUnload(dest, i, order->GetAction(i), order->GetValue(i));
	}
}

void Fleet::Colonize()
{
	if (mOrders.size() == 0 || mOrders[0]->GetType() != OT_COLONIZE)
		return;

	if (!CanColonize()) {
		NCGetOwner()->AddMessage("Colonization failed - no colonization pod", this);
		return;
	}

	Planet * target = InOrbit();
	if (target == NULL) {
		NCGetOwner()->AddMessage("Colonization failed - not in orbit", this);
		return;
	}

	if (target->GetPopulation() > 0) {
		Message * mess = NCGetOwner()->AddMessage("Colonization failed - world occupied", this);
		mess->AddItem("World being colonized", target);
		mess->AddItem("Owner", target->GetOwner());
		return;
	}

	if (GetPopulation() <= 0) {
		Message * mess = NCGetOwner()->AddMessage("Colonization failed - no colonists", this);
		mess->AddItem("World being colonized", target);
		return;
	}

	target->Invade(NCGetOwner(), GetPopulation() + GetCost().GetCrew());
	Scrap(true);
}

void Fleet::CheckMerge()
{
	if (mOrders.size() == 0 || mOrders[0]->GetType() != OT_MERGE)
		return;

	Fleet * f = dynamic_cast<Fleet *>(mOrders[0]->NCGetLocation());
	if (f == NULL) {
		NCGetOwner()->AddMessage("Merge failed - destination is not a fleet", this);
		return;
	}

	if (f->GetOwner() != GetOwner()) {
		NCGetOwner()->AddMessage("Merge failed - destination fleet is not owned", this);
		return;
	}

	MergeTo(f);
}

void Fleet::Scrap(bool colonize)
{
	TechType TechGot = TECH_NONE;
	Salvage * salvage = NULL;
	Planet * planet = InOrbit();

	//Tech first:
	//Is ship in orbit with base
	if (planet && Rules::TechScrap(planet)) {
		// regular tech
		TechGot = Rules::TechFleet(planet->GetOwner(), this);

		///@todo MT tech from scrapping
	}

	// calc minerals
	if (planet) {
		double percent = Rules::ScrapRecover(planet, colonize);
		long tempMins = 0;
		for (int i = 0; i < Rules::MaxMinType; ++i)
		{
			long temp = GetContain(i);
			temp += long(GetCost()[i] * percent + .5);
			planet->AdjustAmounts(i, temp);
			tempMins += temp;
		}

		if (GetOwner() == planet->GetOwner()) {
			planet->AdjustAmounts(POPULATION, GetPopulation());
			planet->AdjustPopulation(GetCost().GetCrew());	// recover crew too
		}
	} else {
		Salvage * salvage = mGame->GetGalaxy()->AddSalvage(*this);
		double percent = Rules::ScrapRecover(NULL, false);
		for (int i = 0; i < Rules::MaxMinType; ++i) {
			long temp = GetContain(i);
			temp += long(GetCost()[i] * percent + .5);
			salvage->AdjustAmounts(i, temp);
		}
	}

	int URGain = 0;
	// calc resources
	if (!colonize && planet) {
		int percent = Rules::ScrapResource(planet);
		if (percent > 0) {
			long temp = GetCost().GetResources() * percent;
			planet->AddScrapRes(temp);
			URGain = (temp*planet->GetResources())/(temp+planet->GetResources());
		}
	}

	string str2;
	// send message to the fleet owner
	// - note that both the fleet owner and the planet owner get a message, even if they are the same player
//	if (!planet || planet->GetOwner() != GetOwner())	// add this line to eliminate a (mostly) duplicate message
	{
		// This is not the same as classic Stars!, that passes the fleet as part of the
		// message and if you reuse the fleet, it points at the new fleet - I didn't think that was important to preserve - EK
		str2 = GetName(GetOwner());
		Message * mess;
		if (colonize) {
			mess = NCGetOwner()->AddMessage("Colonize attempt", planet);
			mess->AddItem("Fleet name", GetName(GetOwner()));
		} else if (planet) {
			mess = NCGetOwner()->AddMessage("Scrap fleet at planet", planet);
			mess->AddItem("Fleet name", GetName(GetOwner()));
			mess->AddLong("UR resource gain", URGain);
		} else {
			mess = NCGetOwner()->AddMessage("Scrap fleet in space", salvage);
			mess->AddItem("Fleet name", GetName(GetOwner()));
		}
	}

	// send message to the planet owner and give tech
	if (!colonize && planet)
	{
		str2.erase();
		if (planet->GetOwner() != GetOwner()) {
			str2 = GetOwner()->GetSingleName();
			str2 += " ";
		}
		str2 += "Fleet #" + Long2String(mID);

		Message * mess;
		mess = planet->NCGetOwner()->AddMessage("Fleet scrapped at planet", planet);
		mess->AddItem("Fleet name", str2);
		mess->AddLong("UR resource gain", URGain);
		if (TechGot >= 0) {
			mess->AddItem("Tech field gained", Rules::GetTechName(TechGot));
			long res = planet->GetOwner()->TechCost(TechGot);
			mess->AddLong("Tech resources gained", res);
			planet->NCGetOwner()->SetGotTech(true);
			planet->NCGetOwner()->GainTech(TechGot, res);
		}
	}

	KillFleet();
}

void Fleet::KillFleet()
{
	GoingAwayNotifyChasers();
	NCGetOwner()->DeleteFleet(this);
}

void Fleet::FreighterReproduction()
{
	assert(GetOwner()->FreighterReproduction() > epsilon);

	if (GetPopulation() <= 0)
		return;

	long Grow = long(GetPopulation() * GetOwner()->FreighterReproduction() * GetOwner()->GrowthRate()) / Rules::PopEQ1kT;
	long Over = 0;
	Planet * p = NULL;

	if (GetCargoMass() + Grow > GetCargoCapacity())
		Over = (GetCargoCapacity() - GetCargoMass()) * Rules::PopEQ1kT;

	Grow = Grow * Rules::PopEQ1kT - Over;
	AdjustPopulation(Grow);
	if (Over) {
		p = InOrbit();
		if (p != NULL && p->GetOwner() == GetOwner())
			p->AdjustPopulation(Over);
		else
			Over = 0;	// if not in orbit of an owned world, overflow is lost
	}

	if (Grow > 0 || Over > 0) {
		Message * mess = NCGetOwner()->AddMessage("Freighter growth", this);
		mess->AddLong("Growth", Grow);
		if (Over > 0) {
			mess->AddLong("Overflow amount", Over);
			mess->AddItem("Overflow world", p);
		}
	}
}

///@retval true Fleet isn't done moving yet (following a fleet that hasn't finished moving)
///@retval false Fleet has finished moving.
bool Fleet::Move()
{
	if (mHasMoved)	// should never happen
		return false; //@todo Log a debug message here -- might be useful if it does ever happen.

	if (mOrders.size() <= 1 || IsWith(*mOrders[1]->GetLocation()) || mOrders[1]->GetSpeed() == 0) {
		mHasMoved = true;
		return false;
	}

	const Planet * destp;
	const SpaceObject * destso;
	destso = dynamic_cast<const SpaceObject *>(mOrders[1]->GetLocation());
	destp = dynamic_cast<const Planet *>(destso);

	if (destp == NULL && destso != NULL && !destso->SeenBy(GetOwner())) {
		// you can target an unseen planet
		NCGetOwner()->AddMessage("Error: Targeting unseen", this);
		return false;
	}

	if (mOrders[1]->GetSpeed() > 0 && Randodd(GetOwner()->EngineFailure(mOrders[1]->GetSpeed()))) {
		Message * mess = NCGetOwner()->AddMessage("Engines failed");
		mess->AddItem("", this);
		mHasMoved = true;
		return false;
	}

	// the only true return from this function
	if (mChasing && !mChasing->mHasMoved)
		return true;

	// is fleet gating?
	if (mOrders[1]->GetSpeed() == -1) {
		mHasMoved = true;
		const Component * send = NULL;
		const Component * recieve = NULL;
		Planet * p;

		// check recieving gate first
		if (destp == NULL) {
			// you can gate to one of your fleets, as long as it's at a gate
			Fleet * f = dynamic_cast<Fleet *>(mOrders[1]);
			if (f != NULL && f->GetOwner() == GetOwner())
				destp = f->InOrbit();
		}

		if (destp != NULL && destp->GetBaseNumber() >= 0 && destp->GetOwner()->GetRelations(GetOwner()) >= PR_FRIEND)
			recieve = destp->GetBaseDesign()->GetGate();

		p = InOrbit();
		if (p != NULL && p->GetBaseNumber() >= 0 && p->GetOwner()->GetRelations(GetOwner()) >= PR_FRIEND)
			send = p->GetBaseDesign()->GetGate();

		if (send == NULL && CanJumpGate())
			send = recieve;
		else if (send && recieve && GetCargoMass() > 0 && !GetOwner()->GateCargo()) {
			// Unload
			if (send && recieve && p->GetOwner() == GetOwner()) {
				Message * mess = NCGetOwner()->AddMessage("Can't gate cargo - cargo dropped");
				mess->AddItem("", this);
				mess->AddItem("Cargo dropped on", p);
				for (int i = 0; i < Rules::MaxMinType; ++i) {
					p->AdjustAmounts(i, GetContain(i));
					AdjustAmounts(i, -GetContain(i));
				}

				p->AdjustAmounts(POPULATION, GetPopulation());
				AdjustAmounts(POPULATION, -GetPopulation());
			} else {
				Message * mess = NCGetOwner()->AddMessage("Can't gate cargo - gate aborted");
				mess->AddItem("", this);
				return false;
			}
		}

		if (send == NULL) {
			Message * mess = NCGetOwner()->AddMessage("No sending gate");
			mess->AddItem("", this);
			if (p != NULL)
				mess->AddItem("Sending", p);
			return false;
		}

		if (recieve == NULL) {
			Message * mess = NCGetOwner()->AddMessage("No recieving gate");
			mess->AddItem("", this);
			mess->AddItem("Destination", mOrders[1]->GetLocation());
			return false;
		}

		// range overgate damage
		double rdam = Rules::OverGateRange(send->GetGateRange(), long(Distance(destp)));
		if (rdam > 1.0 - epsilon) {
			Message * mess = NCGetOwner()->AddMessage("Gate too far");
			mess->AddItem("", this);
			mess->AddItem("Destination", destp);
			return false;
		}

		// mass overgate damage, total damage, and void chance
		double mdam, tdam, vodds;
		bool massOK = true;
		bool checkdam = rdam > epsilon;
		for (int i = 0; i < mStacks.size(); ++i) {
			mdam = Rules::OverGateMass(send->GetGateMass(), recieve->GetGateMass(), mStacks[i].GetDesign()->GetMass());
			if (mdam > 1.0 - epsilon)
				massOK = false;
			else if (mdam > epsilon)
				checkdam = true;
		}

		if (!massOK) {
			Message * mess = NCGetOwner()->AddMessage("Ships too massive for gate");
			mess->AddItem("", this);
			mess->AddItem("Destination", destp);
			return false;
		}

		if (checkdam) {
			long adam;	// armor damage
			Message * mess = NCGetOwner()->AddMessage("Ships damaged by overgating");
			mess->AddItem("", this);
			for (int i = 0; i < mStacks.size(); ++i) {
				mdam = Rules::OverGateMass(send->GetGateMass(), recieve->GetGateMass(), mStacks[i].GetDesign()->GetMass());
				tdam = min(0.98, mdam + rdam - mdam * rdam);
				vodds = (1.0 - (1.0 - rdam) * (1.0 - tdam)) / 3.0 * GetOwner()->OvergateLossFactor();
				adam = long(tdam * mStacks[i].GetDesign()->GetArmor(GetOwner()) + .5);
				long lost = mStacks[i].DamageAllShips(adam);
				if (mStacks[i].GetCount() > 0 && vodds > epsilon) {
					// any survivors face the void
					for (int j = 0; j < mStacks[i].GetCount(); ++j) {
						if (Randodd(vodds))
							lost++;
					}
				}
				if (lost > 0) {
					mess->AddItem("Ships lost name", mStacks[i].GetDesign()->GetName());
					mess->AddLong("Ships lost number", lost);
					if (mStacks[i].KillShips(lost, false, mGame->GetGalaxy())) {
						mStacks.erase(mStacks.begin()+i);
						i--;	// adjust loop counter if the whole stack is gone
					}
					ResetDefaults();
				}
			}

			if (GetShipCount() == 0) {
				KillFleet();
				return false;
			}
		}

		// fleet arrives
		SetLocation(*destp);
		mRepairRate = 0;
		MoveArrive();
		return false;
	}	// if gating

	double dist = Distance(mOrders[1]->GetLocation());
	// find the min speed needed to get to go the distance (MM reduction & mine field hit odds)
	long speed = mOrders[1]->GetSpeed();
	speed--;
	while (dist < speed * speed)
		speed--;
	speed++;

	Location dest = *mOrders[1]->GetLocation();
	if (long(dist) > speed * speed) {
		MoveToward(this, &dest, &mPX, &mPY, speed * speed);
		dist = Distance(mPX, mPY);
	} else {
		mPX = dest.GetPosX() + epsilon;
		mPY = dest.GetPosY() + epsilon;
	}

	double fuelU = GetFuelUsage(speed);
	if (GetFuel() < long(fuelU * dist +.5)) {
		double fdist = GetFuel() / fuelU;
		if (int(fdist) < int(dist)) {
			mOrders[1]->SetSpeed(GetMinFreeSpeed());	// for next turn
			Message * mess = NCGetOwner()->AddMessage("Out of fuel, slowing down");
			mess->AddItem("", this);
			MoveToward(this, &dest, &mPX, &mPY, long(fdist + .5));
			dist = Distance(mPX, mPY);
		}
	}

	mRepairRate = 1;
	double safe;
	mPossibleMines.clear();
	safe = mGame->GetPossibleMines(&mPossibleMines, this, dist);
	if (safe > dist || mPossibleMines.size() == 0) {
		SetLocation(long(mPX), long(mPY));
		AdjustFuel(-long(fuelU * Distance(&mStartPos) + .5));
		mHasMoved = true;
		if (IsWith(*(mOrders[1]->GetLocation()))) {
			Wormhole * wh = dynamic_cast<Wormhole *>(mOrders[1]->NCGetLocation());
			if (wh != NULL && wh->GetAttached() != NULL) {
				SetLocation(*wh->GetAttached());
				wh->Enter(mOwner->GetID());
				wh->GetAttached()->Exit(mOwner->GetID());
			}
		}

		MoveArrive();
		return false;
	} else {
		while (safe > 2.0 && dest.Distance(mPX, mPY) > epsilon) {
			MoveToward(&mStartPos, &dest, &mPX, &mPY, long(safe));	// move closer
			safe = ReClosestMinefield(dest.Distance(mPX, mPY));
		}

		if (dest.Distance(mPX, mPY) > epsilon) {
			while (!Move1LY(&dest, speed))
				;
		}

		SetLocation(long(mPX), long(mPY));
		AdjustFuel(long(fuelU * Distance(&mStartPos) + .5));

		MoveArrive();
		return false;
	}
}

/*!
 * Called after a fleet arrives at its destination.
 * Handles some corner cases like repeating orders and wormholes.
 */
void Fleet::MoveArrive()
{

	mHasMoved = true;
	if (mOrders.size() <= 1) {
		assert(false);
		return;
	}

	WayOrder * wo = mOrders[1];
	if (!IsWith(*wo->GetLocation())) {
		mGame->MoveAlsoHere(this);
		return;	// could happen if movement stopped for some reason (mine hits), or didn't reach destination
	}

	if (mRepeatOrders) {
		WayOrder * wonew = new WayOrder(*wo);
		mOrders.push_back(wonew);
	}

	delete mOrders[0];
	mOrders.pop_front();

	Wormhole * wh = dynamic_cast<Wormhole *>(wo->NCGetLocation());
	if (wh != NULL && wh->GetAttached() != NULL) {
		SetLocation(*wh->GetAttached());
		wh->Enter(mOwner->GetID());
		wh->GetAttached()->Exit(mOwner->GetID());
	}

	mGame->MoveAlsoHere(this);
}

double Fleet::ReClosestMinefield(double dist)
{
	///@todo OPTIMIZE: don't recalc this every step, just every 10ly or so

	double Result = mGame->GetGalaxy()->MaxX() + mGame->GetGalaxy()->MaxY();

	int i;
	double d;
	for (i = 0; i < mPossibleMines.size(); ++i) {
		d = Distance(mPossibleMines[i]) - mPossibleMines[i]->GetRadius();
		if (d < Result)
			Result = d;

		if (d > dist) {
			mPossibleMines.erase(mPossibleMines.begin() + i);
			--i;
		}
	}

	return Result;
}

MineField * Fleet::CheckMineHits(int speed)
{
	int maxMineType = Rules::MaxMineType - 1;
	MineField * Result = NULL;
	deque<int> counts;
	deque<bool> hitcheck;
	counts.insert(counts.begin(), Rules::MaxMineType, 0);
	int id;
	int i;

	bool any = false;
	for (i = 0; i < Rules::MaxMineType; ++i)
		if (Randodd(Rules::GetMineFieldType(i)->GetHitOdds(speed))) {
			hitcheck.push_back(true);
			any = true;
		} else
			hitcheck.push_back(false);

	if (!any)
		return NULL;

	for (i = 0; i < mPossibleMines.size(); ++i) {
		if (Distance(mPossibleMines[i]) > mPossibleMines[i]->GetRadius())
			continue;

		id = mPossibleMines[i]->GetType()->GetID();
		if (!hitcheck[id] || id > maxMineType)
			continue;

		counts[id] += 1;
		if (Random(counts[id]) == 0) {
			maxMineType = id;
			Result = mPossibleMines[i];
		}
	}

	return Result;
}

/*!
 * @param speed Our travel speed, for minefield collision checks.
 * @param dest Our destination.
 * @retval true "Continue to travel."
 * @retval false "Halt! We hit a minefield or arrived at our destination."
 */
bool Fleet::Move1LY(Location * dest, long speed)
{
	mRepairRate = 1;
	mDistMoved++;
	MoveToward(&mStartPos, dest, &mPX, &mPY, mDistMoved);

	MineField * mf = CheckMineHits(speed);
	if (mf != NULL) {
		TakeMinefieldDamage(mf);
		mf->ReduceFieldCollision();
		return false;
	}

	if (dest->Distance(mPX, mPY) > epsilon)
		return true;
	else
		return false;
}

bool Fleet::CanLoadBy(const Player * player) const
{
	return mCanLoadBy[player->GetID() - 1];
}

void Fleet::SetCanLoadBy(const Player * player)
{
	mCanLoadBy[player->GetID()-1] = true;
}

bool Fleet::HasHull(HullType ht) const
{
	deque<Stack>::const_iterator iter;
	for (iter = mStacks.begin(); iter != mStacks.end(); ++iter) {
		if (iter->GetDesign()->GetHull()->GetHullType() & ht)
			return true;
	}

	return false;
}

long Fleet::GetCloak(const Player *, bool) const
{
	return Rules::CloakValue(GetCloaking(), GetMass() + (GetOwner()->CloakCargo() ? 0 : GetCargoMass()));
}

long Fleet::GetFuelNeeded() const	// fuel needed to make it to the next waypoint (waypoint 1)
{
	if (mOrders.size() < 1)
		return 0;
	else
		return long(GetFuelUsage(mOrders[1]->GetSpeed()) * Distance(mOrders[1]->GetLocation()) + .5);
}

void Fleet::AdjustFuel(long amount)
{
	mFuel += amount;
	if (mFuel < 0)
		mFuel = 0;

	if (mFuel > GetFuelCapacity())
		mFuel = GetFuelCapacity();
}

void Fleet::Refuel(const Planet * p)
{
	///@bug Shouldn't this check if fleet is at the location of the base?
	///@note Should probabaly change return type to bool...
	if (p && p->GetBaseNumber() >= 0 && p->GetBaseDesign()->CanRefuel() && p->GetOwner()->GetRelations(GetOwner()) >= PR_FRIEND)
		mFuel = GetFuelCapacity();
}

double Fleet::GetFuelUsage(long speed) const
{
	double MinUsage = -1;
	double MinUsageDone = 0;
	long MinUsageCapacity = 0;
	double Result = 0;
	long CargoLeft = GetCargoMass();

	// first calc everything where we know it's cargo
	deque<Stack>::const_iterator iter;
	for (iter = mStacks.begin(); iter != mStacks.end(); ++iter) {
		if (iter->GetDesign()->GetFreeSpeed() >= speed) {
			// ships going at free speed or less don't care about mass, will carry maximum cargo
			Result += iter->GetDesign()->GetFuelUsage(speed) * iter->GetCount();
			if (CargoLeft > 0)
				CargoLeft -= min(CargoLeft, iter->GetDesign()->GetCargoCapacity() * iter->GetCount());
		} else if (GetCargoMass() == GetCargoCapacity()) {
			// if fleet is packed full, everything goes with full loads
			Result += iter->GetDesign()->GetFuelUsage(speed) *
					iter->GetCount() *
					(iter->GetDesign()->GetMass() + iter->GetDesign()->GetCargoCapacity()) *
					GetOwner()->FuelFactor();
			CargoLeft -= iter->GetDesign()->GetCargoCapacity() * iter->GetCount();
		} else {
			// potentially partially full ships, calc with empty load, add fuel for cargo later
			Result += iter->GetDesign()->GetFuelUsage(speed) *
					iter->GetCount() *
					iter->GetDesign()->GetMass() *
					GetOwner()->FuelFactor();
		}
	}

	// pay for any left over cargo
	while (CargoLeft > 0) {
		for (iter = mStacks.begin(); iter != mStacks.end(); ++iter) {
			if (iter->GetDesign()->GetFreeSpeed() <= speed)
				;	// already done
			else if (iter->GetDesign()->GetCargoCapacity() == 0)
				;	// can't carry cargo, fuel for hull already done
			else {
				double usage = iter->GetDesign()->GetFuelUsage(speed);
				if (usage > MinUsageDone && (MinUsage == -1 || usage < MinUsage)) {
					MinUsage = usage;
					MinUsageCapacity = iter->GetDesign()->GetCargoCapacity() * iter->GetCount();
				}
			}
		}

		Result += MinUsage * MinUsageCapacity * GetOwner()->FuelFactor();
		CargoLeft -= min(CargoLeft, MinUsageCapacity);
		MinUsageDone = MinUsage;
		MinUsage = -1;
	}

	return Result / 200.0;
}

void Fleet::MergeTo(Fleet * to)
{
	// merge ships
//	to->AddChaser(mChasers);
	deque<Stack>::iterator iter;
	for (iter = mStacks.begin(); iter != mStacks.end(); ++iter) {
		to->Merge(*iter, iter->GetCount(), iter->GetDamaged(), mID);
		mStacks.erase(iter);
	}

	// move cargo and fuel
	for (int i = FUEL; i < Rules::MaxMinType; ++i) {
		to->AdjustAmounts(i, GetContain(i));
		AdjustAmounts(i, -GetContain(i));
	}
}

void Fleet::MergeTo(Fleet * to, const Ship * design, long number, long damaged)
{
	Stack* stack = NULL; // stack to be merged
	for (deque<Stack>::iterator iter = mStacks.begin(); iter != mStacks.end(); ++iter)
	{
		if (design == iter->GetDesign())
		{
			stack  = &(*iter);
			break;
		}
	}

	if (stack == NULL)
	{
		Message * mess = NCGetOwner()->AddMessage("Error: Split or merge a design not in the fleet", this);
		mess->AddItem("Ship design", design->GetName());
		return;
	}

	if (number > stack->GetCount()) {
		Message * mess = NCGetOwner()->AddMessage("Error: Split or merge too many ships", this);
		mess->AddItem("Ship design", design->GetName());
		mess->AddLong("Attempted transfer", number);
		number = stack->GetCount();
		mess->AddLong("Actual transfer", number);
	}

	if (damaged > stack->GetDamaged()) {
		Message * mess = NCGetOwner()->AddMessage("Error: Split or merge too many damaged ships", this);
		mess->AddItem("Ship design", design->GetName());
		mess->AddLong("Attempted transfer", damaged);
		damaged = stack->GetDamaged();
		mess->AddLong("Actual transfer", damaged);
	}

	// 20 ships, 15 damaged, transfer 8, damaged needs to be at least 3
	if (damaged < stack->GetDamaged() + number - stack->GetCount()) {
		Message * mess = NCGetOwner()->AddMessage("Error: Split or merge not enough damaged ships", this);
		mess->AddItem("Ship design", design->GetName());
		mess->AddLong("Attempted transfer", damaged);
		damaged = stack->GetDamaged() + number - stack->GetCount();
		mess->AddLong("Actual transfer", damaged);
	}

	// original capacity of fleet
	long original_capacity = GetCargoCapacity();

	// perform merge
	to->Merge(*stack, number, damaged, mID);
//	to->AddChaser(mChasers);

	// handle cargo
	long capacity = stack->GetDesign()->GetCargoCapacity() * number;
	double ratio = double(capacity)/double(original_capacity);
	long amt;
	if (capacity > 0)
	{
		// Population
		amt = GetContain(POPULATION);
		if(amt != 0)
		{
			// Only in Multiples of PopEQ1kT;
			amt = long(amt * ratio) / Rules::PopEQ1kT;
			amt *= Rules::PopEQ1kT;

			to->AdjustAmounts(POPULATION, amt);
			AdjustAmounts(POPULATION, -amt);
		}

		// Minerals
		for (int i = 0; i < Rules::MaxMinType; ++i)
		{
			amt = GetContain(i);
			if(amt != 0)
			{
				amt = long(amt * ratio);

				to->AdjustAmounts(i, amt);
				AdjustAmounts(i, -amt);
			}
		}
	}

	// handle fuel
	original_capacity = GetFuelCapacity();
	capacity = stack->GetDesign()->GetFuelCapacity() * number;
	ratio = double(capacity)/double(original_capacity);
	amt = long(GetFuel() * ratio);
	to->AdjustAmounts(FUEL, amt);
	AdjustAmounts(FUEL, -amt);

	ResetDefaults();

	stack->SetCount(stack->GetCount() - number);
	stack->SetDamaged(stack->GetDamaged() - damaged);
	if (stack->GetCount() <= 0)
		remove_if(mStacks.begin(),mStacks.end(),bind2nd(equal_to<Stack>(),*stack));
}

void Fleet::Merge(Stack &stack, long number, long damaged, long Origin)
{
	ResetDefaults();
	deque<Stack>::iterator iter;
	for (iter = mStacks.begin(); iter != mStacks.end(); ++iter) {
		if (iter->GetDesign() == stack.GetDesign())
			break;
	}

	if (iter == mStacks.end()) {
		iter = mStacks.insert(iter, Stack());
		iter->SetDesign(stack.GetDesign());
	}

	// damage
	long newdamage = 0;
	long Dcount = iter->GetDamaged() + damaged;
	if (Dcount > 0)
		newdamage = (iter->GetDamaged() * iter->GetDamage() + damaged * stack.GetDamage() + Dcount/2) / Dcount;

	iter->SetCount(iter->GetCount() + number);
	iter->SetDamaged(iter->GetDamaged() + damaged);
	iter->SetDamage(newdamage);

	// the idea is that each stack of ships will contain an array holding the number of ships that came from which fleet
	deque<Stack::Origin>::iterator i2;
	long nleft = number;
	long dleft = damaged;
	for (i2 = stack.mOrigins.begin(); i2 != stack.mOrigins.end(); ++i2) {
		long n = long(0.5 + double(i2->ships) * number / stack.GetCount());
		long d = long(0.5 + double(i2->damaged) * damaged / stack.GetDamaged());
		if (n < d)
			n = d;

		iter->AddFromFleet(i2->fleet, n, d);
		i2->ships -= n;
		i2->damaged -= n;
		nleft -= n;
		dleft -= d;
	}

	iter->AddFromFleet(Origin, nleft, dleft);
}

void Fleet::AddShips(const Ship * design, long number)
{
	deque<Stack>::iterator iter;
	for (iter = mStacks.begin(); iter != mStacks.end(); ++iter) {
		if (iter->GetDesign() == design)
			break;
	}

	if (iter == mStacks.end()) {
		iter = mStacks.insert(iter, Stack());
		iter->SetDesign(design);
	}

	iter->SetCount(iter->GetCount() + number);
}

void Fleet::AddShips(long Type, long number)
{
	AddShips(GetOwner()->GetShipDesign(Type), number);
}

void Fleet::ResetSeen()
{
	CargoHolder::ResetSeen();

	mCanLoadBy.clear();
	mCanLoadBy.insert(mCanLoadBy.begin(), mGame->NumberPlayers(), false);
}

void Fleet::SetSeenBy(long p, long seen)
{
	mSeenBy[p] = seen;

	SetSeenDesign(p, seen != 0, mGame->GetPlayer(p+1)->ScanDesign());
}

void Fleet::SetSeenDesign(long p, bool seen, bool design)
{
	deque<Stack>::iterator iter;
	for (iter = mStacks.begin(); iter != mStacks.end(); ++iter) {
		if (design)
			const_cast<Ship *>(iter->GetDesign())->SetSeenDesign(p, seen);
		else
			const_cast<Ship *>(iter->GetDesign())->SetSeenHull(p, seen);
	}
}

void Fleet::ChangeWaypoints(WayOrderList & wol)
{
	if (GetOwner()->WriteXFile())
		NCGetOwner()->AddOrder(new WaypointOrder(GetID(), &mOrders));
	else {
		for (int i = 0; i < mOrders.size(); ++i)
			delete mOrders[i];
	}

	wol.SetNoDelete();
	mOrders = wol.GetOrders();
}

void Fleet::SetRepeat(bool repeat)
{
	if (repeat != mRepeatOrders && GetOwner()->WriteXFile())
		NCGetOwner()->AddOrder(new TypedOrder<bool>(&mRepeatOrders, AddBool, "Repeat", "Fleet", Long2String(GetID()).c_str()));

	mRepeatOrders = repeat;
}

void Fleet::SetBattlePlan(long bp)
{
	if (bp != mBattlePlan && GetOwner()->WriteXFile())
		NCGetOwner()->AddOrder(new TypedOrder<long>(&mBattlePlan, AddLong, "BattlePlan"));

	mBattlePlan = bp;
}

void Fleet::SetStartOrders(Planet * planet)
{
	WayOrder * wo = new WayOrder(planet);
	mOrders.insert(mOrders.begin(), wo);
	SetNextRoute(planet);
}

void Fleet::SetNextRoute(Planet * planet)
{
	if (planet && planet->GetRoute() != NULL) {
		WayOrder * wo = new WayOrder(const_cast<Planet *>(planet->GetRoute()));
		wo->SetType(OT_ROUTE);
		wo->SetSpeed(GetBestSpeed(planet, planet->GetRoute(), OT_ROUTE));
		mOrders.push_back(wo);
	}
}

long Fleet::GetBestSpeed(const Location * L1, const Location * L2, OrderType ot)
{
	long dist = long(L1->Distance(L2));

	const Planet * p1 = dynamic_cast<const Planet *>(L1);
	const Planet * p2 = dynamic_cast<const Planet *>(L2);

	bool oneway = (ot == OT_COLONIZE) || CanColonize();

	if (p2 && p2->GetBaseNumber() >= 0) {
		if (p2->GetBaseDesign()->CanRefuel())
			oneway = true;

		if (GetCargoMass() == 0 || GetOwner()->GateCargo()) {
			if (p1 && p1->GetBaseNumber() >= 0) {
				const Component * gate = p1->GetBaseDesign()->GetGate();
				if (gate->GetGateMass() >= GetMaxMass() && gate->GetRange() >= dist) {
					gate = p2->GetBaseDesign()->GetGate();
					if (gate->GetGateMass() >= GetMaxMass())
						return -1;	// gate
				}
			}
		}
	}

	long speed;
	double fu;

	// use battle speed as minimum speed, battle speed is normally the highest speed at which the engine is safe at 120% fuel efficiency
	for (speed = GetMinSafeSpeed(); speed > GetMinBattleSpeed(); --speed) {
		fu = GetFuelUsage(speed) * dist * (oneway ? 1 : 2);
		if (fu <= GetFuel())
			break;
	}

	return speed;
}

void Fleet::ChaseeGone(SpaceObject * chasee)
{
	Planet * planet = chasee->InOrbit();
	Message * mess = NCGetOwner()->AddMessage("Chasee has vanished", chasee);
	if (planet == NULL)
		mess->AddItem("Last location", new Location(*chasee), true);
	else
		mess->AddItem("Last location", planet);

	int i;
	for (i = 0; i < mOrders.size(); ++i) {
		if (mOrders[i]->GetLocation() == chasee) {
			if (planet == NULL)
				mOrders[i]->SetLocation(new Location(*chasee), true);
			else
				mOrders[i]->SetLocation(planet);
		}
	}
}

int Fleet::GetOwnerID() const
{
	return mOwner->GetID();
}

/*
void Fleet::AddChaser(unsigned long p, unsigned long f)
{
	assert(p != GetOwner()->GetID());
	deque<Chaser>::iterator iter;
	for (iter = mChasers.begin(); iter != mChasers.end(); ++iter)
		if (iter->player == p && iter->fleet == f)
			break;

	if (iter == mChasers.end()) {
		iter = mChasers.insert(mChasers.end(), Chaser());
		iter->fleet = f;
		iter->player = p;
	}
}

void Fleet::AddChaser(const deque<Chaser> &w2)
{
	deque<Chaser>::const_iterator iter;
	for (iter = w2.begin(); iter != w2.end(); ++iter)
		AddChaser(iter->player, iter->fleet);
}

bool Fleet::ChasedBy(const Player * p)
{
	deque<Chaser>::const_iterator iter;
	for (iter = mChasers.begin(); iter != mChasers.end(); ++iter) {
		if (iter->player == p->GetID())
			return true;
	}

	return false;
}
*/
void Fleet::TakeMinefieldDamage(const MineField * field)
{
	int dpse;	// damage per standard engine
	int dpre;	// damage per ram scoop engine
	int temp;
	long ReportedDamage = 0;
	long ReportedKills = 0;

	mRepairRate = 1;	// fleets that hit mine fields don't repair

	dpse = field->GetType()->GetFleetDamage();
	temp = field->GetType()->GetShipDamage();
	if (dpse < temp * GetShipCount())
		dpse = temp * GetShipCount();

	dpre = field->GetType()->GetRamFleetDamage();
	temp = field->GetType()->GetRamShipDamage();
	if (dpre < temp * GetShipCount())
		dpre = temp * GetShipCount();

	if (dpse > 0 || dpre > 0) {
		for (int i = 0; i < mStacks.size(); ++i) {
			if (mStacks[i].GetDesign()->GetFreeSpeed() > 1)
				temp = dpre;
			else
				temp = dpse;

			temp *= mStacks[i].GetDesign()->GetEngines();
			ReportedDamage += temp * mStacks[i].GetCount();
			temp -= min(mStacks[i].GetDesign()->GetShield(GetOwner()), long((temp + 1) / 2));
			long lost = mStacks[i].DamageAllShips(temp);
			ReportedKills += lost;
			if (mStacks[i].KillShips(lost, true, mGame->GetGalaxy())) {
				mStacks.erase(mStacks.begin()+i);
				i--;	// adjust loop counter if the whole stack is gone
			}
			ResetDefaults();
		}
	}

	Message * mess;
	mess = NCGetOwner()->AddMessage("Your fleet hit mine");
	mess->AddItem("", this);
	mess->AddLong("Damage", ReportedDamage);
	mess->AddLong("Kills", ReportedKills);
	if (ReportedKills > 0)
		mess->AddItem("Salvage location", new Location(*this), true);
	mess = field->NCGetOwner()->AddMessage("Fleet hit your mine");
	mess->AddItem("", field);
	mess->AddLong("Damage", ReportedDamage);
	mess->AddLong("Kills", ReportedKills);
	if (ReportedKills > 0)
		mess->AddItem("Salvage location", new Location(*this), true);
}

void Fleet::RemoteMine(bool ARmining)
{
	if (mOrders.size() < 1 || mOrders[0]->GetType() != OT_REMOTEMINE)
		return;

	if (!ARmining && mStartPos != *this)	// Remote mining doesn't happen the same turn a fleet moves
		return;

	Planet * planet = InOrbit();
	if (planet == NULL)	// Attempt to remote mine space, fleet is probably enroute, ignore
		return;

	if (planet->GetOwner() != NULL) {
		if (GetOwner()->ARTechType() >= 0 && GetOwner() == planet->GetOwner()) {
			if (!ARmining)
				return;	// AR remote mining his own world, done previously, ignore silently
		} else {
			// Warning message about remote mining an occupied world
			Message * mess;
			mess = NCGetOwner()->AddMessage("Warning: Remote mining an owned world", this);
			mess->AddItem("", planet);
			mess->AddItem("Owner", planet->GetOwner());
			return;
		}
	}

	if (GetMines() == 0) {
		// Warning message about remote mining orders without a remote mining capable fleet
		Message * mess;
		mess = NCGetOwner()->AddMessage("Warning: Remote mining without mining ability", this);
		mess->AddItem("", planet);
		return;
	}

	planet->Mine(min(GetMines(), Rules::GetConstant("MaxRemoteMining")), GetOwner());
}

long Fleet::GetShipCount() const
{
	long Result = 0;
	deque<Stack>::const_iterator iter;
	for (iter = mStacks.begin(); iter != mStacks.end(); ++iter)
		Result += iter->GetCount();

	return Result;
}

long Fleet::TechLevel(TechType tech) const
{
	if (tech >= Rules::MaxTechType || CVTechLevel[tech] == -1) {
		// loop through ships
		long Level = 0;
		deque<Stack>::const_iterator iter;

		if (tech < Rules::MaxTechType) {
			for (iter = mStacks.begin(); iter != mStacks.end(); ++iter) {
				Level = max(Level, iter->GetDesign()->TechLevel(tech));
			}
		} else {
			for (iter = mStacks.begin(); iter != mStacks.end(); ++iter)
				Level += iter->GetDesign()->TechLevel(tech);
		}
		if (tech < Rules::MaxTechType)
			const_cast<Fleet *>(this)->CVTechLevel[tech] = Level;

		return Level;
	} else
		return CVTechLevel[tech];
}

const Cost & Fleet::GetCost() const
{
	if (ReCost <= mOwner->GetLastTechGainPhase()) {
		const_cast<Fleet *>(this)->ReCost = mGame->GetTurnPhase();
		// loop through ships
		deque<Stack>::const_iterator iter;

		const_cast<Fleet *>(this)->CVCost.Zero();
		for (iter = mStacks.begin(); iter != mStacks.end(); ++iter) {
			for (CargoType ct = RESOURCES; ct < Rules::MaxMinType; ++ct) {
				const_cast<Fleet *>(this)->CVCost[ct] += iter->GetDesign()->GetCost(GetOwner())[ct] * iter->GetCount();
			}
		}
	}

	return CVCost;
}

/*
bool Fleet::CanSteal(CargoType ct, CargoHolder * dest) const
{
	// loop through ships
	bool Result = false;
	deque<Stack>::const_iterator iter;

	for (iter = mStacks.begin(); !Result && iter != mStacks.end(); ++iter)
		Result = iter->GetDesign()->CanSteal(ct, dest);

	return Result;
}
*/

long lGetScanPenetrating(long v, const Stack & s, const Player * player)
{
	return max(v, s.GetDesign()->GetScanPenetrating(player));
}

long Fleet::GetScanPenetrating() const
{
	if (CVScanPenetrating == -2) {
		const_cast<Fleet *>(this)->CVScanPenetrating =
				accumulate(	mStacks.begin(),
							mStacks.end(),
							0,
							lGetScanPenetrating,
							GetOwner());
	}

	return CVScanPenetrating;
}

long lGetScanSpace(long v, const Stack & s, const Player * player)
{
	return max(v, s.GetDesign()->GetScanSpace(player));
}

long Fleet::GetScanSpace() const
{
	if (CVScanSpace != -1) {
		const_cast<Fleet *>(this)->CVScanSpace =
				accumulate(	mStacks.begin(),
							mStacks.end(),
							0,
							lGetScanSpace,
							GetOwner());

		const_cast<Fleet *>(this)->CVScanSpace = long(CVScanSpace * GetOwner()->SpaceScanFactor());
	}

	return CVScanSpace;
}

long lGetTerraPower(long v, const Stack & s, long type)
{
	return v + s.GetDesign()->GetTerraPower(type);
}

//	to ask for +terra: type is: 0x12 to initial (if deterraformed for some reason) 0x22 to tech of ship owner
//	to ask for -terra: type is: 0x11 to initial, 0x21 to tech of ship owner
long Fleet::GetTerraPower(long type) const
{
	return accumulate(	mStacks.begin(),
						mStacks.end(),
						0,
						lGetTerraPower,
						type);
}

long lGetMineAmount(long v, const Stack & s, long type)
{
	return v + s.GetDesign()->GetMineAmount(type);
}

long Fleet::GetMineAmount(long type) const
{
	if (CVMineAmount[type-1] == -1) {
		long Result = accumulate(mStacks.begin(),
								mStacks.end(),
								0,
								lGetMineAmount,
								type);
		const_cast<Fleet *>(this)->CVMineAmount[type-1] = Result;
	}
	return CVMineAmount[type-1];
}

double Fleet::RadDamage() const
{
	double Damage = 0.0;
	for (HabType ht = 0; ht < Rules::MaxHabType; ++ht) {
		if (DoesRadiate(ht))
			Damage += GetOwner()->RadDamage(ht);
	}

	return Damage;
}

bool Fleet::DoesRadiate(HabType ht) const
{
	if (CVRadiation[ht] != -1)
		return CVRadiation[ht] ? true : false;
	const_cast<Fleet *>(this)->CVRadiation[ht] = 1;
	for (deque<Stack>::const_iterator i = mStacks.begin(); i != mStacks.end(); ++i)
		if (i->GetDesign()->DoesRadiate(ht))
			return true;
	const_cast<Fleet *>(this)->CVRadiation[ht] = 0;
	return false;
}

//Macros for the rest of the functions:

// Get the sum of all ships in the fleet
// define a local function that adds current total, and component capability * count
// make a member function that accumulates all slots using the local function, and initial value of the hull capability
#define GET_FLEET_SUM(Function)								\
	long lGet##Function(long v, const Stack & s)			\
	{														\
		return v + s.GetDesign()->Get##Function() * s.GetCount();	\
	}														\
															\
	long Fleet::Get##Function() const						\
	{														\
		if (CV##Function == -1)								\
			const_cast<Fleet *>(this)->CV##Function = std::accumulate(mStacks.begin(), mStacks.end(), 0, lGet##Function);	\
		return CV##Function;								\
	}
#define GET_FLEET_SUMD(Function)							\
	double lGet##Function(double v, const Stack & s)		\
	{														\
		return v + s.GetDesign()->Get##Function() * s.GetCount();	\
	}														\
															\
	double Fleet::Get##Function() const						\
	{														\
		if (!CCalc##Function) {								\
			const_cast<Fleet *>(this)->CCalc##Function = true;\
			const_cast<Fleet *>(this)->CV##Function = std::accumulate(mStacks.begin(), mStacks.end(), 0.0, lGet##Function);	\
		}													\
		return CV##Function;								\
	}
// Get the max value of all ships in the fleet
#define GET_FLEET_MAXD(Function)							\
	double lGetMax##Function(double v, const Stack & s)		\
	{														\
		return max(v, s.GetDesign()->Get##Function());		\
	}														\
															\
	double Fleet::GetMax##Function() const					\
	{														\
		if (!CCalcMax##Function) {							\
			const_cast<Fleet *>(this)->CCalcMax##Function = true;\
			const_cast<Fleet *>(this)->CVMax##Function = std::accumulate(mStacks.begin(), mStacks.end(), 0.0, lGetMax##Function);	\
		}													\
		return CVMax##Function;								\
	}
// Get the max value of all ships in the fleet
#define GET_FLEET_MAX(Function)							\
	long lGetMax##Function(long v, const Stack & s)		\
	{														\
		return max(v, s.GetDesign()->Get##Function());		\
	}														\
															\
	long Fleet::GetMax##Function() const					\
	{														\
		if (!CCalcMax##Function) {							\
			const_cast<Fleet *>(this)->CCalcMax##Function = true;\
			const_cast<Fleet *>(this)->CVMax##Function = std::accumulate(mStacks.begin(), mStacks.end(), 0, lGetMax##Function);	\
		}													\
		return CVMax##Function;								\
	}
// Get the min value of all ships in the fleet
#define GET_FLEET_MIN(Function)								\
	long lGetMin##Function(long v, const Stack & s)			\
	{	/* return min but more then 0 */					\
		return v > 0 ? min(v, s.GetDesign()->Get##Function()) : s.GetDesign()->Get##Function();		\
	}														\
															\
	long Fleet::GetMin##Function() const					\
	{														\
		if (CVMin##Function == -1)							\
			const_cast<Fleet *>(this)->CVMin##Function = std::accumulate(mStacks.begin(), mStacks.end(), 0, lGetMin##Function);	\
		return CVMin##Function;								\
	}
// Get true if any ship in the fleet has the capability, false otherwise
#define GET_FLEET_HAS(Function)								\
	bool Fleet::Can##Function() const						\
	{														\
		if (CV##Function != -1)								\
			return CV##Function ? true : false;				\
		const_cast<Fleet *>(this)->CV##Function = 1;			\
		for (deque<Stack>::const_iterator i = mStacks.begin(); i != mStacks.end(); ++i)	\
			if (i->GetDesign()->Can##Function())			\
				return true;								\
		const_cast<Fleet *>(this)->CV##Function = 0;			\
		return false;										\
	}

GET_FLEET_SUM(Cloaking)
GET_FLEET_SUM(FuelCapacity)
GET_FLEET_SUM(FuelGeneration)
GET_FLEET_SUM(Mass)
GET_FLEET_MAX(Mass)
GET_FLEET_MAXD(Tachyon)
GET_FLEET_SUM(CargoCapacity)
GET_FLEET_SUM(Mines)
GET_FLEET_SUM(Sweeping)

GET_FLEET_MAXD(Dampener)
GET_FLEET_MAXD(RepairRate)
GET_FLEET_MIN(SafeSpeed)
GET_FLEET_MIN(MaxSpeed)
GET_FLEET_MIN(FreeSpeed)
GET_FLEET_MIN(BattleSpeed)

GET_FLEET_HAS(Colonize)
GET_FLEET_HAS(StealShip)
GET_FLEET_HAS(StealPlanet)
GET_FLEET_HAS(Shoot)
GET_FLEET_HAS(NormalBomb)
GET_FLEET_HAS(TerraBomb)
GET_FLEET_HAS(SmartBomb)
GET_FLEET_HAS(JumpGate)

GET_FLEET_SUMD(NormalKillPercentage)
//GET_FLEET_SUM(SmartKillPercentage)
GET_FLEET_SUM(KillMin)
GET_FLEET_SUM(KillInstallation)

}
