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

#include "Creation.h"
#include "MineFieldType.h"

#include <stdlib.h>

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

long Rules::MaxTechType;
unsigned long Rules::MaxTechLevel;
long Rules::MaxHabType;
long Rules::MaxMinType;
long Rules::MaxMineType;
unsigned long Rules::MaxFleets;
long Rules::PopEQ1kT;

map<string, long, less<string> > Rules::Consts;
map<string, double, less<string> > Rules::Floats;
map<string, Array<long> *, less<string> > Rules::Arrays;
map<string, Array<double> *, less<string> > Rules::FloatArrays;

deque<unsigned long> Rules::TechCost;
deque<string> Rules::TechName;
deque<string> Rules::HabName;
deque<string> Rules::MineralName;

string Rules::ModFileName;
double Rules::ModFileVersion;
string Rules::ModFileCRC;

deque<deque<long> *> Rules::mHabOdds;
deque<long> Rules::mMinMC;
deque<long> Rules::mMaxMC;
deque<long> Rules::mHWMinMC;
deque<long> Rules::mHWMaxMC;
deque<long> Rules::mHWFloorMC;
deque<long> Rules::mHWSetupMC;
deque<long> Rules::mHWSetupSM;
deque<MineFieldType *> Rules::mMineFieldTypes;
deque<long> Rules::mTurnOrder;

bool ArrayParser::ParseArrayBool(const TiXmlNode * node, const char * name, const char * Attrib, deque<bool> & arr, const deque<string> * desc /*= NULL*/)
{
	if (!node)
		return false;

	bool IsOK = true;
	const TiXmlNode * child;
	for (child = node->FirstChild(name); child; child = child->NextSibling(name)) {
		const TiXmlElement * el = child->ToElement();
		if (el == NULL) {
			Message * mess = messageSink.AddMessage("Error: Invalid element in ParseArrayBool");
			mess->AddItem("", name);
			IsOK = false;
			continue;
		}

		unsigned int index;
		if (desc == NULL) {
			el->Attribute(Attrib, (int*)(&index));
		} else {
			const char * type;
			type = el->Attribute(Attrib);
			if (type == NULL) {
				Message * mess = messageSink.AddMessage("Error: Invalid attribute in ParseArrayBool");
				mess->AddItem(name, Attrib);
				IsOK = false;
				continue;
			}

			deque<string>::const_iterator in;
			in = find(desc->begin(), desc->end(), type);
			if (in == desc->end()) {
				Message * mess = messageSink.AddMessage("Error: Invalid attribute in ParseArrayBool");
				mess->AddItem(name, Attrib);
				IsOK = false;
				continue;
			}

			index = in - desc->begin() + 1;
		}

		if (index < 1 || index > arr.size()) {
			Message * mess = messageSink.AddMessage("Error: Invalid index in ParseArrayBool");
			mess->AddLong(name, index);
			IsOK = false;
			continue;
		}

		arr[index-1] = GetBool(child);
	}

	return IsOK;
}

bool ArrayParser::ParseArray(const TiXmlNode * node, const char * name, const char * Attrib, deque<long> & arr, const deque<string> * desc /*= NULL*/)
{
	if (!node)
		return false;

	bool IsOK = true;
	const TiXmlNode * child;
	for (child = node->FirstChild(name); child; child = child->NextSibling(name)) {
		const TiXmlElement * el = child->ToElement();
		if (el == NULL) {
			Message * mess = messageSink.AddMessage("Error: Invalid element in ParseArray");
			mess->AddItem("", name);
			IsOK = false;
			continue;
		}

		unsigned int index;
		if (desc == NULL) {
			el->Attribute(Attrib, (int*)(&index));
		} else {
			const char * type;
			type = el->Attribute(Attrib);
			if (type == NULL) {
				Message * mess = messageSink.AddMessage("Error: Invalid attribute in ParseArray");
				mess->AddItem(name, Attrib);
				IsOK = false;
				continue;
			}

			deque<string>::const_iterator in;
			in = find(desc->begin(), desc->end(), type);
			if (in == desc->end()) {
				Message * mess = messageSink.AddMessage("Error: Invalid attribute in ParseArray");
				mess->AddItem(name, Attrib);
				IsOK = false;
				continue;
			}

			index = in - desc->begin() + 1;
		}

		if (index < 1 || index > arr.size()) {
			Message * mess = messageSink.AddMessage("Error: Invalid index in ParseArray");
			mess->AddLong(name, index);
			IsOK = false;
			continue;
		}

		arr[index-1] = GetLong(child);
	}

	return IsOK;
}

