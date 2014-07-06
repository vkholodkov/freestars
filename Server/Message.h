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
 **@file Message.h
 **@ingroup Server
 **@brief Message Communications
 **/

#if !defined(FreeStars_Message_h)
#define FreeStars_Message_h

#include <string>
#include "Galaxy.h"

using namespace std;

namespace FreeStars {
class Location;
class Player;

/**
 * A single message.
 * @ingroup Server
 */
class MessItem {
public:
	virtual ~MessItem();
	virtual void WriteNode(TiXmlNode * node) const = 0;
	virtual string ToString() const = 0;
	void AddDesc(TiXmlElement * node) const;
	static MessItem * ParseNode(const TiXmlNode * node, Game*);

	string mDesc;

protected:
	MessItem(const char * d) : mDesc(d) {}
};

/**
 * All messages at a location.
 * @ingroup Server
 */
class MessLoc : public MessItem {
public:
	MessLoc(const char * d, const Location * loc, bool dwd = false) : MessItem(d), mLoc(loc), mDWD(dwd) {}
	virtual ~MessLoc();
	virtual void WriteNode(TiXmlNode * node) const;
	virtual string ToString() const;
	static MessLoc * ParseNode(const TiXmlNode * node, Game*);
	bool StoreMessageLocation(const Location * loc);

	const Location * mLoc;

private:
	bool mDWD;	// Delete When Done
};

/**
 * All messages for a player.
 * @ingroup Server
 */
class MessPlayer : public MessItem {
public:
	MessPlayer(const char * d, const Player * player) : MessItem(d)	{ mPlayer = player; }
	virtual ~MessPlayer();
	virtual void WriteNode(TiXmlNode * node) const;
	virtual string ToString() const;
	static MessPlayer * ParseNode(const TiXmlNode * node, Game*);

	const Player * mPlayer;
};

/**
 * A message containing a long.
 * @ingroup Server
 */
class MessNumber : public MessItem {
public:
	MessNumber(const char * d, long number) : MessItem(d)			{ mNumber = number; }
	virtual ~MessNumber();
	virtual void WriteNode(TiXmlNode * node) const;
	virtual string ToString() const;
	static MessNumber * ParseNode(const TiXmlNode * node);

	long mNumber;
};

/**
 * A message containing a double.
 * @ingroup Server
 */
class MessFloat : public MessItem {
public:
	MessFloat(const char * d, double number) : MessItem(d)			{ mNumber = number; }
	virtual ~MessFloat();
	virtual void WriteNode(TiXmlNode * node) const;
	virtual string ToString() const;
	static MessFloat * ParseNode(const TiXmlNode * node);

	double mNumber;
};

/**
 * A message containing a string.
 * @ingroup Server
 */ 
class MessString : public MessItem {
public:
	MessString(const char * d, string str) : MessItem(d)			{ mStr = str; }
	virtual ~MessString();
	virtual void WriteNode(TiXmlNode * node) const;
	virtual string ToString() const;
	static MessString * ParseNode(const TiXmlNode * node);

	string mStr;
};

/**
 * A message containing XML for the rest of the message
 * @ingroup Server
 */ 
class MessXMLNode : public MessItem {
public:
	MessXMLNode(const TiXmlNode * node) : MessItem("")			{ mNode = node; }
	virtual ~MessXMLNode();
	virtual void WriteNode(TiXmlNode * node) const;
	virtual string ToString() const;
	static MessXMLNode * ParseNode(const TiXmlNode * node);

	const TiXmlNode * mNode;
};

class MessageVisitor {
public:
    virtual ~MessageVisitor() {};

    virtual void VisitMessage(const string &) const = 0;
    virtual void VisitLocation(const string&, const Location*) const = 0;
    virtual void VisitPlayer(const string&, const Player*) const = 0;
    virtual void VisitNumber(const string&, long) const = 0;
    virtual void VisitFloat(const string&, double) const = 0;
    virtual void VisitString(const string&, const string&) const = 0;
    virtual void VisitXMLNode(const string&, const TiXmlNode*) const = 0;
};

/**
 * Message.
 * @ingroup Server
 */
class Message {
public:
	Message(string type, const Location * loc) : mType(type) { AddItem("", loc); }
	Message(string type) : mType(type) {}
	Message() {}
	~Message();
	void AddItem(const char * d, const Location * loc, bool dwd = false)	{ AddItem(new MessLoc(d, loc, dwd)); }
	void AddItem(const char * d, const Player * player)	{ AddItem(new MessPlayer(d, player)); }
	void AddItem(const char * d, string str)			{ AddItem(new MessString(d, str)); }
	void AddLong(const char * d, long number)			{ AddItem(new MessNumber(d, number)); }
	void AddFloat(const char * d, double number)		{ AddItem(new MessFloat(d, number)); }
	void AddItem(const TiXmlNode * node)				{ AddItem(new MessXMLNode(node)); }
	void StoreMessageLocation(const Location * loc, const Player * player);

	bool IsType(const char * t)			{ return mType.find(t) >= 0; }
	void WriteNode(TiXmlNode * node) const;
	bool ParseNode(const TiXmlNode * node, Game*);
	string ToString() const;

    void ApplyVisitor(MessageVisitor&) const;

private:
	string mType;
	void AddItem(MessItem * mi);
	deque<MessItem *> mItems;
};
}
#endif // !defined(FreeStars_Message_h)
