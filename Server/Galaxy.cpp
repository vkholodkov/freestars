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
#include "Bombing.h"
#include "Creation.h"
#include "Salvage.h"
#include "Wormhole.h"
#include "TempFleet.h"

#include "NodeUtils.hpp"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

Galaxy::~Galaxy()
{
	int i;
	for (i = 0; i < mPlanets.size(); ++i)
		delete mPlanets[i];

	for (i = 0; i < invasions.size(); ++i)
		delete invasions[i];

	for (i = 0; i < mScrap.size(); ++i)
		delete mScrap[i];

	for (i = 0; i < mPackets.size(); ++i)
		delete mPackets[i];

	for (i = 0; i < mWormholes.size(); ++i)
		delete mWormholes[i];

	for (i = 0; i < mJettison.size(); ++i)
		delete mJettison[i];
}

Planet * Galaxy::GetPlanet(int posX, int posY)
{
	// Search list of planets for one at this position
	deque<Planet *>::iterator iter;

	for (iter = mPlanets.begin(); iter != mPlanets.end(); ++iter) {
		if ((*iter)->GetPosX() == posX && (*iter)->GetPosY() == posY)
			return iter[0];
	}

	return NULL;
}

Planet * Galaxy::GetPlanet(const SpaceObject * loc)
{
	Planet * p = dynamic_cast<Planet *>(loc->GetAlsoHere()->at(0));
	if (p != NULL)
		return p;
	else
		return GetPlanet(loc->GetPosX(), loc->GetPosY());
}

Planet * Galaxy::GetPlanet(const char * name)
{
	// Search list of planets for one at this position
	deque<Planet *>::iterator iter;

	if (!name || !*name)
		return NULL;

	for (iter = mPlanets.begin(); iter != mPlanets.end(); ++iter) {
		if ((*iter)->GetName() == name)
			return iter[0];
	}

	return NULL;
}

void Galaxy::AddInvasion(Planet * planet)
{
	deque<Planet *>::const_iterator iter;

	for (iter = invasions.begin(); iter != invasions.end(); ++iter)
		if (planet == *iter)
			return;

	invasions.push_back(planet);
}

void Galaxy::ResolveInvasions()
{
	deque<Planet *>::iterator iter;

	for (iter = invasions.begin(); iter != invasions.end(); ++iter)
		(*iter)->ResolveInvasion();

	invasions.clear();
}

bool Galaxy::ParseSize(const TiXmlNode * node)
{
	if (node == NULL)
		return false;

	mMinX = GetLong(node->FirstChild("MinX"), -1);
	mMinY = GetLong(node->FirstChild("MinY"), -1);
	mMaxX = GetLong(node->FirstChild("MaxX"), -1);
	mMaxY = GetLong(node->FirstChild("MaxY"), -1);

	if (mMinX < 0 || mMinY < 0 || (mMinX + 100 > mMaxX) || (mMinY + 100 > mMaxY))
		return false;
	else
		return true;
}

bool Galaxy::ParseNode(const TiXmlNode * node)
{
	if (stricmp(node->Value(), "Galaxy") != 0)
		return false;
	
	bool addit;
	const TiXmlNode * child1;
	
	/* Planets */
	for(child1 = node->FirstChildElement("Planet"); child1 ; child1 = child1->NextSiblingElement("Planet")) {
		Planet * p;
		p = GetPlanet(GetString(child1, "Name"));
		addit = (p == NULL);
		if(addit){
			p = TheGame->ObjectFactory(p);
			mPlanets.push_back(p);
		}
		
		if(!p->ParseNode(child1))
			return false;
		
		if(addit)
			TheGame->AddAlsoHere(p);
	}
	
	/* Salvage */
	ParseGroup<Salvage>(node,mScrap);
	
	/* Mystery Trader */
	for(child1 = node->FirstChildElement("MTs"); child1; child1 = child1->NextSibling("MTs")) {
		///@todo MT parsing
		//theGame->addAlsoHere(p)
	}
	
	/* Wormholes */
	ParseGroup<Wormhole>(node,mWormholes);
	
	/* Packets */
	ParseGroup<Packet>(node,mPackets);
	
	/* Check for extra sections */
	const char* valid[] = {"Planet","Salvage","MTs","Wormhole","Packet",0};
	for(child1 = node->FirstChildElement(); child1; child1 = child1->NextSiblingElement()) {
		if(!NodeIsOneOf(child1,valid)) {
			Message * mess = TheGame->AddMessage("Warning: Unknown section in Galaxy");
			mess->AddItem("Section", child1->Value());
		}
	}
	
	deque<Planet *>::iterator it;
	for (it = mPlanets.begin(); it != mPlanets.end(); ++it)
		(*it)->SetDestinations();
	return true;
}

