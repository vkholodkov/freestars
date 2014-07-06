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

#include "FSServer.h"

#include "Cost.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {
Cost::Cost()
{
	Zero();
}

Cost::Cost(const Cost & c) :
	mResources(c.mResources),
	mCrew(c.mCrew)
{
	mMinerals.insert(mMinerals.begin(), c.mMinerals.begin(), c.mMinerals.end());
}

bool Cost::IsZero()
{
	bool Result = mResources == 0 && mCrew == 0;
	for (CargoType ct = 0; ct < Rules::MaxMinType; ++ct)
		Result = Result && mMinerals[ct] == 0;
	return Result;
}

void Cost::Zero()
{
	mResources = 0;
	mCrew = 0;
	mMinerals.clear();
	mMinerals.insert(mMinerals.begin(), Rules::MaxMinType, 0);
}

Cost & Cost::operator =(const Cost & c)
{
	mResources = c.mResources;
	mCrew = c.mCrew;
	mMinerals.clear();
	mMinerals.insert(mMinerals.begin(), c.mMinerals.begin(), c.mMinerals.end());

	return *this;
}

Cost & Cost::operator *= (double factor)
{
	mResources = long(mResources * factor);
	mCrew = long(mCrew * factor);
	for (CargoType ct = 0; ct < Rules::MaxMinType; ++ct)
		mMinerals[ct] = long(mMinerals[ct] * factor);

	return *this;
}

Cost & Cost::operator += (const Cost & c)
{
	mResources += c.mResources;
	mCrew += c.mCrew;
	for (CargoType ct = 0; ct < Rules::MaxMinType; ++ct)
		mMinerals[ct] += c.mMinerals[ct];

	return *this;
}

Cost & Cost::operator -= (const Cost & c)
{
	mResources -= c.mResources;
	mCrew -= c.mCrew;
	for (CargoType ct = 0; ct < Rules::MaxMinType; ++ct)
		mMinerals[ct] -= c.mMinerals[ct];

	return *this;
}

long & Cost::operator [](CargoType ct)
{
	if (ct == RESOURCES)
		return mResources;
	else if (ct == POPULATION)
		return mCrew;
	else if (ct >= 0 && ct < Rules::MaxMinType)
		return mMinerals[ct];
	else {
		assert(false);
		return mMinerals[ct];
	}
}

long Cost::operator [](CargoType ct) const
{
	if (ct == RESOURCES)
		return mResources;
	else if (ct == POPULATION)
		return mCrew;
	else if (ct >= 0 && ct < Rules::MaxMinType)
		return mMinerals[ct];
	else {
		assert(false);
		return 0;
	}
}

bool operator == (const Cost & b, const Cost & c)
{
	return c.mResources == b.mResources && c.mCrew == b.mCrew && c.mMinerals == b.mMinerals;
}

/*
long Cost::GetCost(CargoType ct) const
{
	if (ct == RESOURCES)
		return mResources;
	else if (ct == POPULATION)
		return mCrew;
	else if (ct >= 0 && ct < Rules::MaxMinType)
		return mMinerals[ct];
	else {
		assert(false);
		return 0;
	}
}

void Cost::SetCost(CargoType ct, long c)
{
	if (ct == RESOURCES)
		mResources = c;
	else if (ct == POPULATION)
		mCrew = c;
	else if (ct >= 0 && ct < Rules::MaxMinType)
		mMinerals[ct] = c;
	else
		assert(false);
}
*/

void Cost::ReadCosts(const TiXmlNode * node, MessageSink &messageSink)
{
    ArrayParser arrayParser(messageSink);

	if (node == NULL) {
		mResources = 0;
		mCrew = 0;
		deque<long>::iterator li;
		for (li = mMinerals.begin(); li != mMinerals.end(); ++li)
			*li = 0;
	} else {
		mResources = GetLong(node->FirstChild("Resources"));
		mCrew = GetLong(node->FirstChild("Crew"));
		Rules::ParseArray(node, mMinerals, MINERALS, messageSink);
	}
}

TiXmlNode * Cost::WriteCosts(TiXmlNode * node, const char * name) const
{
	deque<long>::const_iterator li;

	li = max_element(mMinerals.begin(), mMinerals.end());
	TiXmlElement * child = NULL;
	if (*li > 0 || mResources > 0 || mCrew > 0) {
		if (*li > 0)
			child = Rules::WriteArray(name, mMinerals, MINERALS);
		else
			child = new TiXmlElement(name);

		if (mResources > 0)
			AddLong(child, "Resources", mResources);
		if (mCrew > 0)
			AddLong(child, "Crew", mCrew);

		node->LinkEndChild(child);
	}

	return child;
}

}
