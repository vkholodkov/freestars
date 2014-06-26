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
 **@file Order.h
 **@ingroup Server
 **@brief Orders.
 **/

#if !defined(FreeStars_Order_h)
#define FreeStars_Order_h

namespace FreeStars {

/**
 * A single order.
 * @ingroup Server
 */	
class Order {
public:
	virtual ~Order();
	bool Undo();
	bool Redo();
	bool IsUndone() const	{ return mUndone; }
	virtual TiXmlNode * WriteNode(TiXmlNode * node) const = 0;
	virtual bool DoUndo() = 0;

	/*
	 * Returns true if if new order replaces current one (e.g. a new order for the same fleet)
	 */
	virtual bool Replaces(const Order*) = 0;
	bool IsReplaced() const { return mReplaced; }
	void SetReplaced() { mReplaced = true; }

protected:
	Order() : mUndone(false), mReplaced(false) {}
	bool mUndone;
	bool mReplaced;
};

/**
 * Templated order.
 * \ingroup Server
 */
template <class Type> class TypedOrder : public Order {
public:
	typedef const string (*StringFunc)(Type);
	// Order with conversion to string for writing to xml file
	TypedOrder(Type * value, const char * name, StringFunc func, const char * additionalLabel = NULL, const char * additionalString = NULL)
		: Order(), m_pValue(value), mWFunc(NULL), mSFunc(func), mPrior(*value), mName(name), mAddLabel(additionalLabel ? additionalLabel : ""), mAddStr(additionalString ? additionalString : "") {}

	// Order with generic function when writing to xml file
	typedef TiXmlElement * (*WriteFunc)(TiXmlNode *, const char *, Type);
	TypedOrder(Type * value, WriteFunc func, const char * name, const char * additionalLabel = NULL, const char * additionalString = NULL)
		: Order(), m_pValue(value), mWFunc(func), mSFunc(NULL), mPrior(*value), mName(name), mAddLabel(additionalLabel ? additionalLabel : ""), mAddStr(additionalString ? additionalString : "") {}

	virtual bool DoUndo()	{ Type t = mPrior; mPrior = *m_pValue; *m_pValue = t; return true; }
	virtual TiXmlNode * WriteNode(TiXmlNode * node) const {
		if (!mUndone) {
			if (!mAddLabel.empty()) {
				string s = "Set";
				s += mName;
				TiXmlElement * child = new TiXmlElement(s.c_str());
				AddString(child, mAddLabel.c_str(), mAddStr.c_str());
				if (mWFunc != NULL)
					mWFunc(child, mName, *m_pValue);
				else
					AddString(child, mName, mSFunc(*m_pValue).c_str());
				node->LinkEndChild(child);
				return child;
			} else {
				if (mWFunc != NULL)
					return mWFunc(node, mName, *m_pValue);
				else
					return AddString(node, mName, mSFunc(*m_pValue).c_str());
			}
		} else
			return NULL;
	}
	virtual bool Replaces(const Order*) { return false; }

protected:
	Type * m_pValue;
	Type mPrior;
	WriteFunc mWFunc;
	StringFunc mSFunc;
	const char * mName;
	string mAddLabel;
	string mAddStr;
};

/**
 * A templated list order.
 * @ingroup Server
 */
template <class Type> class TypedListOrder : public Order {
public:
	TypedListOrder(deque<Type *> * value, const char * name)
		: Order(), m_pValue(value), mPrior(*value), mName(name) {}
	virtual ~TypedListOrder() { for (int i = 0; i < mPrior.size(); ++i) delete mPrior[i]; }

	virtual bool DoUndo()	{ deque<Type *> t = mPrior; mPrior = *m_pValue; *m_pValue = t; return true; }
	virtual TiXmlNode * CallWriteNode(const Type * obj, TiXmlNode * node) const	{ return obj->WriteNode(node); }
	const Type * GetItemNumber(int i) const { return m_pValue->at(i); }
	virtual TiXmlNode * WriteNode(TiXmlNode * node) const {
		if (!mUndone) {
			if (mName == NULL) {
				for (int i = 0; i < m_pValue->size(); ++i)
					CallWriteNode(m_pValue->at(i), node);

				return node;
			} else {
				TiXmlElement * txe = new TiXmlElement(mName);
				for (int i = 0; i < m_pValue->size(); ++i)
					CallWriteNode(m_pValue->at(i), txe);

				node->LinkEndChild(txe);
				return txe;
			}
		}
		return NULL;
	}

private:
	deque<Type *> * m_pValue;
	deque<Type *> mPrior;
	const char * mName;
};

/**
 * A waypoint order.
 * @ingroup Server
 */
class WaypointOrder : public TypedListOrder<WayOrder> {
public:
	WaypointOrder(long f, deque<WayOrder *> * ords)
		: TypedListOrder<WayOrder>(ords, "Waypoints"), mFleet(f) {}

