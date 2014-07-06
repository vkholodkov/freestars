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
#include "Battle.h"
#include "Bombing.h"
#include "RacialTrait.h"
#include "Creation.h"
#include "Order.h"
#include "MineFieldType.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

Player::Player(Game *_game, int id)
    : game(_game)
    , mID(id)
{
	mTechLevel.insert(mTechLevel.begin(), Rules::MaxTechType, -1);
	mTempTechLevel.insert(mTempTechLevel.begin(), Rules::MaxTechType, 0);
	mTechProgress.insert(mTechProgress.begin(), Rules::MaxTechType, -1);
	mTerraLimit.insert(mTerraLimit.begin(), Rules::MaxHabType, 0);
	mShipDesigns.insert(mShipDesigns.begin(), Rules::GetConstant("MaxShipDesigns"), (Ship *)NULL);
	mBaseDesigns.insert(mBaseDesigns.begin(), Rules::GetConstant("MaxBaseDesigns"), (Ship *)NULL);
	mFleets.insert(mFleets.begin(), Rules::MaxFleets, NULL);
	mDefaultPayTax = false;
	mBattlePlans.insert(mBattlePlans.begin(), Rules::GetConstant("MaxBattlePlans"), (BattlePlan *)NULL);
	mHasHW = false;
	mWriteXFile = false;
	mUnsavedChanges = 0;
	mMO = NULL;
	mLastTechGainPhase = 0;
}

Player::~Player()
{
	int i;
	for (i = 0; i < mDefaultQ.size(); ++i)
		delete mDefaultQ[i];

	for (i = 0; i < mFleets.size(); ++i)
		delete mFleets[i];

	mFleets.clear();

	for (i = 0; i < mShipDesigns.size(); ++i)
		delete mShipDesigns[i];

	for (i = 0; i < mBaseDesigns.size(); ++i)
		delete mBaseDesigns[i];

	for (i = 0; i < mMessages.size(); ++i)
		delete mMessages[i];

	for (i = 0; i < mBattlePlans.size(); ++i)
		delete mBattlePlans[i];

	for (i = 0; i < mOrders.size(); ++i)
		delete mOrders[i];
}

Message * Player::AddMessage(string type, const Location * loc)
{
	Message * mess = new Message(type, loc);
	mMessages.push_back(mess);
	return mess;
}

Message * Player::AddMessage(string type)
{
	// only for very bad error messages
	Message * mess = new Message(type);
	mMessages.push_back(mess);
	return mess;
}

void Player::StoreMessageLocation(const Location * loc)
{
	for (int i = 0; i < mMessages.size(); ++i)
		mMessages[i]->StoreMessageLocation(loc, this);
}

void Player::WriteMessages(TiXmlNode * node, const char * type)
{
	for (unsigned int i = 0; i < mMessages.size(); ++i) {
		if (type == NULL || *type == '\0' || mMessages[i]->IsType(type)) {
			mMessages[i]->WriteNode(node);
		}
	}
}

double Player::Miniturize(const Component * comp) const
{
	if (comp->GetType() & (CT_PLANETARY & ~CT_PLANET_SPECIAL))
		return 1.0;	// execpt for special items, planetary stuff does not miniturize

	long TechDif = Rules::MaxTechLevel;
	for (long i = 0; i < Rules::MaxTechType; ++i) {	// check each field
		if (comp->GetTech(i) >= 0)
			TechDif = min(TechDif, GetTechLevel(i) - comp->GetTech(i));
	}

	if (TechDif == Rules::MaxTechLevel) {
		Message * mess = game->AddMessage("Item with no required tech");
		mess->AddItem("Component name", comp->GetName());
	}

	return Race::Miniturize(TechDif);
}

bool Player::GainTechLevel(TechType tech)
{
	bool Result = false;
	mTechProgress[tech] -= TechCost(tech);
	mLastTechGainPhase = game->GetTurnPhase();

	if (Rules::GetTurnEvent(game->GetTurnPhase()) == TP_PRODUCTION)
		mTempTechLevel[tech]++;
	else
		mTechLevel[tech]++;

	Message * mess = AddMessage("Gain tech level");
	mess->AddItem("Tech field", Rules::GetTechName(tech));
	mess->AddLong("New Tech Level", mTechLevel[tech] + mTempTechLevel[tech]);
	if (mResearchField == tech) {
		switch (mResearchNext) {
		case NEXT_SAME:
			break;
		case NEXT_LOW:
			{
				mResearchField = 0;
				for (long t2 = 1; t2 < Rules::MaxTechType; ++t2) {
					if ((mTechLevel[t2] + mTempTechLevel[t2]) < (mTechLevel[mResearchField] + mTempTechLevel[mResearchField]))
						mResearchField = t2;
				}
				break;
			}
		case RESEARCH_ALCHEMY:
			mResearchField = RESEARCH_ALCHEMY;
			Result = true;	// report left over resources
			break;
		case NEXT_CHEAP:
			{
				mResearchField = 0;
				long lowcost = TechCost(0);
				long tempcost;
				for (long t2 = 1; t2 < Rules::MaxTechType; ++t2) {
					tempcost = TechCost(t2);
					if (tempcost < lowcost) {
						mResearchField = t2;
						lowcost = tempcost;
					}
				}
				break;
			}
		default:
			assert(mResearchNext >= 0 && mResearchNext < Rules::MaxTechType);
			mResearchField = mResearchNext;
			mResearchNext = NEXT_SAME;
			break;
		}

		mess->AddItem("Next tech field", Rules::GetTechName(mResearchField));
	}

	return Result;
}

// return the amount of left over resources
long Player::GainTech(long TechGain, TechType tech)
{
	if (tech < 0 || tech >= Rules::MaxTechType)
		return TechGain;

	long Result = 0;
	if (Rules::GetTurnEvent(game->GetTurnPhase()) == TP_PRODUCTION) {
		if (mResearchField == RESEARCH_ALCHEMY)
			return TechGain;

		long cost;
		bool more = true;
		long resources = TechGain;
		while (more) {
			cost = TechCost(mResearchField) - mTechProgress[mResearchField];
			if (resources < cost) {
				more = false;
				mTechProgress[mResearchField] += resources;
				game->GetGalaxy()->TechSpent(resources, tech);
			} else {
				more = true;
				mTechProgress[mResearchField] += cost;
				game->GetGalaxy()->TechSpent(cost, tech);
				resources -= cost;
				if (GainTechLevel(mResearchField)) {
					more = false;
					Result = resources;
				}
			}
		}
	} else
		mTechProgress[tech] += TechGain;

	return Result;
}

void Player::GainSpyTech(long TechGain, TechType tech)
{
	Message * mess = AddMessage("Spy bonus");
	mess->AddItem("Tech field gain", Rules::GetTechName(tech));
	mess->AddLong("Tech reseource gain", TechGain);
	GainTech(TechGain, tech);
	while (mTechProgress[tech] >= TechCost(tech)) {
		GainTechLevel(tech);
	}
}

void Player::CheckTechGain()
{
	for (long tech = 0; tech < Rules::MaxTechType; ++tech) {
		while (mTechProgress[tech] >= TechCost(tech))
			GainTechLevel(tech);
	}
}

void Player::AddProductionTech()
{
	for (long tech = 0; tech < Rules::MaxTechType; ++tech) {
		mTechLevel[tech] += mTempTechLevel[tech];
		mTempTechLevel[tech] = 0;
	}
}

long Player::TechCost(TechType tech) const
{
	assert(tech >= 0 && tech < Rules::MaxTechType);
	if (tech < 0 || tech >= Rules::MaxTechType)
		return 0;

	if (mTechLevel[tech] + mTempTechLevel[tech] == Rules::MaxTechLevel)
		return 0;

	long TotalCost = Rules::TechCost[mTechLevel[tech]+mTempTechLevel[tech]];
	for (long i = 0; i < Rules::MaxTechType; ++i) {
		if (i != tech)
			TotalCost += 10 * (mTechLevel[i] + mTempTechLevel[i]);
	}

	TotalCost = long(TotalCost * mTechCostFactor[tech] + .5);
	TotalCost = long(TotalCost * game->GetTechFactor() + .5);	// 1.0 for normal games, 2.0 for slow tech games

	return TotalCost;
}

