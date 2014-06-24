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

#include "Hull.h"
#include "RacialTrait.h"
#include "Packet.h"

#include "Creation.h"

#include <stdlib.h>
#include <time.h>
#include <typeinfo>

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

Game::Game()
	: galaxy(new Galaxy())
{
	mTurnPhase = 0;
	mCreation = NULL;
    VC = NULL;
	mNumberOfPlayers = 0;
	mWHMin = 0;
	mWHMax = 0;
	mWHMinDistance = 0;
	mGameID = 0;

	mSeed = NULL;
	Rules::Init();
}

Game::~Game()
{
	delete mCreation;
    delete VC;

	deque<Component *>::iterator i1;
	for (i1 = mComponents.begin(); i1 != mComponents.end(); ++i1)
		delete *i1;
	mComponents.clear();

	deque<Player *>::iterator i3;
	for (i3 = mPlayers.begin(); i3 != mPlayers.end(); ++i3)
		delete *i3;
	mPlayers.clear();

	RacialTrait::CleanUp();

	int i;
	for (i = 0; i < mMessages.size(); ++i)
		delete mMessages[i];

	for (i = 0; i < mTopObjects.size(); ++i)
		delete mTopObjects[i];

	mOrders.clear();

	delete [] mSeed;

	delete galaxy;
}

Message * Game::AddMessage(const string &type)
{
	Message * mess = new Message(type);
	mMessages.push_back(mess);
	return mess;
}

void Game::StoreMessageLocation(const Location * loc)
{
	int i;
	for (i = 0; i < mMessages.size(); ++i)
		mMessages[i]->StoreMessageLocation(loc, NULL);

	for (i = 0; i < mNumberOfPlayers; ++i)
		mPlayers[i]->StoreMessageLocation(loc);
}

bool Game::LoadCreation(const TiXmlNode * options)
{
	const char * ptr;
	if (!options)
		return false;

	// Slow Tech Advances game option
	ptr = GetString(options->FirstChild("TechAdvances"));
	if (ptr == NULL || strcmp(ptr, "Normal") == 0)
		TechFactor = 1.0;
	else if (strcmp(ptr, "Slow") == 0)
		TechFactor = 2.0;
	else {
		double tmp = atof(ptr);
		if (tmp > 0 && tmp <= 100)
			TechFactor = tmp;
		else
			TechFactor = 1.0;
	}

	const TiXmlNode * child1;
	const TiXmlNode * child2;
	const TiXmlNode * child3;
	const TiXmlText * text;

	if (!galaxy->ParseSize(options->FirstChild("Size")))
		return false;

	RandomEventsStart = 0;
	child1 = options->FirstChild("RandomEvents");
	if (!child1)
		mRandomEvents = RE_NONE;
	else {
		child2 = child1->FirstChild("Start");
		if (child2)
			RandomEventsStart = GetLong(child2);

		mRandomEvents = RE_NONE;
		for (child2 = child1->FirstChild("Event"); child2; child2 = child2->NextSibling("Event")) {
			child3 = child2->FirstChild();
			if (!child3)
				continue;

			if (child3->Type() != TiXmlNode::TEXT)
				continue;

			text = child3->ToText();
			if (strcmp(text->Value(), "Mystery Trader") == 0)
				mRandomEvents |= RE_MT;
			else if (strcmp(text->Value(), "Artifacts") == 0)
				mRandomEvents |= RE_ARTIFACT;
			else if (strcmp(text->Value(), "Comets") == 0)
				mRandomEvents |= RE_COMET;
			else if (strcmp(text->Value(), "Wormholes") == 0) {
				mRandomEvents |= RE_WORMHOLE;
				child3 = child1->FirstChild("Wormholes");
				mWHMin = GetLong(child3->FirstChild("MinimumPairs"));
				mWHMax = GetLong(child3->FirstChild("MaximumPairs"));
				mWHMinDistance = GetLong(child3->FirstChild("MinDistance"));
				mWHMinDistance = max(mWHMinDistance, 10L);
			} else if (strcmp(text->Value(), "All") == 0) {
				mRandomEvents = RE_ALL;
				break;
			}
		}
	}

	PublicScoreStart = 0;
	child1 = options->FirstChild("PublicScores");
	if (!child1)
		PublicScore = PPS_RANK;
	else {
		PublicScore = PPS_NONE;
		PublicScoreStart = 20;
		child2 = child1->FirstChild("Start");
		if (child2)
			PublicScoreStart = GetLong(child2);

		for (child2 = child1->FirstChild("Category"); child2; child2 = child2->NextSibling("Category")) {
			child3 = child2->FirstChild();
			if (!child3)
				continue;

			if (child3->Type() != TiXmlNode::TEXT)
				continue;

			text = child3->ToText();
			if (strcmp(text->Value(), "Planets") == 0)
				PublicScore |= PPS_PLANET;
			else if (strcmp(text->Value(), "Bases") == 0)
				PublicScore |= PPS_BASES;
			else if (strcmp(text->Value(), "Unarmed Ships") == 0)
				PublicScore |= PPS_UNARM;
			else if (strcmp(text->Value(), "Escort Ships") == 0)
				PublicScore |= PPS_ESCORT;
			else if (strcmp(text->Value(), "Capital Ships") == 0)
				PublicScore |= PPS_CAPSHIP;
			else if (strcmp(text->Value(), "Tech Levels") == 0)
				PublicScore |= PPS_TECH;
			else if (strcmp(text->Value(), "Resources") == 0)
				PublicScore |= PPS_RESOURCE;
			else if (strcmp(text->Value(), "Score") == 0)
				PublicScore |= PPS_SCORE;
			else if (strcmp(text->Value(), "Rank") == 0)
				PublicScore |= PPS_RANK;
			else if (strcmp(text->Value(), "All") == 0) {
				PublicScore = PPS_ALL;
				break;
			}
		}
	}

	child1 = options->FirstChild("VictoryConditions");
	if (child1) {
        VC = new VictoryConditions;
        VC->Parse(child1);
	}

	child1 = options->FirstChild("MineralSettings");
	if (!child1) {
		Message * mess = TheGame->AddMessage("Error: Missing section");
		mess->AddItem("Section", "MineralSettings");
		return false;
	}

	Rules::ParseMinSettings(child1);

	return true;
}

bool Game::LoadStartShips(const TiXmlNode * node)
{
	if (!node)
		return false;

	const TiXmlNode * child;
	RacialTrait * rt;
	for (child = node->FirstChild("PrimaryRacialTrait"); child != NULL; child = child->NextSibling("PrimaryRacialTrait")) {
		rt = const_cast<RacialTrait *>(RacialTrait::ParsePRT(GetString(child->FirstChild("Name"))));
		if (rt == NULL) {
			return false;
		}
		rt->ParseStartShips(child);
	}

	for (child = node->FirstChild("LesserRacialTrait"); child != NULL; child = child->NextSibling("LesserRacialTrait")) {
		rt = const_cast<RacialTrait *>(RacialTrait::ParseLRT(GetString(child->FirstChild("Name"))));
		if (rt == NULL) {
			return false;
		}
		rt->ParseStartShips(child);
	}

	return true;
}

bool Game::LoadComponents(const TiXmlNode * node)
{
	if (!node)
		return false;

	if (!Component::LoadComponents(node, mComponents))
		return false;

#ifdef _DEBUG
	deque<Component *>::const_iterator iter;
	Component * temp;
	Hull * thull;

	for (iter = mComponents.begin(); iter != mComponents.end(); ++iter) {
		temp = *iter;
		thull = dynamic_cast<Hull *>(temp);
		if (thull)
			thull->CheckSlots();
	}
#endif // _DEBUG

	return true;
}

static unsigned long GetRandomSeed()
{
	return (unsigned)time(NULL);
}

void Game::SetFileLocation(const char * hostfile)
{
	mFileLoc = hostfile;
	int pos = max(int(mFileLoc.find_last_of('/')), int(mFileLoc.find_last_of('\\')));
	mFileName = mFileLoc.substr(pos+1);
	mFileName.erase(mFileName.find_last_of('.'));
	mFileLoc.erase(pos+1);
}

