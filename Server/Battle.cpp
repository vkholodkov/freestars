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

#include "Battle.h"
#include "Hull.h"
#include "Stack.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {
long** Battle::StartX = NULL;
long** Battle::StartY = NULL;
long Battle::NumberOfPlayers;
deque<TiXmlElement *> Battle::sReports;


Battle::Battle(Game *_game, const Location & loc)
:	game(_game), Location(loc), mBaseFight(false)
{
	if (StartX == NULL) {
		SetStartPos(game);
	}
}

Battle::~Battle()
{
}

void Battle::Cleanup()
{
	if (StartX != NULL) {
		for (unsigned int i = 0; i < NumberOfPlayers - 1; ++i) {
			delete [] StartX[i];
			delete [] StartY[i];
		}

		delete [] StartX;
		delete [] StartY;
		StartX = NULL;
		StartY = NULL;
	}

	for (int j = 0; j < sReports.size(); ++j)
		delete sReports[j];

	sReports.clear();
}

void Battle::SetStartPos(const Game *game)
{
    NumberOfPlayers = game->NumberPlayers();

	StartX = new long*[game->NumberPlayers() - 1];
	StartY = new long*[game->NumberPlayers() - 1];
	for (unsigned int i = 0; i < game->NumberPlayers() - 1; ++i) {
		StartX[i] = new long[i+2];
		StartY[i] = new long[i+2];
		for (unsigned int j = 0; j < i+2; ++j) {
			StartX[i][j] = 1;
			StartY[i][j] = 1;
		}
	}
// leave as 'unfinished' will generate a warning to this line, easier for ide to jump right here
#pragma unfinished(__FILE__ " : SetStartPos needs to be written still!")
}

void Battle::AddFleet(Fleet * fleet)
{
	mThere.push_back(fleet);
	fleet->SetInBattle(false);
	fleet->SetAlreadyFought();
}

void Battle::AddFleets()
{
	Player * player;
	for (int i = 1; i <= game->NumberPlayers(); ++i) {
		player = game->NCGetPlayer(i);
		player->AddBattleFleets(this);
	}
}

