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
 **@file Game.h
 **@ingroup Server
 **@brief Game master object.
 **/

#if !defined(FreeStars_Game_h)
#define FreeStars_Game_h

#include "WayOrderList.h"
#include "Component.h"
#include "Player.h"
#include "VictoryConditions.h"
#include "MessageSink.h"

class TiXmlNode;

namespace FreeStars {
class RacialTrait;
class Creation;
class MineField;

/**
 * The game object.
 */
class Game : public MessageSink {
public:
	Game();
	virtual ~Game();
	double GetTechFactor() const	{ return TechFactor; }
	long GetTurnPhase() const	{ return mTurnPhase; }
	long GetTurn() const		{ return Turn; }
	bool LoadDefFile(const char * deffile);
	void PlacePlayers();
	void SetFileLocation(const char * hostfile);
	bool LoadHostFile(const char * hostfile);
	bool LoadPlayerFile(const char * playerfile);
	bool LoadXYFile();
	bool LoadRules(const char * file, const char * verify, double version, bool checkver);
	bool LoadRules(const char * file, const char * MainNode);
	bool LoadRacialTraits(const TiXmlNode * node);
	bool LoadComponents(const TiXmlNode * node);
	bool LoadCreation(const TiXmlNode * options);
	static bool LoadStartShips(const TiXmlNode * node);
//	bool LoadOptions();
//	bool LoadPlayer(const char * file);
	bool LoadTurns();
	bool ProcessOrders(long pnumber);
	bool ProcessWaypoints(long pnumber);
	bool AssignWaypoints();

	const Galaxy *GetGalaxy() const { return galaxy; }
	Galaxy *GetGalaxy() { return galaxy; }

	Player * GetCurrentPlayer()		{ return NCGetPlayer(mCurrentPlayer); }
	Player * NCGetPlayer(unsigned int n);
	const Player * GetPlayer(unsigned int n) const	{ return const_cast<Game *>(this)->NCGetPlayer(n); }	// Same thing just const
	unsigned int NumberPlayers()	{ return mNumberOfPlayers; }
	bool ProcessTurn();
	void WriteXYFile();
	bool WriteHostFile();
	bool WritePlayerFiles();
	long GetRandomEvents() const	{ return mRandomEvents; }

	Message * AddMessage(const string &type);
	void StoreMessageLocation(const Location * loc);

	const Component * ParseComponent(const char * name) const;
	const Component * GetBestComp(const Player * player, const char * ValueType, bool CheckRad, HullType hullType) const;
	const Component * GetBestComp(const Player * player, long vt, bool CheckRad, HullType hullType) const;
	const deque<Message *>& GetMessages() {return mMessages;}

//	enum FileLocation {
//		AnyLocation = 1,
//		GameDirOnly = 2,
//		GameNameOnly = 3,
//	};
//	string GetFile(const char * file, FileLocation fLoc = AnyLocation);
	string GetFileLoc() const		{ return mFileLoc; }
	string GetFileName() const		{ return mFileName; }
	const deque<Component *> & GetComponents() const { return mComponents; }

	long GetTerraLimit(const Player * player, HabType ht);

	Creation * GetCreation()	{ return mCreation; }

	long GetGameID() const		{ return mGameID; }
	static bool CheckMetaInfo(const TiXmlNode * node, const char * file, double fileversion);

	deque<SpaceObject *> * GetClosestTop(int x, int y, long max = 0);
	void AddAlsoHere(SpaceObject * loc);
	void MoveAlsoHere(SpaceObject * loc);
	void RemoveAlsoHere(SpaceObject * loc);

	void ReadTurnOrder(const TiXmlNode * node);
	void InitialSeen();
	void UpdateSeen();
	void ResetSeen();
	void UpdateLoadBy();
	long MinWormholes() const	{ return mWHMin; }
	long MaxWormholes() const	{ return mWHMax; }
	long GetWHMinDistance() const	{ return mWHMinDistance; }
	double GetPossibleMines(deque<MineField *> *pm, const Fleet * f, double dist) const;
	void SweepMines(SpaceObject * so, int sweep, const BattlePlan * bp);
	void DetonateMineField(const MineField * mf);
	SpaceObject * GetPatrolTarget(const Fleet * persuer, int range) const;

