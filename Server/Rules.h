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
 **@file Rules.h
 **@ingroup Server
 **@brief Game rules.
 **/

#if !defined(FreeStars_Rules_h)
#define FreeStars_Rules_h

#include <deque>
#include <string>
#include <map>

using namespace std;

namespace FreeStars {
class Player;
class Planet;
class Fleet;
class MineFieldType;

/**
 * Array class.
 * @ingroup Server
 */
template <class Type> class Array {
public:
	Array(int start) : First(start) {}
	virtual ~Array() {}
	void AddItem(Type item)	{ Items.push_back(item); }
	Type operator [] (unsigned int pos) const	{
		if (pos <= First)
			return Items[0];
		else if (pos > Items.size() + First)
			return Items.back();
		else
			return Items[pos-First];
	}

	int size() const { return Items.size(); }

private:
	deque<Type> Items;
	int First;
};

/**
 * Rules.
 * Rules define game constraints and behavior.
 * @ingroup Server
 */
class Rules {
public:
	static void Init();
	static void Cleanup();

	static deque<unsigned long> TechCost;
	static unsigned long MaxTechLevel;
	static long MaxTechType;
	static long MaxMinType;
	static long MaxHabType;
	static long MaxMineType;
	static unsigned long MaxFleets;
	static long PopEQ1kT;

	static TechType TechFleet(const Player * player, const Fleet * fleet);
	static double ScrapRecover(const Planet * planet, bool colonize);
	static long ScrapResource(const Planet * planet);
	static bool TechScrap(const Planet * planet);
	static TechType TechInvasion(Player * invader, const Player * owner);
	static bool Stealable(CargoType ct);
	static long CloakValue(long Cloaking, long Mass);
	static double CalcScanning(double base, long newscan, long count);

	static void ReadCargo(const TiXmlNode * node, deque<long> & q, long * pop);
	static TiXmlElement * WriteCargo(TiXmlNode * node, const char * name, const deque<long> &q, long pop);

	static bool LoadRules(const TiXmlNode * node, const char * file, const char * verify, double version);
	static void WriteRulesFile(TiXmlNode * node);
	static long GetConstant(const string name, long Default = 0);
	static double GetFloat(const string name, double Default = 0.0);
	static long GetArrayValue(const string name, int position);
	static double GetArrayFloat(const string name, int position);

	static void ReadTurnOrder(const TiXmlNode * node);
	static long GetTurnEvent(long Phase)	{ return mTurnOrder[Phase]; }
	static long GetNumberTurnEvents()		{ return mTurnOrder.size(); }

	static long TechID(const char * name);
	static long MineralID(const char * name);
	static long HabID(const char * name);
	static CargoType GetCargoType(const char * name);
	static const MineFieldType * GetMineFieldType(int type);
	static long MineID(const char * name);

	static double OverGateRange(long range, long dist);
	static double OverGateMass(long cap1, long cap2, long mass);

	static const string GetCargoName(long type);
	static const string GetHabName(long type)
	{	if (type < MaxHabType)
			return HabName[type];
		else
			return "BadName";
	}
	static const string GetTechName(long type);

	static TiXmlElement * WriteArray(const char * node, const deque<long> & q, long Type);
	static TiXmlElement * WriteArrayFloat(const char * node, const deque<double> & q, long Type);
	static bool ParseArray(const TiXmlNode * node, deque<long> & q, long Type);
	static bool ParseArrayFloat(const TiXmlNode * node, deque<double> & q, long Type);

	static TiXmlElement * WriteArrayBool(const char * node, const char * name, const char * Attrib, const deque<bool> & arr, const deque<string> * desc = NULL);
	static TiXmlElement * WriteArray(const char * node, const char * name, const char * Attrib, const deque<long> & arr, const deque<string> * desc = NULL);
	static TiXmlElement * WriteArrayFloat(const char * node, const char * name, const char * Attrib, const deque<double> & arr, const deque<string> * desc = NULL);
	static bool ParseArrayBool(const TiXmlNode * node, const char * name, const char * Attrib, deque<bool> & arr, const deque<string> * desc = NULL);
	static bool ParseArray(const TiXmlNode * node, const char * name, const char * Attrib, deque<long> & arr, const deque<string> * desc = NULL);
	static bool ParseArrayFloat(const TiXmlNode * node, const char * name, const char * Attrib, deque<double> & arr, const deque<string> * desc = NULL);

	static bool ParsePacketMapping(const TiXmlNode * node, deque<long> & array);
	static long RandomHab(HabType ht);
	static long GetSecondHab(HabType ht, const Player * owner);
	static bool ParseMinSettings(const TiXmlNode * node);
	static void WriteMinSettings(TiXmlNode * node);
	static long MinMC(long i)	{ return mMinMC[i]; }
	static long MaxMC(long i)	{ return mMaxMC[i]; }
	static long HWMinMC(long i)	{ return mHWMinMC[i]; }
	static long HWMaxMC(long i)	{ return mHWMaxMC[i]; }
	static long HWFloorMC(long i)	{ return mHWFloorMC[i]; }
	static deque<long> * GetHabOddArray(HabType ht, bool Create = false);
	static long GetHWMC(long mintype);
	static long GetHWStartMinerals(long mintype);
	static long GetSWStartMinerals(long mintype);

	static long MultiOdds(double odds, long trials);

	static string GetModFileName()	{ return ModFileName; }

private:
	static deque<string> TechName;
	static deque<string> HabName;
	static deque<string> MineralName;
	static map<string, long, less<string> > Consts;
	static map<string, double, less<string> > Floats;
	static map<string, Array<long> *, less<string> > Arrays;
	static map<string, Array<double> *, less<string> > FloatArrays;
	static string ModFileName;
	static double ModFileVersion;
	static string ModFileCRC;
	static deque<long> mTurnOrder;

	static deque<deque<long> *> mHabOdds;
	static deque<long> mMinMC;
	static deque<long> mMaxMC;
	static deque<long> mHWMinMC;
	static deque<long> mHWMaxMC;
	static deque<long> mHWFloorMC;
	static deque<long> mHWSetupMC;
	static deque<long> mHWSetupSM;
	static deque<MineFieldType *> mMineFieldTypes;
};

// Types of arrays
const long MINERALS	= 1;
const long TECHS	= 2;
const long HABS		= 3;

// Non mineral cargo Types
const CargoType POPULATION	= -1;
const CargoType FUEL		= -2;
const CargoType RESOURCES	= -2; /**<@bug Woah, there! Shouldn't this be different than FUEL?
See http://sourceforge.net/mailarchive/forum.php?thread_id=7607156&forum_id=42960 for context... */
}
#endif // !defined(FreeStars_Rules_h)