void Battle::Resolve()
{
	bool Combat = false;	// will combat happen?
	// First, see who is fighting
	deque<Fleet *>::iterator if1, if2;
	unsigned long i, j;
	for (i = 1; i <= game->NumberPlayers(); ++i)
		game->NCGetPlayer(i)->ClearBattleEnemies();

	for (if1 = mThere.begin(); if1 != mThere.end(); ++if1) {
		if ((*if1)->CanShoot() && (*if1)->GetBattlePlan()->GetEnemy() != BPE_NONE) {
			if (mBPlanet && mBPlanet->GetBaseNumber() >= 0) {
				if ((*if1)->GetBattlePlan()->WillFight((*if1)->GetOwner(), mBPlanet->GetOwner())) {
					// set both races to fight each other
					(*if1)->NCGetOwner()->SetBattleEnemy(mBPlanet->GetOwner()->GetID());
					mBPlanet->NCGetOwner()->SetBattleEnemy((*if1)->GetOwner()->GetID());
					Combat = true;
					mBaseFight = true;
					mBPlanet->SetInBattle();
				}
			}

			for (if2 = mThere.begin(); if2 != mThere.end(); ++if2) {
				if (*if1 == *if2)
					continue;

				if ((*if1)->GetBattlePlan()->WillFight((*if1)->GetOwner(), (*if2)->GetOwner())) {
					// set both races to fight each other
					(*if1)->NCGetOwner()->SetBattleEnemy((*if2)->GetOwner()->GetID());
					(*if2)->NCGetOwner()->SetBattleEnemy((*if1)->GetOwner()->GetID());
					Combat = true;
				}
			}
		}
	}

	if (!Combat)	// if no combat here, we're done
		return;

	// Battle report xml object
	bReport = new TiXmlElement("BattleReport");
	sReports.push_back(bReport);

	long Races = 0;	// number of races in this battle
	for (i = 1; i <= game->NumberPlayers(); ++i) {
		if (game->GetPlayer(i)->InThisBattle())
			Races++;
	}

	double Dampener = 0;
	long Race = 0;
	Player * p1 = NULL;

	// if this player is in the battle, set all their fleets at the same location to be there
	// Note: you're in the battle if you attack anyone there, or if anyone there attacks you
	// also setup stacks
	for (if1 = mThere.begin(); if1 != mThere.end(); ++if1) {
		if ((*if1)->GetOwner()->InThisBattle()) {
			if (p1 != (*if1)->GetOwner()) {
				p1 = (*if1)->NCGetOwner();
				++Race;
			}

			(*if1)->SetInBattle(true);
			if ((*if1)->GetMaxDampener() > Dampener)
				Dampener = (*if1)->GetMaxDampener();

			if (mBaseFight && mBPlanet && mBPlanet->GetOwner() == p1) {
				// add a 'stack' for the base
				Stack * s = new Stack();

				s->SetupBase(mBPlanet);
				s->bx = StartX[Races-2][Race-1];
				s->by = StartY[Races-2][Race-1];
				mFighting.push_back(s);
			}

			// add this fleets stacks to the list, set battle variables
			for (j = 0; j < (*if1)->mStacks.size(); ++j) {
				Stack * s = &((*if1)->mStacks[j]);
				if ((*if1)->GetCargoMass() > 0 && s->mShip->GetCargoCapacity() > 0)
					s->SetupShips(p1, (*if1)->GetCargoMass() / (*if1)->GetCargoCapacity() * s->mShip->GetCargoCapacity());
				else
					s->SetupShips(p1, 0);

				s->bx = StartX[Races-2][Race-1];
				s->by = StartY[Races-2][Race-1];
				mFighting.push_back(s);
			}
		}
	}
	assert(Races == Race);

	// Sort by mass
	sort(mFighting.begin(), mFighting.end(), Stack::StackLighter);

	TiXmlElement Stacks("StartingStacks");

	// Apply energy dampener
	for (j = 0; j < mFighting.size(); ++j) {
		if (!mFighting[j]->IsBase()) {
			mFighting[j]->bSpeed -= long(Dampener * 4);
			if (mFighting[j]->bSpeed < 2)
				mFighting[j]->bSpeed = 2;
		}

		TiXmlElement stack("Stack");
		stack.SetAttribute("IDNumber", j);
		mFighting[j]->WriteNode(&stack);
		Stacks.InsertEndChild(stack);
	}

	bReport->InsertEndChild(Stacks);

	// everyone in the battle gets to see all designs
	for (i = 1; i <= game->NumberPlayers(); ++i) {
		p1 = game->NCGetPlayer(i);
		if (p1->InThisBattle()) {
			for (j = 0; j < mFighting.size(); ++j) {
				Player * o = mFighting[j]->GetFleetIn()->NCGetOwner();
				if (o != p1)
					o->SetSeenDesign(p1->GetID(), o->GetShipNumber(mFighting[j]->mShip), mFighting[j]->IsBase());
			}
		}
	}

	// setup the slot firing order
	for (j = 0; j < mFighting.size(); ++j) {
		for (i = 0; i < mFighting[j]->mShip->GetHull()->GetNumberSlots(); ++i) {
			if (mFighting[j]->mShip->GetSlot(i).GetComp()->GetType() == CT_WEAPON) {
				mSlots.push_back(bSlot(&(mFighting[j]->mShip->GetSlot(i)), mFighting[j]));
			}
		}
	}

	// Sort by initiative
	sort(mSlots.begin(), mSlots.end(), HighInitiative);

	// fight the battle
	for (mRound = 1; mRound <= 16; ++mRound) {
		if (!FightRound())
			break;
	}



	// finally, send a message to all players at the location
	for (i = 1; i <= game->NumberPlayers(); ++i) {
		p1 = game->NCGetPlayer(i);
		if (p1->InThisBattle()) {
			Message * mess = p1->AddMessage("Battle");
			if (mBPlanet != NULL)
				mess->AddItem("Battle Location", mBPlanet);
			else
				mess->AddItem("Battle Location", new Location(*this), true);

			mess->AddItem(bReport);
		} else {
			Message * mess = p1->AddMessage("Battle, Not involved");
			if (mBPlanet != NULL)
				mess->AddItem("Battle Location", mBPlanet);
			else
				mess->AddItem("Battle Location", new Location(*this), true);
		}
	}

	// delete the base 'stack'
	if (mBaseFight) {
		for (j = 0; j < mFighting.size(); ++j) {
			if (mFighting[j]->IsBase()) {
				delete mFighting[j];
				mFighting[j] = NULL;
			}
		}
	}
}

