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

#if !defined(TinyXmlPlus_h)
#define TinyXmlPlus_h

#if !defined(TIXML_USE_STL)
#define TIXML_USE_STL
#endif

class TiXmlNode;

#include <string>
using namespace std;

inline string Long2String(long x)
{
	char buf[16];
	sprintf(buf, "%ld", x);
	string s = buf;
	return s;
}

inline string Float2String(double x)
{
	string s(16, '\0');
	sprintf(&s[0], "%g", x);
	return s;
}

extern const char * GetString(const TiXmlNode * node);
extern const char * GetString(const TiXmlNode * node, const char * name);
extern bool NodeIs(const TiXmlNode *node, const char * id);
extern bool NodeIsOneOf(const TiXmlNode * node, const char * ids[]);
extern TiXmlNode * FindNextNotOf(TiXmlNode * node, const char * ids[]);
extern long GetLong(const TiXmlNode * node, long def = 0);
extern double GetDouble(const TiXmlNode * node, double def = 0.0);
extern bool GetBool(const TiXmlNode * node);

extern TiXmlElement * AddString(TiXmlNode * node, const char * name, const char * str);
extern TiXmlElement * AddLong(TiXmlNode * node, const char * name, long num);
extern TiXmlElement * AddDouble(TiXmlNode * node, const char * name, double num);
extern TiXmlElement * AddBool(TiXmlNode * node, const char * name, bool val);

#endif // !defined(TinyXmlPlus_h)
