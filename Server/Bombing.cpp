/*
Copyright 2003 - 2005 Grant Speelman, Elliott Kleinrock

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

*/
#include <map>

#include "FSServer.h"

#include "Bombing.h"
#include "Component.h"
#include "algorithm"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {
Bombing::Bombing(const Location & loc) : Location(loc)
{
	mTargetPlanet = NULL;
	mPopulationLoss = 0;
	mMinesLoss = mFactoriesLoss = mDefensesLoss = 0;
}


Bombing::~Bombing()
{
}

void Bombing::SetPlanet(Planet* targetPlanet)
{
	mTargetPlanet = targetPlanet;
}

void Bombing::AddFleets()
{
	// Find all fleets orbiting the planet that want to Bomb it
	for (unsigned int j = 0; j < TheGame->NumberPlayers(); ++j )
	{
		Player* player = TheGame->NCGetPlayer(j);
			
		if(player->AddBombingFleets(this,mTargetPlanet->GetOwner(),BT_NORMAL))
			mNormalBombingPlayers.push_back(player);
			
		if(player->AddBombingFleets(this,mTargetPlanet->GetOwner(),BT_SMART))
			mSmartBombingPlayers.push_back(player);
		
		if(player->AddBombingFleets(this,mTargetPlanet->GetOwner(),BT_TERRA))
			mTerraBombingPlayers.push_back(player);
	}
}

double Bombing::GetSmartKillPercentage() const
{
	// Smart bombs do *not* add linearly; instead, they use this formula:
	// (1 - multiply[ (1 - kill_perc(n)^#n) ])

	// Where "multiply[x(n)]" is the math "big-pi" operator, which means
	// multiply all the terms together, i.e.:
  	//multiply[x(n)] = x(n1)*x(n2)*x(n3)... *x(ni) 
	
	map<const Component*,int> bombtype_count;

	// OPTIMIZE This could possible be optimize
	for (deque<Fleet*>::const_iterator k = mSmartBombingFleets.begin(); k != mSmartBombingFleets.end(); ++k)
	{
		for(deque<Stack>::const_iterator i = (*k)->mStacks.begin(); i != (*k)->mStacks.end(); ++i)
		{
			for(deque<Slot>::const_iterator j = i->mShip->mSlots.begin(); j != i->mShip->mSlots.end(); ++j )
			{
				const Component* comp = j->GetComp();
				if(comp->IsType(CT_BOMB) && comp->GetBombType() == BT_SMART)
				{
					if(bombtype_count.count(comp) == 0)
						bombtype_count[comp] = 1;
					else
						bombtype_count[comp]++;
				}
			}
		}
	}
	
	double killPercentage = 1;
	for (map<const Component*,int>::const_iterator t = bombtype_count.begin(); t != bombtype_count.end(); ++t)
	{
		double base = t->first->GetSmartKillPercentage();
		double result = 1 - pow(base,t->second);
		killPercentage *= result;
	}
	
	return killPercentage;
}

void Bombing::DoPopulationLosses(const double& normal_KillPercentage, const long& normal_minimum , const double& smart_KillPercentage)
{
	long population_loss = 0;
	long population = mTargetPlanet->GetPopulation();
	double normal_coverage = 1.0 - mTargetPlanet->GetDefenseValue();
	double smart_coverage = 1.0 - mTargetPlanet->GetSmartDefenseValue();
	
	// KillPercentage or minimum for normal bombing
	if((normal_KillPercentage * normal_coverage * population) > (normal_minimum * normal_coverage))
		population_loss += static_cast<long>(normal_KillPercentage * normal_coverage * population);
	else
		population_loss += static_cast<long>(normal_minimum * normal_coverage);
	
	// add smart bombing KillPercentage
	population_loss += static_cast<long>(smart_KillPercentage * smart_coverage);
	
	//Apply Losses
	mTargetPlanet->AdjustPopulation(-mPopulationLoss);
}

void Bombing::DoInstallationLosses(const long& installation_loss)
{
	long installations_sum = mTargetPlanet->GetFactories() + mTargetPlanet->GetMines() + mTargetPlanet->GetDefenses();
	double coverage = 1.0 - mTargetPlanet->GetInstallationDefenseValue();
	
	// Calculate Losses
	if(long(installation_loss * coverage) > installations_sum)
	{
		mFactoriesLoss = mTargetPlanet->GetFactories();
		mMinesLoss = mTargetPlanet->GetMines();
		mDefensesLoss = mTargetPlanet->GetDefenses();
	}
	else
	{
		double factories_ratio, mines_ratio, defenses_ratio;
		factories_ratio = mTargetPlanet->GetFactories() / installations_sum;
		mines_ratio = mTargetPlanet->GetMines() / installations_sum;
		defenses_ratio = mTargetPlanet->GetDefenses() / installations_sum;
		
		mFactoriesLoss = long(factories_ratio * installation_loss + 0.5);
		mDefensesLoss = long(defenses_ratio * installation_loss + 0.5);
		mMinesLoss = long(mines_ratio * installation_loss + 0.5);
	}
	
	// Apply Losses
	mTargetPlanet->AdjustFactories(-mFactoriesLoss);
	mTargetPlanet->AdjustFactories(-mMinesLoss);
	mTargetPlanet->AdjustDefenses(-mDefensesLoss);
}

