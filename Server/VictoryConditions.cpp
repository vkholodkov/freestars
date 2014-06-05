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

#include "FSServer.h"

#include "VictoryConditions.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

VictoryConditions::VictoryConditions()
{
	Worlds = 0;
	TechLevel = 0;
	TechCount = 0;
	Score = 0;
	Times2nd = 0.0;
	Resources = 0;
	CapShips = 0;
	HighScoreAt = 0;
	Count = 0;
	Start = 0;
}

bool VictoryConditions::Parse(const TiXmlNode *child1)
{
    const TiXmlNode *child2;

    Count = GetLong(child1->FirstChild("WinnerConditions"));
    Start = GetLong(child1->FirstChild("MinimumYears"));
    Worlds = GetLong(child1->FirstChild("ControledWorlds"));
    child2 = child1->FirstChild("TechLevels");
    if(child2) {
        TechLevel = GetLong(child2->FirstChild("Level"));
        TechCount = GetLong(child2->FirstChild("Number"));
    }
    Score = GetLong(child1->FirstChild("Score"));
    Times2nd = GetDouble(child1->FirstChild("OverSecond"));
    Resources = GetLong(child1->FirstChild("Resources"));
    CapShips = GetLong(child1->FirstChild("CapShips"));
    HighScoreAt = GetLong(child1->FirstChild("HighestScoreAt"));
}

void VictoryConditions::WriteNode(TiXmlNode *cre) const
{
	TiXmlElement * vc = new TiXmlElement("VictoryConditions");
	AddLong(vc, "WinnerConditions", Count);
	AddLong(vc, "MinimumYears", Start);
	if (Worlds > 0)
		AddLong(vc, "ControledWorlds", Worlds);
	if (TechLevel > 0) {
		TiXmlElement * tl = new TiXmlElement("TechLevels");
		AddLong(tl, "Level", TechLevel);
		AddLong(tl, "Number", TechCount);
		vc->LinkEndChild(tl);
	}
	if (Score > 0)
		AddLong(vc, "Score", Score);
	if (Times2nd > 0.0)
		AddDouble(vc, "OverSecond", Times2nd);
	if (Resources > 0)
		AddLong(vc, "Resources", Resources);
	if (CapShips > 0)
		AddLong(vc, "CapShips", CapShips);
	if (HighScoreAt > 0)
		AddLong(vc, "HighestScoreAt", HighScoreAt);
	cre->LinkEndChild(vc);
}

};