void Galaxy::WriteNode(TiXmlNode * node, const Player * viewer) const
{
	WriteGroup<Planet>(node,viewer,mPlanets);
	WriteGroup<Salvage>(node,viewer,mScrap);
	WriteGroup<Packet>(node,viewer,mPackets);
	WriteGroup<Wormhole>(node,viewer,mWormholes);
}

void Galaxy::WriteXYFile(TiXmlNode * node)
{
	deque<Planet *>::const_iterator pi;
	for (pi = mPlanets.begin(); pi != mPlanets.end(); ++pi) {
		TiXmlElement planet("Planet");
		planet.SetAttribute("IDNumber", (*pi)->GetID());
		AddString(&planet, "Name", (*pi)->GetName().c_str());
		(*pi)->Location::WriteNode(&planet);
		node->InsertEndChild(planet);
	}
}

Salvage * Galaxy::GetSalvage(long n) const
{
	deque<Salvage *>::const_iterator it;
	for (it = mScrap.begin(); it != mScrap.end(); ++it) {
		if ((*it)->GetID() == n)
			return *it;
	}

	return NULL;
}

Salvage * Galaxy::AddSalvage(const CargoHolder & ch)
{
	for (int i = 0; i < mScrap.size(); ++i) {
		if (mScrap[i]->IsWith(ch) && mScrap[i]->GetOwner() == ch.GetOwner())
			return mScrap[i];
	}

	Salvage * scrap = NULL;
	scrap = TheGame->ObjectFactory(scrap);
	scrap->SetLocation(ch);
	mScrap.push_back(scrap);
	return scrap;
}

const Packet * Galaxy::GetPacket(long n) const
{
	deque<Packet *>::const_iterator it;
	for (it = mPackets.begin(); it != mPackets.end(); ++it) {
		if ((*it)->GetID() == n)
			return *it;
	}

	return NULL;
}

bool Galaxy::Mine()
{
	deque<Planet *>::iterator pi;
	for (pi = mPlanets.begin(); pi != mPlanets.end(); ++pi) {
		(*pi)->Mine();
	}

	return true;
}

void Galaxy::DecaySalvage()
{
	deque<Salvage *>::iterator si;
	for (si = mScrap.begin(); si != mScrap.end(); ++si)
		(*si)->Decay();
}

void Galaxy::GrowPop()
{
	deque<Planet *>::iterator pi;
	for (pi = mPlanets.begin(); pi != mPlanets.end(); ++pi) {
		(*pi)->AdjustPopulation((*pi)->PopGrowth());
	}
}

void Galaxy::DoProduction()
{
	mTechSpent.erase(mTechSpent.begin(), mTechSpent.end());
	mTechSpent.insert(mTechSpent.begin(), Rules::MaxTechType, 0);

	deque<Planet *>::iterator pi;
	for (pi = mPlanets.begin(); pi != mPlanets.end(); ++pi) {
		(*pi)->DoProduction();
	}
}

void Galaxy::TechSpent(long TechGain, TechType tech)
{
	if (tech < 0 || tech >= Rules::MaxTechType)
		return;

	assert(Rules::GetTurnEvent(TheGame->GetTurnPhase()) == TP_PRODUCTION);
	mTechSpent[tech] += TechGain;
}

void Galaxy::GainSpyTech(Player * player)
{
	long gain;
	for (TechType tech = 0; tech < Rules::MaxTechType; ++tech) {
		gain = long(player->SpyTechBonus() * mTechSpent[tech] / TheGame->NumberPlayers() + .5);
		if (gain > 0)
			player->GainSpyTech(gain, tech);
	}
}

void Galaxy::DeadCheck()
{
	deque<Planet *>::iterator pi;
	for (pi = mPlanets.begin(); pi != mPlanets.end(); ++pi) {
		(*pi)->DeadCheck();
	}
}

void Galaxy::DoBombing()
{
	// Go through all planets
	for (deque<Planet *>::iterator target_planet = mPlanets.begin(); target_planet != mPlanets.end(); ++target_planet)
	{
		// Check if planet has owner
		Player* planet_owner = const_cast<Player*>((*target_planet)->GetOwner());
		if (planet_owner == NULL)
			continue;
			
		// Check if Planet has a Starbase
		if((*target_planet)->GetBaseNumber() != -1)
			continue;
			
		Bombing bom(*(*target_planet));
		bom.SetPlanet((*target_planet));
		bom.AddFleets();
		bom.Resolve();
	}
}

template<class II> long GetID(II first, II last)
{
	long Result;
	bool retry;
	long trys = 0;

	do {
		Result = genrand_int32();
		retry = Result == 0;
		II i;
		// Search the ID space to check if the random ID is in use.
		for (i = first; !retry && i != last; ++i) {
			if (Result == (*i)->GetID()) {
				// Yes, we need to try again.
				retry = true;
				break;
			}
		}
	} while (retry && trys++ < 1000);

	assert(!retry); // If we can't grab a new one in 1000 tries, there are an AWFUL lot of objects in this game, or our random number generator broke.
	return Result;
}