Fleet * Player::NCGetFleet(unsigned long n)
{
	if (this == NULL || n < 1 || n > mFleets.size())
		return NULL;

	return mFleets[n-1];
}

Fleet * Player::GetFleetCreate(unsigned long n, const CargoHolder &loc)
{
	if (n < 0 || n > mFleets.size())
		return NULL;

	if (mFleets[n-1] == NULL)
		mFleets[n-1] = new Fleet(n, loc);

	return mFleets[n-1];
}

const Ship * Player::GetShipDesign(unsigned long n) const
{
	if (this == NULL || n <= 0 || n > mShipDesigns.size())
		return NULL;

	return mShipDesigns[n-1];
}

const Ship * Player::GetBaseDesign(unsigned long n) const
{
	if (n < 0 || n >= mBaseDesigns.size())
		return NULL;

	return mBaseDesigns[n];
}

void Player::SetShipDesign(unsigned long n, Ship *ship)
{
	if(n <= 0 || n > mShipDesigns.size())
		return;

    delete mShipDesigns[n-1];
    mShipDesigns[n-1] = ship;
}

void Player::SetBaseDesign(unsigned long n, Ship *ship)
{
	if(n >= mBaseDesigns.size())
		return;

    delete mBaseDesigns[n];
	mBaseDesigns[n] = ship;
}

void Player::DeleteFleet(Fleet * gone)
{
	if (gone != mFleets[gone->GetID()-1]) {
		assert(false);
		return;	// serious error;
	}

	game->RemoveAlsoHere(gone);
	game->StoreMessageLocation(gone);	// make any messages referring to this fleet point to it's last location
	mFleets[gone->GetID()-1] = NULL;
	delete gone;
}

bool Player::TransferFleet(const Fleet * from)
{
	long f = -1;
	int i;
	for (i = 0; i < Rules::MaxFleets; ++i) {
		if (mFleets[i] == NULL) {
			f = i;
			break;
		}
	}

	if (f == -1) {
		Message * mess;
		mess = AddMessage("Transfer to you failed - max fleets reached");
		mess->AddItem("From", from->GetOwner());
		return false;
	}

	int count = 0;
	Ship ship(game);
	Ship * design;
	for (i = 0; i < from->GetStacks(); ++i) {
		ship.CopyDesign(from->GetStack(i)->GetDesign(), true);
		design = &ship;
		if (GetExistingDesign(design) == NULL)
			++count;
	}

	for (i = 0; count > 0 && i < mShipDesigns.size(); ++i) {
		if (mShipDesigns[i] == NULL)
			--count;
	}

	if (count > 0) {
		Message * mess;
		mess = AddMessage("Transfer to you failed - max ship designs reached");
		mess->AddItem("From", from->GetOwner());
		return false;
	}

	Fleet * to = FleetFactory();
	for (i = 0; i < from->GetStacks(); ++i) {
		const Stack * stack = from->GetStack(i);
		design = GetExistingDesign(stack->GetDesign());
		if (design == NULL) {
			int j;
			for (j = 0; j < mShipDesigns.size(); ++j) {
				if (mShipDesigns[j] == NULL) {
					Ship * newShip = NULL;
					newShip = game->ObjectFactory(newShip);
					newShip->CopyDesign(stack->GetDesign(), true);
					mShipDesigns[j] = newShip;
					design = newShip;
				}
			}
			assert(design != NULL);
		}

		to->AddShips(design, stack->GetCount());
		to->GetStack(i)->SetDamaged(stack->GetDamaged());
		to->GetStack(i)->SetDamage(stack->GetDamage());
		design->SetSeenDesign(from->GetOwner()->GetID()-1, true);
	}

	to->AdjustFuel(from->GetFuel());
	for (i = 0; i < Rules::MaxMinType; ++i)
		to->AdjustAmounts(i, from->GetContain(i));

	Message * mess = AddMessage("Fleet transfered to you", to);
	mess->AddItem("From", from->GetOwner());

	return true;
}

const BattlePlan * Player::GetBattlePlan(unsigned long num) const
{
	if (num < 0 || num >= mBattlePlans.size())
		return mBattlePlans[0];
	else
		return mBattlePlans[num];
}

bool Player::ParseNode(const TiXmlNode * node, bool other)
{
	if (stricmp(node->Value(), "Player") != 0)
		return false;

	ArrayParser arrayParser(*GetGame());

	mID = GetLong(node->FirstChild("PlayerNumber"));
	if (mID < 1 || mID > game->NumberPlayers()) {
		Message * mess = game->AddMessage("Error: Invalid player number");
		mess->AddLong("", mID);
		return false;
	}

	// setup relations
	mRelations.erase(mRelations.begin(), mRelations.end());
	mRelations.insert(mRelations.begin(), game->NumberPlayers(), PR_NEUTRAL);
	arrayParser.ParseArray(node->FirstChild("Relations"), "Race", "IDNumber", mRelations);
	if (!ParseCommon(node))
		return false;

	const TiXmlNode * child;
	for (child = node->FirstChild(); child; child = child->NextSibling()) {
		if (child->Type() == TiXmlNode::COMMENT)
			continue;

		if (stricmp(child->Value(), "PlayerNumber") == 0) {
			// skip, already done.
		} else if (!other && stricmp(child->Value(), "Password") == 0) {
		} else if (stricmp(child->Value(), "RaceDefinition") == 0) {
			if (!Race::ParseNode(child, other))
				return false;
		} else if (stricmp(child->Value(), "ResearchTax") == 0) {
		} else if (stricmp(child->Value(), "ResearchField") == 0) {
		} else if (stricmp(child->Value(), "ResearchNext") == 0) {
		} else if (stricmp(child->Value(), "TechLevels") == 0) {
			if (!Rules::ParseArray(child, mTechLevel, TECHS, *GetGame()))
				return false;
		} else if (!other && stricmp(child->Value(), "TechProgress") == 0) {
			if (!Rules::ParseArray(child, mTechProgress, TECHS, *GetGame()))
				return false;
		} else if (!other && stricmp(child->Value(), "BattlePlan") == 0) {
		} else if (!other && stricmp(child->Value(), "ProductionQueue") == 0) {
		} else if (!other && stricmp(child->Value(), "DefaultPayTax") == 0) {
		} else if (!other && stricmp(child->Value(), "Relations") == 0) {
			if (!Rules::ParseArray(child, mTechLevel, TECHS, *GetGame()))
				return false;
		} else if (!other && stricmp(child->Value(), "TechProgress") == 0) {
			if (!Rules::ParseArray(child, mTechProgress, TECHS, *GetGame()))
				return false;
		} else if (stricmp(child->Value(), "PlanetarySpaceScan") == 0) {
			mScanSpace = GetLong(child);
		} else if (stricmp(child->Value(), "PlanetaryPenScan") == 0) {
			mScanPenetrating = GetLong(child);
		} else if (stricmp(child->Value(), "DefenseValue") == 0) {
			mDefenseValue = GetDouble(child);
		} else if (stricmp(child->Value(), "TerraformLimit") == 0) {
			if (!Rules::ParseArray(child, mTerraLimit, HABS, *GetGame()))
				return false;
		} else if (stricmp(child->Value(), "ShipDesign") == 0) {
			const TiXmlElement * tie = child->ToElement();
			int num;
			tie->Attribute("IDNumber", &num);
			if (num <= 0 || num > Rules::GetConstant("MaxShipDesigns")) {
				Message * mess = game->AddMessage("Error: Invalid ship slot number");
				mess->AddLong("", num);
			} else {
				num--;
				if (mShipDesigns[num] == NULL)
					mShipDesigns[num] = game->ObjectFactory(mShipDesigns[num]);
				if (!mShipDesigns[num]->ParseNode(child, this, other)) {
					delete mShipDesigns[num];
					mShipDesigns[num] = NULL;
				}
			}
		} else if (stricmp(child->Value(), "BaseDesign") == 0) {
			const TiXmlElement * tie = child->ToElement();
			int num;
			tie->Attribute("IDNumber", &num);
			if (num <= 0 || num > Rules::GetConstant("MaxBaseDesigns")) {
				Message * mess = game->AddMessage("Error: Invalid base slot number");
				mess->AddLong("", num);
			} else {
				num--;
				if (mBaseDesigns[num] == NULL)
					mBaseDesigns[num] = game->ObjectFactory(mBaseDesigns[num]);
				if (!mBaseDesigns[num]->ParseNode(child, this, other)) {
					delete mBaseDesigns[num];
					mBaseDesigns[num] = NULL;
				}
			}
		} else if (stricmp(child->Value(), "Minefield") == 0) {
//			if (!(*mMineFields.insert(mMineFields.begin()))->ParseNode(child, this))
//				return false;
//			game->AddAlsoHere();
		} else if (stricmp(child->Value(), "Fleet") == 0) {
			ParseFleet(child, other);
		} else {
			Message * mess = game->AddMessage("Warning: Unknown section");
			mess->AddItem("Section name", child->Value());
			mess->AddItem("Player", this);
		}
	}

	return true;
}