bool Game::LoadHostFile(const char * hostfile)
{
	SetFileLocation(hostfile);

	TiXmlDocument doc(hostfile);
	doc.SetCondenseWhiteSpace(false);
	if (!doc.LoadFile()) {
		Message * mess = TheGame->AddMessage("Error: Cannot open file");
		mess->AddItem("File name", hostfile);
		mess->AddItem("TinyXML error description", doc.ErrorDesc());
		mess->AddLong("TinyXML error row", doc.ErrorRow());
		mess->AddLong("TinyXML error column", doc.ErrorCol());
		return false;
	}

	const TiXmlNode * hf;
	const TiXmlNode * node;

	hf = doc.FirstChild("HostFile");
	if (!hf) {
		Message * mess = TheGame->AddMessage("Error: Missing section");
		mess->AddItem("File name", hostfile);
		mess->AddItem("Section", "HostFile");
		return false;
	}

	if (!Game::CheckMetaInfo(hf, hostfile, HOSTFILEVERSION))
		return false;

	mGameID = GetLong(hf->FirstChild("GameID"));
	if (mGameID == 0) {
		Message * mess = TheGame->AddMessage("Error: Invalid Game ID");
		mess->AddLong("Game ID", mGameID);
		return false;
	}

	node = hf->FirstChild("Turn");
	Turn = GetLong(node);
	if (Turn < 1) {
		Message * mess = TheGame->AddMessage("Error: Turn too low");
		mess->AddLong("Turn", Turn);
		return false;
	}

	++Turn;	// This is a new turn
	int seedc = 0;
	int seeds = GetLong(hf->FirstChild("Seeds"));
	if (seeds <= 18)
		seeds = 18;
	else if (seeds > 600)
		seeds = 600;

	mSeed = new unsigned long[seeds + 1];
	memset(mSeed, 0, (seeds + 1) * sizeof(int));
	node = hf->FirstChild("Seed");
	if (node == NULL)
		mSeed[seedc++] = GetRandomSeed();
	else {
		for (; node != NULL; node = node->NextSibling("Seed")) {
			mSeed[seedc] = GetLong(node);
			if (mSeed[seedc] == 0)
				mSeed[seedc] = GetRandomSeed();

			++seedc;
		}
	}

	// do not ask for random numbers before this point
	// set seed to what is defined in the .hst file
	init_by_array(mSeed, seedc);

	if (!LoadXYFile())	// this also loads # of players needed below
		return false;

	for (node = hf->FirstChild("Player"); node; node = node->NextSibling("Player")) {
		const TiXmlNode * child2 = node->FirstChild("PlayerNumber");
		unsigned int p = GetLong(child2);
		if (p < 1 || p > NumberPlayers() || p != mPlayers.size()+1) {
			Message * mess = TheGame->AddMessage("Error: Invalid player number");
			mess->AddLong("", p);
			return false;
		}

		Player * dummy = NULL;
		deque<Player *>::iterator pi;
		pi = mPlayers.insert(mPlayers.end(), TheGame->ObjectFactory(dummy, p));
		if (!(*pi)->ParseNode(node, false))
			return false;
	}

	// Load after loading players, will cause problems with planet owners otherwise
	node = hf->FirstChild("Galaxy");
	if (!node) {
		Message * mess = TheGame->AddMessage("Error: Missing section");
		mess->AddItem("File name", hostfile);
		mess->AddItem("Section", "Galaxy");
		return false;
	}
	if (!galaxy->ParseNode(node))
		return false;

	if (mNumberOfPlayers != mPlayers.size())
		return false;

	return true;
}

bool Game::LoadPlayerFile(const char * playerfile)
{
	SetFileLocation(playerfile);

	string file = playerfile;
	int pos = file.find_last_of('.');
	mCurrentPlayer = atol(file.substr(pos+2).c_str());

	TiXmlDocument doc(playerfile);
	doc.SetCondenseWhiteSpace(false);
	if (!doc.LoadFile()) {
		Message * mess = TheGame->AddMessage("Error: Cannot open file");
		mess->AddItem("File name", playerfile);
		mess->AddItem("TinyXML error description", doc.ErrorDesc());
		mess->AddLong("TinyXML error row", doc.ErrorRow());
		mess->AddLong("TinyXML error column", doc.ErrorCol());
		return false;
	}

	const TiXmlNode * ptf;
	const TiXmlNode * node;

	ptf = doc.FirstChild("PlayerTurnFile");
	if (!ptf) {
		Message * mess = TheGame->AddMessage("Error: Missing section");
		mess->AddItem("File name", playerfile);
		mess->AddItem("Section", "PlayerTurnFile");
		return false;
	}

	if (!Game::CheckMetaInfo(ptf, playerfile, TURNFILEVERSION))
		return false;

	mGameID = GetLong(ptf->FirstChild("GameID"));
	if (mGameID == 0) {
		Message * mess = TheGame->AddMessage("Error: Invalid Game ID");
		mess->AddLong("Game ID", mGameID);
		return false;
	}

	node = ptf->FirstChild("Turn");
	Turn = GetLong(node);
	if (Turn < 1) {
		Message * mess = TheGame->AddMessage("Error: Turn too low");
		mess->AddLong("Turn", Turn);
		return false;
	}

	if (!LoadXYFile())
		return false;

	mPlayers.insert(mPlayers.begin(), mNumberOfPlayers, NULL);
	for (node = ptf->FirstChild("Player"); node; node = node->NextSibling("Player")) {
		const TiXmlNode * child2 = node->FirstChild("PlayerNumber");
		unsigned int p = GetLong(child2);
		if (p < 1 || p > NumberPlayers()) {
			Message * mess = TheGame->AddMessage("Error: Invalid player number");
			mess->AddLong("", p);
			return false;
		}

		Player * dummy = NULL;
		mPlayers[p-1] = TheGame->ObjectFactory(dummy, p);
		if (!mPlayers[p-1]->ParseNode(node, p != mCurrentPlayer))
			return false;
	}

	if (mNumberOfPlayers != mPlayers.size())
		return false;

	if (mPlayers[mCurrentPlayer-1] == NULL)
		return false;

	// Load after getting # of players, will cause problems with the mSeenBy array otherwise
	node = ptf->FirstChild("Galaxy");
	if (!node) {
		Message * mess = TheGame->AddMessage("Error: Missing section");
		mess->AddItem("File name", playerfile);
		mess->AddItem("Section", "Galaxy");
		return false;
	}
	if (!galaxy->ParseNode(node))
		return false;

	mPlayers[mCurrentPlayer-1]->ParseMessages(ptf->FirstChild("Messages"));

	file[pos+1] = 'x';
	mPlayers[mCurrentPlayer-1]->SetWriteXFile();
	mPlayers[mCurrentPlayer-1]->OpenOrdersFile(file.c_str());

	return true;
}

bool Game::CheckMetaInfo(const TiXmlNode * node, const char * file, double fileversion)
{
	const TiXmlNode * mn = node->FirstChild("MetaInfo");
	if (!mn) {
		Message * mess = TheGame->AddMessage("Error: Missing section");
		mess->AddItem("File name", file);
		mess->AddItem("Section", "MetaInfo");
		return false;
	}

	double meta = GetDouble(mn->FirstChild("FileVersion"));
	if (meta > fileversion + epsilon || meta < fileversion - epsilon) {
		Message * mess = TheGame->AddMessage("Error: Invalid FileVersion");
		mess->AddItem("File name", file);
		mess->AddItem("FileVersion", GetString(node->FirstChild("FileVersion")));
		return false;
	}

	meta = GetDouble(mn->FirstChild("FreeStarsVersion"));
	if (meta > FREESTARSVERSION + epsilon || meta < FREESTARSVERSION - epsilon) {
		Message * mess = TheGame->AddMessage("Error: Invalid FreeStarsVersion");
		mess->AddItem("File name", file);
		mess->AddItem("FreeStarsVersion", GetString(node->FirstChild("FreeStarsVersion")));
		return false;
	}

	return true;
}