bool Battle::HighInitiative(const bSlot& s1, const bSlot& s2)
{
	if (s1.initiative > s2.initiative)
		return true;
	else if (s1.initiative == s2.initiative) {
		if (s1.stack == s2.stack)
			return s1.slot->GetPosition() < s2.slot->GetPosition();
		else {
			while (s1.stack->bRandInitiative == 0)
				s1.stack->bRandInitiative = genrand_int32();

			if (s2.stack->bRandInitiative == 0) {
				while (s2.stack->bRandInitiative == 0 || s1.stack->bRandInitiative == s2.stack->bRandInitiative)
					s2.stack->bRandInitiative = genrand_int32();
			}

			return s1.stack->bRandInitiative > s2.stack->bRandInitiative;	// ties go to s1
		}
	} else
		return false;
}

bool Battle::FightRound()
{
	bool fought = false;
	int i, j;

	// Get MAT (Most Attractive Target) for every stack
	for (j = 0; j < mFighting.size(); ++j) {
		mFighting[j]->bMAT = NULL;
		if (mFighting[j]->bShips > 0 && mFighting[j]->bPlan != BPT_DISENGAGE) {
			mFighting[j]->bMAT = GetTarget(mFighting[j], mFighting[j]->GetFleetIn()->GetBattlePlan()->GetPrimary(), 0, NULL);
			if (mFighting[j]->bMAT == NULL)
				mFighting[j]->bMAT = GetTarget(mFighting[j], mFighting[j]->GetFleetIn()->GetBattlePlan()->GetSecondary(), 0, NULL);
		}
	}

	// Move first
	for (i = 3; i > 0; --i) {
		for (j = 0; j < mFighting.size(); ++j) {
			if (mFighting[j]->bShips > 0 && GetSpeed(mFighting[j]->bSpeed, mRound) >= i)
				if (MoveStack(j))
					fought = true;
		}
	}
	
	if (!fought)	// no one has a target, stop fighting
		return fought;

	for (j = 0; j < mSlots.size(); ++j) {
		if (mSlots[j].stack->bShips > 0)
			ShootSlot(mSlots[j]);
	}

	return fought;
}

long Battle::GetRange(const Stack * s1, const Stack * s2, int dx/* = 0*/, int dy/* = 0*/) const
{
	return max(abs(s1->bx + dx - s2->bx), abs(s1->by + dy - s2->by));
}

long Battle::GetSpeed(long speed, long round)
{
	switch (speed % 4) {
	case 0:
	default:	// default should never happen, added to remove warning
		return speed / 4;
	case 1:
		return speed / 4 + round % 4 == 1 ? 1 : 0;
	case 2:
		return speed / 4 + round % 2 == 1 ? 1 : 0;
	case 3:
		return speed / 4 + round % 4 == 3 ? 0 : 1;
	}
}

