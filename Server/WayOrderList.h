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
 **@file WayOrderList.h
 **@ingroup Server
 **@brief Waypoint Order List.
 **/

#if !defined(FreeStars_WayOrderList_h)
#define FreeStars_WayOrderList_h

class TiXmlNode;

namespace FreeStars {
class Player;
class WayOrder;
class Fleet;
class Game;

/**
 * List of Waypoint Orders.
 * @ingroup Server
 */
class WayOrderList {
public:
	WayOrderList() : nPlayer(0), nFleet(0), mNoDelete(false) {}
	~WayOrderList();

	bool ParseNode(const TiXmlNode * node, Player * player, Game*);
	const deque<WayOrder *> & GetOrders() const	{ return orders; }
	deque<WayOrder *> & GetOrders() { return orders; }
	void SetFleet(long fleet)		{ nFleet = fleet; }
	long GetPlayer() const			{ return nPlayer; }
	long GetFleet() const			{ return nFleet; }
	void SetNoDelete()				{ mNoDelete = true; }

protected:
	unsigned long nPlayer;
	unsigned long nFleet;
	deque<Fleet *> chasing;
	deque<WayOrder *> orders;
	bool mNoDelete;
};
}
#endif // !defined(FreeStars_WayOrderList_h)