	virtual TiXmlNode * WriteNode(TiXmlNode * node) const;
	virtual bool Replaces(const Order *_new) {
		const WaypointOrder *o = dynamic_cast<const WaypointOrder*>(_new);
		return o != NULL && this->mFleet == o->mFleet;
	}

private:
	long mFleet;
};

/**
 * Battle plan order.
 * @ingroup Server
 */
class BattlePlanOrder : public Order {
public:
	BattlePlanOrder(BattlePlan * bp, int number, Player * p)
		: Order(), mBP(bp), mNum(number), mPlayer(p) {}
	virtual ~BattlePlanOrder();

	virtual bool DoUndo();
	virtual bool Replaces(const Order*) { return false;  }
	virtual TiXmlNode * WriteNode(TiXmlNode * node) const;

	BattlePlan * GetBattlePlan(BattlePlan * old)
		{ BattlePlan * t = mBP; mBP = old; return t; }
	int GetNumber()	{ return mNum; }

private:
	BattlePlan * mBP;
	int mNum;
	Player * mPlayer;
};

/**
 * Relations order.
 * @ingroup Server
 */
class RelationsOrder : public Order {
public:
	RelationsOrder(deque<long> * rel)
		: Order(), m_pValue(rel), mPrior(*rel) {}

	virtual bool DoUndo()	{ deque<long> t = mPrior; mPrior = *m_pValue; *m_pValue = t; return true; }
	virtual TiXmlNode * WriteNode(TiXmlNode * node) const;
	virtual bool Replaces(const Order*) { return false;  }

private:
	deque<long> mPrior;
	deque<long> * m_pValue;
};

/**
 * Production order.
 * @ingroup Server
 */
class ProductionOrder : public TypedListOrder<ProdOrder> {
public:
	ProductionOrder(const string &name, deque<ProdOrder *> * ords)
		: TypedListOrder<ProdOrder>(ords, "ProductionQueue"), mQName(name) {}

	virtual TiXmlNode * WriteNode(TiXmlNode * node) const;
	virtual bool Replaces(const Order *_new) {
		const ProductionOrder *o = dynamic_cast<const ProductionOrder*>(_new);
		return o != NULL && this->mQName == o->mQName;
	}

private:
	string mQName;
};

/**
 * Multiple order.
 * @ingroup Server
 */
class MultipleOrder : public Order {
public:
	MultipleOrder() : Order() {}
	virtual ~MultipleOrder();
	virtual TiXmlNode * WriteNode(TiXmlNode * node) const;
	virtual bool DoUndo();
	virtual bool Replaces(const Order*) { return false;  }

	void AddOrder(Order * o);

private:
	deque<Order *> mOrds;
};

/**
 * Transport order.
 * @ingroup Server
 */
class TransportOrder : public Order {
public:
	TransportOrder(Player * p, CargoHolder * owned, CargoHolder * other, long pop, long fuel, deque<long> & cargo)
		: Order(), mPlayer(p), mOwned(owned), mOther(other), mPop(pop), mFuel(fuel), mCargo(cargo) {}

	virtual bool DoUndo();
	virtual TiXmlNode * WriteNode(TiXmlNode * node) const;
	virtual bool Replaces(const Order *_new) {
		const TransportOrder *o = dynamic_cast<const TransportOrder*>(_new);
		return o != NULL && this->mPlayer == o->mPlayer
			&& this->mOwned == o->mOwned && this->mOther == o->mOther;
	}

private:
	Player * mPlayer;
	CargoHolder * mOwned;
	CargoHolder * mOther;
	long mPop;
	long mFuel;
	deque<long> mCargo;
};
}
#endif // !defined(FreeStars_Order_h)
