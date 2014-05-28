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
 **@file MineField.h
 **@ingroup Server
 **@brief Minefields.
 **/

//Dan Neely
//2-9-03 fully debuged
//2-11-03 changed to inherit from GameObject
//2-13-03 reordered methods to be consistant in .h and .cpp

#if !defined(FreeStars_MineField_h)
#define FreeStars_MineField_h

#include "Location.h"

namespace FreeStars {
class MineFieldType;

/**
 * A mine field.
 * Mine fields are deployed by minelaying ships.
 * @ingroup Server
 */
class MineField : public SpaceObject
{
public:
	MineField(Player * owner);
	~MineField();//only to make gameobject happy. mmineType points to an
			//element in a reference list not a unique object in the class

	void SetID();
	void SetType(int mineType)	{ mType = Rules::GetMineFieldType(mineType); }
	void SetNumMines(int num)	{ mNumMines = num; }
	int GetNumMines() const { return mNumMines; }
	void AddMines(const Location * L, int n);
	void Sweep(const SpaceObject * so, int sweep);
	void Decay(); 
	void ReduceFieldCollision();  
	bool TestCollide(const Location * L, int warp);
	bool CollisionPossible(const Location & L1, const Location & L2);
	bool InField(int x, int y); // is x,y in the field
	bool InField(const Location * L); // is x,y in the field
	const MineFieldType * GetType() const		{ return mType; }
	double GetRadius() const	{ return sqrt(double(mNumMines)); }
	bool IsEmpty() const		{ return mNumMines == 0; }
	bool GetDetonate() const	{ return mDetonate; }
	void SetDetonate(bool det);

	virtual double GetMaxTachyon() const	{ return 1.0; }
	virtual long GetScanPenetrating() const			{ return -1; }
	virtual long GetScanSpace() const;
	virtual long GetCloak(const Player * p, bool pen) const;
	// while mine fields can technically be 'in orbit' it has no effect, so just claim we're never in orbit
	virtual Planet * InOrbit()				{ return NULL; }
	virtual const Planet * InOrbit() const	{ return NULL; }

protected:
	deque<bool> mHadSeen;
	int mNumMines;
	Player * mOwner;
	const MineFieldType * mType;
	bool mDetonate;
	bool RectFilter(int x1, int y1, int x2, int y2); //true if rectange 
				//intersect.  this indicates colision is possible
				//this is the preliminary step in colisions possible


// don't think I need either of these, just leaving them incase I'm wrong
//		void CollisionPathFindEdge(int &x1, int &y1, int x2, int y2);
				// x1,y1 and x2,y2 straddle a minefield edge
				// returns the boundary in the coord that was originally
				// outside the field second coord contains garbage
				// x2,y2 must be in field, x1,y1 in open space.  
				// errortrap aborts program if not true
//		void CollisionPath(int &x1, int &y1, int &x2, int &y2);
		//takes the start/end points of the path and returns the start/end
		//end points of the path segment through the minefield the boundary
		//is defined as the 1st/last points of the path in the field

};
}
#endif // !defined(FreeStars_MineField_h)
