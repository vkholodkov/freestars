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

#include "TempFleet.h"
#include "Order.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

Order::~Order()
{
}

bool Order::Undo()
{
	if (mUndone)
		return false;

	mUndone = true;
	return DoUndo();
}

bool Order::Redo()
{
	if (!mUndone)
		return false;

	mUndone = false;
	return DoUndo();
}

TiXmlNode * WaypointOrder::WriteNode(TiXmlNode * node) const
{
	TiXmlNode * wo = TypedListOrder<WayOrder>::WriteNode(node);
	if (wo != NULL)
		AddLong(wo, "Fleet", mFleet);

	return wo;
}

BattlePlanOrder::~BattlePlanOrder()
{
	delete mBP;
}

bool BattlePlanOrder::DoUndo()
{
	mPlayer->UndoBattlePlan(this);
	return true;
}

TiXmlNode * BattlePlanOrder::WriteNode(TiXmlNode * node) const
{
	if (!mUndone)
		return mPlayer->WriteBattlePlan(node, mNum);
	else
		return NULL;
}

TiXmlNode * RelationsOrder::WriteNode(TiXmlNode * node) const
{
	if (!mUndone)
		return node->LinkEndChild(Rules::WriteArray("Relations", "Race", "IDNumber", *m_pValue));
	else
		return NULL;
}

TiXmlNode * ProductionOrder::WriteNode(TiXmlNode * node) const
{
	TiXmlNode * wo = TypedListOrder<ProdOrder>::WriteNode(node);
	if (wo != NULL)
		AddString(wo, "Planet", mQName);

	return wo;
}

MultipleOrder::~MultipleOrder()
{
	for (int i = 0; i < mOrds.size(); ++i)
		delete mOrds[i];
}

TiXmlNode * MultipleOrder::WriteNode(TiXmlNode * node) const
{
	node->LinkEndChild(new TiXmlElement("StartMultipleOrder"));
//	AddString(node, "StartMultipleOrder", "");
	for (int i = 0; i < mOrds.size(); ++i)
		mOrds[i]->WriteNode(node);

	node->LinkEndChild(new TiXmlElement("EndMultipleOrder"));
//	AddString(node, "EndMultipleOrder", "");
	return node;
}

bool MultipleOrder::DoUndo()
{
	bool Result = true;
	for (int i = 0; Result && i < mOrds.size(); ++i)
		Result = mOrds[i]->DoUndo();

	return Result;
}

void MultipleOrder::AddOrder(Order * o)
{
	mOrds.push_back(o);
}

bool TransportOrder::DoUndo()
{
	mPop = -mPop;
	mFuel = -mFuel;
	for (int i = 0; i < Rules::MaxMinType; ++i)
		mCargo[i] = -mCargo[i];

	mPlayer->TransferCargo(mOwned, mOther, mPop, mFuel, mCargo, false);

	return true;
}

TiXmlNode * TransportOrder::WriteNode(TiXmlNode * node) const
{
	TiXmlElement * trans = new TiXmlElement("Transfer");
	TiXmlElement * owned = new TiXmlElement("Owned");
	mOwned->WriteTransport(owned);
	trans->LinkEndChild(owned);

	CargoHolder * ch;
	TempFleet * tf = dynamic_cast<TempFleet *>(mOther);
	if (tf == NULL)
		ch = mOther;
	else
		ch = tf->GetRealCH();

	TiXmlElement * other = new TiXmlElement("Other");
	if (ch == NULL)
		other->LinkEndChild(new TiXmlElement("Space"));
	else
		ch->WriteTransport(other);
	trans->LinkEndChild(other);

	TiXmlElement * cargo = Rules::WriteCargo(trans, "Cargo", mCargo, mPop);
	if (mFuel != 0)
		AddLong(cargo, "Fuel", mFuel);

	node->LinkEndChild(trans);

	return trans;
}

}
