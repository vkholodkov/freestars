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
 **@file ProdOrder.h
 **@ingroup Server
 **@brief Production Orders.
 **/

#if !defined(FreeStars_ProdOrder_h)
#define FreeStars_ProdOrder_h

#include "FSTypes.h"
#include "Rules.h"
#include "Cost.h"
class TiXmlNode;

namespace FreeStars {
class Player;
class Planet;

const long POP_FACTS		= 1001;
const long POP_MINES		= 1002;
const long POP_DEFS			= 1003;
const long POP_ALCHEMY		= 1004;
const long POP_SCANNER		= 1005;
const long POP_MINTERRA		= 1006;
const long POP_MAXTERRA		= 1007;
const long POP_TERRAFORM	= 1008;
const long POP_MIXEDPACKET	= 2000;	// Leave space after packets blank for mineral type

/**
 * Production order.
 * \ingroup Server
 */
class ProdOrder {
public:
	virtual ~ProdOrder();
	static deque<ProdOrder *> ParseNode(const TiXmlNode * node, Planet * planet, Player * player = NULL, bool TrustPartials = false);
	static TiXmlNode * WriteNode(TiXmlNode * node, const deque<ProdOrder *> & ords);
	virtual TiXmlNode * WriteNode(TiXmlNode * node) const = 0;

	virtual bool Produce(Planet * planet, long * resources, bool * AutoAlchemy) = 0;
	virtual void Built(Planet * planet, long number) = 0;
	ProdOrder * Copy() const;
	Cost& GetPartial() {return Partial;}
	virtual string TypeToString() const = 0;
    virtual string AmountToString() const;
	long GetType() const	{ return Type; }

protected:
	ProdOrder();
	ProdOrder(long type, long amount);
	ProdOrder(const ProdOrder & orig);
	Cost Partial;
	long Type;
	long Amount;
	bool DoProduce(const Cost & cost, Planet * planet, long * resources, bool * AutoAlchemy, long maxbuild = -1);

private:
	void CheckPartials(Planet * planet, const TiXmlNode * node, bool TrustPartials);
	void BuildPartial(const Cost & cost, Planet * planet, long * resources, double Build);
	void init();
};

/**
 * Production Order,Base (?).
 * \ingroup Server
 */
class POBase : public ProdOrder {
public:
	POBase(long ship) : ProdOrder(ship, 1) {}
	POBase(const POBase & orig) : ProdOrder(orig.Type, orig.Amount) {}
	~POBase();
	virtual TiXmlNode * WriteNode(TiXmlNode * node) const;

	virtual bool Produce(Planet * planet, long * resources, bool * AutoAlchemy);
	virtual void Built(Planet * planet, long number);
	virtual string TypeToString() const;
};
/**
 * Production Order, Planetary (?).
 * \ingroup Server
 */
class POPlanetary : public ProdOrder {
public:
	POPlanetary(const POPlanetary & orig) : ProdOrder(orig.Type, orig.Amount) {}
	POPlanetary(long type, long num) : ProdOrder(type, num) {}
	~POPlanetary();
	virtual TiXmlNode * WriteNode(TiXmlNode * node) const;

	virtual bool Produce(Planet * planet, long * resources, bool * AutoAlchemy);
	virtual void Built(Planet * planet, long number);
	virtual string TypeToString() const;
};

/**
 * Automatic planetary production order (?).
 * \ingroup Server
 */
class POAuto : public POPlanetary {
public:
	POAuto(long type, long num) : POPlanetary(type, num) {}
	POAuto(const POAuto & orig) : POPlanetary(orig.Type, orig.Amount) {}
	~POAuto();
	virtual TiXmlNode * WriteNode(TiXmlNode * node) const;

	virtual bool Produce(Planet * planet, long * resources, bool * AutoAlchemy);
	virtual string TypeToString() const;
    virtual string AmountToString() const;
};

/**
 * Packet production order (?).
 * \ingroup Server
 */
class POPacket : public POPlanetary {
public:
	POPacket(long type, long num) : POPlanetary(type + POP_MIXEDPACKET + 1, num) {}
	POPacket(const POPacket & orig) : POPlanetary(orig.Type, orig.Amount) {}
	~POPacket();
	virtual TiXmlNode * WriteNode(TiXmlNode * node) const;
	static bool CheckPacket(Planet * planet);

	virtual bool Produce(Planet * planet, long * resources, bool * AutoAlchemy);
	virtual string TypeToString() const;
};

/**
 * Ship production order (?).
 * \ingroup Server
 */
class POShip : public ProdOrder {
public:
	POShip(long ship, long num) : ProdOrder(ship, num) {}
	POShip(const POShip & orig) : ProdOrder(orig.Type, orig.Amount) {}
	~POShip();
	virtual TiXmlNode * WriteNode(TiXmlNode * node) const;

	virtual bool Produce(Planet * planet, long * resources, bool * AutoAlchemy);
	virtual void Built(Planet * planet, long number);
	virtual string TypeToString() const;
};

/**
 * Terraform production order (?).
 * \ingroup Server
 */
class POTerraform : public ProdOrder {
public:
	POTerraform(long type, long num) : ProdOrder(type, num), mResCost(0) { assert(type == POP_MINTERRA || type == POP_MAXTERRA || type == POP_TERRAFORM); }
	POTerraform(const POTerraform & orig) : ProdOrder(orig.Type, orig.Amount), mResCost(orig.mResCost) {}
	virtual ~POTerraform();
	virtual TiXmlNode * WriteNode(TiXmlNode * node) const;

	virtual bool Produce(Planet * planet, long * resources, bool * AutoAlchemy);
	virtual void Built(Planet * planet, long number);
	virtual string TypeToString() const;
    virtual string AmountToString() const;

private:
	long mResCost;
};
}
#endif // !defined(FreeStars_ProdOrder_h)