bool Game::LoadDefFile(const char * deffile)
{
	SetFileLocation(deffile);

	// generate a random game ID with a different seed then the rest of the game uses
	init_genrand(GetRandomSeed());
	do mGameID = genrand_int32();
	while (mGameID == 0);

	TiXmlDocument doc(deffile);
	doc.SetCondenseWhiteSpace(false);
	if (!doc.LoadFile()) {
		Message * mess = TheGame->AddMessage("Error: Cannot open file");
		mess->AddItem("File name", deffile);
		return false;
	}

	const TiXmlNode * df;
	const TiXmlNode * node;

	mCreation = new Creation;

	df = doc.FirstChild("DefFile");
	if (!df) {
		Message * mess = TheGame->AddMessage("Error: Missing section");
		mess->AddItem("File name", deffile);
		mess->AddItem("Section", "DefFile");
		return false;
	}

	if (!CheckMetaInfo(df, deffile, HOSTFILEVERSION))
		return false;

#ifdef _DEBUG
	long id;
	id = GetLong(df->FirstChild("GameID"));
	if (id != 0)
		mGameID = id;
#endif // _DEBUG

	node = df->FirstChild("Rules");
	if (!node) {
		Message * mess = TheGame->AddMessage("Error: Missing section");
		mess->AddItem("File name", deffile);
		mess->AddItem("Section", "Rules");
		return false;
	}

	if (!LoadRules(GetString(node->FirstChild("File")), "", 0.0, false))
		return false;

	node = df->FirstChild("Turn");
	Turn = GetLong(node);
	if (Turn < 1) {
		Message * mess = TheGame->AddMessage("Error: Turn too low");
		mess->AddLong("Turn", Turn);
		return false;
	}

	int seedc = 0;
	int seeds = GetLong(df->FirstChild("Seeds"));
	if (seeds <= 18)
		seeds = 18;
	else if (seeds > 600)
		seeds = 600;

	mSeed = new unsigned long[seeds + 1];
	memset(mSeed, 0, (seeds + 1) * sizeof(int));
	node = df->FirstChild("Seed");
	if (node == NULL)
		mSeed[seedc++] = GetRandomSeed();
	else {
		for (; node != NULL; node = node->NextSibling("Seed")) {
			mSeed[seedc] = GetLong(node);
			if (mSeed[seedc] == 0)
				mSeed[seedc] = GetRandomSeed();

			++seedc;
		}
	}

	unsigned long randomize;
	long points;
	for (node = df->FirstChild("RaceFile"); node != NULL; node = node->NextSibling("RaceFile")) {
		Player * p = NULL;
		p = ObjectFactory(p, mPlayers.size() + 1);
		mPlayers.push_back(p);
		string file;
		file = mFileLoc;
		file += GetString(node);
		randomize = p->CreateFromFile(file.c_str());
		if (randomize < 0)
			return false;
		else if (randomize > 0 && seedc < seeds)
			mSeed[seedc++] = randomize;

		points = p->GetAdvantagePoints();
		if (points < 0)
			return false;
		p->SetLeftoverPoints(points / 3);
	}

	mNumberOfPlayers = mPlayers.size();
	int i;
	for (i = 0; i < mNumberOfPlayers; ++i)
		mPlayers[i]->SetupRelations();

	// do not ask for random numbers before this point, execpt for the game ID above
	// set seed to what is defined in the .def file and player race files
	init_by_array(mSeed, seedc);

	const TiXmlNode * cre;
	cre = df->FirstChild("Creation");
	if (!LoadCreation(cre))
		return false;

	if (cre == NULL || !mCreation->LoadCreation(cre->FirstChild("UniverseSetup")))
		return false;

	string NameFile;
	NameFile = mFileLoc;
	NameFile += GetString(df->FirstChild("PlanetNames"));
	if (!mCreation->LoadNames(NameFile.c_str())) {
		Message * mess = TheGame->AddMessage("Error: Cannot load planet names");
		return false;
	}

	// if they include a galaxy use that
	if (df->FirstChild("Galaxy") != NULL) {
		if (!galaxy->ParseNode(df->FirstChild("Galaxy")))
			return false;
	}

	if (mCreation->mWorlds > galaxy->GetPlanetCount())
		galaxy->Build(mCreation);	// make one

	PlacePlayers();

	return true;
}

void Game::PlacePlayers()
{
	deque<long> PNums(NumberPlayers());
	unsigned int i;

	for (i = 0; i < NumberPlayers(); ++i)
		PNums[i] = i+1;

	// MS random_shuffle has a bug so use this one instead
	Random_Shuffle(PNums.begin(), PNums.end());

	for (i = 0; i < NumberPlayers(); ++i) {
		galaxy->PlacePlayer(NCGetPlayer(PNums[i]));
	}

	// for all left over home worlds
	Planet * hw;
	while ((hw = TheGame->GetCreation()->GetNextHW()) != NULL) {
		hw->SetBaseNumber(-1);
	}
}

bool Game::LoadRules(const char * file, const char * MainNode)
{
	TiXmlDocument doc(file);
	doc.SetCondenseWhiteSpace(false);
	if (!doc.LoadFile()) {
		Message * mess = TheGame->AddMessage("Error: Cannot open file");
		mess->AddItem("", file);
		return false;
	}

	const TiXmlNode * node;
	const TiXmlNode * child;

	node = doc.FirstChild(MainNode);
	if (!node) {
		Message * mess = TheGame->AddMessage("Error: Missing section");
		mess->AddItem("File name", file);
		mess->AddItem("Section", MainNode);
		return false;
	}

	child = node->FirstChild("Rules");
	if (!child) {
		Message * mess = TheGame->AddMessage("Error: Missing section");
		mess->AddItem("File name", file);
		mess->AddItem("Section", "Rules");
		return false;
	}

	return LoadRules(GetString(child->FirstChild("File")),
					GetString(child->FirstChild("Verification")),
					GetDouble(child->FirstChild("Version")), true);
}

bool Game::LoadRules(const char * file, const char * verify, double version, bool checkver)
{
	string File;
	File = "rules/";
	File += file;

	if (checkver && Rules::GetModFileName() != "") {
//		if (!CheckCRC(File.c_str(), verify))
//			return false;
	} else {
//		verify = GetCRC(File.c_str());
	}

/*
	if (Rules::ModFileName != "") {
		if (Rules::ModFileName != File) {
			Message * mess = TheGame->AddMessage("Error: Different Rules Files");
			mess->AddItem("File name", File);
			return false;
		}
		if (Rules::ModFileVersion != version) {
			Message * mess = TheGame->AddMessage("Error: Different Rules Versions");
			mess->AddItem("File name", File);
			mess->AddItem("File version", version);
			return false;
		}
		if (Rules::ModFileCRC != verify) {
			Message * mess = TheGame->AddMessage("Error: Different Rules Checksums");
			mess->AddItem("File name", File);
			return false;
		}
	}
*/

	TiXmlDocument doc(File);
	doc.SetCondenseWhiteSpace(false);
	if (!doc.LoadFile()) {
		Message * mess = TheGame->AddMessage("Error: Cannot open file");
		mess->AddItem("File name", File);
		return false;
	}

	const TiXmlNode * rd;
	const TiXmlNode * node;

	rd = doc.FirstChild("RulesDefinition");
	if (!rd) {
		Message * mess = TheGame->AddMessage("Error: Missing section");
		mess->AddItem("File name", File);
		mess->AddItem("Section", "RulesDefinition");
		return false;
	}

	node = rd->FirstChild("MetaInfo");
	if (!node) {
		Message * mess = TheGame->AddMessage("Error: Missing section");
		mess->AddItem("File name", File);
		mess->AddItem("Section", "MetaInfo");
		return false;
	}

	double meta = GetDouble(node->FirstChild("FileVersion"));
	if (meta > RULESFILEVERSION + epsilon || meta < RULESFILEVERSION - epsilon) {
		Message * mess = TheGame->AddMessage("Warning: Incorrect FileVersion");
		mess->AddItem("File name", File);
		mess->AddItem("FileVersion", GetString(node->FirstChild("FileVersion")));
		return false;
	}

	meta = GetDouble(node->FirstChild("RulesVersion"));
	if (checkver) {
		if (meta > version + epsilon || meta < version - epsilon) {
			Message * mess = TheGame->AddMessage("Warning: Incorrect RulesVersion");
			mess->AddItem("File name", File);
			mess->AddItem("RulesVersion", GetString(node->FirstChild("RulesVersion")));
			return false;
		}
	} else {
		version = meta;
	}

	if (!Rules::LoadRules(rd->FirstChild("Rules"), file, verify, version))
		return false;

	if (!RacialTrait::LoadRacialTraits(rd->FirstChild("RacialTraits")))
		return false;

	if (!LoadComponents(rd->FirstChild("Components")))
		return false;

	if (mCreation) {
		if (!LoadStartShips(rd->FirstChild("RacialTraits")))
			return false;
	}

	return true;
}

