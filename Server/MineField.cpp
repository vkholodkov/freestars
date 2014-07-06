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

//dan Neely
//2-9-03
#include "FSServer.h"

#include "MineField.h"
#include "MineFieldType.h"
#include "Order.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

/*
int Midpoint(int a, int b)
{
	if (a < b)
		return  a + ((b - a +1)>>1);
	return  b + ((a - b +1)>>1);
}
*/

MineField::~MineField(void)
{
}

MineField::MineField(Game *game, Player * owner)
    : SpaceObject(game)
{
	mOwner = owner;
}

void MineField::SetID()
{
	mID = mOwner->GetMineFieldID();
}

long MineField::GetScanSpace() const
{
	return GetOwner()->MineFieldScanning() ? long(GetRadius()) : -1;
}

long MineField::GetCloak(const Player * p, bool pen) const
{
	return (pen || mHadSeen[p->GetID()-1]) ? 0 : Rules::GetConstant("MineFieldCloak", 75);
}

void MineField::SetDetonate(bool det)
{
	mDetonate = det;
	if (det != mDetonate && GetOwner()->WriteXFile())
		NCGetOwner()->AddOrder(new TypedOrder<bool>(&mDetonate, AddBool, "DetonateMineField", "MineField", Long2String(GetID()).c_str()));
}

void MineField::AddMines(const Location * L, int n)
{
	mNumMines += n;
	SetPosX(long((double(n) / double(mNumMines)) * (L->GetPosX() - GetPosX()) + GetPosX() + .5));
	SetPosY(long((double(n) / double(mNumMines)) * (L->GetPosY() - GetPosY()) + GetPosY() + .5));
}

void MineField::Sweep(const SpaceObject * so, int sweep)
{
	double dist = Distance(so);
	if (dist > GetRadius())
		return;

	int old = mNumMines;	// for messages
	// set mNumMines to the max of mNumMines - sweep, dist^2-1
	mNumMines = max(int(dist*dist) - 1, mNumMines - sweep);
	mNumMines = max(mNumMines, 0);	// dist^2 - 1 could be -1 if dist is 0

	// add messages
	Message * mess;
	mess = mOwner->AddMessage("Your minefield swept", this);
	mess->AddLong("Mines swept", old - mNumMines);

	mess = so->NCGetOwner()->AddMessage("You've swept a minefield", this);
	mess->AddItem("Sweeper", so);
	mess->AddLong("Mines swept", old - mNumMines);
}

void MineField::Decay()
{
	int numPlanets;

	numPlanets = mGame->GetGalaxy()->GetPlanetsWithin(this, GetRadius());

	double d;
	d = Rules::GetFloat("MineFieldBaseDecayRate");
	d += Rules::GetFloat("MineFieldPlanetDecayRate") * numPlanets;
	if (GetDetonate())
		d += Rules::GetFloat("MineFieldDetonateDecayRate");

	d *= GetOwner()->MineDecayFactor();
	d = min(d, Rules::GetFloat("MineFieldMaxDecayRate"));

	long decay = long(mNumMines * d + .5);
	decay = max(decay, long(10 * GetOwner()->MineDecayFactor() + .5));
	mNumMines -= decay;
	//	caller must check for mine fields that evaporate
}

void MineField::ReduceFieldCollision()
{
	if (mNumMines <= 10) {
		mNumMines = 0;
		NCGetOwner()->DeleteMineField(this);
	} else if (mNumMines <= 200)
		mNumMines -=10;
	else if (mNumMines <= 1000)
		mNumMines = static_cast<int>(mNumMines * 0.95);
	else if (mNumMines <= 5000)
		mNumMines -= 50;
	else mNumMines = static_cast<int>(mNumMines * 0.95);
}