bool Player::ParseCommon(const TiXmlNode * node)
{
	ArrayParser arrayParser(*GetGame());

	// setup relations
	mRelations.erase(mRelations.begin(), mRelations.end());
	mRelations.insert(mRelations.begin(), game->NumberPlayers(), PR_NEUTRAL);
	arrayParser.ParseArray(node->FirstChild("Relations"), "Race", "IDNumber", mRelations);

	const TiXmlNode * child;
	for (child = node->FirstChild(); child; child = child->NextSibling()) {
		if (child->Type() == TiXmlNode::COMMENT)
			continue;

		if (stricmp(child->Value(), "Password") == 0) {
			// skipping passwords for now
		} else if (stricmp(child->Value(), "ResearchTax") == 0) {
			ParseResearchTax(child);
		} else if (stricmp(child->Value(), "ResearchField") == 0) {
			SetResearchField(ParseResearchField(child));
		} else if (stricmp(child->Value(), "ResearchNext") == 0) {
			SetResearchNext(ParseResearchField(child));
		} else if (stricmp(child->Value(), "BattlePlan") == 0) {
			int num;
			child->ToElement()->Attribute("IDNumber", &num);
			if (num < 0 || num >= Rules::GetConstant("MaxBattlePlans")) {
				Message * mess = game->AddMessage("Error: Invalid battle plan number");
				mess->AddLong("", num);
			} else {
				if (mBattlePlans[num] == NULL)
					mBattlePlans[num] = BattlePlanFactory();
				else
					delete mBattlePlans[num];	// this shouldn't happen

				mBattlePlans[num]->ParseNode(child, this);
			}
		} else if (stricmp(child->Value(), "ProductionQueue") == 0) {
			SetProduction(ProdOrder::ParseNode(child, NULL, this));
		} else if (stricmp(child->Value(), "DefaultPayTax") == 0) {
			mDefaultPayTax = GetBool(child);
		}
	}

	return true;
}

void Player::ParseResearchTax(const TiXmlNode * node)
{
	double tax = mResearchTax;

	tax = GetDouble(node);
	if (tax < 0.0) {
		Message * mess = game->AddMessage("Error: Invalid tech tax");
		mess->AddFloat("tax", tax);
		tax = 0.0;
	}
	if (tax > 1.0) {
		Message * mess = game->AddMessage("Error: Invalid tech tax");
		mess->AddFloat("tax", tax);
		tax = 1.0;
	}
	SetResearchTax(tax);
}

long Player::ParseResearchField(const TiXmlNode * node)
{
	long field = Rules::TechID(GetString(node));
	if (field == TECH_NONE) {
		Message * mess = game->AddMessage("Error: Invalid tech type");
		mess->AddItem("Research field", GetString(node));
	}

	return field;
}

void Player::ParseMessages(const TiXmlNode * node)
{
	if (node == NULL)
		return;

	const TiXmlNode * child;
	Message * mess;
	for (child = node->FirstChild("Message"); child != NULL; child = child->NextSibling("Message")) {
		mess = new Message();
		if (mess->ParseNode(child, game))
			mMessages.push_back(mess);
		else
			delete mess;
	}
}

bool Player::WriteNode(TiXmlNode * node, const Player * viewer) const
{
	bool Result = false;

	AddLong(node, "PlayerNumber", GetID());
	if (viewer == NULL || viewer == this) {
		Result = true;
		TiXmlElement race("RaceDefinition");
		Race::WriteNode(&race);
		node->InsertEndChild(race);

//		AddString(player, "Password", );
		AddDouble(node, "ResearchTax", mResearchTax);
		AddString(node, "ResearchField", Rules::GetTechName(mResearchField).c_str());
		AddString(node, "ResearchNext", Rules::GetTechName(mResearchNext).c_str());

		node->LinkEndChild(Rules::WriteArray("TechLevels", mTechLevel, TECHS));
		node->LinkEndChild(Rules::WriteArray("TechProgress", mTechProgress, TECHS));
		AddLong(node, "PlanetarySpaceScan", mScanSpace);
		AddLong(node, "PlanetaryPenScan", mScanPenetrating);
		AddDouble(node, "DefenseValue", mDefenseValue);
		node->LinkEndChild(Rules::WriteArray("Relations", "Race", "IDNumber", mRelations));
		node->LinkEndChild(Rules::WriteArray("TerraformLimit", mTerraLimit, HABS));

		int i;
		for (i = 0; i < mBattlePlans.size(); ++i) {
			if (mBattlePlans[i] && mBattlePlans[i]->IsDefined())
				WriteBattlePlan(node, i);
		}

		// Default Production Queue and if new planets should pay tax or not
		ProdOrder::WriteNode(node, mDefaultQ);
		AddBool(node, "DefaultPayTax", mDefaultPayTax);
	}

	deque<Ship *>::const_iterator sdi;
	for (sdi = mShipDesigns.begin(); sdi != mShipDesigns.end(); ++sdi) {
		if (*sdi == NULL)
			continue;
		if (viewer == NULL || viewer == this || (*sdi)->SeenDesign(viewer->GetID()-1) || (*sdi)->SeenHull(viewer->GetID()-1)) {
			TiXmlElement SD("ShipDesign");
			SD.SetAttribute("IDNumber", sdi - mShipDesigns.begin() + 1);
			(*sdi)->WriteNode(&SD, viewer == NULL, viewer == this, viewer == NULL || (*sdi)->SeenDesign(viewer->GetID()-1));
			node->InsertEndChild(SD);
			Result = true;
		}
	}
	for (sdi = mBaseDesigns.begin(); sdi != mBaseDesigns.end(); ++sdi) {
		if (*sdi == NULL)
			continue;
		if (viewer == NULL || viewer == this || (*sdi)->SeenDesign(viewer->GetID()-1) || (*sdi)->SeenHull(viewer->GetID()-1)) {
			TiXmlElement SD("BaseDesign");
			SD.SetAttribute("IDNumber", sdi - mBaseDesigns.begin() + 1);
			(*sdi)->WriteNode(&SD, viewer == NULL, viewer == this, viewer == NULL || (*sdi)->SeenDesign(viewer->GetID()-1));
			node->InsertEndChild(SD);
			Result = true;
		}
	}

	deque<Fleet *>::const_iterator fi;
	for (fi = mFleets.begin(); fi != mFleets.end(); ++fi) {
		if (*fi != NULL && (viewer == NULL || viewer == this || (*fi)->SeenBy(viewer))) {
			TiXmlElement fleet("Fleet");
			(*fi)->WriteNode(&fleet, viewer);
			node->InsertEndChild(fleet);
			Result = true;
		}
	}

	if (viewer != NULL && viewer != this) {
		TiXmlElement race("RaceDefinition");
		if (mSeenHab[viewer->GetID()-1]) {
			Race::WriteHabs(&race);
			Result = true;
		}

		if (Result) {
			AddString(&race, "SingularName", mSingularName.c_str());
			AddString(&race, "PluralName", mPluralName.c_str());
			AddLong(&race, "RaceEmblem", mRaceEmblem);
			node->InsertEndChild(race);
		}
	}

	return Result;
}