long Galaxy::GetPacketID() const
{
	return GetID(mPackets.begin(), mPackets.end());
}

long Galaxy::GetSalvageID() const
{
	return GetID(mScrap.begin(), mScrap.end());
}

long Galaxy::GetWormholeID() const
{
	return GetID(mWormholes.begin(), mWormholes.end());
}

/*!
 * "If you want to make an apple pie from scratch, you must first create the universe." -Carl Sagan
 *
 * This function will create the galaxy, based on the parameters from the Creation object c.
 * @param c Galaxy creation settings.
 */
void Galaxy::Build(Creation * c)
{
	long i;

	// add planets
	for (i = GetPlanetCount(); i < c->mWorlds; ++i) {
		// create a planet
		Planet * p = NULL;
		p = TheGame->ObjectFactory(p);
		c->SetLocation(p, this);
		p->CreateRandom(c);
		mPlanets.push_back(p);
		TheGame->AddAlsoHere(p);
	}

	// add wormholes
	if (TheGame->GetRandomEvents() | RE_WORMHOLE) {
		i = Random(TheGame->MinWormholes(), TheGame->MaxWormholes());
		for ( ; i > 0; --i) {
			Wormhole * wh1 = new Wormhole(this);
			Wormhole * wh2 = new Wormhole(this);
			wh1->Shift();
			wh2->Shift();
			wh1->SetAttached(wh2);
			wh2->SetAttached(wh1);
			mWormholes.push_back(wh1);
			mWormholes.push_back(wh2);
			TheGame->AddAlsoHere(wh1);
			TheGame->AddAlsoHere(wh2);
		}
	}
}

void Galaxy::PlacePlayer(Player * player)
{
	if (player->HasHW())
		return;

	Planet * world;
	world = TheGame->GetCreation()->GetNextHW();
	if (world == NULL) {
		// for now, just random selection
		long count = 0;
		while (count++ < 100) {
			world = mPlanets[Random(GetPlanetCount())];
			if (world->GetOwner() != NULL)
				continue;

			break;
		}
	}

	player->PlaceHW(world);


	// place secondary planets
	if (player->HasSecondPlanet() && TheGame->GetCreation()->mSecondaryWorlds) {
		Planet * second = NULL;
		second = TheGame->GetCreation()->GetSecond(player);
		if (second == NULL) {
			long count = 0;
			long dist;
			long w;
			while (count++ < 100) {
				w = Random(GetPlanetCount());
				if (mPlanets[w]->GetOwner() != NULL)
					continue;

				second = mPlanets[w];
				dist = long(second->Distance(world));
				if (dist < TheGame->GetCreation()->mMinSWDistance || dist > TheGame->GetCreation()->mMaxSWDistance)
					continue;

				break;
			}
		}

		player->PlaceSW(second, world);
	}
}

const Planet * Galaxy::ClosestPlanet(const Location * loc)
{
	long dist = -1;
	Planet * Result = NULL;
	// Search list of planets for closest
	deque<Planet *>::iterator iter;

	for (iter = mPlanets.begin(); iter != mPlanets.end(); ++iter) {
		if (dist < 0 || dist > loc->Distance(*iter)) {
			dist = long(loc->Distance(*iter));
			Result = *iter;
		}
	}

	return Result;
}

void Galaxy::LoadPlanets()
{
	int i;
	for (i = 0; i < mPlanets.size(); ++i)
		TheGame->AddAlsoHere(mPlanets[i]);
}

void Galaxy::LoadScrap()
{
	int i;
	for (i = 0; i < mScrap.size(); ++i)
		TheGame->AddAlsoHere(mScrap[i]);
}

void Galaxy::LoadPackets()
{
	int i;
	for (i = 0; i < mPackets.size(); ++i)
		TheGame->AddAlsoHere(mPackets[i]);
}

void Galaxy::LoadWormholes()
{
	int i;
	for (i = 0; i < mWormholes.size(); ++i)
		TheGame->AddAlsoHere(mWormholes[i]);
}