bool MineField::TestCollide(const Location * L, int warp)
{
	long radius = long(GetRadius() + .5);
	long negRadius = -radius;

	int dx = L->GetPosX() - posX;
	if (dx > radius || dx < negRadius)
		return false; // no collision

	int dy = L->GetPosY() - posY;
	if (dy > radius || dy < negRadius)
		return false; // no collision

	int dr2 = dx*dx + dy*dy; //r^2
	if (mNumMines >= dr2)
	{
		if (warp <= mType->GetSafeSpeed())
			return false;

		if (Randodd(mType->GetHitOdds() * (warp - mType->GetSafeSpeed())))
			return true;
	}
	return false;
}

bool MineField::CollisionPossible (const Location & L1, const Location & L2)
{
	// do the easy test before the complex one
	if (!RectFilter(L1.GetPosX(),L1.GetPosY(),L2.GetPosX(),L2.GetPosY()))
		return false;

	//substituions for speed and readability of following code
	long x1= L1.GetPosX();
	long y1= L1.GetPosY();
	long x2= L2.GetPosX();
	long y2= L2.GetPosY();

//http://www.exaflop.org/docs/cgafaq/cga1.html#Subject%201.02:%20How%20do%20I%20find%20the%20distance%20from%20a%20point%20to%20a%20line?
//Comp.Graphics.Algorithms Frequently Asked Questions
//Section 1. 2D Computations: Points, Segments, Circles, Etc.
//Subject 1.02: How do I find the distance from a point to a line?
//Let the point be C (Cx,Cy) and the line be AB (Ax,Ay) to (Bx,By).    The 
//	length of the line segment AB is L:
//
//        L= sqrt( (Bx-Ax)^2 + (By-Ay)^2 ) .

	int L = static_cast<int>(sqrt((double)((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2))));

//Let P be the point of perpendicular projection of C onto AB. Let r be a 
//	parameter to indicate P's location along the line containing AB, with 
//	the following meaning:
//
//          r=0      P = A
//          r=1      P = B
//          r<0      P is on the backward extension of AB
//          r>1      P is on the forward extension of AB
//          0<r<1    P is interior to AB
//
//Compute r with this:
//
//            (Ay-Cy)(Ay-By)-(Ax-Cx)(Bx-Ax)
//        r = -----------------------------
//                        L^2

	int r=  ( (y1-posY)*(y1-y2)-(y1-posY)*(y2-y1) ) / (L * L) ;

	// closest point before x1,y1 is x1,y1 in circle?
	if (r<0)
	{	// r^2 > distance from x1,y1 to center of field
		return InField(x1,y1);
	}
	if (r>1)
	{
		return InField(x2,y2);
	}

//The point P can then be found:
//
//        Px = Ax + r(Bx-Ax)
//        Py = Ay + r(By-Ay)

//  don't actaully need this (I think)
//	int Px = x1 + r*(x2-x1);
//	int Py = y1 + r*(y2-y1);

//And the distance from A to P = r*L.
//Use another parameter s to indicate the location along PC, with the 
//following meaning:
//
//           s<0      C is left of AB
//           s>0      C is right of AB
//           s=0      C is on AB
//
//Compute s as follows:
//    (Ay-Cy)(Bx-Ax)-(Ax-Cx)(By-Ay)
//        s = -----------------------------
//                        L^2

	int s = ((y1-posY)*(x2-x1)-(x1-posX)*(y2-y1)) / (L*L);

//Then the distance from C to P = s*L.

	return (GetRadius() >= s*L);
}

