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
 **@file Location.h
 **@ingroup Server
 **@brief Not every coord is interesting.
 **/

#if !defined(FreeStars_Location_h)
#define FreeStars_Location_h

#include "Galaxy.h"

namespace FreeStars {

/**
 * Location.
 */	
class Location {
public:
	Location(const Location &source) : posX(source.posX), posY(source.posY) {}
//	Location(long x, long y) : posX(x), posY(y) {}
	Location() : posX(0), posY(0) {}

	virtual ~Location();
	virtual bool ParseNode(const TiXmlNode * node, Game*);  ///< Load.
	virtual TiXmlNode * WriteNode(TiXmlNode * node) const	{ return WriteLocation(node); } ///< Save.
	TiXmlNode * WriteLocation(TiXmlNode * node) const; ///< Save this location.

	void SetLocation(const Location & loc)	{ posX = loc.posX; posY = loc.posY; } ///< Set location to match another location.
	void SetLocation(long px, long py)		{ posX = px; posY = py; } ///< Set location.
	long GetPosX() const		{ return posX; } ///< Get X position.
	long GetPosY() const		{ return posY; } ///< Get Y position.
	void SetPosX(long x)		{ posX = x; } ///< Set X Position.
	void SetPosY(long y)		{ posY = y; } ///< Set Y Position.
	double Distance(const Location * other) const; ///< Calculate distance to another location.
	double Distance(double px, double py) const; ///< Calculate distance to another location.
	bool IsWith(const Location & other) const	{ return posX == other.posX && posY == other.posY; } ///< Check if locations are in the same place.
	bool IsWith(long x, long y) const			{ return posX == x && posY == y; } ///< Check if location is at x,y.

	static void MoveToward(const Location * start, const Location * dest, double * px, double * py, long distance); ///< Move a location toward another location.

	friend bool operator ==(const Location &a, const Location &b)	{ return a.IsWith(b); } ///< Are locations at the same coordinate?
	friend bool operator !=(const Location &a, const Location &b)	{ return !(a == b); } ///< Are locations _not_ at the same coordinate?

protected:
	long posX; ///< X position.
	long posY; ///< Y position.
};
}
#endif // !defined(FreeStars_Location_h)