bool ArrayParser::ParseArrayFloat(const TiXmlNode * node, const char * name, const char * Attrib, deque<double> & arr, const deque<string> * desc /*= NULL*/)
{
	if (!node)
		return false;

	bool IsOK = true;
	const TiXmlNode * child;
	for (child = node->FirstChild(name); child; child = child->NextSibling(name)) {
		const TiXmlElement * el = child->ToElement();
		if (el == NULL) {
			Message * mess = messageSink.AddMessage("Error: Invalid element in ParseArray");
			mess->AddItem("", name);
			IsOK = false;
			continue;
		}

		unsigned int index;
		if (desc == NULL) {
			el->Attribute(Attrib, (int*)(&index));
		} else {
			const char * type;
			type = el->Attribute(Attrib);
			if (type == NULL) {
				Message * mess = messageSink.AddMessage("Error: Invalid attribute in ParseArray");
				mess->AddItem(name, Attrib);
				IsOK = false;
				continue;
			}

			deque<string>::const_iterator in;
			in = find(desc->begin(), desc->end(), type);
			if (in == desc->end()) {
				Message * mess = messageSink.AddMessage("Error: Invalid attribute in ParseArray");
				mess->AddItem(name, Attrib);
				IsOK = false;
				continue;
			}

			index = in - desc->begin() + 1;
		}

		if (index < 1 || index > arr.size()) {
			Message * mess = messageSink.AddMessage("Error: Invalid index in ParseArray");
			mess->AddLong(name, index);
			IsOK = false;
			continue;
		}

		arr[index-1] = GetDouble(child);
	}

	return IsOK;
}

void Rules::Init()
{
	MaxTechType = 0;
	MaxTechLevel = 0;
	MaxHabType = 0;
	MaxMinType = 0;
	MaxMineType = 0;
	MaxFleets = 512;
	PopEQ1kT = 100;
	ModFileName = "";
	ModFileVersion = 0.0;
	ModFileCRC = "";
}

void Rules::Cleanup()
{
	int i;
	for (i = 0; i < mHabOdds.size(); ++i)
		delete mHabOdds[i];
	mHabOdds.clear();

	for (i = 0; i < mMineFieldTypes.size(); ++i)
		delete mMineFieldTypes[i];
	mMineFieldTypes.clear();

	TechCost.clear();
	TechName.clear();
	HabName.clear();
	MineralName.clear();

	Consts.clear();
	Floats.clear();
	map<string, Array<long> *, less<string> >::iterator iml;
	for (iml = Arrays.begin(); iml != Arrays.end(); ++iml)
		delete iml->second;
	Arrays.clear();

	map<string, Array<double> *, less<string> >::iterator imd;
	for (imd = FloatArrays.begin(); imd != FloatArrays.end(); ++imd)
		delete imd->second;
	FloatArrays.clear();

//	delete ModFileName;
//	delete ModFileCRC;
	mMinMC.clear();
	mMaxMC.clear();
	mHWMinMC.clear();
	mHWMaxMC.clear();
	mHWFloorMC.clear();
	mHWSetupMC.clear();
	mHWSetupSM.clear();
	mTurnOrder.clear();
}

bool Rules::TechScrap(const Planet * planet)
{
	if (planet &&
		planet->GetOwner() &&
		planet->GetBaseNumber() >= 0 &&
		!planet->GetOwner()->HasGotTech() &&
		Randodd(GetFloat("TechEventChance")))
	{
		return true;
	} else
		return false;
}

TechType Rules::TechFleet(const Player * player, const Fleet * fleet)
{
	TechType TechGot = TECH_NONE;

	// Have we gotten tech already?
	if (!player->HasGotTech() && Randodd(GetFloat("TechEventChance")))
	{
		long Count = 0;
		for (long i = 0; i < Rules::MaxTechType; ++i) {	// check each field
			// if it's possible to gain, and we pass a coin toss
			if (player->GetTechLevel(i) < fleet->TechLevel(i) && Randodd(GetFloat("TechFieldChance")))
				if (Random(++Count) == 0)	// This will select between all that you could get, randomly
					TechGot = i;
		}

		if (!TechGot) {
			/* need to redo MT stuff
			for (long i = MIN_MTTECH; i < MAX_MTTECH; ++i)
			{
				long items = fleet->TechLevel(i);
				if (items > 25)
					items = 25;

				if (Random(200) < items) {	// half a percent chance per tech item.
					if (Random(++Count) == 0)	// This will select between all that you could get, randomly
						TechGot = i;
				}
			}
			*/
		}
	}

	return TechGot;
}

double Rules::ScrapRecover(const Planet * planet, bool colonize)
{
	if (planet == NULL)
		return 1.0 / 3.0;

	double percent;
	if (colonize)
		percent = .75;
	else if (planet->GetBaseNumber() >= 0) {
		if (planet->GetOwner()->UltimateRecycle())
			percent = .9;
		else
			percent = .8;
	} else {
		if (planet->GetOwner() && planet->GetOwner()->UltimateRecycle())
			percent = .45;
		else
			percent = 1.0 / 3.0;
	}

	return percent;
}

long Rules::ScrapResource(const Planet * planet)
{
	long percent;
	if (!planet || !planet->GetOwner()->UltimateRecycle())
		percent = 0;
	else if (planet->GetBaseNumber() >= 0)
		percent = 7000;
	else
		percent = 3500;

	return percent;
}

TechType Rules::TechInvasion(Player * invader, const Player * owner)
{
	TechType TechGot = TECH_NONE;

	// Have we gotten tech already?
	if (!invader->HasGotTech() && Randodd(GetFloat("TechEventChance")))
	{
		long Count = 0;
		for (long i = 0; i < Rules::MaxTechType; ++i) {	// check each field
			// if it's possible to gain, and we pass a coin toss
			if (invader->GetTechLevel(i) < owner->GetTechLevel(i) && Randodd(GetFloat("TechFieldChance")))
				if (Random(++Count) == 0)	// This will select between all that you could get, randomly
					TechGot = i;
		}
	}

	return TechGot;
}

bool Rules::Stealable(CargoType ct)
{
	return ct != POPULATION;
}