void Player::ParseFleet(const TiXmlNode * node, bool other)
{
	const TiXmlElement * tie = node->ToElement();
	int num;
	tie->Attribute("IDNumber", &num);

	if (num < 1 || num >= Rules::MaxFleets) {
		Message * mess = game->AddMessage("Error: Invalid fleet number");
		mess->AddLong("", num);
	} else {
		--num;
		if (mFleets[num] == NULL)
			mFleets[num] = FleetFactory();
		if (!mFleets[num]->ParseNode(node, this, other)) {
			delete mFleets[num];
			mFleets[num] = NULL;
		} else
			game->AddAlsoHere(mFleets[num]);
	}
}

void Player::ParseOrders(const TiXmlNode * orders)
{
	const TiXmlNode * node;
	const TiXmlNode * child;
	const char * ptr;
	ArrayParser arrayParser(*GetGame());

	Planet * pfrom;
	Planet * pto;
	Fleet * ffrom;
	Fleet * fto;

	// process all orders
	for (node = orders->FirstChild(); node; node = node->NextSibling()) {
		if (node->Type() == TiXmlNode::COMMENT)
			continue;

		// already checked, skip it here
		if (stricmp(node->Value(), "PlayerNo") == 0) {
		} else if (stricmp(node->Value(), "Turn") == 0) {
		} else if (stricmp(node->Value(), "GameID") == 0) {
		} else if (stricmp(node->Value(), "MetaInfo") == 0) {

		} else if (stricmp(node->Value(), "StartMultipleOrder") == 0) {
			StartMultipleOrder();
		} else if (stricmp(node->Value(), "EndMultipleOrder") == 0) {
			EndMultipleOrder();
		} else if (stricmp(node->Value(), "ProductionQueue") == 0) {
			ptr = GetString(node->FirstChild("Planet"));
			if (stricmp(ptr, "Default") == 0) {
				SetProduction(ProdOrder::ParseNode(node, NULL, this));
			} else {
				pto = game->GetGalaxy()->GetPlanet(ptr);
				if (pto != NULL && pto->GetOwner() == this)
					pto->ParseProduction(node);
				else {
					Message * mess = AddMessage("Error: Invalid planet");
					mess->AddItem("", ptr);
					mess->AddItem("Where", "production queue");
					continue;
				}
			}
		} else if (stricmp(node->Value(), "SetPayTax") == 0) {
			ptr = GetString(node->FirstChild("Planet"));
			pto = game->GetGalaxy()->GetPlanet(ptr);
			if (pto != NULL && pto->GetOwner() == this)
				pto->SetPayTax(GetBool(node->FirstChild("PayTax")));
			else {
				Message * mess = AddMessage("Error: Invalid planet");
				mess->AddItem("", ptr);
				mess->AddItem("Where", "PayTax");
				continue;
			}
		} else if (stricmp(node->Value(), "DefaultPayTax") == 0) {
			SetPayTax(GetBool(node));
		} else if (stricmp(node->Value(), "ChangePassword") == 0) {
			//@todo passwords to be done later
		} else if (stricmp(node->Value(), "ResearchTax") == 0) {
			ParseResearchTax(node);
		} else if (stricmp(node->Value(), "ResearchField") == 0) {
			SetResearchField(ParseResearchField(node));
		} else if (stricmp(node->Value(), "ResearchNext") == 0) {
			SetResearchNext(ParseResearchField(node));
		} else if (stricmp(node->Value(), "SetPacketSpeed") == 0) {
			pfrom = game->GetGalaxy()->GetPlanet(GetString(node->FirstChild("Planet")));
			if (!pfrom || pfrom->GetOwner() != this) {
				Message * mess = AddMessage("Error: Invalid planet");
				mess->AddItem("", GetString(node->FirstChild("Planet")));
				mess->AddItem("Where", "SetPacketSpeed");
				continue;
			}

			pfrom->SetPacketSpeed(GetLong(node->FirstChild("PacketSpeed")));
		} else if (stricmp(node->Value(), "SetPacketDestination") == 0) {
			pfrom = game->GetGalaxy()->GetPlanet(GetString(node->FirstChild("Planet")));
			if (!pfrom || pfrom->GetOwner() != this) {
				Message * mess = AddMessage("Error: Invalid planet");
				mess->AddItem("", GetString(node->FirstChild("Planet")));
				mess->AddItem("Where", "SetPacketDestination");
				continue;
			}

			pto = game->GetGalaxy()->GetPlanet(GetString(node->FirstChild("PacketDestination")));
			if (!pto) {
				Message * mess = AddMessage("Error: Invalid planet");
				mess->AddItem("", GetString(node->FirstChild("PacketDestination")));
				mess->AddItem("Where", "PacketDestination");
				continue;
			}
			pfrom->SetPacketDest(pto);
		} else if (stricmp(node->Value(), "SetRouteDestination") == 0) {
			pfrom = game->GetGalaxy()->GetPlanet(GetString(node->FirstChild("From")));
			if (!pfrom || pfrom->GetOwner() != this) {
				Message * mess = AddMessage("Error: Invalid planet");
				mess->AddItem("", GetString(node->FirstChild("From")));
				mess->AddItem("Where", "route from");
				continue;
			}

			pto = game->GetGalaxy()->GetPlanet(GetString(node->FirstChild("RouteDestination")));
			if (!pto) {
				Message * mess = AddMessage("Error: Invalid planet");
				mess->AddItem("", GetString(node->FirstChild("RouteDestination")));
				mess->AddItem("Where", "route to");
				continue;
			}
			pfrom->SetRoute(pto);
		} else if (stricmp(node->Value(), "SplitMerge") == 0) {
			///@todo split/merge fleets undo/redo and following
			child = node->FirstChild("FromFleet");
			ffrom = NCGetFleet(GetLong(child));
			if (!ffrom) {
				Message * mess = AddMessage("Error: Invalid fleet number in split/merge");
				mess->AddLong("", GetLong(child));
				continue;
			}
			child = node->FirstChild("ToFleet");
			fto = GetFleetCreate(GetLong(child), *ffrom);
			if (!fto) {
				AddMessage("Error: Cannot create fleet for split");
				continue;
			}

			if (!ffrom->IsWith(*fto)) {
				Message * mess = AddMessage("Error: merge at seperate locations", ffrom);
				mess->AddItem("Merge to", fto);
				continue;
			}

			child = node->FirstChild("Ship");
			if (child == NULL)
				ffrom->MergeTo(fto);

			for (; child; child = child->NextSibling("Ship"))
			{
				const Ship * design = GetShipDesign(GetLong(child->FirstChild("Design")));
				if (design == NULL)
				{
					Message * mess = AddMessage("Error: Invalid ship number");
					mess->AddLong("", GetLong(child->FirstChild("Design")));
					continue;
				}
				int number = GetLong(child->FirstChild("Number"));
				int damaged = GetLong(child->FirstChild("Damaged"));
				ffrom->MergeTo(fto, design, number, damaged);
			}

			if (ffrom->IsEmpty()) {
				DeleteFleet(ffrom);
			}

			if (fto->IsEmpty()) {
				DeleteFleet(fto);
			}
		} else if (stricmp(node->Value(), "Transfer") == 0) {
			child = node->FirstChild("Owned");
			CargoHolder * owned = ParseTransport(child, NULL);
			if (!owned) {
				AddMessage("Error: Missing source in transfer order");
				continue;
			}

			child = node->FirstChild("Other");
			CargoHolder * other =  ParseTransport(child, owned);
			if (!other || !owned->IsWith(*other)) {
				// same message so you can't learn about others fleets by sending spurious transfer orders
				Message * mess = AddMessage("Error: Missing destination in transfer order", owned);
				if (other && other->GetOwner() == this)	// if you own it in a seperate location send more information
					mess->AddItem("Transfer destination", other);
				continue;
			}

			child = node->FirstChild("Cargo");
			if (child == NULL) {
				AddMessage("Error: Invalid cargo type");
				continue;
			}

			deque<long> cargo;
			cargo.insert(cargo.begin(), Rules::MaxMinType, 0);
			long pop;
			long fuel;
			Rules::ReadCargo(child, cargo, &pop, *GetGame());
			fuel = GetLong(child->FirstChild("Fuel"));
			TransferCargo(owned, other, pop, fuel, cargo);
		} else if (stricmp(node->Value(), "BattlePlan") == 0) {
			int num;
			node->ToElement()->Attribute("IDNumber", &num);
			if (num < 0 || num >= Rules::GetConstant("MaxBattlePlans")) {
				Message * mess = AddMessage("Error: Invalid battle plan number");
				mess->AddLong("", num);
			} else {
				BattlePlan * bp = BattlePlanFactory();
				bp->ParseNode(node, this);
				ChangeBattlePlan(bp, num);
			}
		} else if (stricmp(node->Value(), "Relations") == 0) {
			// setup relations
			deque<long> rel;
			rel.insert(rel.begin(), game->NumberPlayers(), PR_NEUTRAL);
			arrayParser.ParseArray(node, "Race", "IDNumber", rel);
			SetRelations(rel);
		} else if (stricmp(node->Value(), "SetRepeat") == 0) {
			ffrom = NCGetFleet(GetLong(node->FirstChild("Fleet")));
			if (ffrom)
				ffrom->SetRepeat(GetBool(node->FirstChild("Repeat")));
		} else if (stricmp(node->Value(), "Waypoints") == 0) {
			// Already done for host processing, so skip it here.
			if (mWriteXFile) {
				WayOrderList wol;
				wol.SetFleet(GetLong(node->FirstChild("Fleet")));
				wol.ParseNode(node, this, game);

				Fleet * f = NCGetFleet(GetLong(node->FirstChild("Fleet")));
				if (f == NULL)
					continue;

				f->ChangeWaypoints(wol);
			}
		} else if (stricmp(node->Value(), "SetDetonateMineField") == 0) {
			long ID;
			ID = GetLong(node->FirstChild("MineField"));
			bool det = GetBool(node->FirstChild("DetonateMineField"));
			MineField * mf = GetMineField(ID);
			if (mf == NULL || mf->GetOwner() != this) {
				Message * mess = AddMessage("Error: Invalid mine field ID");
				mess->AddLong("ID", ID);
				continue;
			}

			if (!mf->GetType()->CanDetonate()) {
				Message * mess = AddMessage("Error: Attempt to detonate a mine type that cannot be detonated");
				mess->AddLong("ID", ID);
				mess->AddItem("Mine type", mf->GetType()->GetName());
				continue;
			}

			mf->SetDetonate(det);
		} else {
			Message * mess = AddMessage("Error: Invalid .x file command");
			mess->AddItem("", node->Value());
		}
	}
}