CargoHolder * Galaxy::GetJettison(const CargoHolder * ch, CargoHolder * ch2)
{
	if (ch2 == NULL) {
		Planet * planet = dynamic_cast<Planet *>(ch->GetAlsoHere()->at(0));
		if (planet != NULL)
			ch2 = planet;

		const deque<SpaceObject *> & deq = *ch->GetAlsoHere();
		int i;
		for (i = 0; i < deq.size(); ++i) {
			Packet * packet = dynamic_cast<Packet *>(deq[i]);
			if (packet != NULL) {
				return packet;
			}
		}
	}

	if (ch->GetOwner() == ch2->GetOwner())
		return ch2;

	int i;
	for (i = 0; i < mJettison.size(); ++i) {
		if (ch->IsWith(*mJettison[i]) && mJettison[i]->GetRealCH() == ch2)
			break;
	}

	if (i == mJettison.size())
		mJettison.push_back(new TempFleet(ch, ch2));

	return mJettison[i];
}

void Galaxy::CleanJettison()
{
	int i;
	CargoHolder * ch;
	for (i = 0; i < mJettison.size(); ++i) {
		ch = mJettison[i]->GetRealCH();
		if (ch == NULL) {
			delete mJettison[i];
			continue;
		}

		assert(mJettison[i]->GetPopulation() >= 0);
		if (mJettison[i]->GetPopulation() > 0) {
			Planet * p = dynamic_cast<Planet *>(ch);
			if (p == NULL) {
				delete mJettison[i];
				continue;
			}

			p->Invade(mJettison[i]->NCGetOwner(), mJettison[i]->GetPopulation());
		}

		assert(mJettison[i]->GetFuel() >= 0);
		if (mJettison[i]->GetFuel() > 0) {
			Fleet * f = dynamic_cast<Fleet *>(ch);
			if (f == NULL) {
				delete mJettison[i];
				continue;
			}

			f->AdjustFuel(mJettison[i]->GetFuel());
		}

		for (int ct = 0; ct < Rules::MaxMinType; ++ct) {
			assert(mJettison[i]->GetContain(ct) >= 0);
			ch->AdjustAmounts(ct, mJettison[i]->GetContain(ct));
		}

		delete mJettison[i];
	}
}

void Galaxy::RepairBases()
{
	int i;
	for (i = 0; i < mPlanets.size(); ++i)
		mPlanets[i]->RepairBase();
}

void Galaxy::Instaform()
{
	int i;
	for (i = 0; i < mPlanets.size(); ++i)
		mPlanets[i]->Instaform();
}

void Galaxy::MovePackets(bool firstyear)
{
	int i;
	for (i = 0; i < mPackets.size(); ++i) {
		if (mPackets[i]->Move(firstyear)) {
			delete mPackets[i];
			mPackets.erase(mPackets.begin() + i);
			--i;
		}
	}
}

void Galaxy::AddPacket(Packet * packet, const Planet * planet)
{
	Message * mess = packet->NCGetOwner()->AddMessage("Packet built", planet);
	int i;
	for (i = 0; i < mPackets.size(); ++i) {
		if (mPackets[i]->IsWith(*packet) &&
			mPackets[i]->GetOwner() == packet->GetOwner() &&
			mPackets[i]->GetDestination() == packet->GetDestination() &&
			mPackets[i]->GetSpeed() == packet->GetSpeed())
		{
			for (int ct = 0; ct < Rules::MaxMinType; ++ct)
				mPackets[i]->AdjustAmounts(ct, packet->GetContain(ct));

			delete packet;
			mess->AddItem("Added to", mPackets[i]);
			return;
		}
	}

	mess->AddItem("Packet", packet);
	mPackets.push_back(packet);
}

const Wormhole * Galaxy::GetWormhole(long id) const
{
	int i;
	for (i = 0; i < mWormholes.size(); ++i) {
		if (mWormholes[i]->GetID() == id)
			return mWormholes[i];
	}

	return NULL;
}

void Galaxy::JiggleWormholes()
{
	int i;
	for (i = 0; i < mWormholes.size(); ++i)
		mWormholes[i]->Jiggle();
}

void Galaxy::AdjustWormholeTraverse()
{
	int i;
	for (i = 0; i < mWormholes.size(); ++i)
		mWormholes[i]->AdjustTraverse();
}

int Galaxy::GetPlanetsWithin(const Location * l, double range) const
{
	int Result = 0;
	int i;

	for (i = 0; i < mPlanets.size(); ++i) {
		if (mPlanets[i]->Distance(l) <= range)
			++Result;
	}

	return Result;
}

void Galaxy::SweepMines()
{
	int i;

	for (i = 0; i < mPlanets.size(); ++i)
		mPlanets[i]->SweepMines();
}

SpaceObject * Galaxy::GetPatrolTarget(const Fleet * persuer, int range) const
{
	SpaceObject * Result = NULL;
	double minrange = range;

	int i;
	for (i = 0; i < mPackets.size(); ++i) {
		if (!mPackets[i]->SeenBy(persuer->GetOwner()))
			continue;

		double dist = persuer->Distance(mPackets[i]);
		if (dist > minrange)
			continue;

		// FUTURE: Patrol vs packets
	}

	return Result;
}

}