long Rules::CloakValue(long Cloaking, long Mass)
{
	long Cloak = Cloaking / Mass;
	long percent;
	if (Cloak <= 100)
		percent = Cloak / 2;
	else {
		Cloak -= 100;
		if (Cloak <= 200)
			percent = 50 + Cloak / 8;
		else {
			Cloak -= 200;
			if (Cloak <= 312)
				percent = 75 + Cloak / 24;
			else {
				Cloak -= 312;
				if (Cloak <= 512)
					percent = 88 + Cloak/64;
				else if (Cloak < 768)
					percent = 96;
				else if (Cloak < 1000)
					percent = 97;
				else
					percent = 98;
			}
		}
	}

	return percent;
}

double Rules::CalcScanning(double base, long newscan, long count)
{
	double Result = base;
	if (newscan > 0)
		Result = pow(pow(Result, 4) + pow((double)newscan, 4) * count, 0.25);
	else if (newscan == 0 && Result < 0)
		Result = 0;

	return Result;
}

void Rules::WriteRulesFile(TiXmlNode * node)
{
	TiXmlElement rulefile("Rules");
	AddString(&rulefile, "File", ModFileName.c_str());
	AddDouble(&rulefile, "Version", ModFileVersion);
	AddString(&rulefile, "Verification", ModFileCRC.c_str());
	node->InsertEndChild(rulefile);
}

bool Rules::LoadRules(const TiXmlNode * node, const char * file, const char * verify, double version, MessageSink &messageSink)
{
	if (!node)
		return false;

	ModFileName = file;
//	ModFileName = new string(file);
	ModFileVersion = version;
	if (verify)
		ModFileCRC = verify;
//		ModFileCRC = new string(verify);

	const TiXmlNode * child1;
	const TiXmlElement * el;
	const char * ptr;
//	string ptr;
	long val;
	double dval;
    ArrayParser arrayParser(messageSink);

	for (child1 = node->FirstChild(); child1; child1 = child1->NextSibling()) {
		if (child1->Type() == TiXmlNode::COMMENT)
			continue;

		el = child1->ToElement();
		if (el == NULL)
			continue;

		if (stricmp(child1->Value(), "Constant") == 0) {
			// <Constant Name="MaxShipDesigns">16</Constant>
			ptr = el->Attribute("Name");
			val = GetLong(child1);
			Consts.insert(pair<string, long>(ptr, val));
			if (stricmp(ptr, "MaxFleets") == 0)
				MaxFleets = val;
			else if (stricmp(ptr, "PopEQ1kT") == 0)
				PopEQ1kT = val;
		} else if (stricmp(child1->Value(), "Float") == 0) {
			// <Float Name="MineDecayRate">0.02</Float>
			ptr = el->Attribute("Name");
			dval = GetDouble(child1);
			Floats.insert(pair<string, double>(ptr, dval));
		} else if (stricmp(child1->Value(), "Array") == 0) {
			// <Array Name="PacketDecay" start="0"><Item>0</Item><Item>1000</Item><Item>2500</Item><Item>5000</Item></Array>
			const char * ptr = el->Attribute("Name");
			if (ptr == NULL)
				continue;

			int start;
			el->Attribute("Start", &start);
			Array<long> * arr = new Array<long>(start);
			const TiXmlNode * child2;
			for (child2 = child1->FirstChild("Item"); child2; child2 = child2->NextSibling("Item")) {
				if (child2->Type() == TiXmlNode::COMMENT)
					continue;

				arr->AddItem(GetLong(child2));
			}
			Arrays.insert(pair<string, Array<long> *>(ptr, arr));
		} else if (stricmp(child1->Value(), "FloatArray") == 0) {
			// <Array Name="PacketDecay" start="0"><Item>0</Item><Item>1000</Item><Item>2500</Item><Item>5000</Item></Array>
			const char * ptr = el->Attribute("Name");
			if (ptr == NULL)
				continue;

			int start;
			el->Attribute("Start", &start);
			Array<double> * arr = new Array<double>(start);
			const TiXmlNode * child2;
			for (child2 = child1->FirstChild("Item"); child2; child2 = child2->NextSibling("Item")) {
				if (child2->Type() == TiXmlNode::COMMENT)
					continue;

				arr->AddItem(GetDouble(child2));
			}
			FloatArrays.insert(pair<string, Array<double> *>(ptr, arr));
		} else if (stricmp(child1->Value(), "TechCost") == 0) {
			// <TechCost Level="1">50</TechCost
			int level = 0;
			el->Attribute("Level", &level);
			val = GetLong(child1);
			if (static_cast<int>(TechCost.size()) < level)
				TechCost.insert(TechCost.end(), level - TechCost.size(), 0);

			TechCost[level-1] = val;
			MaxTechLevel = max(MaxTechLevel, static_cast<unsigned long>(level));
		} else if (stricmp(child1->Value(), "TechName") == 0) {
			// <TechName>Energy</TechName>
			TechName.push_back(GetString(child1));
			++MaxTechType;
		} else if (stricmp(child1->Value(), "Habitat") == 0) {
			HabName.push_back(GetString(child1->FirstChild("Name")));
			deque<long> * hoa;
			hoa = Rules::GetHabOddArray(MaxHabType, true);
			arrayParser.ParseArray(child1->FirstChild("Array"), "TickOdds", "TickNumber", *hoa);
			long sum = 0;
			for (long i = 0; i < hoa->size(); ++i) {
				sum += (*hoa)[i];
				(*hoa)[i] = sum;
			}

			++MaxHabType;
		} else if (stricmp(child1->Value(), "MineralName") == 0) {
			MineralName.push_back(GetString(child1));
			++MaxMinType;
		} else if (stricmp(child1->Value(), "MineFieldType") == 0) {
			MineFieldType * mft = new MineFieldType;
			if (mft->ParseNode(child1)) {
				mft->SetID(MaxMineType);
				mMineFieldTypes.push_back(mft);
				++MaxMineType;
			} else
				delete mft;
		} else if (stricmp(child1->Value(), "PacketTerraformMinerals") == 0) {
			deque<long> array;
			if (ParsePacketMapping(child1, array, messageSink)) {
				Array<long> * arr = new Array<long>(0);
				for (int index = 0; index < MaxMinType; ++index)
					arr->AddItem(array[index]);

				Arrays.insert(pair<string, Array<long> *>("PacketTerraformMinerals", arr));
			}
		} else if (stricmp(child1->Value(), "OrderOfEvents") == 0) {
			ReadTurnOrder(child1);
		}
	}

	return true;
}

