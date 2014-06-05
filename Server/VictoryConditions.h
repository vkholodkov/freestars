/*
Copyright (C) 2014 Valery Kholodkov

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
*/

/**
 **@file VictoryConditions.h
 **@ingroup Server
 **@brief Victory conditions representation and management.
 **/

#if !defined(FreeStars_VictoryConditions_h)
#define FreeStars_VictoryConditions_h

#include <deque>

#include "FSServer.h"

namespace FreeStars {

class VictoryConditions {
public:
    VictoryConditions();

    bool Parse(const TiXmlNode*);
    void WriteNode(TiXmlNode*) const;

	long Worlds;
	long TechLevel;
	long TechCount;
	long Score;
	double Times2nd;
	long Resources;
	long CapShips;
	long HighScoreAt;
	long Count;
	long Start;
};

};

#endif // !defined(FreeStars_VictoryConditions_h)
