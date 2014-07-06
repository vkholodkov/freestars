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
 **@file Cost.h
 **@ingroup Server
 **@brief Cost stuff.
 **/

#if !defined(FreeStars_Cost_h)
#define FreeStars_Cost_h

namespace FreeStars {

/**
 * Cost.
 * This class defines the cost of items, in minerals, resources, and crew.
 * Original Stars! didn't include a crew cost, so it will be 0 to begin with.
 * Just cargo is done in Rules::WriteCargo and Rules::ReadCargo
 */
class Cost {
public:
	Cost();
	Cost(const Cost & c);

	void ReadCosts(const TiXmlNode * node, MessageSink &messageSink);
	TiXmlNode * WriteCosts(TiXmlNode * node, const char * name) const;
	long GetResources() const { return mResources; }
	long GetCrew() const { return mCrew; }
	void SetResources(unsigned long r) { mResources = r; }
	void SetCrew(unsigned long c) { mCrew = c; }
	bool IsZero();
	void Zero();
	long & operator [](CargoType ct);
	long operator [](CargoType ct) const;
	Cost & operator = (const Cost & c);
	Cost & operator *= (double factor);
	Cost & operator += (const Cost & c);
	Cost & operator -= (const Cost & c);
	friend bool operator == (const Cost & a, const Cost & c);
	friend Cost operator *(const Cost & c, double factor)	{ Cost r(c); r *= factor; return r; }
	friend Cost operator +(const Cost & a, const Cost & b)	{ Cost r(a); r += b; return r; }
	friend Cost operator -(const Cost & a, const Cost & b)	{ Cost r(a); r -= b; return r; }

	void Cleanup()	{ mMinerals.clear(); }

protected:
	deque<long> mMinerals;
	long mResources;
	long mCrew;
};
}
#endif // !defined(FreeStars_Cost_h)
