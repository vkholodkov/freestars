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

/**
 **@file Galaxy.h
 **@ingroup Server
 **@brief Galaxy creation and management.
 **/

#if !defined(FreeStars_Galaxy_h)
#define FreeStars_Galaxy_h

#include <deque>

using namespace std;

namespace FreeStars {
class Location;
class Planet;
class Salvage;
class Wormhole;
class Packet;
class Creation;
class TempFleet;
class SpaceObject;
class CargoHolder;
class Game;

/**
 * The galaxy.
 * @ingroup Server
 */
class Galaxy {
public:
	Galaxy() : game(0), mMaxX(0), mMaxY(0) {}
	virtual ~Galaxy();

    void SetGame(Game *_game) { game = _game; }

	bool ParseNode(const TiXmlNode * node);
	void WriteNode(TiXmlNode * node, const Player * viewer) const;
	void WriteXYFile(TiXmlNode * node);
	/**
	 * Search for planet at postition.
	 */
	Planet * GetPlanet(int posX, int posY);
	/**
	 * Search for planet at the same location as the specified object.
	 * @param loc An object in orbit of a planet.
	 * @returns The planet, or NULL if there was no planet at that location.
	 */
	Planet * GetPlanet(const SpaceObject * loc);
	/**
	 * Search for planet by name.
	 * @note OPTIMIZE: probably almost all calls of this could call with an id instead
	 * @param name Name to search on.
	 * @returns The planet, or NULL if there was no planet by that name.
	 */
	Planet * GetPlanet(const char * name);
	
	/**
	 * Get planet by ID.
	 * @note This is the fastest way to get a Planet object.
	 * @param id The ID number of the planet to retrieve.
	 * @returns The planet, or NULL if the ID was out of range.
	 */
	Planet * GetPlanet(int id) const { return (id < 1 || id > mPlanets.size()) ? NULL : mPlanets[id-1]; }
	
	/**
	 * Count planets near a location.
	 * @param l The location to center the search on.
	 * @param range The search radius.
	 * @returns The number of planets that match the criteria.
	 */
	int GetPlanetsWithin(const Location * l, double range) const;

	void AddInvasion(Planet * planet);
	void ResolveInvasions();

	void AddSalvage(Salvage * salvage)	{ mScrap.push_back(salvage); }
	Salvage * AddSalvage(const CargoHolder & ch);
	Salvage * GetSalvage(long n) const;
	void DecaySalvage();
	void SweepMines();

	void MovePackets(bool firstyear);
	void AddPacket(Packet * packet, const Planet * planet);
	const Packet * GetPacket(long n) const;
	SpaceObject * GetPatrolTarget(const Fleet * persuer, int range) const;

	bool Mine();		///< Do galaxy-wide mining.
	void GrowPop();		///< Do galaxy-wide population growth.
	void DoProduction();///< Do galaxy-wide production.
	void TechSpent(long TechGain, TechType tech); ///< Do tech research procedure.
	void GainSpyTech(Player * player); ///< Award tech for players that have a SpyTechBonus.
	void DeadCheck();	///< Check population and update the habited status.

	void RepairBases();	///< Do galaxy-wide starbase repair.
	void Instaform();	///< Do galaxy-wide instaform checks.
	
	void DoBombing();	///< Do galaxy-wide bombing activity/calculations.
	long GetSalvageID() const;	///< Generate an unused Salvage ID.
	long GetPacketID() const;	///< Generate an unused Packet ID.
	long GetWormholeID() const;	///< Generate an unused Wormhole ID.
	/// Get the next unused Planet ID.
	long GetNextPlanetID() const	{ return mPlanets.size()+1; }

	/// Get the number of planets in the galaxy.
	int GetPlanetCount() const	{ return mPlanets.size(); }
	void Build(Creation * c, Game*);	///< Create the universe.
	void PlacePlayer(Player * player);
	const Planet * ClosestPlanet(const Location * loc);
	long MinX() const		{ return mMinX; }
	long MinY() const		{ return mMinY; }
	long MaxX() const		{ return mMaxX; }
	long MaxY() const		{ return mMaxY; }
	void LoadPlanets();
	void LoadScrap();
	void LoadPackets();
	void LoadWormholes();
	const Wormhole * GetWormhole(long id) const;
	void JiggleWormholes();
	void AdjustWormholeTraverse();

	CargoHolder * GetJettison(const CargoHolder * ch, CargoHolder * ch2);
	void CleanJettison();
	bool ParseSize(const TiXmlNode * node);

protected:
    Game *game;
	deque<Planet *> mPlanets;
	deque<Planet *> invasions;
	deque<Salvage *> mScrap;
	deque<Packet *> mPackets;
	deque<Wormhole *> mWormholes;
	deque<TempFleet *> mJettison;
	deque<long> mTechSpent;

	//friend void Player::DoBombing();
	long mMinX;
	long mMinY;
	long mMaxX;
	long mMaxY;
};
}

#endif // !defined(FreeStars_Galaxy_h)
