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

/**
 **@file Bombing.h
 **@ingroup Server
 **@brief Bombing.
 **/

#if !defined(FreeStars_Bombing_h)
#define FreeStars_Bombing_h

namespace FreeStars {

/**
 * Bombing.
 * Bombing is the act of killing enemy colonists by dropping
 * bombs from orbit.
 * @ingroup Server
 */
class Bombing : public Location
{
public:
    Bombing(const Location & loc);
    ~Bombing();

    void SetPlanet(Planet* targetPlanet);
	void AddFleets();
	void AddTerraFleet(Fleet* f){mTerraBombingFleets.push_back(f);}
	void AddSmartFleet(Fleet* f){mSmartBombingFleets.push_back(f);}
	void AddNormalFleet(Fleet* f){mNormalBombingFleets.push_back(f);}
	void Resolve();
	
private:
	void SendMessages(){SendSmartMessages();SendTerraMessages();SendNormalMessages();}
	void SendSmartMessages();
	void SendTerraMessages();
	void SendNormalMessages();
	void DoPopulationLosses(const double& normal_KillPercentage, const long& normal_minimum , const double& smart_KillPercentage);
	void DoInstallationLosses(const long& installation_loss);
	void DoTerraTransforming();
	double GetSmartKillPercentage() const;

	Planet* mTargetPlanet;	///< The planet targeted for bombing.
	
	deque<Player*> mSmartBombingPlayers;	///< The players that take part in smart bombing.
	deque<Player*> mNormalBombingPlayers; 	///< The players that take part in normal bombing.
	deque<Player*> mTerraBombingPlayers;	///< The players that take part in terra bombing.
	
	deque<Fleet*> mSmartBombingFleets;	///< The fleets that take part in smart bombing.
	deque<Fleet*> mNormalBombingFleets;	///< The fleets that take part in normal bombing.
	deque<Fleet*> mTerraBombingFleets;	///< The fleets that take part in terra bombing.
	
	
	long mPopulationLoss;	///< The amount of people killed
	long mDefensesLoss, mMinesLoss, mFactoriesLoss;	///< The amount of installations destroyed
	bool mKilledAll;	///< Was the entire population killed?
	double mNormalPercentageStopped; ///< Percentage bombs stopped
	double mSmartPercentageStopped; ///< Percentage smart bombs stopped

};
}
#endif // !defined(FreeStars_Bombing_h)
