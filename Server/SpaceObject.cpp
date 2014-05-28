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

#include "SpaceObject.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {
void SpaceObject::Init()
{
	mSeenBy.insert(mSeenBy.begin(), TheGame->NumberPlayers(), 0L);
	mID = 0;
}

SpaceObject::~SpaceObject()
{
}

bool SpaceObject::ParseNode(const TiXmlNode * node)
{
	unsigned long num = GetLong(node->FirstChild("Owner"));
	if (num < 0 || num > TheGame->NumberPlayers()) {
		Message * mess = TheGame->AddMessage("Error: Invalid player number");
		mess->AddLong("", num);
		mess->AddItem("Owener of", this);
		return false;
	}

	return ParseNode(node, TheGame->NCGetPlayer(num));
}

bool SpaceObject::ParseNode(const TiXmlNode * node, Player * player)
{
	if (!Location::ParseNode(node->FirstChild("Location")))
		return false;

	mOwner = player;
	const TiXmlElement * tie = node->ToElement();
	mID = atol(tie->Attribute("IDNumber"));
	Rules::ParseArray(node->FirstChild("SeenBy"), "Race", "Number", mSeenBy);

	return true;
}

TiXmlNode * SpaceObject::WriteNode(TiXmlNode * node, const Player * viewer) const
{
	if (!SeenBy(viewer))
		return NULL;

	if (mAlsoHere && mAlsoHere->at(0) != this && dynamic_cast<Planet *>(mAlsoHere->at(0)) != NULL) {
		TiXmlElement *loc = new TiXmlElement("Location");
		AddString(loc, "Planet", mAlsoHere->at(0)->GetName(NULL).c_str());
		node->LinkEndChild(loc);
	} else
		Location::WriteNode(node);

	if (SeenBy(viewer) & SEEN_OWNER) {
		if (GetOwner() == NULL)
			AddLong(node, "Owner", 0);
		else
			AddLong(node, "Owner", GetOwner()->GetID());
	}

	assert(mID != 0);
	TiXmlElement * tie = node->ToElement();
	tie->SetAttribute("IDNumber", mID);

	if (viewer == NULL)
		node->LinkEndChild(Rules::WriteArray("SeenBy", "Race", "Number", mSeenBy));

	return node;
}

long SpaceObject::SeenBy(const Player * p) const
{
	if (p == NULL)
		return SEEN_HOST;
	else if (p == GetOwner())
		return SEEN_BYOWNER;
	else
		return SeenBy(p->GetID()-1);
}

long SpaceObject::SeenBy(unsigned long p) const
{
	if (p < 0 || p >= TheGame->NumberPlayers())
		return SEEN_NONE;
	else
		return mSeenBy[p];
}

void SpaceObject::ResetSeen()
{
	mSeenBy.clear();
	mSeenBy.insert(mSeenBy.begin(), TheGame->NumberPlayers(), false);
}

void SpaceObject::GoingAwayNotifyChasers()
{
	int i;
	for (i = 0; i < mChasers.size(); ++i)
		mChasers[i]->ChaseeGone(this);
}

void SpaceObject::CheckSeenByChasers()
{
	int i;
	for (i = 0; i < mChasers.size(); ++i)
		if (!SeenBy(mChasers[i]->GetOwnerID()-1))
			mChasers[i]->ChaseeGone(this);
}

Planet * SpaceObject::InOrbit()
{
	return dynamic_cast<Planet *>(mAlsoHere->at(0));
}

void SpaceObject::GetChasers(deque<Chaser *> * chasers, int owner)
{
	chasers->clear();
	if (owner == 0)
		chasers->insert(chasers->begin(), mChasers.begin(), mChasers.end());
	else {
		int i;
		for (i = 0; i < mChasers.size(); ++i)
			if (mChasers[i]->GetOwnerID() == owner)
				chasers->push_back(mChasers[i]);
	}
}

}
