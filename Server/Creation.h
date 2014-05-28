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

/**
 **@file Creation.h
 **@ingroup Server
 **@brief Game Creation parameters.
 */

#if !defined(FreeStars_Creation_h)
#define FreeStars_Creation_h

#include <string>
#include <deque>
using namespace std;

namespace FreeStars {

/**
 * Game creation settings.
 * This class contains all the settings passed to the galaxy generator.
 */
class Creation {
public:
	Creation();
	~Creation();

	bool LoadCreation(const TiXmlNode * options);
	bool LoadNames(const char * NameFile);
	/**
	 * Planet placement function.
	 * Overload this function if you need to change the planet placement algorithm.
	 */
	void SetLocation(Planet * p);

	long mWorlds;	///< Number of planets to generate

	long mMinDistance;			///< Minimum distance between planets.
	/**
	 * Cluster odds.
	 * Odds of planet cluster forming.
	 * Must be in range 0--1.
	 */
	double mClusterOdds;
	/**
	 * Cluster odds falloff.
	 * Rate of degrading in cluster odds after successfully starting a cluster.
	 * A higher value means clusters with less planets.
	 * Each planet created in the cluster decreases the cluster odds by this much.
	 * Should be greater than 0 and less than mClusterOdds.
	 */
	double mClusterOddsDegrade; ///< Rate of degrading in the odds.
	/**
	 * Current cluster odds.
	 * Set to 1.0 to force an initial cluster around the initial planet.
	 */
	double mCurrClusterOdds;
	long mClusterMaxDistance;	///< Maximum size of a cluster

	long mStartPositionCount;
	long mStartPositionMin;
	long mStartPositionMax;

	long mHWBasePop;
	long mHWPopBonus;
	long mHWFactories;
	long mHWMines;
	long mHWDefenses;

	bool mSecondaryWorlds;
	double mPopMultiplierFor2nd;
	double mSecondaryPop;
	long mSWFactories;
	long mSWMines;
	long mSWDefenses;
	long mSWMaxMin;
	long mSWMinMin;
	long mMinSWDistance;
	long mMaxSWDistance;

	string GetNextName();
	void AddHW(Planet * p)	{ mPrePlacedHWs.push_back(p); }
	Planet * GetNextHW()	{ return mHW == mPrePlacedHWs.end() ? NULL : *mHW++; }
	Planet * GetSecond(const Player * p);
	void AddSecond(const Player * p, Planet * sw)	{ mSecondWorlds.insert(pair<const Player *, Planet *>(p, sw)); }

private:
	deque<string> mNames;
	deque<string>::iterator mNamePos;
	long mNameCount;
	deque<Planet *> mPrePlacedHWs;
	deque<Planet *>::iterator mHW;
	map<const Player *, Planet *> mSecondWorlds;

	Planet * LastP;
};
}
#endif // !defined(FreeStars_Creation_h)
