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

#include "FSServer.h"

#include "MineFieldType.h"

/*
  <MineFieldType Name="Standard">
    <SafeSpeed>4</SafeSpeed>
    <HitOdds>.003</HitOdds>
    <SweepDivisor>1</SweepDivisor>
    <FleetDamage>500</FleetDamage>
    <RamFleetDamage>600</RamFleetDamage>
    <ShipDamage>100</ShipDamage>
    <RamShipDamage>125</RamShipDamage>
  </MineFieldType>
*/

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

bool MineFieldType::ParseNode(const TiXmlNode * node)
{
	const TiXmlElement * txe = node->ToElement();
	if (txe == NULL)
		return false;

	mName = txe->Attribute("Name");
	mSafeSpeed = GetLong(node->FirstChild("SafeSpeed"));
	mHitOdds = GetDouble(node->FirstChild("HitOdds"));
	mSweepDivisor = GetLong(node->FirstChild("SweepDivisor"));
	mFleetDamage = GetLong(node->FirstChild("FleetDamage"));
	mRamFleetDamage = GetLong(node->FirstChild("RamFleetDamage"));
	mShipDamage = GetLong(node->FirstChild("ShipDamage"));
	mRamShipDamage = GetLong(node->FirstChild("RamShipDamage"));
	mCanDetonate = GetBool(node->FirstChild("CanDetonate"));

	if (mSafeSpeed <= 1 || mHitOdds < epsilon || mHitOdds > 1.0)
		return false;
	else
		return true;
}

}
