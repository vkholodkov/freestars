/*
Copyright 2005 Elliott Kleinrock, Dan Neely, Kurt W. Over, Damon Domjan

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

#include <math.h>
#include <ctype.h>

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

Creation::Creation()
{
	LastP = NULL;
	mNameCount = 1;
	mHW = mPrePlacedHWs.begin();
}

Creation::~Creation()
{
}

bool Creation::LoadCreation(const TiXmlNode * options)
{
	const TiXmlNode * child1;
	if (!options)
		return false;

	mWorlds = GetLong(options->FirstChild("Worlds"));
	if (mWorlds <= 0) {
		return false;
	}

	mMinDistance = GetLong(options->FirstChild("MinDistance"));
	if (mMinDistance < 1) {
		return false;
	}

	mClusterOdds = GetDouble(options->FirstChild("ClusterOdds"));
	mClusterOddsDegrade = GetDouble(options->FirstChild("ClusterOddsDegrade"));
	mClusterMaxDistance = GetLong(options->FirstChild("ClusterMaxDistance"));

	if (mClusterOdds < 0.0 || mClusterOdds >= 1.0) {
		return false;
	}

	if (mClusterOddsDegrade < 0.0 || mClusterOddsDegrade > mClusterOdds) {
		return false;
	}

	if (mClusterMaxDistance <= mMinDistance) {
		return false;
	}

	mCurrClusterOdds = mClusterOdds;

	mStartPositionCount = GetLong(options->FirstChild("StartPositionCount"));
	mStartPositionMin = GetLong(options->FirstChild("StartPositionMin"));
	mStartPositionMax = GetLong(options->FirstChild("StartPositionMax"));
	if (mStartPositionCount > 0) {
		if (mStartPositionMin >= mStartPositionMax) {
			return false;
		}
		if (mStartPositionMin <= 10) {
			return false;
		}
		if (mStartPositionMax <= 100) {
			return false;
		}
	} else if (mStartPositionCount < 0) {
		return false;
	}

//	mMapFile = options->FirstChild("MapFile"));

	child1 = options->FirstChild("HomeWorld");
	if (child1 == NULL) {
		return false;
	}

	mHWBasePop = GetLong(child1->FirstChild("BasePop"), 25000);
	mHWPopBonus = GetLong(child1->FirstChild("BonusTimesGR"), 500000);
	mHWFactories = GetLong(child1->FirstChild("Factories"), 10);
	mHWMines = GetLong(child1->FirstChild("Mines"), 10);
	mHWDefenses = GetLong(child1->FirstChild("Defenses"), 10);

	child1 = options->FirstChild("SecondWorld");
	if (!child1)
		mSecondaryWorlds = false;
	else {
		mSecondaryWorlds = true;

		mPopMultiplierFor2nd = GetDouble(child1->FirstChild("PopMultiplierFor2nd"), 0.8);
		if (mPopMultiplierFor2nd < .01) {
			return false;
		}

		mSecondaryPop = GetDouble(child1->FirstChild("SecondaryPop"), 0.5);
		if (mSecondaryPop < .01) {
			return false;
		}

		mSWFactories = GetLong(child1->FirstChild("Factories"), 4);
		mSWMines = GetLong(child1->FirstChild("Mines"), 10);
		mSWDefenses = GetLong(child1->FirstChild("Defenses"), 0);
		mSWMaxMin = GetLong(child1->FirstChild("MaxMinerals"), 300);
		mSWMinMin = GetLong(child1->FirstChild("MinMinerals"), 100);
		mMinSWDistance = GetLong(child1->FirstChild("MinDistance"), 100);
		mMaxSWDistance = GetLong(child1->FirstChild("MaxDistance"), 300);
	}

	return true;
}

bool Creation::LoadNames(const char * NameFile)
{
	if (!NameFile || !*NameFile)
		return false;

	FILE * f = fopen(NameFile, "r");
	if (!f)
		return false;
	
	char ptr[128];
	char * rv = ptr;
	while (rv != NULL) {
		rv = fgets(ptr, 80, f);
		while (isspace(ptr[strlen(ptr)-1]))
			ptr[strlen(ptr)-1] = '\0';

		if (rv != NULL)
			mNames.push_back(ptr);
	}

	Random_Shuffle(mNames.begin(), mNames.end());
	mNamePos = mNames.begin();

	if (mNames.size() < mWorlds) {
		return false;
	} else
		return true;
}

void Creation::SetLocation(Planet * p, Galaxy *galaxy)
{
	int count = 0;
	long dist = 0;
	bool inCluster = Randodd(mCurrClusterOdds);

	while (count++ < 1000 || dist == 0) {	// hard coded max to planet placement for safety
		if (LastP == NULL || !inCluster) {
			p->SetPosX(Random(galaxy->MinX(), galaxy->MaxX()));
			p->SetPosY(Random(galaxy->MinY(), galaxy->MaxY()));
		} else {
			// this gives higher density for nearby planets then distant ones
			double angle = genrand_real2() * 2.0 * 3.1415926535;
			long range = Random(mMinDistance, mClusterMaxDistance) + 1;
			p->SetPosX(LastP->GetPosX() + long(sin(angle) * range));
			if (p->GetPosX() < galaxy->MinX() || p->GetPosX() >= galaxy->MaxX())
				continue;

			p->SetPosY(LastP->GetPosY() + long(cos(angle) * range));
			if (p->GetPosY() < galaxy->MinY() || p->GetPosY() >= galaxy->MaxY())
				continue;
		}

		const Planet * cp = galaxy->ClosestPlanet(p);
		if (cp == NULL)
			break; // first planet, place it anywhere

		dist = long(p->Distance(cp));
		if (dist >= mMinDistance)	// min mMinDistance is 1, so dist has to be >= 1
			break;	// far enough apart, go with it
	}

	if (LastP && inCluster)
		mCurrClusterOdds -= mClusterOddsDegrade;
	else
		mCurrClusterOdds = mClusterOdds;

	LastP = p;
}

string Creation::GetNextName(Galaxy *galaxy)
{
	string name;
	do {
		if (mNamePos == mNames.end()) {
			mNamePos = mNames.begin();
			mNameCount++;
		}

		name = *mNamePos++;
		if (mNameCount > 1)
			name += " #" + Long2String(mNameCount);
	} while (galaxy->GetPlanet(name.c_str()) != NULL);

	return name;
}

Planet * Creation::GetSecond(const Player * p)
{
	map<const Player *, Planet *>::iterator iter;

	iter = mSecondWorlds.find(p);
	if (iter == mSecondWorlds.end())
		return NULL;
	else
		return iter->second;
}

}