void Rules::ReadTurnOrder(const TiXmlNode * node)
{
	const TiXmlNode * child;
	for (child = node->FirstChild("Event"); child != NULL; child = child->NextSibling("Event")) {
		if (strcmp(GetString(child), "ScrapFleet") == 0)
			mTurnOrder.push_back(TP_SCRAPFLEET);
		else if (strcmp(GetString(child), "Waypoint0Unload") == 0)
			mTurnOrder.push_back(TP_WAY0_UNLOAD);
		else if (strcmp(GetString(child), "Waypoint0Colonize") == 0)
			mTurnOrder.push_back(TP_WAY0_COLONIZE);
		else if (strcmp(GetString(child), "Waypoint0Invade") == 0)
			mTurnOrder.push_back(TP_WAY0_INVADE);
		else if (strcmp(GetString(child), "Waypoint0Load") == 0)
			mTurnOrder.push_back(TP_WAY0_LOAD);
		else if (strcmp(GetString(child), "CheckForTechnology") == 0)
			mTurnOrder.push_back(TP_TECH_CHECK);
		else if (strcmp(GetString(child), "MysteryTraderMoves") == 0)
			mTurnOrder.push_back(TP_MT_MOVE);
		else if (strcmp(GetString(child), "InSpacePacketsMove") == 0)
			mTurnOrder.push_back(TP_PACKETS1);
		else if (strcmp(GetString(child), "CheckForDeadPlanets") == 0)
			mTurnOrder.push_back(TP_DEADCHECK);
		else if (strcmp(GetString(child), "Movement") == 0)
			mTurnOrder.push_back(TP_MOVEMENT);
		else if (strcmp(GetString(child), "FreighterPopulationGrows") == 0)
			mTurnOrder.push_back(TP_FREIGHTER_GROW);
		else if (strcmp(GetString(child), "SalvageDecay") == 0)
			mTurnOrder.push_back(TP_SALVAGEDECAY);
		else if (strcmp(GetString(child), "Wormholes") == 0)
			mTurnOrder.push_back(TP_WORMHOLES);
		else if (strcmp(GetString(child), "ResetScanning") == 0)
			mTurnOrder.push_back(TP_RESETSEEN);
		else if (strcmp(GetString(child), "DetonateMines") == 0)
			mTurnOrder.push_back(TP_MINES_DETONATE);
		else if (strcmp(GetString(child), "PlanetBasedMining") == 0)
			mTurnOrder.push_back(TP_MINING);
		else if (strcmp(GetString(child), "Production") == 0)
			mTurnOrder.push_back(TP_PRODUCTION);
		else if (strcmp(GetString(child), "SpyBonus") == 0)
			mTurnOrder.push_back(TP_SPYBONUS);
		else if (strcmp(GetString(child), "RefuelFleets") == 0)
			mTurnOrder.push_back(TP_REFUEL);
//		else if (strcmp(GetString(child), "CheckForTechnology") == 0)
//			mTurnOrder.push_back(TP_TECH_CHECK);
		else if (strcmp(GetString(child), "PlanetaryPopulationGrows") == 0)
			mTurnOrder.push_back(TP_POPULATION);
		else if (strcmp(GetString(child), "NewPacketsMove") == 0)
			mTurnOrder.push_back(TP_INSTA_PACKETS);
//		else if (strcmp(GetString(child), "CheckForDeadPlanets") == 0)
//			mTurnOrder.push_back(TP_DEADCHECK);
		else if (strcmp(GetString(child), "RandomEvents") == 0)
			mTurnOrder.push_back(TP_RANDOM_EVENTS);
		else if (strcmp(GetString(child), "Battles") == 0)
			mTurnOrder.push_back(TP_BATTLES);
		else if (strcmp(GetString(child), "RemoteMining") == 0)
			mTurnOrder.push_back(TP_REMOTE_MINE);
		else if (strcmp(GetString(child), "MeetMysterTrader") == 0)
			mTurnOrder.push_back(TP_MEET_MT);
		else if (strcmp(GetString(child), "Bombing") == 0)
			mTurnOrder.push_back(TP_BOMBING);
		else if (strcmp(GetString(child), "Repair") == 0)
			mTurnOrder.push_back(TP_REPAIR);
		else if (strcmp(GetString(child), "Waypoint1Unload") == 0)
			mTurnOrder.push_back(TP_WAY1_UNLOAD);
		else if (strcmp(GetString(child), "Waypoint1Colonize") == 0)
			mTurnOrder.push_back(TP_WAY1_COLONIZE);
		else if (strcmp(GetString(child), "Waypoint1Invade") == 0)
			mTurnOrder.push_back(TP_WAY1_INVADE);
		else if (strcmp(GetString(child), "UpdateLoadBy") == 0)
			mTurnOrder.push_back(TP_UPDATELOADBY);
		else if (strcmp(GetString(child), "Waypoint1Load") == 0)
			mTurnOrder.push_back(TP_WAY1_LOAD);
//		else if (strcmp(GetString(child), "CheckForTechnology") == 0)
//			mTurnOrder.push_back(TP_TECH_CHECK);
//		else if (strcmp(GetString(child), "CheckForDeadPlanets") == 0)
//			mTurnOrder.push_back(TP_DEADCHECK);
		else if (strcmp(GetString(child), "LayingMinefields") == 0)
			mTurnOrder.push_back(TP_MINELAYING);
		else if (strcmp(GetString(child), "TransferFleets") == 0)
			mTurnOrder.push_back(TP_TRANSFER);
		else if (strcmp(GetString(child), "MergeFleets") == 0)
			mTurnOrder.push_back(TP_MERGE);
		else if (strcmp(GetString(child), "InstantTerraforming") == 0)
			mTurnOrder.push_back(TP_INSTA_TERRAFORM);
		else if (strcmp(GetString(child), "MinefieldsDecay") == 0)
			mTurnOrder.push_back(TP_MINEDECAY);
		else if (strcmp(GetString(child), "SweepingMinefields") == 0)
			mTurnOrder.push_back(TP_MINESWEEP);
		else if (strcmp(GetString(child), "RemoteTerraforming") == 0)
			mTurnOrder.push_back(TP_REMOTE_TERRAFORM);
		else if (strcmp(GetString(child), "UpdateScanning") == 0)
			mTurnOrder.push_back(TP_UPDATESCANNING);
		else if (strcmp(GetString(child), "SetupPatrolOrders") == 0)
			mTurnOrder.push_back(TP_PATROL);
	}
	
	// mTurnOrder
}

