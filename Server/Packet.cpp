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

#include "Packet.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

//dan Neely 7-1-03
namespace FreeStars {

Packet::Packet(const CargoHolder &source, long speed, long driverSpeed, Planet * destination)
:	CargoHolder(source),
	mSpeed(speed),
	mDriverSpeed(driverSpeed),
	mDestination(destination),
	mFirstYear(true)
{
	mID = TheGalaxy->GetPacketID();
}

Packet::~Packet()
{
}

bool Packet::ParseNode(const TiXmlNode * node)
{
	mFirstYear = false;
	if (!CargoHolder::ParseNode(node))
		return false;

	mSpeed = GetLong(node->FirstChild("Speed"));
	if (mSpeed < 0) {
		Message * mess = NCGetOwner()->AddMessage("Error: Invalid packet speed");
		mess->AddLong("Speed", mSpeed);
		return false;
	}

	mDestination = TheGalaxy->GetPlanet(GetString(node->FirstChild("Destination")));
	if (mDestination == NULL) {
		Message * mess = NCGetOwner()->AddMessage("Error: Invalid packet destination");
		mess->AddItem("Destination", GetString(node->FirstChild("Destination")));
		return false;
	}

	TheGame->AddAlsoHere(this);
	return true;
}

TiXmlNode * Packet::WriteNode(TiXmlNode * node, const Player * viewer) const
{
	if (viewer != NULL && !SeenBy(viewer))
		return NULL;

	CargoHolder::WriteNode(node, viewer);
	AddLong(node, "Speed", mSpeed);
	AddString(node, "Destination", mDestination->GetName().c_str());

	return node;
}

long Packet::GetCloak(const Player *, bool) const
{
	return GetOwner()->InherentCloaking(HC_ALL);
}

long Packet::GetScanPenetrating() const
{
	if (GetOwner()->PacketScanning())
		return mSpeed * mSpeed;
	else
		return -1;
}

long Packet::GetScanSpace() const
{
	if (GetOwner()->PacketScanning())
		return mSpeed * mSpeed;
	else
		return -1;
}

void Packet::Collide()
{
	Message * mess = NULL;
	if (mDestination->GetOwner() != NULL)
		mess = mDestination->NCGetOwner()->AddMessage("Packet hit", mDestination);

//Speed
//spdPacket = Packet Warp ^ 2
//spdReceiver = Rcvr Accel ^ 2

//Percent Caught Safely: The percentage of the packet recovered intact.
//%CaughtSafely = spdReceiver / spdPacket
//Minerals Recovered: The receiver recovers 1/3 of the portion not caught safely.
//(packetkT x %CaughtSafely + packetkT x %remaining x 1/3)
	const Ship * defendingStarBase = mDestination->GetBaseDesign();

	long defendingDriver = 0;

	if (defendingStarBase != NULL) {
		long defendingDriver = defendingStarBase->GetDriverSpeed();
		defendingDriver += (defendingStarBase->CountDrivers());
		defendingDriver--;  // could use an if and ++ if two returned but this
		//will generalize for N driver bases.
	}
	
	long catchAs = defendingDriver * defendingDriver;
	const Player * defender = mDestination->NCGetOwner();
	// used to factor ITs not beign able to safely catch packets at any speed
	if (defender != NULL)
		catchAs = long(catchAs * defender->PacketCatchFactor());

	long minsCaught;

	long packetMass = GetCargoMass();
	if (mDestination->GetOwner() != NULL)
		mess->AddLong("Packet size", GetCargoMass());

	double habvalue = 0.0;
	if (mDestination->GetOwner() != NULL)
		habvalue = mDestination->GetOwner()->HabFactor(mDestination);

	double fractionCaught = double(GetSpeed() * GetSpeed()) / catchAs;
	for (CargoType i = 0; i < Rules::MaxMinType; i++)
	{
		minsCaught = int(GetContain(i) * fractionCaught + .5);
		packetMass -= minsCaught;
		if (GetOwner()->PacketTerraformOdds() > epsilon) {
			int mass = GetContain(i) - minsCaught;
			int amount = 0;
			if (mass > GetOwner()->PacketPermaformMass())
				amount = Rules::MultiOdds(GetOwner()->PacketPermaformOdds(), long(mass / GetOwner()->PacketPermaformMass()));

			mDestination->Permaform(GetOwner(), amount, GetOwner()->PacketTerraform(i), true);

			if (mass > GetOwner()->PacketTerraformMass())
				amount = Rules::MultiOdds(GetOwner()->PacketTerraformOdds(), long(mass / GetOwner()->PacketTerraformMass()));

			mDestination->Terraform(amount, 0, GetOwner()->PacketTerraform(i), GetOwner(), GetOwner(), NULL);
		}
		minsCaught += int((GetContain(i) - minsCaught + .5) / 3.0);
		mDestination->AdjustAmounts(i, minsCaught);
	}

	if (mDestination->GetOwner() != NULL) {
		double habvalue2 = mDestination->GetOwner()->HabFactor(mDestination);
		if ((habvalue < habvalue2 + epsilon) || (habvalue > habvalue2 + epsilon)) {
			mess->AddFloat("Old hab value", habvalue);
			mess->AddFloat("New hab value", habvalue2);
		}
		//Raw Damage
		//dmgRaw = (spdPacket - spdReceiver) x wtPacket / 160
		double rawDamage = packetMass / 160;

		//Raw Damage modified by planetary defenses
		//dmgRaw2 = dmgRaw x (100% - pctDefCoverage)
		rawDamage = rawDamage * (1.0 - mDestination->GetDefenseValue());

		//Colonists Killed: The number colonists killed is the larger (maximum) 
		//of the following:
		//dmgRaw2 x Population / 1000 
		//dmgRaw2 x 100
		int killed = max(long(rawDamage * 100), long((rawDamage * mDestination->GetPopulation() + 500) / 1000));
		mDestination->AdjustAmounts(POPULATION, -killed);
		mess->AddLong("Colonists killed", killed);

		// check if Destination dead is done in Game

		//Destinationary Defenses Destroyed
		//#destroyed = #defenses x dmgRaw2 / 1000
		//If #destroyed is less than dmgRaw2 / 20, then it is that number.
		long numDefs = mDestination->GetDefenses();
		long numDefsDestroyed;
		numDefsDestroyed = max(long(numDefs * rawDamage / 1000), long(rawDamage / 20));
		numDefsDestroyed = min(numDefsDestroyed, numDefs);

		mDestination->AdjustDefenses(-numDefsDestroyed);
		mess->AddLong("Defenses destroyed", numDefsDestroyed);
	}
}

bool Packet::Move(bool FirstYear)
{
	if (FirstYear != mFirstYear)
		return false;

	long distance = mSpeed * mSpeed;
	if (mFirstYear)
		distance /= 2;

	double mPX, mPY;
	MoveToward(this, mDestination, &mPX, &mPY, distance);
	Location pt;
	pt.SetLocation(long(mPX), long(mPY));

	distance = long(Distance(&pt));
	SetLocation(pt);

	Decay(distance);
	if (GetCargoMass() == 0)
		return true;

	if (IsWith(*mDestination)) {
		Collide();
		return true;
	}

	if (mFirstYear) {
		mFirstYear = false;
		TheGame->AddAlsoHere(this);
	} else
		TheGame->MoveAlsoHere(this);

	return false;
}

void Packet::Decay(long distance)
{
	double decayRate;
	long speed = GetSpeed(); // note will be adjusted for ITs may not be true speed
	long firingDriver = GetDriverSpeed();
	const Player * owner =NCGetOwner();

	//ITcheck
	if (owner->PacketDecayPenalty())
		speed = max(speed, firingDriver) + 1;

	decayRate = (double)Rules::GetArrayFloat("PacketDecayRates", min(3L, speed - this->GetDriverSpeed())) / 100;
	decayRate *= owner->PacketDecayFactor(); // PPcheck
	if (GetSpeed() * GetSpeed() > distance)
		decayRate *= GetSpeed() * GetSpeed() / distance;	// didn't go full distance

	long decay;
	long mindecay = long(Rules::GetConstant("PacketMinimumDecay") * owner->PacketDecayFactor() + .5);
	for (int i = 0; i < Rules::MaxMinType; i++) {
		decay = min(GetContain(i), max(mindecay, long(GetContain(i) * decayRate + .5)));
		AdjustAmounts(i, -decay);
	}

	mMaxSize = GetCargoMass();
}

}