CargoHolder * Player::ParseTransport(const TiXmlNode * node, const CargoHolder * owned)
{
	CargoHolder * ch;
	const TiXmlNode * child;

	child = node->FirstChild("Planet");
	if (child != NULL) {
		ch = game->GetGalaxy()->GetPlanet(GetString(child));
		if (owned == NULL) {
			if (this == ch->GetOwner())
				return ch;
		} else if (this == ch->GetOwner())
			return ch;
		else
			return game->GetGalaxy()->GetJettison(owned, ch);

		Message * mess = AddMessage("Error: Not owner", ch);
		mess->AddItem("Where", "cargo transfer");
		return NULL;
	}

	child = node->FirstChild("Fleet");
	if (child != NULL) {
		long f = GetLong(child);
		if (owned != NULL) {
			Player * p = game->NCGetPlayer(GetLong(node->FirstChild("Owner")));
			if (!p) {
				Message * mess = AddMessage("Error: Invalid player number");
				mess->AddLong("", GetLong(child));
				mess->AddItem("Where", "Cargo Transfer");
				return NULL;
			}

			ch = game->GetGalaxy()->GetJettison(owned, p->NCGetFleet(f));
		} else
			ch = NCGetFleet(f);

		return ch;
	}

	if (owned != NULL) {
		child = node->FirstChild("Space");
		if (child != NULL) {
			return game->GetGalaxy()->GetJettison(owned, NULL);
		}

		child = node->FirstChild("Packet");
		if (child != NULL) {
			return NULL;
		}
	}

	return NULL;
}

void Player::TransferCargo(CargoHolder * owned, CargoHolder * other, long pop, long fuel, deque<long> & cargo, bool write/*= true*/)
{
	if (pop < 0) {
		pop = -pop;
		other->TransferCargo(owned, POPULATION, &pop, this);
		pop = -pop;
	} else
		owned->TransferCargo(other, POPULATION, &pop, this);

	if (fuel < 0) {
		fuel = -fuel;
		other->TransferCargo(owned, FUEL, &fuel, this);
		fuel = -fuel;
	} else
		owned->TransferCargo(other, FUEL, &fuel, this);

	for (int i = 0; i < Rules::MaxMinType; ++i) {
		if (cargo[i] < 0) {
			cargo[i] = -cargo[i];
			other->TransferCargo(owned, i, &cargo[i], this);
			cargo[i] = -cargo[i];
		} else
			owned->TransferCargo(other, i, &cargo[i], this);
	}

	if (write && mWriteXFile)
		AddOrder(new TransportOrder(this, owned, other, pop, fuel, cargo));
}

long Player::ForEachFleet(Fleet::FuncType func, bool arg)
{
	long Result = 0;
	for (long i = 0; i < Rules::MaxFleets; ++i) {
		if (mFleets[i] != NULL)
			Result += mFleets[i]->Process(func, arg) ? 1 : 0;
	}

	return Result;
}

void Player::BuildShips(Planet * planet, long Type, long number)
{
	long f = -1;
	bool newfleet = false;
	for (long i = 0; i < Rules::MaxFleets; ++i) {
		if (mFleets[i] == NULL) {
			f = i;
			break;
		} else if (f == -1 && mFleets[i]->InOrbit() == planet)
			f = i;
	}

	Message * mess;
	if (f < 0) {
		mess = AddMessage("Production lost due to too many fleets", planet);
	} else {
		if (mFleets[f] == NULL) {
			mFleets[f] = new Fleet(f+1, *planet);
			mess = AddMessage("New ships built into new fleet", mFleets[f]);
			newfleet = true;
		} else {
			mess = AddMessage("New ships built into existing fleet", mFleets[f]);
		}

		mFleets[f]->AddShips(Type, number);

		// Ships are always built with full fuel tanks
		mFleets[f]->AdjustFuel(mShipDesigns[Type-1]->GetFuelCapacity() * number);
	}

	if (newfleet) {
		mFleets[f]->SetStartOrders(planet);
		game->AddAlsoHere(mFleets[f]);
	}

	mess->AddItem("Ship name", GetShipDesign(Type)->GetName());
	mess->AddLong("Number built", number);

	mShipDesigns[Type-1]->IncrementBuilt(number);
}

void Player::ResetTerraLimits()
{
	mTerraLimit.clear();
	mTerraLimit.insert(mTerraLimit.begin(), Rules::MaxHabType, 0);
}

void Player::SetTerraLimit(HabType ht, long limit)
{
	if (ht == -1) {
		for (ht = 0; ht < Rules::MaxHabType; ++ht)
			mTerraLimit[ht] = max(limit, mTerraLimit[ht]);
	} else
		mTerraLimit[ht] = max(limit, mTerraLimit[ht]);
}

// clear Seen flags
void Player::ResetSeen()
{
	unsigned long i;
	for (i = 0; i < mShipDesigns.size(); ++i) {
		if (mShipDesigns[i] != NULL)
			mShipDesigns[i]->ResetSeen();
	}

	for (i = 0; i < mBaseDesigns.size(); ++i) {
		if (mBaseDesigns[i] != NULL)
			mBaseDesigns[i]->ResetSeen();
	}

	mSeenHab.empty();
	mSeenHab.insert(mSeenHab.begin(), game->NumberPlayers(), false);
}