bool Rules::ParsePacketMapping(const TiXmlNode * node, deque<long> & array, MessageSink &messageSink)
{
	assert(MaxMinType > 0 && MaxHabType > 0);
	const TiXmlNode * child2;
	const TiXmlElement * txe;
	array.insert(array.begin(), MaxMinType, -1);
	int index;
	for (child2 = node->FirstChild("Mineral"); child2; child2 = child2->NextSibling("Mineral")) {
		txe = child2->ToElement();
		if (txe == NULL) {
			messageSink.AddMessage("Missing mineral name in PacketTerraformMinerals");
			continue;
		}

		index = MineralID(txe->Attribute("Name"));
		if (index < 0) {
			messageSink.AddMessage("Bad or missing mineral name in PacketTerraformMinerals");
			continue;
		}

		array[index] = HabID(GetString(child2));
		if (array[index] < 0) {
			Message * mess = messageSink.AddMessage("Bad or missing hab name in PacketTerraformMinerals");
			mess->AddItem("For mineral", GetCargoName(index));
			continue;
		}
	}

	deque<bool> map;
	map.insert(map.begin(), MaxHabType, false);
	assert(array.size() == MaxMinType);
	for (index = 0; index < MaxMinType; ++index) {
		if (array[index] >= 0)
			map[array[index]] = true;
	}

	bool OK = true;
	for (index = 0; index < MaxHabType; ++index) {
		if (!map[index]) {
			OK = false;
			Message * mess = messageSink.AddMessage("Missing hab type in PacketTerraformMinerals");
			mess->AddItem("Missing hab", GetHabName(index));
		}
	}

	return OK;
}

long Rules::GetConstant(const string name, long Default /*= 0*/)
{
	map<string, long, less<string> >::const_iterator im;
	im = Consts.find(name);
	if (im == Consts.end())
		return Default;

	return im->second;
}

double Rules::GetFloat(const string name, double Default /*= 0.0*/)
{
	map<string, double, less<string> >::const_iterator im;
	im = Floats.find(name);
	if (im == Floats.end())
		return Default;

	return im->second;
}

long Rules::GetArrayValue(const string name, int position)
{
	map<string, Array<long> *, less<string> >::const_iterator im;
	im = Arrays.find(name);
	if (im == Arrays.end())
		return -1;

	return (*im->second)[position];
}

double Rules::GetArrayFloat(const string name, int position)
{
	map<string, Array<double> *, less<string> >::const_iterator im;
	im = FloatArrays.find(name);
	if (im == FloatArrays.end())
		return -1;

	return (*im->second)[position];
}

long Rules::TechID(const char * name)
{
	if (name == NULL || *name == '\0')
		return -1;

	if (stricmp(name, "Alchemy instead of research") == 0)
		return RESEARCH_ALCHEMY;
	else if (stricmp(name, "Same field") == 0)
		return NEXT_SAME;
	else if (stricmp(name, "Lowest field") == 0)
		return NEXT_LOW;
	else if (stricmp(name, "Cheapest field") == 0)
		return NEXT_CHEAP;

	deque<string>::const_iterator it;
	it = find(TechName.begin(), TechName.end(), name);
	if (it == TechName.end())
		return TECH_NONE;

	return it - TechName.begin();
}

