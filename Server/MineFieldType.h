/*
Copyright 2003 - 2005 Elliott Kleinrock

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
 **@file MineFieldType.h
 **@ingroup Server
 **@brief Mine behavior.
 **/

#if !defined(FreeStars_MineFieldType_h)
#define FreeStars_MineFieldType_h

namespace FreeStars {

/**
 * A mine field type.
 *@ingroup Server
 */
class MineFieldType
{
public:
	MineFieldType() : mName(""), mSafeSpeed(0), mHitOdds(0.0), mSweepDivisor(1), mFleetDamage(0), mRamFleetDamage(0), mShipDamage(0), mRamShipDamage(0) {}
	~MineFieldType() {}

	bool ParseNode(const TiXmlNode * node);

	const string & GetName() const	{ return mName; }
	long GetSafeSpeed() const		{ return mSafeSpeed; }
	double GetHitOdds() const		{ return mHitOdds; }
	double GetHitOdds(int speed) const	{ return (speed <= mSafeSpeed) ? 0.0 : (speed - mSafeSpeed) * mHitOdds; }
	long GetSweepDivisor() const	{ return mSweepDivisor; }
	long GetFleetDamage() const		{ return mFleetDamage; }
	long GetRamFleetDamage() const	{ return mRamFleetDamage; }
	long GetShipDamage() const		{ return mShipDamage; }
	long GetRamShipDamage() const	{ return mRamShipDamage; }
	bool CanDetonate() const		{ return mCanDetonate; }
	int	GetID() const				{ return mID; }
	void SetID(int id)				{ mID = id; }

private:
	string mName;
	long mSafeSpeed;
	double mHitOdds;
	long mSweepDivisor;
	long mFleetDamage;
	long mRamFleetDamage;
	long mShipDamage;
	long mRamShipDamage;
	bool mCanDetonate;
	int mID;
};
}
#endif //#if !defined(FreeStars_MineFieldType_h)
