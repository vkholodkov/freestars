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
 **@file WayOrder.h
 **@ingroup Server
 **@brief Waypoint Orders
 **/

#if !defined(FreeStars_WayOrder_h)
#define FreeStars_WayOrder_h

#include "Location.h"
class TiXmlNode;

namespace FreeStars {
class Player;
class Planet;
class Salvage;
class Fleet;

/**
 * Waypoint Order Type.
 * @ingroup Server
 */
enum OrderType
{
	OT_NONE			= 0, ///< (no task here).
	OT_COLONIZE		= 1, ///< Colonize.
	OT_REMOTEMINE	= 2, ///< Remote Mining.
	OT_SCRAP		= 3, ///< Scrap Fleet.
	OT_ROUTE		= 4, ///< Route.
	OT_MERGE		= 5, ///< Merge with Fleet.
	OT_LAYMINE		= 6, ///< Lay Mine Field.
	OT_TRANSFER		= 7, ///< Transfer Fleet.
	OT_PATROL		= 8, ///< Patrol.
	OT_TRANSPORT	= 9, ///< Transport.
};

/**
 * Waypoint Order.
 * A Waypoint Order is an action performed by a fleet upon
 * reaching a waypoint.
 * @ingroup Server
 */
class WayOrder {
public:
	WayOrder(Location * loc, bool ForMeOnly = false) : mLoc(loc), mForMeOnly(ForMeOnly), mSpeed(0), mOrder(OT_NONE) {}
	WayOrder(const WayOrder & copy);
	virtual ~WayOrder();
	TiXmlNode * WriteNode(TiXmlNode * node) const;

	OrderType GetType() const	{ return mOrder; }
	void SetType(OrderType ot)	{ mOrder = ot; }
	Location * NCGetLocation()	{ return mLoc; }
	const Location * GetLocation() const	{ return mLoc; }
	void SetLocation(Location * loc, bool fmo = false);
	long GetSpeed() const		{ return mSpeed; }
	void SetSpeed(long s)		{ mSpeed = s; }

	WayOrder * Copy() const;

protected:
	Location * mLoc;
	bool mForMeOnly;	// true if mLoc is For Me Only, and I need to delete it when done.
	long mSpeed;		// -1 to gate
	OrderType mOrder;

	friend class WayOrderList;
};

/**
 * Waypoint order number.
 * @ingroup Server
 */
class WayOrderNumber : public WayOrder {
public:
	WayOrderNumber(long number, Location * loc, bool ForMeOnly = false) : WayOrder(loc, ForMeOnly), mNumber(number) {}
	WayOrderNumber(const WayOrderNumber & copy) : WayOrder(copy), mNumber(copy.mNumber) {}
	virtual ~WayOrderNumber();
	TiXmlNode * WriteNode(TiXmlNode * node) const;
	long GetNumber() const	{ return mNumber; }

protected:
	long mNumber;
};

/**
 * Waypoint order Patrol.
 * @ingroup Server
 */
class WayOrderPatrol : public WayOrder {
public:
	WayOrderPatrol(long speed, long range, Location * loc, bool ForMeOnly = false) : WayOrder(loc, ForMeOnly), mPatrolSpeed(speed), mRange(range) {}
	WayOrderPatrol(const WayOrderPatrol & copy) : WayOrder(copy), mPatrolSpeed(copy.mPatrolSpeed), mRange(copy.mRange) {}
	virtual ~WayOrderPatrol();
	TiXmlNode * WriteNode(TiXmlNode * node) const;
	long GetPatrolSpeed() const		{ return mPatrolSpeed; }
	long GetPatrolRange() const		{ return mRange; }
	
protected:
	long mPatrolSpeed;	// 0 for automatic
	long mRange;
};

/**
 * Waypoint order transport.
 * @ingroup Server
 */
class WayOrderTransport : public WayOrder {
public:
	WayOrderTransport(Location * loc, bool ForMeOnly = false);
	WayOrderTransport(const WayOrderTransport & copy);
	virtual ~WayOrderTransport();
	bool ParseNode(const TiXmlNode * node, Player * player);
	TiXmlNode * WriteNode(TiXmlNode * node) const;
	TransferType GetAction(int i) const	{ return actions[i]; }
	long GetValue(int i) const			{ return values[i]; }

protected:
	deque<TransferType> actions;
	deque<long> values;
};
}
#endif // !defined(FreeStars_WayOrder_h)