	virtual Component * ObjectFactory(const Component *);
	virtual Player * ObjectFactory(const Player *, int PlayerNumber);
	virtual Race * ObjectFactory(const Race *);
	virtual RacialTrait * ObjectFactory(const RacialTrait *);
	virtual Ship * ObjectFactory(const Ship *);
	virtual Planet * ObjectFactory(const Planet *);
	virtual Salvage * ObjectFactory(const Salvage *);
	virtual Packet * ObjectFactory(const Packet *);
	virtual Wormhole * ObjectFactory(const Wormhole *);

protected:
	long mCurrentPlayer;	///< current player for client display
	long mGameID;
	double TechFactor;		///< 1.0 for normal games, 2.0 for slow tech games
	long mTurnPhase;
	long Turn;
	string Name;
	string mFileLoc;
	string mFileName;
//	string GameDir;
	unsigned long mRandomEvents;
	long RandomEventsStart;		///< random events wait to start (all at once - execpt artifacts, they're either on or off)
	long mWHMin;
	long mWHMax;
	long mWHMinDistance;
	unsigned long PublicScore;
	long PublicScoreStart;		// rank score is either on or off, others wait for a start time
	unsigned int mNumberOfPlayers;

	deque<Component *> mComponents;
	deque<Player *> mPlayers;

	deque<WayOrderList> mOrders;
	deque<Message *> mMessages;
	deque<deque<SpaceObject *> *> mTopObjects;
	unsigned long * mSeed;

	Galaxy *galaxy;
	Creation * mCreation;
    VictoryConditions *VC;
};

// turn phases
const long TP_PARSEWAYPOINTS	=  10;
const long TP_SCRAPFLEET		=  20;
const long TP_WAY0_UNLOAD		=  30;
const long TP_WAY0_COLONIZE		=  40;
const long TP_WAY0_INVADE		=  50;
const long TP_WAY0_LOAD			=  60;
const long TP_TECH_CHECK		=  70;
const long TP_MT_MOVE			=  80;
const long TP_PACKETS1			=  90;
const long TP_DEADCHECK			=  95;
const long TP_MOVEMENT			= 100;
//const long TP_MOVEMENT_CYCLE	= 110;
const long TP_FREIGHTER_GROW	= 120;
const long TP_SALVAGEDECAY		= 140;
const long TP_WORMHOLES			= 150;
const long TP_RESETSEEN			= 155;
const long TP_MINES_DETONATE	= 160;
const long TP_MINING			= 171;
const long TP_PRODUCTION		= 182;
const long TP_SPYBONUS			= 190;
const long TP_REFUEL			= 200;
const long TP_POPULATION		= 210;
const long TP_INSTA_PACKETS		= 220;
const long TP_RANDOM_EVENTS		= 230;
const long TP_BATTLES			= 240;
const long TP_REMOTE_MINE		= 250;
const long TP_MEET_MT			= 260;
const long TP_BOMBING			= 270;
const long TP_REPAIR			= 280;
const long TP_WAY1_UNLOAD		= 290;
const long TP_WAY1_COLONIZE		= 300;
const long TP_WAY1_INVADE		= 310;
const long TP_UPDATELOADBY		= 315;
const long TP_WAY1_LOAD			= 320;
const long TP_MINELAYING		= 340;
const long TP_TRANSFER			= 350;
const long TP_MERGE				= 360;
const long TP_INSTA_TERRAFORM	= 370;
const long TP_MINEDECAY			= 380;
const long TP_MINESWEEP			= 390;
const long TP_REMOTE_TERRAFORM	= 400;
const long TP_UPDATESCANNING	= 405;
const long TP_PATROL			= 410;
}

extern FreeStars::Game * TheGame;

#endif // !defined(FreeStars_Game_h)