void Bombing::DoTerraTransforming()
{
	for (deque<Fleet*>::iterator k = mTerraBombingFleets.begin(); k != mTerraBombingFleets.end(); ++k)
	{
		(*k)->RemoteTerraform(true);
	}
}

void Bombing::Resolve()
{
	// Are there any Fleets
	if( mSmartBombingFleets.size() == 0 && mNormalBombingFleets.size() == 0 && mTerraBombingFleets.size() == 0)
		return;
		
	// Calculate Total bombing  
	double normal_KillPercentage = 0.0;
	double smart_KillPercentage = 0.0;
	long normal_minimum = 0;
	long installation_loss = 0;
	
	// Calculate Normal bombing (Sum up bombing)
	for (deque<Fleet*>::iterator k = mNormalBombingFleets.begin(); k != mNormalBombingFleets.end(); ++k)
	{
		normal_KillPercentage += (*k)->GetNormalKillPercentage();
		normal_minimum += (*k)->GetKillMin();
		installation_loss += (*k)->GetKillInstallation();
	}
	
	smart_KillPercentage = GetSmartKillPercentage();
	
	// percentage stopped 
	mNormalPercentageStopped = mTargetPlanet->GetDefenseValue();
	mSmartPercentageStopped = mTargetPlanet->GetSmartDefenseValue();
	
	// Population Loss
	DoPopulationLosses(normal_KillPercentage,normal_minimum,smart_KillPercentage);
	
	// Installation Losses 
	DoInstallationLosses(installation_loss);
	
	// check if planet is now uninhabited later in turn processing, don't check now
	
	// Do Remote Terrraforming
	DoTerraTransforming();
			
	// Send Messages To Everyone
	SendMessages();
}

void Bombing::SendNormalMessages() 
{
	// Check if there was a bomb
	if(mNormalBombingPlayers.size() == 0)
		return;
		
	// First to Planet Owner
	Message * mess = mTargetPlanet->AddMessageOwner("PlanetBombed");
	mess->AddItem("Target", this);
	mess->AddLong("Population lost", mPopulationLoss);
	mess->AddFloat("Defense effectiveness", mNormalPercentageStopped);
	mess->AddLong("Factories lost", mFactoriesLoss);
	mess->AddLong("Mines lost", mMinesLoss);
	mess->AddLong("Defenses lost", mDefensesLoss);
	
	// Other Players
	for (deque<Player*>::iterator i = mNormalBombingPlayers.begin(); i != mNormalBombingPlayers.end(); ++i)
	{
		Message * mess = (*i)->AddMessage("FleetBombed");
		mess->AddItem("Target", this);
		mess->AddLong("Population lost", mPopulationLoss);
		mess->AddFloat("Defense effectiveness", mNormalPercentageStopped);
		mess->AddLong("Installations lost", mFactoriesLoss + mMinesLoss + mDefensesLoss);
	}
}

void Bombing::SendTerraMessages() 
{
	// Check if there was a bomb
	if(mTerraBombingPlayers.size() == 0)
		return;
		
	// First to Planet Owner
	Message * mess = mTargetPlanet->AddMessageOwner("PlanetTerraBombed");
	mess->AddItem("Target", this);
	///@todo add needed data

	
	// Other Players
	for (deque<Player*>::iterator i = mTerraBombingPlayers.begin(); i != mTerraBombingPlayers.end(); ++i)
	{
		Message * mess = (*i)->AddMessage("FleetTerraBombed");
		mess->AddItem("Target", this);
		///@todo add needed data

	}
}


void Bombing::SendSmartMessages() 
{
	// Check if there was a bomb
	if(mSmartBombingPlayers.size() == 0)
		return;
		
	// First to Planet Owner
	Message * mess = mTargetPlanet->AddMessageOwner("PlanetSmartBombed");
	mess->AddItem("Target", this);
	mess->AddLong("Population lost", mPopulationLoss);
	mess->AddFloat("Defense effectiveness", mSmartPercentageStopped);
	
	// Other Players
	for (deque<Player*>::iterator i = mSmartBombingPlayers.begin(); i != mSmartBombingPlayers.end(); ++i)
	{
		Message * mess = (*i)->AddMessage("FleetSmartBombed");
		mess->AddItem("Target", this);
		mess->AddLong("Population lost", mPopulationLoss);
		mess->AddFloat("Defense effectiveness", mSmartPercentageStopped);
	}
}

}