const string Rules::GetTechName(long type)
{
	if (type == RESEARCH_ALCHEMY)
		return "Alchemy instead of research";
	else if (type == NEXT_SAME)
		return "Same field";
	else if (type == NEXT_LOW)
		return "Lowest field";
	else if (type == NEXT_CHEAP)
		return "Cheapest field";
	else if (type >= 0 && type < MaxTechType)
		return TechName[type];
	else
		return "BadName";
}

long Rules::MineralID(const char * name)
{
	if (name == NULL || *name == '\0')
		return -1;

	deque<string>::const_iterator it;
	it = find(MineralName.begin(), MineralName.end(), name);
	if (it == MineralName.end())
		return -1;

	return it - MineralName.begin();
}

long Rules::HabID(const char * name)
{
	if (name == NULL || *name == '\0')
		return -1;

	deque<string>::const_iterator it;
	it = find(HabName.begin(), HabName.end(), name);
	if (it == HabName.end())
		return -1;

	return it - HabName.begin();
}

long Rules::MineID(const char * name)
{
	if (name == NULL || *name == '\0')
		return -1;

	int i;
	for (i = 0; i < mMineFieldTypes.size(); ++i) {
		if (strcmp(name, mMineFieldTypes[i]->GetName().c_str()) == 0)
			return i;
	}

	return -1;
}

const MineFieldType * Rules::GetMineFieldType(int type)
{
	if (type < 0 || type >= MaxMineType)
		return NULL;

	return mMineFieldTypes[type];
}

void Rules::ReadCargo(const TiXmlNode * node, deque<long> & q, long * pop, MessageSink &messageSink)
{
	deque<long>::iterator li;
	if (node == NULL) {
		if (pop)	*pop = 0;
		for (li = q.begin(); li != q.end(); ++li)
			*li = 0;
	} else {
		if (pop)	*pop = GetLong(node->FirstChild("Population"));
		ParseArray(node, q, MINERALS, messageSink);
	}
}

TiXmlElement * Rules::WriteCargo(TiXmlNode * node, const char * name, const deque<long> &q, long pop)
{
	deque<long>::const_iterator lmax, lmin;

	lmax = max_element(q.begin(), q.end());
	lmin = min_element(q.begin(), q.end());
	if (*lmax > 0 || *lmin < 0 || pop != 0 ) {
		TiXmlElement * child;
		child = WriteArray(name, q, MINERALS);
		if (pop != 0)
			AddLong(child, "Population", pop);

		node->LinkEndChild(child);
		return child;
	} else
		return NULL;
}

long Rules::GetCargoType(const char * name)
{
	if (stricmp(name, "Population") == 0 || stricmp(name, "Crew") == 0)
		return POPULATION;
	else if (stricmp(name, "Fuel") == 0)
		return FUEL;
	else if (stricmp(name, "Resources") == 0)
		return RESOURCES;
	else {
		long result = MineralID(name);
		if (result < 0) {
            return UNKNOWN_CARGO;
		}

		return result;
	}
}

const string Rules::GetCargoName(long type)
{
	if (type == POPULATION)
		return "Population";
	else if (type == FUEL)
		return "Fuel";
	else {
		if (type >= 0 && type < MaxMinType)
			return MineralName[type];
		else
			return "BadName";
	}
}

TiXmlElement * Rules::WriteArray(const char * node, const deque<long> & q, long Type)
{
	if (Type == MINERALS)
		return WriteArray(node, "Mineral", "Name", q, &MineralName);
	else if (Type == TECHS)
		return WriteArray(node, "Tech", "Name", q, &TechName);
	else if (Type == HABS)
		return WriteArray(node, "Hab", "Name", q, &HabName);
	else {
		assert(false);
		return NULL;
	}
}

TiXmlElement * Rules::WriteArrayFloat(const char * node, const deque<double> & q, long Type)
{
	if (Type == MINERALS)
		return WriteArrayFloat(node, "Mineral", "Name", q, &MineralName);
	else if (Type == TECHS)
		return WriteArrayFloat(node, "Tech", "Name", q, &TechName);
	else if (Type == HABS)
		return WriteArrayFloat(node, "Hab", "Name", q, &HabName);
	else {
		assert(false);
		return NULL;
	}
}

bool Rules::ParseArray(const TiXmlNode * node, deque<long> & q, long Type, MessageSink &messageSink)
{
    ArrayParser arrayParser(messageSink);

	if (Type == MINERALS)
		return arrayParser.ParseArray(node, "Mineral", "Name", q, &Rules::MineralName);
	else if (Type == TECHS)
		return arrayParser.ParseArray(node, "Tech", "Name", q, &TechName);
	else if (Type == HABS)
		return arrayParser.ParseArray(node, "Hab", "Name", q, &HabName);
	else
		return false;
}

bool Rules::ParseArrayFloat(const TiXmlNode * node, deque<double> & q, long Type, MessageSink &messageSink)
{
    ArrayParser arrayParser(messageSink);

	if (Type == MINERALS)
		return arrayParser.ParseArrayFloat(node, "Mineral", "Name", q, &MineralName);
	else if (Type == TECHS)
		return arrayParser.ParseArrayFloat(node, "Tech", "Name", q, &TechName);
	else if (Type == HABS)
		return arrayParser.ParseArrayFloat(node, "Hab", "Name", q, &HabName);
	else
		return false;
}

