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
#include "Location.h"
#include "Wormhole.h"
#include <deque>

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

Wormhole::Wormhole()
{
	Init();
	TheGalaxy->GetWormholeID();
}

Wormhole::Wormhole(const Wormhole &hole)
{
	Init();
	mID = hole.mID;
	mStability = hole.mStability;
	mMaxStability = hole.mMaxStability;
	mMinStability = hole.mMinStability;
	mAttached = hole.mAttached;
	mAttachedID = hole.mAttachedID;
}

void Wormhole::Init()
{
	mAttached = NULL;
	mAttachedID = 0;
	mStability = WORM_VERYSTABLE;
	mMaxStability = WORM_VERYSTABLE;
	mMinStability = WORM_VERYUNSTABLE;
	mID = 0;
	mHadSeen.insert(mHadSeen.begin(), TheGame->NumberPlayers(), false);
	mTraversed.insert(mTraversed.begin(), TheGame->NumberPlayers(), false);
}

bool Wormhole::ParseNode(const TiXmlNode * node)
{
	SpaceObject::ParseNode(node);

	mStability = GetLong(node->FirstChild("Stability"));
	mMaxStability = GetLong(node->FirstChild("MaxStability"));
	mMinStability = GetLong(node->FirstChild("MinStability"));
	mAttachedID = GetLong(node->FirstChild("Attached"));
	Rules::ParseArrayBool(node->FirstChild("Traversed"), "Race", "Number", mTraversed);
	for (int i = 0; i < TheGame->NumberPlayers(); ++i) {
		if (SeenBy(i))
			mHadSeen[i] = true;
	}

	TheGame->AddAlsoHere(this);

	if(mID == 0)
		return false;
	else
		return true;
}

void Wormhole::AdjustTraverse()
{
	for (int i = 0; i < TheGame->NumberPlayers(); ++i) {
		if (!SeenBy(i))
			mTraversed[i] = false;
		else if (GetAttached() && GetAttached()->SeenBy(i) == 0)
			mTraversed[i] = false;
	}
}

TiXmlNode * Wormhole::WriteNode(TiXmlNode * node, const Player * viewer) const
{
	if (viewer == NULL || SeenBy(viewer)) {
		SpaceObject::WriteNode(node, viewer);
		AddLong(node, "Stability", mStability);
		if (viewer == NULL || (mTraversed[viewer->GetID()-1] && GetAttached()->SeenBy(viewer->GetID()-1)))
			AddLong(node, "Attached", mAttachedID);

		if (viewer == NULL) {
			AddLong(node, "MaxStability", mMaxStability);
			AddLong(node, "MinStability", mMinStability);
			node->LinkEndChild(Rules::WriteArrayBool("Traversed", "Race", "Number", mTraversed));
		}
	}

	return node;
}

const Wormhole * Wormhole::GetAttached() const
{
	if (mAttachedID == 0)
		return NULL;

	if (mAttached == NULL)
		const_cast<Wormhole *>(this)->mAttached = TheGalaxy->GetWormhole(mAttachedID);

	return mAttached;
}

void Wormhole::Enter(long id)
{
	mTraversed[id-1] = true;
	mHadSeen[id-1] = true;
}

void Wormhole::Exit(long id) const
{
	const_cast<Wormhole *>(this)->mHadSeen[id-1] = true;
}

void Wormhole::Jiggle()
{
	///@todo validate the odds are somewhat correct
	posX += Random(mStability * 4) - mStability * 2;
	posY += Random(mStability * 4) - mStability * 2;

	if (Random(50) < mStability)
		Shift();
	else if (Random(10) == 0)
		mStability = min(mStability++, mMinStability);
}

void Wormhole::Shift()
{
	// reset the wormhole
	mID = TheGalaxy->GetWormholeID();
	mSeenBy.erase(mSeenBy.begin(), mSeenBy.end());
	mSeenBy.insert(mSeenBy.begin(), TheGame->NumberPlayers(), 0L);

	///@todo figure out how to hide the newly moved wormhole. new ID is probably best
//	mID = TheGalaxy->GetWormholeID();
//	GetAttached()->SetOtherEnd(this);

	mStability = mMaxStability;

	long dist = 0;
	long count = 0;
	while (count++ < 1000 || dist == 0) {	// hard coded max to planet placement for safety
		SetPosX(Random(TheGalaxy->MinX(), TheGalaxy->MaxX()));
		SetPosY(Random(TheGalaxy->MinY(), TheGalaxy->MaxY()));

		dist = long(Distance(TheGalaxy->ClosestPlanet(this)));
		if (dist >= TheGame->GetWHMinDistance())
			break;	// far enough apart, go with it
	}
}

}