/*
void MineField::CollisionPath(int &x1, int &y1, int &x2, int &y2)
{	
	if (InField(x1,y1) && InField(x2,y2))
		return;  // both points are in minefield
	int mx = Midpoint(x1,x2);
	int my = Midpoint(y1,y2); 

	if(InField(x1,y1)) // x1y1 in
	{
		CollisionPathFindEdge(x2,y2,x1,y1);
	} // x1y1 in
	else if(InField(x2,y2)) 
	{
		CollisionPathFindEdge(x1,y1,x2,y2);
	} // x2y2 in
	else //niether in
	{
		if (!InField(mx,my)) 
		{
			//since we need a point on the fleet path, drop center of mine 
			//up/down to lie on path we need to convert the fleet path into 
			//the form y = mx + b

			// first need to test special case of vertical path.  in so 
			// shift center right/left instead
			if (x1==x2)
			{	//since line is vertical mx doesn't need moved
				my =posY;
			}
			else
			{
				int m = (y1-y2)/(x1-x2);
				int b = (-m*x1+y1);
				mx = posX;
				my = m*mx + b;
			}
		}
		CollisionPathFindEdge(x1,y1,mx,my);
		CollisionPathFindEdge(x2,y2,mx,my);
	}
}
*/
/////////////////////////////////////////////////////////////////////////////
//////////////////////////////Protected Methods//////////////////////////////
/////////////////////////////////////////////////////////////////////////////

bool MineField::InField(int x, int y)
{
	// really radius ^ 2, but that's just mNumMines
	return mNumMines <= Distance(x, y);
}

bool MineField::InField(const Location * L)
{
	return mNumMines <= Distance(L);
}

bool MineField::RectFilter(int x1, int y1, int x2, int y2)
{
	// reorder so x1,y1 is upper left corner
	// since we're making a box it doesn't matter if we swap x's but not y's
	if (x1 > x2)
	{
		int temp = x1;
		x1 = x2;
		x2= temp;
	}
	if (y1 > y2)
	{
		int temp = y1;
		y1 = y2;
		y2= temp;
	}
	int X1, Y1, X2, Y2;
	long radius = long(GetRadius() + .5);
	X1 = posX - radius;
	X2 = posX + radius;
	Y1 = posY - radius;
	Y2 = posY + radius;

	return !((x2 < X1) || (x1 > X2) || (y2 < Y1) || (y1 > Y2));
}

/*
void  MineField::CollisionPathFindEdge(int &x1, int &y1, int x2, int y2)
{
	// midpoints
	int mx = Midpoint(x1,x2);
	int my = Midpoint(y1,y2); 
	

	if(InField(x2,y2)) // x2y2 in should be the case, just an error check
	{
		bool abort =false;
		while (!InField(mx,my) &&  !abort)
		{
			x1= mx;
			y1= my;
			mx = Midpoint(x1,x2);
			my = Midpoint(y1,y2);
			abort = ((x1-1 == x2) || (y1-1 == y2));
		}
		// at this point boundary is between mx,my and X2,Y2

		// this special case is only needed when the fleet just grazes the
		//edge of the field  eg r=30 center 100,100 fleet moving due north
		// on x = 70 in that case without this check a 2 ly path is returned 
		//instead of the correct result of only at 70,100
		if ( ((x1-1) != x2) && ((y1-1) != y2) )
		{
			x2= mx;
			y2= my;
		}

		bool p1ismid = false;
		bool p2ismid = false;

		while  ( !p1ismid && !p2ismid )
		{
			mx = Midpoint(x1,x2);
			my = Midpoint(y1,y2); 
			
			p1ismid = ((x1 == mx)&&(y1 == my));
			p2ismid = ((x2 == mx)&&(y2 == my));


			if (InField(mx,my))
			{
				x2 = mx;
				y2 = my;
			}
			else
			{
				x1 = mx;
				y1 = my;
			}
			
		}
		//loop runs once more than needed, and when going accross right/bottom
		//edge of field generates a fencepost error.  in this case, mx,my
		//is one unit outside field, while x2,y2 is on the edge exactly
		//since x2,y2 always is in that position and sicne I can't figure out
		//how to make the loop terminate one iteration sooner I'm just dumping
		//x2,y2 into x1,y1

		//x1 = mx
		//y1 = my
		if (InField(x2,y2))
		{
			x1 = x2;
			y1 = y2;
		}
	}
	else
	{
		std::cerr << "error in MineField::CollisionPathFindEdge x2,y2 not in"
				  << " field\n";
		abort();
	}
}

*/
}