// move ship, return true if this ship is trying to shoot someone, and wants the battle to continue
bool Battle::MoveStack(long s)
{
	int dx, dy;
	long DamDone[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
	long DamTake[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
	long Range;
	long DDRange = 0;	// Damage done at this range, to limit recalcing damage at the same range
	long DTRange = 0;	// Damage taken at this range, to limit recalcing damage at the same range

	// For every stack in the battle
	for (int j = 0; j < mFighting.size(); ++j) {
		// check moving +- 1
		Range = -1;
		for (dx = -1; dx <= 1; ++dx) for (dy = -1; dy < 1; ++dy) if (dx+mFighting[s]->bx >= 0 && dx+mFighting[s]->bx <= 9 && dy+mFighting[s]->by >= 0 && dy+mFighting[s]->by <= 9) {
			// if we can shoot and want to shoot this stack
			if (mFighting[s]->mShip->CanShoot() &&
				mFighting[s]->GetFleetIn()->GetOwner()->GetBattleEnemy(mFighting[j]->GetFleetIn()->GetOwner()->GetID()) &&
				(mFighting[j]->mShip->IsBattleTarget(mFighting[s]->GetFleetIn()->GetBattlePlan()->GetPrimary()) ||
					mFighting[j]->mShip->IsBattleTarget(mFighting[s]->GetFleetIn()->GetBattlePlan()->GetSecondary())))
			{
				if (Range != GetRange(mFighting[s], mFighting[j], dx, dy)) {
					Range = GetRange(mFighting[s], mFighting[j], dx, dy);
					DDRange = PotentialDamage(mFighting[s], mFighting[j], Range, NULL);
					DDRange /= mFighting[j]->mShip->IsBattleTarget(mFighting[s]->GetFleetIn()->GetBattlePlan()->GetPrimary()) ? 1 : 10;
					DDRange *= mFighting[j]->mShip->GetAttractiveCost(mFighting[j]->GetFleetIn()->GetOwner()) * mFighting[j]->mShips / mFighting[j]->DP();
				}
				DamDone[dx+1][dy+1] = max(DamDone[dx+1][dy+1], DDRange);
			}
			// if they can shoot and want to shoot us
			if (mFighting[j]->mShip->CanShoot() &&
				mFighting[j]->GetFleetIn()->GetOwner()->GetBattleEnemy(mFighting[s]->GetFleetIn()->GetOwner()->GetID()) &&
				(mFighting[s]->mShip->IsBattleTarget(mFighting[j]->GetFleetIn()->GetBattlePlan()->GetPrimary()) ||
					mFighting[s]->mShip->IsBattleTarget(mFighting[j]->GetFleetIn()->GetBattlePlan()->GetSecondary())))
			{
				if (Range != GetRange(mFighting[s], mFighting[j], dx, dy)) {
					Range = GetRange(mFighting[s], mFighting[j], dx, dy);
					DTRange = PotentialDamage(mFighting[j], mFighting[s], Range, NULL);
					DTRange *= mFighting[s]->mShip->GetAttractiveCost(mFighting[s]->GetFleetIn()->GetOwner()) * mFighting[s]->mShips / mFighting[s]->DP();
				}
				DamTake[dx+1][dy+1] += DTRange;
			}
		}
	}

	double bestP, bestD;
	int count = 0;
	long mdx = 0, mdy = 0;
	double di;
	switch (mFighting[s]->bPlan) {
	// do disengage later because other orders can become disengage
	default:
		break;	// unknown plan, don't move
	case BPT_DISIFHIT:
	case BPT_MINDAM:
		bestP = DamTake[0][0];
		bestD = DamDone[0][0];
		for (dx = -1; dx <= 1; ++dx) for (dy = -1; dy < 1; ++dy) {
			if ((bestD == 0 && DamDone[dx+1][dy+1] > 0) ||
				(bestD > 0 && DamDone[dx+1][dy+1] > 0 && bestP < DamTake[dx+1][dy+1]) ||
				(bestD < DamDone[dx+1][dy+1] && bestP == DamTake[dx+1][dy+1]))
			{
				bestP = DamTake[dx+1][dy+1];
				bestD = DamDone[dx+1][dy+1];
				count = 0;
			} else if (dx > -1 || dy > -1 && (bestD == DamDone[dx+1][dy+1] && bestP == DamTake[dx+1][dy+1])) {
				++count;
				if (Random(count) == 0) {
					mdx = dx;
					mdy = dy;
				}
			}
		}
		break;
	case BPT_MAXNET:
		bestP = DamDone[0][0] - DamTake[0][0];
		for (dx = -1; dx <= 1; ++dx) for (dy = -1; dy < 1; ++dy) {
			di = DamDone[dx+1][dy+1] - DamTake[dx+1][dy+1];
			if (bestP > di) {
				bestP = di;
				count = 0;
			} else if (dx > -1 || dy > -1 && bestP == di) {
				++count;
				if (Random(count) == 0) {
					mdx = dx;
					mdy = dy;
				}
			}
		}
		break;
	case BPT_MAXRATIO:
		bestP = double(DamDone[0][0]) / double(DamTake[0][0]);
		for (dx = -1; dx <= 1; ++dx) for (dy = -1; dy < 1; ++dy) {
			di = double(DamDone[dx+1][dy+1]) / double(DamTake[dx+1][dy+1]);
			if (bestP > di) {
				bestP = di;
				count = 0;
			} else if (dx > -1 || dy > -1 && bestP == di) {
				++count;
				if (Random(count) == 0) {
					mdx = dx;
					mdy = dy;
				}
			}
		}
		break;
	case BPT_MAXDAM:
		bestP = DamDone[0][0];
		for (dx = -1; dx <= 1; ++dx) for (dy = -1; dy < 1; ++dy) {
			if (bestP > DamDone[dx+1][dy+1]) {
				bestP = DamDone[dx+1][dy+1];
				count = 0;
			} else if (dx > -1 || dy > -1 && bestP == DamDone[dx+1][dy+1]) {
				++count;
				if (Random(count) == 0) {
					mdx = dx;
					mdy = dy;
				}
			}
		}
		break;
	}

	if (mFighting[s]->bPlan != BPT_DISENGAGE && DamDone[mdx+1][mdy+1] == 0 && mFighting[s]->mShip->CanShoot()) {
		// alternate move: if we can't yet do damage and want to, find most attractive target, and close with it
		count = 1;
		const Stack * target;
		target = GetTarget(mFighting[s], mFighting[s]->GetFleetIn()->GetBattlePlan()->GetPrimary(), 0, NULL);
		if (target == NULL)
			target = GetTarget(mFighting[s], mFighting[s]->GetFleetIn()->GetBattlePlan()->GetSecondary(), 0, NULL);
		if (target != NULL) {
			if (target->bx < mFighting[s]->bx)
				mdx = -1;
			else if (target->bx > mFighting[s]->bx)
				mdx = 1;
			else
				mdx = 0;

			if (target->by < mFighting[s]->by)
				mdy = -1;
			else if (target->by > mFighting[s]->by)
				mdy = 1;
			else
				mdy = 0;
		} else {
			mFighting[s]->bPlan = BPT_DISENGAGE;	// if there are no targets, flee
			AddLong(bReport, "StackDisengages", s);
		}
	}

	// disengage here because other orders can become disengage
	if (mFighting[s]->bPlan == BPT_DISENGAGE) {
		bestP = DamTake[0][0];
		for (dx = -1; dx <= 1; ++dx) for (dy = -1; dy < 1; ++dy) {
			if (bestP < DamTake[dx+1][dy+1]) {
				bestP = DamTake[dx+1][dy+1];
				count = 0;
			} else if (dx > -1 || dy > -1 && bestP == DamTake[dx+1][dy+1]) {
				++count;
				if (Random(count) == 0) {
					mdx = dx;
					mdy = dy;
				}
			}
		}
	}

	mFighting[s]->bx += mdx;
	mFighting[s]->by += mdy;

	// add move to battle report
	TiXmlElement move("StackMove");
	AddLong(&move, "XCoord", mFighting[s]->bx);
	AddLong(&move, "YCoord", mFighting[s]->by);
	bReport->InsertEndChild(move);

	// stop fighting if no one has a target
	return 	(mFighting[s]->bPlan == BPT_DISENGAGE);
}

const Stack * Battle::GetTarget(const Stack * hunter, HullType hc, long Range, const Slot * slot) const
{
	double attract = 0.0;
	double attemp;
	const Stack * Result = NULL;
	long count = 0;
	long tRange = Range;

	for (int j = 0; j < mFighting.size(); ++j) {
		// if I don't like the owner, and it's a valid target, calc attractiveness
		if (hunter->GetFleetIn()->GetOwner()->GetBattleEnemy(mFighting[j]->GetFleetIn()->GetOwner()->GetID()) &&
			mFighting[j]->mShip->IsBattleTarget(hc))
		{
			if (Range == -1)
				tRange = this->GetRange(hunter, mFighting[j]);

			// check attractiveness: damage / dp * cost of stack
			attemp = mFighting[j]->mShip->GetAttractiveCost(mFighting[j]->GetFleetIn()->GetOwner())
					* mFighting[j]->mShips
					* PotentialDamage(hunter, mFighting[j], tRange, slot)
					/ mFighting[j]->DP();

			if (attemp > attract) {
				attract = attemp;
				count = 1;
				Result = mFighting[j];
			} else if (attemp == attract) {
				++count;
				if (Random(count) == 0)
					Result = mFighting[j];
			}
		}
	}

	return Result;
}

// calculate the potential damage when one stack shoots at another,
// this function doesn't consider other ships firing
// If slot is null it calcs the whole ship, if slot is specified, it just calcs for that slot
// Gatling flag: false-> return damage from all weapons, including gatlings, true-> return just gatling damage
long Battle::PotentialDamage(const Stack * shoot, const Stack * target, long Range, const Slot * slot, bool Gatling/*= false*/) const
{
	long Damage = 0;
	long Sapper = 0;
	double temp;
	const Slot * curSlot;

	for (int i = 0; i < mSlots.size(); ++i) {
		if (slot == NULL)
			curSlot = mSlots[i].slot;
		else
			curSlot = slot;

		if (slot != NULL || mSlots[i].stack == shoot) {
			if (curSlot->GetComp()->GetType() == CT_WEAPON && (curSlot->GetComp()->GetRange() + shoot->IsBase()?1:0) <= Range) {
				if (Gatling && curSlot->GetComp()->GetWeaponType() != WT_GATLING)
					continue;

				temp = curSlot->GetComp()->GetPower() * curSlot->GetCount() * shoot->bShips;
				switch (curSlot->GetComp()->GetWeaponType()) {
				case WT_BEAM:
				case WT_SAPPER:
				case WT_GATLING:
					temp *= shoot->mShip->GetCapacitors() * target->mShip->GetDeflection();
					temp *= 1.0 - 0.1 * double(Range) / (curSlot->GetComp()->GetRange() + shoot->IsBase()?1:0);
					if (curSlot->GetComp()->GetWeaponType() == WT_SAPPER)
						Sapper += max(long(temp), target->bShield - Sapper);
					else {
						if (target->bShield <= Sapper)
							Damage += long(temp);
						else {
							Damage += max(0L, long(temp) - (target->bShield - Sapper));
							Sapper += min(long(temp), (target->bShield - Sapper));
						}
					}
					break;
				case WT_TORP:
				case WT_MISSILE:
					{
						long temp2;
						double Acc = shoot->mShip->GetAccuracy(*curSlot->GetComp(), target->mShip);
						temp *= Acc;
						if (curSlot->GetComp()->GetWeaponType() == WT_MISSILE && target->bShield <= Sapper)
							Damage += long(temp);
						temp2 = max(long(temp/2), (target->bShield - Sapper));
						Sapper += temp2;
						Sapper += long((1.0 - Acc) * temp / 8);	// misses act like sappers
						Damage += long(temp) - temp2;
					}
					break;
				default:
					break;
				}
			}
		}

		Sapper = min(Sapper, target->bShield);	// don't claim more sapper damage then the target has shields
		if (slot != NULL)
			break;
	}

	return min(Sapper+Damage, target->DP());	// don't claim more total damage then the target has
}

void Battle::ShootSlot(bSlot & shooter)
{
	/*
	bool done = false;


	const Stack * target;
	long Range;

	long damage;
	if (curSlot->GetComp()->GetWeaponType() & (WT_BEAM | WT_SAPPER)) {
		// apply capacitors
		damage = shooter.slot->GetComp()->GetPower() * shooter.slot->GetCount() * shooter.stack->bShips;
		damage *= shooter.stack->mShip->GetCapacitors();
	}

	while (!done) {
		target = GetTarget(shooter.stack, shooter.stack->GetFleetIn()->GetBattlePlan()->GetPrimary(), -1, shooter.slot);
		if (target == NULL)
			target = GetTarget(shooter.stack, shooter.stack->GetFleetIn()->GetBattlePlan()->GetSecondary(), -1, shooter.slot);
		if (target == NULL) {
			done = true;
			break;
		}

		Range = GetRange(shooter.stack, target);
		assert(shooter.slot->GetComp()->GetRange() + shooter.stack->IsBase()?1:0) <= Range);

		if (curSlot->GetComp()->GetWeaponType() & (WT_BEAM | WT_SAPPER | WT_GATLING)) {
			// apply deflectors of target ship
			long tdam = damage * target->mShip->GetDeflection();

			// first hit shields
			sdam = min(tdam, target->bShield);
			target->bShield -= sdam;
			tdam -= sdam;

			// then hit armor
			if (curSlot->GetComp()->GetWeaponType() & (WT_BEAM | WT_SAPPER | WT_GATLING)) {
		}

		
		switch (curSlot->GetComp()->GetWeaponType()) {
		case WT_BEAM:
		case WT_SAPPER:
		case WT_GATLING:
			temp *= shoot->mShip->GetCapacitors() * target->mShip->GetDeflection();
			temp *= 1.0 - 0.1 * double(Range) / (curSlot->GetComp()->GetRange() + shoot->IsBase()?1:0);
			if (curSlot->GetComp()->GetWeaponType() == WT_SAPPER)
				Sapper += long(temp);
			else
				Damage += long(temp);
			break;
		case WT_TORP:
		case WT_MISSILE:
			{
				double Acc = shoot->mShip->GetAccuracy(*curSlot->GetComp(), target->mShip);
				Sapper += long((1.0 - Acc) * temp / 8);	// misses act like sappers
				temp *= Acc;
				if (curSlot->GetComp()->GetWeaponType() == WT_MISSILE && target->bShield == 0)
					temp *= 2;
				Damage += long(temp);
			}
			break;
		default:
			break;
		}
	}
	*/
}

}