bool Game::LoadXYFile()
{
	string File;
	File = mFileLoc;
	File += mFileName;
	File += ".xy";

	TiXmlDocument doc(File);
	doc.SetCondenseWhiteSpace(false);
	if (!doc.LoadFile()) {
		Message * mess = TheGame->AddMessage("Error: Cannot open file");
		mess->AddItem("File name", File);
		return false;
	}

	const TiXmlNode * xy;
	const TiXmlNode * node;

	xy = doc.FirstChild("XYFile");
	if (!xy) {
		Message * mess = TheGame->AddMessage("Error: Missing section");
		mess->AddItem("File name", File);
		mess->AddItem("Section", "XYFile");
		return false;
	}

	if (!Game::CheckMetaInfo(xy, File.c_str(), XYFILEVERSION))
		return false;

	long id = GetLong(xy->FirstChild("GameID"));
	if (id != TheGame->GetGameID()) {
		Message * mess = TheGame->AddMessage("Error: Missmatched Game IDs");
		mess->AddLong("Host file GameID", mGameID);
		mess->AddLong("XY file GameID", id);
		return false;
	}

	node = xy->FirstChild("Rules");
	if (!node) {
		Message * mess = TheGame->AddMessage("Error: Missing section");
		mess->AddItem("File name", File);
		mess->AddItem("Section", "Rules");
		return false;
	}

	if (!LoadRules(	GetString(node->FirstChild("File")),
					GetString(node->FirstChild("Verification")),
					GetDouble(node->FirstChild("Version")), true))
	{
		return false;
	}

	mNumberOfPlayers = GetLong(xy->FirstChild("NumberOfPlayers"));
	if (mNumberOfPlayers < 1) {
		Message * mess = TheGame->AddMessage("Error: Missing section");
		mess->AddItem("File name", File);
		mess->AddItem("Section", "NumberOfPlayers");
		return false;
	}

	if (!LoadCreation(xy->FirstChild("Creation")))
		return false;

	if (!galaxy->ParseNode(xy->FirstChild("Galaxy")))
		return false;

	return true;
}

void Game::WriteXYFile()
{
	cout << "Writing XYfile File" << endl;

	TiXmlDocument doc;
	doc.SetCondenseWhiteSpace(false);

	TiXmlDeclaration decl("1.0", "", "yes");
	doc.InsertEndChild(decl);

	TiXmlElement * XYFile = new TiXmlElement("XYFile");
	TiXmlComment * comment = new TiXmlComment();
	comment->SetValue("Generated by FreeStars Server");
	XYFile->LinkEndChild(comment);
	TiXmlElement * MetaInfo = new TiXmlElement("MetaInfo");
	AddDouble(MetaInfo, "FreeStarsVersion", FREESTARSVERSION);
	AddDouble(MetaInfo, "FileVersion", HOSTFILEVERSION);
	XYFile->LinkEndChild(MetaInfo);
	AddLong(XYFile, "GameID", TheGame->GetGameID());
	Rules::WriteRulesFile(XYFile);
	AddLong(XYFile, "NumberOfPlayers", mNumberOfPlayers);

	TiXmlElement * cre = new TiXmlElement("Creation");
	AddString(cre, "GameName", Name.c_str());
	AddDouble(cre, "TechAdvances", TechFactor);

	TiXmlElement * size = new TiXmlElement("Size");
	AddLong(size, "MinX", galaxy->MinX());
	AddLong(size, "MaxX", galaxy->MaxX());
	AddLong(size, "MinY", galaxy->MinY());
	AddLong(size, "MaxY", galaxy->MaxY());
	cre->LinkEndChild(size);

	TiXmlElement * re = new TiXmlElement("RandomEvents");
	AddLong(re, "Start", RandomEventsStart);
	if (mRandomEvents == RE_ALL)
		AddString(re, "Event", "All");
	else {
		if (mRandomEvents & RE_MT)
			AddString(re, "Event", "Mystery Trader");
		if (mRandomEvents & RE_ARTIFACT)
			AddString(re, "Event", "Artifacts");
		if (mRandomEvents & RE_COMET)
			AddString(re, "Event", "Comets");
		if (mRandomEvents & RE_WORMHOLE) {
			AddString(re, "Event", "Wormholes");
			TiXmlElement * wh = new TiXmlElement("Wormholes");
			AddLong(wh, "MinimumPairs", mWHMin);
			AddLong(wh, "MaximumPairs", mWHMax);
			AddLong(wh, "MinDistance", mWHMinDistance);
			re->LinkEndChild(wh);
		}
	}
	cre->LinkEndChild(re);

	TiXmlElement * ps = new TiXmlElement("PublicScores");
	AddLong(ps, "Start", PublicScoreStart);
	if (PublicScore == PPS_ALL)
		AddString(ps, "Category", "All");
	else {
		if (mRandomEvents & PPS_PLANET)
			AddString(ps, "Category", "Planets");
		if (mRandomEvents & PPS_BASES)
			AddString(ps, "Category", "Bases");
		if (mRandomEvents & PPS_UNARM)
			AddString(ps, "Category", "Unarmed Ships");
		if (mRandomEvents & PPS_ESCORT)
			AddString(ps, "Category", "Escort Ships");
		if (mRandomEvents & PPS_CAPSHIP)
			AddString(ps, "Category", "Capital Ships");
		if (mRandomEvents & PPS_TECH)
			AddString(ps, "Category", "Tech Levels");
		if (mRandomEvents & PPS_RESOURCE)
			AddString(ps, "Category", "Resources");
		if (mRandomEvents & PPS_SCORE)
			AddString(ps, "Category", "Score");
		if (mRandomEvents & PPS_RANK)
			AddString(ps, "Category", "Rank");
	}
	cre->LinkEndChild(ps);

    VC->WriteNode(cre);

	Rules::WriteMinSettings(cre);
	XYFile->LinkEndChild(cre);

	TiXmlElement * galaxyElement = new TiXmlElement("Galaxy");
	galaxy->WriteXYFile(galaxyElement);
	XYFile->LinkEndChild(galaxyElement);
	doc.LinkEndChild(XYFile);

// For debug, save to a new file,
	string File;

	File = mFileLoc;
#ifdef _DEBUG
	File += "new/";
#endif // _DEBUG
	File += mFileName;
	File += ".xy";
	doc.SaveFile(File);
}