void Player::DoBattles()
{
	// there is probably a better way to find who is fighting where, but
	// battle order needs to be consistant, so it has to go by player # and then fleet #
	unsigned int i;
	for (i = 0; i < Rules::MaxFleets; ++i) {
		if (mFleets[i] != NULL && !mFleets[i]->AlreadyFought()) {
			Battle bat(game, *mFleets[i]);
//			bat.AddFleet(mFleets[i]); Do not do this, it makes the next function have to check to see if the fleet has already been added
			Planet * planet;
			planet = game->GetGalaxy()->GetPlanet(mFleets[i]);
			bat.SetPlanet(planet);
			bat.AddFleets();
			bat.Resolve();
		}
	}
}

void Player::AddBattleFleets(Battle * bat)
{
	unsigned int i;
	for (i = 0; i < Rules::MaxFleets; ++i) {
		if (mFleets[i] != NULL && !mFleets[i]->AlreadyFought() && mFleets[i]->IsWith(*bat))
			bat->AddFleet(mFleets[i]);
	}
}

bool Player::AddBombingFleets(Bombing * bom, const Player* owner, const long bomb_type) const
{
	bool added = false;
	unsigned int i;
	for (i = 0; i < Rules::MaxFleets; ++i)
	{
		//See if Fleet has orders to bomb
		//mFleets[i]->GetFirstOrder()
		if (mFleets[i]->IsWith(*bom) &&	mFleets[i]->GetBattlePlan()->WillFight(this,owner))
		{
			switch(bomb_type)
			{
				case BT_NORMAL:
					mFleets[i] != NULL && mFleets[i]->CanNormalBomb();
					bom->AddNormalFleet(mFleets[i]);
					added = true;
				break;
				case BT_SMART:
					mFleets[i] != NULL && mFleets[i]->CanSmartBomb();
					bom->AddSmartFleet(mFleets[i]);
					added = true;
				break;
				case BT_TERRA:
					mFleets[i] != NULL && mFleets[i]->CanTerraBomb();
					bom->AddTerraFleet(mFleets[i]);
					added = true;
				break;

			}
		}
	}
	return added;
}

void Player::ClearBattleEnemies()
{
	mBattleEnemy.clear();
	mBattleEnemy.insert(mBattleEnemy.begin(), game->NumberPlayers(), false);
	mInBattle = false;
}

void Player::SetSeenDesign(long p, long design, bool base)
{
	if (base)
		mBaseDesigns[design]->SetSeenDesign(p, true);
	else
		mShipDesigns[design]->SetSeenDesign(p, true);
}

void Player::SetSeenHull(long p, long design, bool base)
{
	if (base)
		mBaseDesigns[design]->SetSeenHull(p, true);
	else
		mShipDesigns[design]->SetSeenHull(p, true);
}

long Player::GetMineFieldID() const
{
	long Result;
	bool retry;
	long trys = 0;

	do {
		Result = genrand_int32();
		retry = Result == 0;
		deque<MineField *>::const_iterator mfi;
		for (mfi = mMineFields.begin(); !retry && mfi != mMineFields.end(); ++mfi) {
			if (Result == (*mfi)->GetID()) {
				retry = true;
				break;
			}
		}
	} while (retry && trys++ < 10);

	assert(!retry);
	return Result;
}

MineField * Player::GetMineField(long ID) const
{
	deque<MineField *>::const_iterator mfi;
	for (mfi = mMineFields.begin(); mfi != mMineFields.end(); ++mfi) {
		if (ID == (*mfi)->GetID())
			return *mfi;
	}

	return NULL;
}

void Player::DeleteMineField(MineField * mf)
{
	deque<MineField *>::iterator mfi;
	for (mfi = mMineFields.begin(); mfi != mMineFields.end(); ++mfi) {
		if (mf == *mfi) {
			mMineFields.erase(mfi);
			delete mf;
			break;
		}
	}

	assert(mfi != mMineFields.end());
}

long Player::CreateFromFile(const char * file)
{
	TiXmlDocument doc(file);
	doc.SetCondenseWhiteSpace(false);
	if (!doc.LoadFile()) {
		Message * mess = game->AddMessage("Error: Cannot open file");
		mess->AddItem("", file);
		return -1;
	}

	const TiXmlNode * node;

	node = doc.FirstChild("RaceDefinition");
	if (!node) {
		Message * mess = game->AddMessage("Error: Missing section");
		mess->AddItem("File name", file);
		mess->AddItem("Section", "RaceDefinition");
		return -1;
	}

	if (!game->CheckMetaInfo(node, file, RACEFILEVERSION))
		return -1;

//	child = node->FirstChild("Rules");
//	if (!child) {
//		Message * mess = game->AddMessage("Error: Missing section");
//		mess->AddItem("File name", file);
//		mess->AddItem("Section", "Rules");
//		return false;
//	}

//	if (!game->LoadRules(GetString(child->FirstChild("File")),
//							GetString(child->FirstChild("Verification")),
//							GetDouble(child->FirstChild("Version")), true))
//	{
//		return false;
//	}

	if (!Race::ParseNode(node, false))
		return -1;

	if (!ParseCommon(node->FirstChild("InitialSettings")))
		return -1;

	if (GetBattlePlan(0) == NULL) {
		Message * mess = game->AddMessage("Error: No default battle plan");
		mess->AddItem("File name", file);
		return -1;
	}
	// copy password

	mResearchTax = 0.0;
	if (ARTechType() > 0)
		mResearchField = ARTechType();
	else
		mResearchField = 0;

	mResearchNext = NEXT_CHEAP;
	int i;

	for (i = 0; i < Rules::MaxTechType; ++i) {
		mTechProgress[i] = 0;
		mTechLevel[i] = PRTStartTech(i);
		// if the start at 3(4) box is checked, tech is expensive, and it's currently less then the start level
		if (StartAt() && (TechCostFactor(i) > 1.75 - epsilon) && mTechLevel[i] < StartAtBonus())
			mTechLevel[i] = StartAtBonus();

		// now adjust by LRTs
		mTechLevel[i] += LRTStartTech(i);
	}

	AddMessage("Game starting");

	// tech and LRT dependant
	const Component * comp;
	comp = game->GetBestComp(this, "PlanetScanSpace", false, HC_ALL);
	mScanSpace = comp == NULL ? 0 : comp->GetScanSpace();
	comp = game->GetBestComp(this, "PlanetScanPen", false, HC_ALL);
	mScanPenetrating = comp == NULL ? 0 : comp->GetScanPenetrating();
	comp = game->GetBestComp(this, "DefensePower", false, HC_ALL);
	mDefenseValue = comp == NULL ? 0 : comp->GetDefensePower();
	for (i = 0; i < Rules::MaxHabType; ++i)
		mTerraLimit[i] = game->GetTerraLimit(this, i);

	return GetLong(node->FirstChild("Randomize"), 1200) % 10000;
}

void Player::SetupRelations()
{
	mRelations.erase(mRelations.begin(), mRelations.end());
	mRelations.insert(mRelations.begin(), game->NumberPlayers(), PR_NEUTRAL);
}

void Player::AddStartShips(Planet * planet, bool HomeWorld)
{
	const RacialTrait * rt;
	rt = GetPRT();
	int i;

	for (i = 0; i < rt->StartShipCount(); ++i) {
		AddStartShips(rt, i, planet, HomeWorld);
	}

	for (int j = 0; j < GetLRTCount(); ++j) {
		rt = GetLRT(j);
		for (i = 0; i < rt->StartShipCount(); ++i) {
			AddStartShips(rt, i, planet, HomeWorld);
		}
	}
}