TiXmlElement * Rules::WriteArrayBool(const char * node, const char * name, const char * Attrib, const deque<bool> & arr, const deque<string> * desc /*= NULL*/)
{
	TiXmlElement * result;
	result = new TiXmlElement(node);
	for (unsigned int i = 0; i < arr.size(); ++i) {
		TiXmlElement txe(name);
		if (desc == NULL)
			txe.SetAttribute(Attrib, i+1);
		else
			txe.SetAttribute(Attrib, (*desc)[i]);

		TiXmlText txt(arr[i] ? "True" : "False");
		txe.InsertEndChild(txt);
		result->InsertEndChild(txe);
	}
	return result;
}

TiXmlElement * Rules::WriteArray(const char * node, const char * name, const char * Attrib, const deque<long> & arr, const deque<string> * desc /*= NULL*/)
{
	TiXmlElement * result;
	result = new TiXmlElement(node);
	for (unsigned int i = 0; i < arr.size(); ++i) {
		TiXmlElement txe(name);
		if (desc == NULL)
			txe.SetAttribute(Attrib, i+1);
		else
			txe.SetAttribute(Attrib, (*desc)[i]);

		TiXmlText txt(Long2String(arr[i]));
		txe.InsertEndChild(txt);
		result->InsertEndChild(txe);
	}
	return result;
}

TiXmlElement * Rules::WriteArrayFloat(const char * node, const char * name, const char * Attrib, const deque<double> & arr, const deque<string> * desc /*= NULL*/)
{
	TiXmlElement * result;
	result = new TiXmlElement(node);
	for (unsigned int i = 0; i < arr.size(); ++i) {
		TiXmlElement txe(name);
		if (desc == NULL)
			txe.SetAttribute(Attrib, i+1);
		else
			txe.SetAttribute(Attrib, (*desc)[i]);

		TiXmlText txt(Float2String(arr[i]));
		txe.InsertEndChild(txt);
		result->InsertEndChild(txe);
	}
	return result;
}

long Rules::RandomHab(HabType ht)
{
	deque<long> * hoa = GetHabOddArray(ht, false);

	if (hoa == NULL) {
		// serious problem
		return 50;
	}

	long rnd = Random((*hoa)[hoa->size() - GetConstant("MinHabValue")]) + 1;

	// now do a binary search: g is the guess, hig is the prior guess on the high side
	// log is the prior guess on the low side
	long hig = GetConstant("MaxHabValue") - GetConstant("MinHabValue");
	long log = 0;
	long g = (log + hig) / 2;	// This is here to remove a warning, not really needed
	bool done = false;

	while (!done) {
		g = (log + hig) / 2;

		if (g > 0 && rnd <= (*hoa)[g-1])
			hig = g - 1;
		else if (g < hig && rnd > (*hoa)[g])
			log = g + 1;
		else
			done = true;
	}

	return g + GetConstant("MinHabValue");
}

// regenerate hab till it matches the race's hab
long Rules::GetSecondHab(HabType ht, const Player * owner)
{
	long Result;
	long count = 0;
	do {
		Result = RandomHab(ht);
	} while (count++ < 1000 &&
			owner->HabCenter(ht) >= 0 &&
				(Result < owner->HabCenter(ht) - owner->HabWidth(ht) ||
				Result > owner->HabCenter(ht) + owner->HabWidth(ht))
			);

	return Result;
}

bool Rules::ParseMinSettings(const TiXmlNode * node, MessageSink &messageSink)
{
	const TiXmlNode * child;
	mMinMC.insert(mMinMC.begin(), Rules::MaxMinType, 1);
	mMaxMC.insert(mMaxMC.begin(), Rules::MaxMinType, 120);
	mHWMinMC.insert(mHWMinMC.begin(), Rules::MaxMinType, 30);
	mHWMaxMC.insert(mHWMaxMC.begin(), Rules::MaxMinType, 120);
	mHWFloorMC.insert(mHWFloorMC.begin(), Rules::MaxMinType, 30);

	child = node->FirstChild("AllWorlds");
	if (!child) {
		Message * mess = messageSink.AddMessage("Error: Missing section");
		mess->AddItem("Section", "Mineral settings - AllWorlds");
		return false;
	}

	if (!ParseArray(child->FirstChild("Minimum"), mMinMC, MINERALS, messageSink))
		return false;

	if (!ParseArray(child->FirstChild("Maximum"), mMaxMC, MINERALS, messageSink))
		return false;

	child = node->FirstChild("HomeWorlds");
	if (!child) {
		Message * mess = messageSink.AddMessage("Error: Missing section");
		mess->AddItem("Section", "Mineral settings - HomeWorlds");
		return false;
	}

	if (!ParseArray(child->FirstChild("Minimum"), mHWMinMC, MINERALS, messageSink))
		return false;

	if (!ParseArray(child->FirstChild("Maximum"), mHWMaxMC, MINERALS, messageSink))
		return false;

	if (!ParseArray(child->FirstChild("HWFloor"), mHWFloorMC, MINERALS, messageSink))
		return false;

	return true;
}