bool Game::WriteHostFile()
{
	cout << "Writing Host File" << endl;

	deque<Player *>::const_iterator pi;

	TiXmlDocument doc;
	doc.SetCondenseWhiteSpace(false);

	TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "yes");
	doc.LinkEndChild(decl);

	TiXmlElement * HostFile = new TiXmlElement("HostFile");
	TiXmlComment * comment = new TiXmlComment();
	comment->SetValue("Generated by FreeStars Server");
	HostFile->LinkEndChild(comment);
	TiXmlElement * MetaInfo = new TiXmlElement("MetaInfo");
	AddDouble(MetaInfo, "FreeStarsVersion", FREESTARSVERSION);
	AddDouble(MetaInfo, "FileVersion", HOSTFILEVERSION);
	HostFile->LinkEndChild(MetaInfo);

	AddLong(HostFile, "GameID", TheGame->GetGameID());
	AddLong(HostFile, "Turn", Turn);

	// Add seeds for the prior turn so that we can regen a turn if there is some hard to reproduce bug
	TiXmlElement * Seeds = new TiXmlElement("OldSeeds");
	int i;
	for (i = 0; i < 601; ++i) {
		if (mSeed[i] == 0)
			break;
		else
			AddLong(Seeds, "Seed", mSeed[i]);
	}
	AddLong(Seeds, "Seeds", i);
	HostFile->LinkEndChild(Seeds);

#ifdef _DEBUG
	AddLong(HostFile, "Seed", genrand_int32());
#endif // _DEBUG

	for (pi = mPlayers.begin(); pi != mPlayers.end(); ++pi) {
		TiXmlElement * player = new TiXmlElement("Player");
		(*pi)->WriteNode(player, NULL);

		HostFile->LinkEndChild(player);
	}

	TiXmlElement * galaxyElement = new TiXmlElement("Galaxy");
	galaxy->WriteNode(galaxyElement, NULL);
	HostFile->LinkEndChild(galaxyElement);

	TiXmlElement * eMess = new TiXmlElement("ErrorMessages");
	for (i = 0; i < mMessages.size(); ++i)
		mMessages[i]->WriteNode(eMess);

	for (pi = mPlayers.begin(); pi != mPlayers.end(); ++pi) {
		TiXmlElement * player = new TiXmlElement("Player");
		AddLong(player, "PlayerNumber", (*pi)->GetID());
		(*pi)->WriteMessages(player, "Error");
		eMess->LinkEndChild(player);
	}
	HostFile->LinkEndChild(eMess);

	doc.LinkEndChild(HostFile);

// For debug, save to a new file,
	string File;

	File = mFileLoc;
#ifdef _DEBUG
	File += "new/";
#endif // _DEBUG
	File += mFileName;
	File += ".hst";
	doc.SaveFile(File);

//	if (rename(File.c_str(), Backup.c_str()) != 0)
//		return false;

	return true;
}

void Game::UpdateLoadBy()
{
	int i, j, l;	// loop counters
	CargoHolder * other;
	bool pickpocket;

	// for all objects
	for (i = 0; i < mTopObjects.size(); ++i) for (j = 0; j < mTopObjects[i]->size(); ++j) {
		Fleet * fleet = dynamic_cast<Fleet *>(mTopObjects[i]->at(j));
		if (fleet == NULL)
			continue;

		pickpocket = fleet->CanStealShip();

		// check everything here at the same location
		for (l = 0; l < mTopObjects[i]->size(); ++l) {
			other = dynamic_cast<CargoHolder *>(mTopObjects[i]->at(l));
			if (other != NULL && fleet->GetOwner() != other->GetOwner()) {
				if (l == 0 && dynamic_cast<Planet *>(other) != NULL) {
					if (fleet->CanStealPlanet() || (fleet->GetFirstOrder()->GetType() == OT_REMOTEMINE && other->GetOwner() == NULL))
						other->SetCanLoadBy(fleet->GetOwner());
				} else if (pickpocket)
					other->SetCanLoadBy(fleet->GetOwner());
			}
		}
	}
}

void Game::UpdateSeen()
{
/*
for all topobjects
 for each obj
  reset seen
for all topobjects
 for each obj
  get space scan & pen scan
  see everything here
  for all topobjects not here
   calc dist
    for each obj
     calc cloakdist
     if cloakdist < pen scan
      set cansee
     else if cloakdist < space scan
      if not in orbit or obj type is mine or packet
       set cansee
*/

	/**
	 *@todo
	 * Notes on UpdateSeen.
	 @code
	// TODO: switch loop as in optimize, and then we don't need to clear the seen flags first, and can send a message to the player
	// if they could see it last year, but cannot see it this year. Player can duplicate some of this functionality by having a junk
	// fleet target every enemy fleet
	// OPTIMIZE: it is probably faster to loop over objects, checking to see if a player can see it, and stopping the first time it can
	// OPTIMIZE: or keep current loop, but don't check an object if you've already seen it
	 @endcode
	 */

	int i, j, k, l;	// loop counters
	long pen;
	long space;
	double mradius;
	SpaceObject * viewer;
	SpaceObject * other;
	long inorbit;
	Planet * planet = NULL;
	MineField * mine;
	double dist, cloakdist;

	// for all objects
	for (i = 0; i < mTopObjects.size(); ++i) for (j = 0; j < mTopObjects[i]->size(); ++j) {
		// get space scan & pen scan
		viewer = mTopObjects[i]->at(j);
		planet = dynamic_cast<Planet *>(viewer);
		if (planet != NULL)
			planet = NULL;

		if (viewer->GetOwner() == NULL)
			continue;

		mine = dynamic_cast<MineField *>(viewer);
		if (mine != NULL)
			mradius = mine->GetRadius();
		else
			mradius = -1;

		pen = viewer->GetScanPenetrating();
		space = viewer->GetScanSpace();

		if (pen <= 0 && space < 0 && mradius <= 0)	// if viewer can't see, stop now
			continue;

		if (space >= 0) {
			// see everything else at the same location
			for (l = 0; l < mTopObjects[i]->size(); ++l) if (j != l) {
				other = mTopObjects[i]->at(l);
				if (viewer->GetOwner() != other->GetOwner())
					other->SetSeenBy(viewer->GetOwner()->GetID()-1, pen >= 0 ? SEEN_PENSCAN : SEEN_SCANNER);
			}
		}

		if (pen <= 0 && space <= 0 && mradius < epsilon)	// if viewer can't see beyond current position, stop now
			continue;

		// for all topobjects not here
		for (k = 0; k < mTopObjects.size(); ++k) if (k != i) {
			// calc dist
			dist = viewer->Distance(mTopObjects[k]->at(0));
			if (dist > pen && dist > space)	// if we can't see it uncloaked, stop now
				continue;

			inorbit = -1;
			planet = NULL;
			// if dist < pen scan
			// for each obj

			for (l = 0; l < mTopObjects[k]->size(); ++l) {
				other = mTopObjects[k]->at(l);
				if (l == 0) {
					planet = dynamic_cast<Planet *>(other);
					inorbit = planet != NULL;
				}

				if (dist < mradius) {
					if (other->GetOwner() != NULL && (typeid(*other) == typeid(Fleet) || typeid(*other) == typeid(Planet)))
						viewer->SetSeenBy(other->GetOwner()->GetID()-1, SEEN_SCANNER);

					if (!planet && viewer->GetOwner()->MineFieldScanning() && Random(100) > other->GetCloak(viewer->GetOwner(), false))
						other->SetSeenBy(viewer->GetOwner()->GetID()-1, SEEN_SCANNER);
				}

				cloakdist = dist / (1.0 - other->GetCloak(viewer->GetOwner(), true) * viewer->GetMaxTachyon());
				if (cloakdist < pen) {
					// check for (possibly cloaked) bases
					if (planet != NULL && planet->GetOwner() && planet->GetBaseNumber() >= 0) {
						cloakdist = dist/ (1.0 - Rules::CloakValue(planet->GetBaseDesign()->GetCloaking(), planet->GetBaseDesign()->GetMass()) * viewer->GetMaxTachyon());
						if (cloakdist < pen)
							planet->SetSeenBy(viewer->GetOwner()->GetID()-1, SEEN_PENSCAN | SEEN_HULL);
					}
					// check cansee
					other->SetSeenBy(viewer->GetOwner()->GetID()-1, (l == 0 && planet) ? SEEN_PENSCAN : (SEEN_PENSCAN | SEEN_HULL));
				} else {
					cloakdist = dist / (1.0 - other->GetCloak(viewer->GetOwner(), false) * viewer->GetMaxTachyon());
					if (cloakdist < space) {
						if (inorbit == -1)
							inorbit = dynamic_cast<Planet *>(mTopObjects[k]->at(0)) != NULL ? 1 : 0;
						// if not in orbit or obj type is mine or packet
						if (inorbit == 0 ||
							dynamic_cast<MineField *>(other) != NULL ||
							dynamic_cast<Packet *>(other) != NULL)
						{
							// check cansee
							other->SetSeenBy(viewer->GetOwner()->GetID()-1, SEEN_SCANNER);
						}
					}
				}
			}
		}
	}
}