void Player::AddStartShips(const RacialTrait * rt, int i, Planet * planet, bool HomeWorld)
{
	if (rt->StartShipCount() <= i)
		return;

	const Ship * basicDesign = rt->GetStartDesign(i);
	if ((basicDesign->GetCannotBuild() == NULL || !basicDesign->GetCannotBuild()->IsBuildable(this)) && basicDesign->IsValidDesign(this)) {
		Ship * upgrade = new Ship(game);
		upgrade->CopyDesign(rt->GetStartDesign(i), false);
		if (!(upgrade->GetHull()->GetHullType() & HC_BASE))
			upgrade->Upgrade(this);

		long designNumber;
		const Ship * design;
		if (upgrade->GetHull()->GetHullType() & HC_BASE) {
			design = GetExistingBaseDesign(upgrade);
			if (design == NULL) {
				for (designNumber = 0; designNumber < mBaseDesigns.size(); ++designNumber) {
					if (mBaseDesigns[designNumber] == NULL) {
						mBaseDesigns[designNumber] = upgrade;
						break;
					}
				}
				design = upgrade;
			} else {
				designNumber = GetBaseDesignNumber(design);
				delete upgrade;
			}
		} else {
			design = GetExistingDesign(upgrade);
			if (design == NULL) {
				for (designNumber = 0; designNumber < mBaseDesigns.size(); ++designNumber) {
					if (mShipDesigns[designNumber] == NULL) {
						mShipDesigns[designNumber] = upgrade;
						break;
					}
				}
				design = upgrade;
			} else {
				designNumber = GetShipNumber(design);
				delete upgrade;
			}
		}

		if (design->GetHull()->GetHullType() & HC_BASE) {
			if (HomeWorld && rt->GetStartCount(i) > 0 && planet->GetBaseNumber() == -2)
				planet->SetBaseNumber(designNumber);

			if (!HomeWorld && rt->GetStart2ndCount(i) > 0)
				planet->SetBaseNumber(designNumber);
		} else {
			if (HomeWorld && rt->GetStartCount(i) > 0)
				BuildShips(planet, designNumber+1, rt->GetStartCount(i));

			if (!HomeWorld && rt->GetStart2ndCount(i) > 0)
				BuildShips(planet, designNumber+1, rt->GetStart2ndCount(i));
		}
	}
}

Ship * Player::GetExistingDesign(const Ship * check) const
{
	deque<Ship *>::const_iterator s;
	for (s = mShipDesigns.begin(); s != mShipDesigns.end(); ++s) {
		if (*s != NULL && *check == **s)
			return *s;
	}

	return NULL;
}

Ship * Player::GetExistingBaseDesign(const Ship * check) const
{
	deque<Ship *>::const_iterator s;
	for (s = mBaseDesigns.begin(); s != mBaseDesigns.end(); ++s) {
		if (*s != NULL && *check == **s)
			return *s;
	}

	return NULL;
}

long Player::GetFreeShipDesignSlot() const {
    deque<Ship*>::const_iterator i = find(mShipDesigns.begin(), mShipDesigns.end(), (Ship*)NULL);
    return i != mShipDesigns.end() ? (i - mShipDesigns.begin()) + 1 : -1;
}

long Player::GetFreeBaseDesignSlot() const {
    deque<Ship*>::const_iterator i = find(mBaseDesigns.begin(), mBaseDesigns.end(), (Ship*)NULL);
    return i != mBaseDesigns.end() ? i - mBaseDesigns.begin() : -1;
}

void Player::PlaceHW(Planet * planet)
{
	if (planet == NULL) {
		// error
		return;
	}

	if (planet->GetOwner() == NULL) {
		planet->CreateHW(this, game->GetCreation());
	} else if (planet->GetOwner() != this) {
		// error
		return;
	}

	mHasHW = true;
	// adjust based on player settings
	planet->AdjustHW(this, game->GetCreation());
	AddMessage("Homeworld is", planet);
	AddStartShips(planet, true);
}

void Player::PlaceSW(Planet * second, Planet * homeworld)
{
	if (second == NULL || homeworld == NULL) {
		// error
		return;
	}

	if (second->GetOwner() == NULL) {
		second->CreateSecondWorld(homeworld, game->GetCreation());
	} else if (second->GetOwner() != this) {
		// error
		return;
	}

	// adjust based on player settings
	second->AdjustSecondWorld(this);
	AddStartShips(second, false);
}

void Player::LoadFleets()
{
	int i;
	for (i = 0; i < mFleets.size(); ++i) {
		if (mFleets[i] != NULL)
			game->AddAlsoHere(mFleets[i]);
	}
}

void Player::LoadMinefields()
{
	int i;
	for (i = 0; i < mMineFields.size(); ++i)
		game->AddAlsoHere(mMineFields[i]);
}

bool Player::OpenOrdersFile()
{
	string File;
	File = game->GetFileLoc();
	File += game->GetFileName();
	File += ".x" + Long2String(GetID() + 1);

	return OpenOrdersFile(File.c_str());
}

bool Player::SaveXFile()
{
	TiXmlDocument doc;

	doc.SetCondenseWhiteSpace(false);

	TiXmlDeclaration decl("1.0", "", "yes");
	doc.InsertEndChild(decl);

	TiXmlElement node("OrdersFile");

	TiXmlElement MetaInfo("MetaInfo");
	AddDouble(&MetaInfo, "FreeStarsVersion", FREESTARSVERSION);
	AddDouble(&MetaInfo, "FileVersion", ORDERSFILEVERSION);
	node.InsertEndChild(MetaInfo);

	AddLong(&node, "GameID", game->GetGameID());
	AddLong(&node, "Turn", game->GetTurn());
	AddLong(&node, "PlayerNo", GetID());
	for (int i = 0; i < mOrders.size(); ++i) {
		if(!mOrders[i]->IsReplaced()) {
			mOrders[i]->WriteNode(&node);
		}
	}

	doc.InsertEndChild(node);

	char buf[1024];
#ifdef _DEBUG
	sprintf(buf, "%snew/%s.x%ld", game->GetFileLoc().c_str(), game->GetFileName().c_str(), GetID());
#else
	sprintf(buf, "%s%s.x%ld", game->GetFileLoc().c_str(), game->GetFileName().c_str(), GetID());
#endif // _DEBUG
	bool saved = doc.SaveFile(buf);
	if (saved)
		mUnsavedChanges = 0;

	return saved;
}

bool Player::OpenOrdersFile(const char * file)
{
	TiXmlDocument doc(file);
	doc.SetCondenseWhiteSpace(false);
	if (!doc.LoadFile())
		return false;

	const TiXmlNode * orders;
	orders = doc.FirstChild("OrdersFile");
	if (!orders) {
		Message * mess = AddMessage("Error: Missing section");
		mess->AddItem("File name", file);
		mess->AddItem("Section", "OrdersFile");
		return false;
	}

	long id = GetLong(orders->FirstChild("GameID"));
	if (id != game->GetGameID()) {
		Message * mess = AddMessage("Error: Missmatched Game IDs");
		mess->AddLong("Host file GameID", game->GetGameID());
		mess->AddLong("Orders file GameID", id);
		return false;
	}

	if (!game->CheckMetaInfo(orders, file, ORDERSFILEVERSION))
		return false;

	const TiXmlNode * child;
	child = orders->FirstChild("PlayerNo");
	if (GetLong(child) != GetID()) {
		Message * mess = AddMessage("Error: Invalid player number");
		mess->AddLong("", GetLong(child));
		mess->AddItem("File name", file);
		return false;
	}

	child = orders->FirstChild("Turn");
	if (GetLong(child) != game->GetTurn()) {
		Message * mess = AddMessage("Error: Wrong year number in turn file");
		mess->AddLong("Turn specified", GetLong(child));
		mess->AddLong("Actual turn", game->GetTurn());
		return false;
	}

	ParseOrders(orders);
	mUnsavedChanges = 0;

	return true;
}

void Player::StartMultipleOrder()
{
	if (!mWriteXFile)
		return;

	if (mMO != NULL) {
		assert(false);
		EndMultipleOrder();
	}

	mMO = new MultipleOrder();
	AddOrder(mMO);
}

void Player::EndMultipleOrder()
{
	mMO = NULL;
}

void Player::AddOrder(Order * o)
{
	if (!mWriteXFile)
		return;

	if (mMO == NULL || mMO == o) {
		int i;
		for (i = mOrders.size() - 1; i >= 0; --i) {
			if (mOrders[i]->IsUndone()) {
				delete mOrders[i];
				mOrders.pop_back();
			} else
				break;
		}

		for (i = 0 ; i != mOrders.size(); i++) {
			if(!o->IsReplaced() && o->Replaces(mOrders[i])) {
				mOrders[i]->SetReplaced();
			}
		}

		if(i == mOrders.size()) {
			mOrders.push_back(o);
		}

		mUnsavedChanges++;
	} else
		mMO->AddOrder(o);
}

