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
 **@file Packet.h
 **@ingroup Server
 **@brief Packets.
 **/

#if !defined(FreeStars_Packet_h)
#define FreeStars_Packet_h

//dan Neely 7-1-03

#include "CargoHolder.h"
#include "Planet.h"

namespace FreeStars {

/**
 * Packet.
 * A packet is a hunk of minerals thrown at another planet,
 * where it is then caught and recovered.
 * @ingroup Server
 */	
class Packet : public CargoHolder 
{
public:
	Packet() : mSpeed(0), mDriverSpeed(0), mDestination(NULL), mFirstYear(false) { SetID(0); }
	Packet(const CargoHolder &source, long speed, long driverSpeed, Planet * destination);

	/// Name of XML element associated with class.
	static const char* ELEMENT_NAME()	{return "Packet";}
	virtual bool ParseNode(const TiXmlNode * node);
	virtual TiXmlNode * WriteNode(TiXmlNode * node, const Player * viewer) const;

	bool IsEmpty() const	{ return mMaxSize == 0; }
	virtual long GetCargoCapacity() const	{ return mMaxSize; }
	virtual long GetCloak(const Player *, bool) const;
	virtual double GetMaxTachyon() const		{ return 0.0; }
	virtual long GetScanPenetrating() const;
	virtual long GetScanSpace() const;
	virtual void SetCanLoadBy(const Player *)	{}
	virtual bool CanLoadBy(const Player *) const	{ return true; }	// anyone can load from a packet
	// while packets can technically be 'in orbit' it has no effect, so just claim we're never in orbit
	virtual Planet * InOrbit()				{ return NULL; }
	virtual const Planet * InOrbit() const	{ return NULL; }

	long GetSpeed() const		{ return mSpeed; }
	long GetDriverSpeed() const	{ return mDriverSpeed; }
	Planet * GetDestination()	{ return mDestination; }
	bool Move(bool FirstYear);
	 ~Packet();

private:
	void Collide();
	void Decay(long distance); //distance traveled this turn
	long mSpeed;
	long mDriverSpeed;
	long mMaxSize;
	Planet * mDestination;
	bool mFirstYear;
//	deque<long> mTargeted;	// who has targeted this packet, and with how much cargo capacity, used for Patrol vs packets
};
}
#endif // !defined(FreeStars_Packet_h)
