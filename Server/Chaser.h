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
 **@file Chaser.h
 **@ingroup Server
 **@brief Chaser.
 **/

#if !defined(FreeStars_Chaser_h)
#define FreeStars_Chaser_h

namespace FreeStars {
class SpaceObject;

/**
 * Chaser.
 * This is an Abstract Base Class (ABC) used as an Interface, it describes
 * functions requred to be a chaser of a SpaceObject.
 * @ingroup Server
 */
class Chaser {
public:
	Chaser() {}
	virtual ~Chaser() {}

	virtual void ChaseeGone(SpaceObject * chasee) = 0;
	virtual int GetOwnerID() const = 0;
};
}
#endif // !defined(FreeStars_Chaser_h)