bool Player::UndoOrder(int pos/*= -1*/)
{
	bool last = false;
	if (pos < 0) {
		last = true;
		for (pos = mOrders.size() - 1; pos >= 0; --pos) {
			if (!mOrders[pos]->IsUndone())
				break;
		}

		if (pos < 0)
			return false;
	}

	if (!mOrders[pos]->Undo())
		return false;

	mUnsavedChanges--;

	if (!last) {
		delete mOrders[pos];
		mOrders.erase(mOrders.begin() + pos);
	}

	return true;
}

bool Player::RedoOrder()
{
	int pos;
	for (pos = 0; pos < mOrders.size(); ++pos) {
		if (mOrders[pos]->IsUndone())
			break;
	}

	if (pos >= mOrders.size())
		return false;

	if (!mOrders[pos]->Redo())
		return false;

	mUnsavedChanges++;
	return true;
}

void Player::TestUndoRedo()
{
	int size = mOrders.size();

	int i;
	for (i = 0; i < size; ++i)
		UndoOrder();

	for (i = 0; i < size; ++i)
		RedoOrder();
}

void Player::SetResearchTax(double tax)
{
	if ((mResearchTax > tax + epsilon || mResearchTax < tax - epsilon) && mWriteXFile)
		AddOrder(new TypedOrder<double>(&mResearchTax, AddDouble, "ResearchTax"));

	mResearchTax = tax;
}

void Player::SetResearchField(long current)
{
	if (mResearchField != current && mWriteXFile)
		AddOrder(new TypedOrder<long>(&mResearchField, "ResearchField", Rules::GetTechName));

	mResearchField = current;
}

void Player::SetResearchNext(long next)
{
	if (mResearchNext != next && mWriteXFile)
		AddOrder(new TypedOrder<long>(&mResearchNext, "ResearchNext", Rules::GetTechName));

	mResearchNext = next;
}

void Player::ChangeBattlePlan(BattlePlan * bp, int num)
{
	if (mWriteXFile)
		AddOrder(new BattlePlanOrder(mBattlePlans[num], num, this));
	else
		delete mBattlePlans[num];

	mBattlePlans[num] = bp;
}

void Player::UndoBattlePlan(BattlePlanOrder * bpo)
{
	mBattlePlans[bpo->GetNumber()] = bpo->GetBattlePlan(mBattlePlans[bpo->GetNumber()]);
}

TiXmlNode * Player::WriteBattlePlan(TiXmlNode * node, int num) const
{
	if (mBattlePlans[num] != NULL) {
		TiXmlElement * BPO = new TiXmlElement("BattlePlan");
		BPO->SetAttribute("IDNumber", num);
		mBattlePlans[num]->WriteNode(BPO, *game);
		node->LinkEndChild(BPO);
		return BPO;
	} else
		return NULL;
}

void Player::SetRelations(const deque<long> rel)
{
	if (mWriteXFile)
		AddOrder(new RelationsOrder(&mRelations));

	assert(rel.size() == mRelations.size());
	mRelations = rel;
}

void Player::SetProduction(const deque<ProdOrder *> & ords)
{
	if (mWriteXFile)
		AddOrder(new ProductionOrder("Default", &mDefaultQ));
	else {
		for (int i = 0; i < mDefaultQ.size(); ++i)
			delete mDefaultQ[i];
	}

	mDefaultQ = ords;
}

void Player::SetPayTax(bool paytax)
{
	if (paytax != mDefaultPayTax && mWriteXFile)
		AddOrder(new TypedOrder<bool>(&mDefaultPayTax, AddBool, "DefaultPayTax"));

	mDefaultPayTax = paytax;
}

void Player::ParseWaypoints(const TiXmlNode * node)
{
	WayOrderList wol;
	wol.SetFleet(GetLong(node->FirstChild("Fleet")));
	wol.ParseNode(node, this, game);

	NCGetFleet(wol.GetFleet())->ChangeWaypoints(wol);
}

double Player::GetPossibleMines(deque<MineField *> *pm, const Fleet * f, double dist) const
{
	// OPTIMIZE This gets every mine field in range, regardless of the direction of travel

	double Result = game->GetGalaxy()->MaxX() + game->GetGalaxy()->MaxY();
	if (GetRelations(f->GetOwner()) >= PR_FRIEND)
		return Result;

	int i;
	double d;
	for (i = 0; i < mMineFields.size(); ++i) {
		d = mMineFields[i]->Distance(f) - mMineFields[i]->GetRadius();
		if (d < Result)
			Result = d;

		if (d > dist)
			continue;

		pm->push_back(mMineFields[i]);
	}

	return Result;
}

void Player::DecayMineFields()
{
	int i;
	for (i = 0; i < mMineFields.size(); ++i) {
		mMineFields[i]->Decay();
		if (mMineFields[i]->GetNumMines() <= 0) {
			mMineFields.erase(mMineFields.begin() + i);
			--i;
		}
	}
}

void Player::SweepMines(SpaceObject * so, int sweep)
{
	int i;
	for (i = 0; i < mMineFields.size(); ++i) {
		mMineFields[i]->Sweep(so, sweep);
		if (mMineFields[i]->GetNumMines() <= 0) {
			mMineFields.erase(mMineFields.begin() + i);
			--i;
		}
	}
}

void Player::LayMines(const Location * loc, long type, int mines)
{
	int i;
	for (i = 0; i < mMineFields.size(); ++i) {
		if (mMineFields[i]->GetType()->GetID() == type && mMineFields[i]->Distance(loc) < mMineFields[i]->GetRadius()) {
			// combine them
			mMineFields[i]->AddMines(loc, mines);
			Message * mess = AddMessage("Mines added to existing mine field", mMineFields[i]);
			mess->AddItem("Fleet that laid it", loc);
			mess->AddLong("Number of mines", mines);
			mess->AddItem("Type of mines", mMineFields[i]->GetType()->GetName());
			return;
		}
	}

	if (mMineFields.size() < Rules::GetConstant("MaxMineFields", 512)) {
		MineField * mf = MineFieldFactory();
		mf->SetType(type);
		mf->SetLocation(*loc);
		mf->SetNumMines(mines);
		mf->SetID();
		mMineFields.push_back(mf);
		Message * mess = AddMessage("New mine field", mf);
		mess->AddItem("Fleet that laid it", loc);
		mess->AddLong("Number of mines", mines);
		mess->AddItem("Type of mines", mf->GetType()->GetName());
	} else {
		AddMessage("Too many mine fields", loc);
	}
}

void Player::DetonateMineFields()
{
	int i;
	for (i = 0; i < mMineFields.size(); ++i) {
		if (!mMineFields[i]->GetDetonate())
			continue;

		assert(mMineFields[i]->GetType()->CanDetonate());
		game->DetonateMineField(mMineFields[i]);
	}
}

SpaceObject * Player::GetPatrolTarget(const Fleet * persuer, double * range) const
{
	SpaceObject * Result = NULL;
	int i;
	for (i = 0; i < mFleets.size(); ++i) {
		if (!mFleets[i]->SeenBy(persuer->GetOwner()))
			continue;

		double dist = persuer->Distance(mFleets[i]);
		if (dist > *range)
			continue;

		HullType ht = persuer->GetBattlePlan()->GetPrimary();
		if (!mFleets[i]->HasHull(ht))
			continue;

		deque<Chaser *> chasers;
		mFleets[i]->GetChasers(&chasers, persuer->GetOwner()->GetID());
		if (chasers.size() > 0)
			continue;

		Result = mFleets[i];
		*range = dist;
	}

	return Result;
}

Fleet * Player::FleetFactory()
{
	return new Fleet(game);
}

BattlePlan * Player::BattlePlanFactory()
{
	return new BattlePlan(false);
}

MineField * Player::MineFieldFactory()
{
	return new MineField(game, this);
}

}
