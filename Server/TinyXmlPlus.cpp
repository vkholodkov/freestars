/*
Copyright 2003 - 2005,2006 Elliott Kleinrock, Dan Neely, Kurt W. Over, Damon Domjan,
Brandon Bergren

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

#if !defined(TIXML_USE_STL)
#define TIXML_USE_STL
#endif
#include "tinyxml.h"

#ifdef WIN32
// Kill STL warnings
#pragma warning(disable : 4100)		// unreferenced formal parameter
#pragma warning(disable : 4511)		// copy constructor could not be generated
#pragma warning(disable : 4512)		// assignment operator could not be generated
// disabling these warnings doesn't seem to work...
#pragma warning(disable : 4663)		// C++ language change: to explicitly specialize class template use the following syntax:
#pragma warning(disable : 4018)		// signed/unsigned mismatch
#pragma warning(disable : 4245)		// 'initializing' : conversion signed/unsigned mismatch
#endif

#include <stdlib.h>
#include "TinyXmlPlus.h"

#ifndef WIN32
// For compiling on linux
#define stricmp strcasecmp
#endif

const char * GetString(const TiXmlNode * node)
{
	const TiXmlNode * node2;
	const TiXmlText * text;

	if (!node)
		return NULL;

	node2 = node->FirstChild();
	while (node2 && node2->Type() == TiXmlNode::COMMENT)
		node2 = node2->NextSibling();

	if (!node2)
		return NULL;

	text = node2->ToText();
	if (!text)
		return NULL;

	return text->Value();
}

const char * GetString(const TiXmlNode * node, const char * name)
{
	if(!node)
		return NULL;
	const TiXmlNode * obj = node->FirstChild(name);
	if(obj) obj = obj->FirstChild();
	if(obj && obj->Type() == TiXmlNode::TEXT)
		return obj->Value();
	else
		return NULL;
}

long GetLong(const TiXmlNode * node, long def)
{
	const char * ptr = GetString(node);
	if (ptr == NULL)
		return def;
	else
		return atol(ptr);
}

double GetDouble(const TiXmlNode * node, double def)
{
	const char * ptr = GetString(node);
	if (ptr == NULL)
		return def;
	else
		return atof(ptr);
}

bool GetBool(const TiXmlNode * node)
{
	const char * ptr = GetString(node);
	if (ptr && stricmp(ptr, "true") == 0)
		return true;
	else
		return false;
}

TiXmlElement * AddString(TiXmlNode * node, const char * name, const char * str)
{
	TiXmlElement * txe = new TiXmlElement(name);
	TiXmlText txt(str);
	txe->InsertEndChild(txt);
	node->LinkEndChild(txe);
	return txe;
}

TiXmlElement * AddLong(TiXmlNode * node, const char * name, long num)
{
	return AddString(node, name, Long2String(num).c_str());
}

TiXmlElement * AddDouble(TiXmlNode * node, const char * name, double num)
{
	return AddString(node, name, Float2String(num).c_str());
}

TiXmlElement * AddBool(TiXmlNode * node, const char * name, bool val)
{
	return AddString(node, name, val ? "true" : "false");
}

bool NodeIs(const TiXmlNode * node, const char * id)
{
	/* Case insensitive version */
	//return (stricmp(node->Value(),id) == 0);
	/* Case sensitive version */
	return (strcmp(node->Value(),id) == 0);
}

bool NodeIsOneOf(const TiXmlNode * node, const char * ids[])
{
	if(!node)
		return false;
	int index = 0;
	for(index=0;ids[index];index++) {
		if(NodeIs(node, ids[index]))
			return true;
	}
	return false;
}

TiXmlNode * FindNextNotOf(TiXmlNode * node, const char * ids[])
{
	if(!node)
		return NULL;
	TiXmlNode * loc;
	int index;
		return false;
	for(loc = node->NextSibling(); loc; loc = loc->NextSibling()) {
		if (loc->Type() == TiXmlNode::COMMENT)
			continue;
		index = 0;
		indexcheck:
		if(NodeIs(loc,ids[index]))
			continue;
		if(ids[++index]) goto indexcheck;
		return loc;
	}
	return NULL;
}