void Game::ResetSeen()
{
	int i, j;
	// for all objects
	for (i = 0; i < mTopObjects.size(); ++i) for (j = 0; j < mTopObjects[i]->size(); ++j)
		mTopObjects[i]->at(j)->ResetSeen();

	for (i = 0; i < mPlayers.size(); ++i)
		mPlayers[i]->ResetSeen();
}

bool Game::WritePlayerFiles()
{
	cout << "Writing Player Files" << endl;

	deque<Player *>::const_iterator pi;
	for (pi = mPlayers.begin(); pi != mPlayers.end(); ++pi) {
		TiXmlDocument doc;
		doc.SetCondenseWhiteSpace(false);

		TiXmlDeclaration decl("1.0", "", "yes");
		doc.InsertEndChild(decl);

		TiXmlElement * TurnFile = new TiXmlElement("PlayerTurnFile");

		TiXmlComment * comment = new TiXmlComment();
		comment->SetValue("Generated by FreeStars Server");
		TurnFile->LinkEndChild(comment);
		TiXmlElement * MetaInfo = new TiXmlElement("MetaInfo");
		AddDouble(MetaInfo, "FreeStarsVersion", FREESTARSVERSION);
		AddDouble(MetaInfo, "FileVersion", TURNFILEVERSION);
		TurnFile->LinkEndChild(MetaInfo);

		AddLong(TurnFile, "GameID", TheGame->GetGameID());
		AddLong(TurnFile, "Turn", Turn);

		TiXmlElement * player = new TiXmlElement("Player");
		(*pi)->WriteNode(player, *pi);
		TurnFile->LinkEndChild(player);

		TiXmlElement * galaxyElement = new TiXmlElement("Galaxy");
		galaxy->WriteNode(galaxyElement, *pi);
		TurnFile->LinkEndChild(galaxyElement);

		deque<Player *>::const_iterator pi2;
		for (pi2 = mPlayers.begin(); pi2 != mPlayers.end(); ++pi2) {
			if (*pi != *pi2) {
				TiXmlElement * player = new TiXmlElement("Player");
				if ((*pi2)->WriteNode(player, *pi))
					TurnFile->LinkEndChild(player);
				else
					delete player;
			}
		}

		TiXmlElement * eMess = new TiXmlElement("Messages");
		(*pi)->WriteMessages(eMess, "");
		TurnFile->LinkEndChild(eMess);

		doc.LinkEndChild(TurnFile);

		char buf[1024];
#ifdef _DEBUG
		sprintf(buf, "%snew/%s.m%ld", mFileLoc.c_str(), mFileName.c_str(), long(pi - mPlayers.begin() + 1));
#else
		sprintf(buf, "%s%s.m%ld", mFileLoc.c_str(), mFileName.c_str(), long(pi - mPlayers.begin() + 1));
#endif // _DEBUG
		doc.SaveFile(buf);
	}

//	if (rename(File.c_str(), Backup.c_str()) != 0)
//		return false;

	return true;
}

bool Game::LoadTurns()
{
	deque<long> PNums(NumberPlayers());
	unsigned int i;

	for (i = 0; i < NumberPlayers(); ++i)
		PNums[i] = i;

	// MS random_shuffle has a bug so use this one instead
	Random_Shuffle(PNums.begin(), PNums.end());

	for (i = 0; i < NumberPlayers(); ++i) {
		if (!ProcessWaypoints(i))
			return false;
	}

	for (i = 0; i < NumberPlayers(); ++i) {
		if (!ProcessOrders(PNums[i]))
			return false;
	}

	if (!AssignWaypoints())
		return false;

	return true;
}

bool Game::ProcessWaypoints(long pnumber)
{
	string File;
	File = mFileLoc;
	File += mFileName;
	File += ".x" + Long2String(pnumber+1);

	Player * player = mPlayers[pnumber];

	TiXmlDocument doc(File.c_str());
	doc.SetCondenseWhiteSpace(false);
	if (!doc.LoadFile()) {
		Message * mess = TheGame->AddMessage("Error: Cannot open file");
		mess->AddItem("File name", File);
		return false;
	}

	const TiXmlNode * orders;
	const TiXmlNode * node;
	orders = doc.FirstChild("OrdersFile");
	if (!orders) {
		Message * mess = TheGame->AddMessage("Error: Missing section");
		mess->AddItem("File name", File);
		mess->AddItem("Section", "OrdersFile");
		return false;
	}

	if (!Game::CheckMetaInfo(orders, File.c_str(), ORDERSFILEVERSION))
		return false;

	long id = GetLong(orders->FirstChild("GameID"));
	if (id != TheGame->GetGameID()) {
		Message * mess = TheGame->AddMessage("Error: Missmatched Game IDs");
		mess->AddLong("Host file GameID", mGameID);
		mess->AddLong("Orders file GameID", id);
		return false;
	}

	node = orders->FirstChild("PlayerNo");
	if (GetLong(node) != pnumber+1) {
		Message * mess = player->AddMessage("Error: Invalid player number");
		mess->AddLong("", GetLong(node));
		mess->AddItem("File name", File);
		return false;
	}

	node = orders->FirstChild("Turn");
	if (GetLong(node) != Turn-1) {
		Message * mess = player->AddMessage("Error: Wrong year number in turn file");
		mess->AddLong("Turn specified", GetLong(node));
		mess->AddLong("Actual turn", Turn);
		return false;
	}

	// process all orders
	for (node = orders->FirstChild("Waypoints"); node; node = node->NextSibling("Waypoints")) {
		WayOrderList & wol = *mOrders.insert(mOrders.end(), WayOrderList());
		wol.SetFleet(GetLong(node->FirstChild("Fleet")));
		wol.ParseNode(node, player, galaxy);
	}

	return true;
}

bool Game::AssignWaypoints()
{
	Player * p;
	Fleet * f;
	for (int i = 0; i < mOrders.size(); ++i) {
		p = NCGetPlayer(mOrders[i].GetPlayer());
		if (p == NULL)
			return false;

		f = p->NCGetFleet(mOrders[i].GetFleet());
		if (f == NULL)
			return false;

		f->ChangeWaypoints(mOrders[i]);
	}

	return true;
}

bool Game::ProcessOrders(long pnumber)
{
	const TiXmlNode * node;

	string File;
	File = mFileLoc;
	File += mFileName;
	File += ".x" + Long2String(pnumber+1);

	TiXmlDocument doc(File.c_str());
	doc.SetCondenseWhiteSpace(false);
	if (!doc.LoadFile()) {
		Message * mess = mPlayers[pnumber]->AddMessage("Error: Cannot open file");
		mess->AddItem("File name", File);
		return false;
	}

	const TiXmlNode * orders;
	orders = doc.FirstChild("OrdersFile");
	if (!orders) {
		Message * mess = mPlayers[pnumber]->AddMessage("Error: Missing section");
		mess->AddItem("File name", File);
		mess->AddItem("Section", "OrdersFile");
		return false;
	}

	long id = GetLong(orders->FirstChild("GameID"));
	if (id != TheGame->GetGameID()) {
		Message * mess = TheGame->AddMessage("Error: Missmatched Game IDs");
		mess->AddLong("Host file GameID", mGameID);
		mess->AddLong("Orders file GameID", id);
		return false;
	}

	node = orders->FirstChild("PlayerNo");
	if (GetLong(node) != pnumber+1) {
		Message * mess = mPlayers[pnumber]->AddMessage("Error: Invalid player number");
		mess->AddLong("", GetLong(node));
		mess->AddItem("File name", File);
		return false;
	}

	node = orders->FirstChild("Turn");
	if (GetLong(node) != Turn-1) {
		Message * mess = mPlayers[pnumber]->AddMessage("Error: Wrong year number in turn file");
		mess->AddLong("Turn specified", GetLong(node));
		mess->AddLong("Actual turn", Turn);
		return false;
	}

	mPlayers[pnumber]->ParseOrders(orders);

	return true;
}