void Rules::WriteMinSettings(TiXmlNode * node)
{
	TiXmlElement minset("MineralSettings");

	TiXmlElement All("AllWorlds");
	All.LinkEndChild(Rules::WriteArray("Minimum", mMinMC, MINERALS));
	All.LinkEndChild(Rules::WriteArray("Maximum", mMaxMC, MINERALS));
	minset.InsertEndChild(All);

	TiXmlElement HWs("HomeWorlds");
	HWs.LinkEndChild(Rules::WriteArray("Minimum", mHWMinMC, MINERALS));
	HWs.LinkEndChild(Rules::WriteArray("Maximum", mHWMaxMC, MINERALS));
	minset.InsertEndChild(HWs);

	minset.LinkEndChild(Rules::WriteArray("HWFloor", mHWFloorMC, MINERALS));
	node->InsertEndChild(minset);
}
/*
    <MineralSettings>
<!-- AKA BMM -->
        <AllWorlds>
			<Minimum>
				<Mineral Name="Ironium">1</Mineral>
				<Mineral Name="Boranium">1</Mineral>
				<Mineral Name="Germanium">1</Mineral>
			</Minimum>
			<Maximum>
				<Mineral Name="Ironium">120</Mineral>
				<Mineral Name="Boranium">120</Mineral>
				<Mineral Name="Germanium">120</Mineral>
			</Maximum>
        </AllWorlds>
        <HomeWorlds>
			<Minimum>
				<Mineral Name="Ironium">1</Mineral>
				<Mineral Name="Boranium">1</Mineral>
				<Mineral Name="Germanium">1</Mineral>
			</Minimum>
			<Maximum>
				<Mineral Name="Ironium">120</Mineral>
				<Mineral Name="Boranium">120</Mineral>
				<Mineral Name="Germanium">120</Mineral>
			</Maximum>
		</HomeWorlds>
        <HWFloor>
            <Mineral Name="Ironium">30</Mineral>
            <Mineral Name="Boranium">30</Mineral>
            <Mineral Name="Germanium">30</Mineral>
        </HWFloor>
    </MineralSettings>
*/
deque<long> * Rules::GetHabOddArray(HabType ht, bool Create /*= false*/)
{
	while (Create && ht >= mHabOdds.size()) {
		deque<long> * hoa;
		hoa = new deque<long>;
		hoa->insert(hoa->begin(), 99, 1);
		mHabOdds.push_back(hoa);
	}

	if (ht >= mHabOdds.size())
		return NULL;
	else
		return mHabOdds[ht];
}

// All HWs start with the same mineral concentrations and stockpiles
long Rules::GetHWMC(long mintype)
{
	if (mHWSetupMC.size() == 0) {
		mHWSetupMC.insert(mHWSetupMC.begin(), Rules::MaxMinType, 1);
		for (int i = 0; i < Rules::MaxMinType; ++i) {
			if (Random(100) < Rules::HWMinMC(i))
				mHWSetupMC[i] = Rules::HWMinMC(i);
			else {
				long die1 = (Rules::HWMaxMC(i) - Rules::HWMinMC(i)) / 2;
				long die2 = (Rules::HWMaxMC(i) - Rules::HWMinMC(i) + 1) / 2;
				mHWSetupMC[i] = Random(die1) + Random(die2) + Rules::HWMinMC(i) + 1;
			}

			if (mHWSetupMC[i] < mHWFloorMC[i])
				mHWSetupMC[i] = mHWFloorMC[i];
		}
	}

	return mHWSetupMC[mintype];
}

long Rules::GetHWStartMinerals(long mintype, const Creation *creation)
{
	long lMax;
	long lMin = 165;
	if (creation != NULL && mHWSetupSM.size() == 0) {
		mHWSetupSM.insert(mHWSetupSM.begin(), Rules::MaxMinType, 1);
		for (int i = 0; i < Rules::MaxMinType; ++i) {
			lMax = Rules::GetHWMC(i) * 11;
			if (lMax < 500)
				lMax = 500;

			mHWSetupSM[i] = Random(lMin, lMax);
		}
	}

// leave as 'unfinished' will generate a warning to this line, easier for ide to jump right here
#pragma unfinished(__FILE__ " : Rules::GetHWStartMinerals needs to set starting minerals correctly")

	return mHWSetupSM[mintype];
}

long Rules::GetSWStartMinerals(long /*mintype*/, const Creation *creation)
{
	if (creation != NULL)
		return Random(creation->mSWMinMin, creation->mSWMaxMin);
	else
		return 0;
}

double Rules::OverGateRange(long range, long dist)
{
	if (range == -1 || range >= dist)
		return 0;
	else
		return double(dist - range) / 4.0 * range;
}

double Rules::OverGateMass(long cap1, long cap2, long mass)
{
	double safe = 1.0;
	if (cap1 != -1 && cap1 < mass)
		safe = 1.0 - double(mass - cap1) / 4.0 * cap1;
	if (cap2 != -1 && cap2 < mass)
		safe *= 1.0 - double(mass - cap2) / 4.0 * cap2;

	return 1.0 - safe;
}

long Rules::MultiOdds(double odds, long trials)
{
	if (odds < epsilon)
		return 0;
	else if (odds >= 1.0)
		return trials;
	else {
		long Result = 0;
		for (long i = 0; i < trials; ++i) {
			if (Randodd(odds))
				++Result;
		}

		return Result;
	}
}

}