Player * Game::NCGetPlayer(unsigned int n)
{
	if (n < 1 || n > mPlayers.size())
		return NULL;

	return mPlayers[n-1];
}

const Component * Game::ParseComponent(const char * name) const
{
	if (name != NULL && *name != '\0') {
		deque<Component *>::const_iterator iter;
		for (iter = mComponents.begin(); iter != mComponents.end(); ++iter) {
			if (stricmp((*iter)->GetName().c_str(), name) == 0)
				return *iter;
		}
	}

	return NULL;
}

const Component * Game::GetBestComp(const Player * player, const char * ValueType, bool CheckRad, HullType hullType) const
{
	long vt = Component::ParseSubType(ValueType, false);
	if (vt < 0)
		return NULL;
	else
		return GetBestComp(player, vt, CheckRad, hullType);
}

const Component * Game::GetBestComp(const Player * player, long vt, bool CheckRad, HullType hullType) const
{
	long temp;
	long Score = 0;
	const Component * Result = NULL;

	deque<Component *>::const_iterator iter;
	for (iter = mComponents.begin(); iter != mComponents.end(); ++iter) {

		if (!(*iter)->IsBuildable(player))
			continue;

		if (CheckRad && player->RadDamage((*iter)->GetRadiation()) > 0.0)
			continue;

		if (!((*iter)->GetHullType() & hullType))
			continue;

		temp = Component::GetScore(*iter, vt);
		if (temp > Score) {
			Result = *iter;
			Score = temp;
		}
	}

	return Result;
}

bool Game::ProcessTurn()
{
	deque<long> PNums(NumberPlayers());
	unsigned int i;

	for (i = 0; i < NumberPlayers(); ++i)
		PNums[i] = i;

	// MS random_shuffle has a bug so use thise one instead
	Random_Shuffle(PNums.begin(), PNums.end());

	for (mTurnPhase = 0; mTurnPhase < Rules::GetNumberTurnEvents(); ++mTurnPhase) {
		switch(Rules::GetTurnEvent(mTurnPhase)) {
		case TP_SCRAPFLEET:
			// Scrapping Fleets -- in numeric order so players can coordinate tech scrapping
			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[i]->ForEachFleet(Fleet::FTScrap, true);
			break;

		case TP_WAY0_UNLOAD:
			// Waypoint zero tasks -- in random order
			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[PNums[i]]->ForEachFleet(Fleet::FTUnload0, true);
			break;

		case TP_WAY0_COLONIZE:
			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[PNums[i]]->ForEachFleet(Fleet::FTColonize0, true);
			break;

		case TP_WAY0_INVADE:
			galaxy->ResolveInvasions();
			break;

		case TP_WAY0_LOAD:
			for (i = 0; i < NumberPlayers(); ++i) {
				mPlayers[PNums[i]]->ForEachFleet(Fleet::FTLoad0, false);
				mPlayers[PNums[i]]->ForEachFleet(Fleet::FTLoad0, true);		// do dunnage loads
			}
			break;

		case TP_TECH_CHECK:
			// Check for Tech gains
			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[i]->CheckTechGain();
			break;

		case TP_MT_MOVE:
			///@todo Mystery Trader Moves
			break;

		case TP_PACKETS1:
			// Packets
			galaxy->MovePackets(false);
			break;

		case TP_DEADCHECK:
			// Check for dead worlds
			galaxy->DeadCheck();
			break;

		case TP_MOVEMENT:
			{
				// Fleets move, hit mines
				long Left = 0;
				long Last = -1;

				// move fleets till only cycles left
				while (Left != Last) {
					for (i = 0; i < NumberPlayers(); ++i) {
						Last = Left;
						// ForEach will return the number of fleets that need more work
						// so keep calling it till there same number of fleets needing work for two runs
						Left = mPlayers[i]->ForEachFleet(Fleet::FTMove, true);
					}
				}

				///@todo Move cycles of fleets following each other
			}
			break;

		case TP_FREIGHTER_GROW:
			// Inner Strength colonists grow -- in order cause it doesn't matter
			for (i = 0; i < NumberPlayers(); ++i) {
				if (mPlayers[i]->FreighterReproduction() > epsilon)
					mPlayers[i]->ForEachFleet(Fleet::FTFreighterReproduction, true);
			}
			break;

		case TP_SALVAGEDECAY:
			// Salvage decay
			galaxy->DecaySalvage();
			break;

		case TP_WORMHOLES:
			// Wormholes shift
			galaxy->JiggleWormholes();
			break;

		case TP_RESETSEEN:
			// Reset seen objects, must be before battles (maybe before mines detonate) and after movement
			ResetSeen();
			break;

		case TP_MINES_DETONATE:
			// Minefields detonate
			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[i]->DetonateMineFields();

			break;

		case TP_MINING:
			// Mining
			if (!galaxy->Mine())
				return false;

			//deque<Player *>::iterator pi;
			// AR's Remote mining their own worlds, happens right after normal mining
			for (i = 0; i < NumberPlayers(); ++i) {
				if (mPlayers[i]->ARTechType() >= 0)
					mPlayers[i]->ForEachFleet(Fleet::FTRemoteMine, true);
			}

			break;

		case TP_PRODUCTION:
			// Production and Research
			galaxy->DoProduction();
			break;

		case TP_SPYBONUS:
			// SS Spy bonus obtained
			for (i = 0; i < NumberPlayers(); ++i) {
				if (mPlayers[i]->SpyTechBonus() > epsilon)
					galaxy->GainSpyTech(mPlayers[i]);
			}
			break;

		case TP_REFUEL:
			// Refuel fleets
			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[i]->ForEachFleet(Fleet::FTRefuel, true);
			break;

			//  Population grows/dies
		case TP_POPULATION:
			galaxy->GrowPop();
			break;

		case TP_INSTA_PACKETS:
			// Packets that just launched move
			galaxy->MovePackets(true);
			break;

		case TP_RANDOM_EVENTS:
			///@todo Random events
			break;

		case TP_BATTLES:
			///@todo Fleet battles
			for (i = 0; i < NumberPlayers(); ++i) {
				mPlayers[i]->DoBattles();
			}
			break;

		case TP_REMOTE_MINE:
			// Remote mine uninhabited worlds, in random player order
			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[PNums[i]]->ForEachFleet(Fleet::FTRemoteMine, false);
			break;

		case TP_MEET_MT:
			///@todo Meet MT
			break;

		case TP_BOMBING:
			// Bombing
			galaxy->DoBombing();
			break;

		case TP_REPAIR:
			// Starbase and fleet repair
			galaxy->RepairBases();
			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[i]->ForEachFleet(Fleet::FTRepair, false);
			break;

		// Waypoint 1 tasks
		case TP_WAY1_UNLOAD:
			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[i]->ForEachFleet(Fleet::FTCheckWaypoint, true);

			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[PNums[i]]->ForEachFleet(Fleet::FTUnload1, true);
			break;

		case TP_WAY1_COLONIZE:
			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[PNums[i]]->ForEachFleet(Fleet::FTColonize1, true);
			break;

		case TP_WAY1_INVADE:
			galaxy->ResolveInvasions();
			break;

		case TP_UPDATELOADBY:
			// Check for robber barron scanners and remote miners ability to load from unowned things
			// this check must be after colonizations(invasions), and before loads
			UpdateLoadBy();

		case TP_WAY1_LOAD:
			for (i = 0; i < NumberPlayers(); ++i) {
				mPlayers[PNums[i]]->ForEachFleet(Fleet::FTLoad1, false);
				mPlayers[PNums[i]]->ForEachFleet(Fleet::FTLoad1, true);		// do dunnage loads
			}
			break;

		case TP_MINELAYING:
			// Mine Laying
			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[i]->ForEachFleet(Fleet::FTLayMines, true);
			break;

		case TP_MINEDECAY:
			// Minefield Decay
			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[i]->DecayMineFields();
			break;

		case TP_MINESWEEP:
			// Mine sweeping
			galaxy->SweepMines();
			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[i]->ForEachFleet(Fleet::FTSweepMines, true);
			break;

		case TP_TRANSFER:
			// Fleet Transfer -- be sure to update CanLoadBy if the ship transferred has the capability to steal minerals
			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[i]->ForEachFleet(Fleet::FTTransfer, true);
			break;

		case TP_MERGE:
			// Fleet Merges
			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[i]->ForEachFleet(Fleet::FTMerge, true);
			break;

		case TP_INSTA_TERRAFORM:
			// Instaforming
			galaxy->Instaform();
			break;

		case TP_REMOTE_TERRAFORM:
			// Remote Terraforming
			for (i = 0; i < NumberPlayers(); ++i)
				mPlayers[i]->ForEachFleet(Fleet::FTRemoteTerraform, true);
			break;

		case TP_UPDATESCANNING:
			// Update seen things, must be before patrol, best to be as late as possible, after transfer at least
			UpdateSeen();
			galaxy->AdjustWormholeTraverse();
			break;

		default:
			// error, unknown turn phase
			break;
		}
	}
	// clear old orders
	for (i = 0; i < NumberPlayers(); ++i)
		mPlayers[i]->ForEachFleet(Fleet::FTClearWaypoint, true);

	return true;
}

void Game::InitialSeen()
{
	ResetSeen();
	UpdateSeen();
}

long Game::GetTerraLimit(const Player * player, HabType ht)
{
	long temp;
	long Score = 0;

	deque<Component *>::const_iterator iter;
	for (iter = mComponents.begin(); iter != mComponents.end(); ++iter) {
		if ((*iter)->GetType() != CT_TERRAFORM)
			continue;

		if (!(*iter)->IsBuildable(player))
			continue;

		if ((*iter)->GetTerraType() == ht || (*iter)->GetTerraType() == -1) {
			temp = (*iter)->GetTerraLimit();
			if (temp > Score)
				Score = temp;
		}
	}

	return Score;
}

deque<SpaceObject *> * Game::GetClosestTop(int x, int y, long max/*= 0*/)
{
	if (mTopObjects.size() <= 0)
		return NULL;

	double lowestdist = mTopObjects[0]->at(0)->Distance(x, y);
	double dist;
	int closest = 0;
	int j;
	for (j = 1; j < mTopObjects.size(); ++j) {
		dist = mTopObjects[j]->at(0)->Distance(x, y);
		if (dist < lowestdist) {
			lowestdist = dist;
			closest = j;
		}

		if (dist < epsilon)
			break;
	}

	if (max <= 0 || lowestdist < max)
		return mTopObjects[closest];
	else
		return NULL;
}

void Game::AddAlsoHere(SpaceObject * loc)
{
	int j;
	for (j = 0; j < mTopObjects.size(); ++j) {
		if (loc->IsWith(*mTopObjects[j]->at(0)))
			break;
	}

	if (j < mTopObjects.size()) {
		assert(find(mTopObjects[j]->begin(), mTopObjects[j]->end(), loc) == mTopObjects[j]->end());
		mTopObjects[j]->push_back(loc);
		loc->SetAlsoHere(mTopObjects[j]);
	} else {
		mTopObjects.push_back(new deque<SpaceObject *>(1, loc));
		loc->SetAlsoHere(mTopObjects[mTopObjects.size()-1]);
	}
}

void Game::RemoveAlsoHere(SpaceObject * loc)
{
	deque<SpaceObject *>::iterator i;
	deque<SpaceObject *> * deq = loc->GetAlsoHere();

	i = find(deq->begin(), deq->end(), loc);
	deq->erase(i);

	if (deq->size() == 0) {
		deque<deque<SpaceObject *> *>::iterator i2;
		i2 = find(mTopObjects.begin(), mTopObjects.end(), deq);
		mTopObjects.erase(i2);
	}
}

void Game::MoveAlsoHere(SpaceObject * loc)
{
	deque<SpaceObject *> * deq = loc->GetAlsoHere();

	if (loc->IsWith(*deq->at(0)))
		return;

	RemoveAlsoHere(loc);
	AddAlsoHere(loc);
}

double Game::GetPossibleMines(deque<MineField *> *pm, const Fleet * f, double dist) const
{
	double Result = galaxy->MaxX() + galaxy->MaxY();

	int i;
	double d;
	for (i = 0; i < mNumberOfPlayers; ++i) {
		d = mPlayers[i]->GetPossibleMines(pm, f, dist);
		if (d < Result)
			Result = d;
	}

	return Result;
}

void Game::SweepMines(SpaceObject * so, int sweep, const BattlePlan * bp)
{
	int i;
	for (i = 0; i < mNumberOfPlayers; ++i) {
		if (bp->WillFight(so->GetOwner(), mPlayers[i]))
			mPlayers[i]->SweepMines(so, sweep);
	}
}

void Game::DetonateMineField(const MineField * mf)
{
	double radius = mf->GetRadius();
	int i, j;
	for (j = 0; j < mTopObjects.size(); ++j) {
		if (mf->Distance(mTopObjects[j]->at(0)) > radius)
			continue;

		for (i = 0; i < mTopObjects[j]->size(); ++i) {
			Fleet * f = dynamic_cast<Fleet *>(mTopObjects[j]->at(i));
			if (f != NULL)
				f->TakeMinefieldDamage(mf);
		}
	}
}

SpaceObject * Game::GetPatrolTarget(const Fleet * persuer, int range) const
{
	SpaceObject * Result = NULL;
	SpaceObject * ftemp;
	double rtemp;
	double minrange = range;
	int i;

	if (persuer->CanShoot()) {
		for (i = 0; i < mNumberOfPlayers; ++i) {
			if (persuer->GetBattlePlan()->WillFight(persuer->GetOwner(), mPlayers[i])) {
				ftemp = mPlayers[i]->GetPatrolTarget(persuer, &rtemp);
				if (ftemp != NULL && rtemp <= minrange) {
					Result = ftemp;
					minrange = rtemp;
				}
			}
		}
	}

	if (Result == NULL && persuer->GetCargoCapacity() > persuer->GetCargoMass()) {
		Result = galaxy->GetPatrolTarget(persuer, range);
	}

	return Result;
}

Component * Game::ObjectFactory(const Component *)
{
	return new Component();
}

Player * Game::ObjectFactory(const Player *, int PlayerNumber)
{
	return new Player(galaxy, PlayerNumber);
}

RacialTrait * Game::ObjectFactory(const RacialTrait *)
{
	return new RacialTrait();
}

Race * Game::ObjectFactory(const Race *)
{
	return new Race();
}

Ship * Game::ObjectFactory(const Ship *)
{
	return new Ship();
}

Planet * Game::ObjectFactory(const Planet *)
{
	return new Planet(galaxy);
}

Salvage * Game::ObjectFactory(const Salvage *)
{
	return new Salvage(galaxy);
}

Packet * Game::ObjectFactory(const Packet *)
{
	return new Packet(galaxy);
}

Wormhole * Game::ObjectFactory(const Wormhole *)
{
	return new Wormhole(galaxy);
}

}
