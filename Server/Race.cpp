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

#include "RacialTrait.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {

class TestPlanet {
public:
	long GetHabValue(HabType ht) const		{ return mHabTerra[ht]; }
	void SetHabValue(HabType ht, long v)	{ mHabTerra[ht] = v; }

private:
	deque<long> mHabTerra;
};

Race::Race()
{
	ResetDefaults();
	mLeftoverPoints = 0;
	mLeftoverBuys = 0;
	mStartAt = false;

	mHabCenter.insert(mHabCenter.begin(), Rules::MaxHabType, 50);
	mHabWidth.insert(mHabWidth.begin(), Rules::MaxHabType, 20);
	mTechCostFactor.insert(mTechCostFactor.begin(), Rules::MaxTechType, 1.0);
}

Race::~Race()
{
}

double Race::RadDamage(HabType ht) const
{
	if (ht == -1)
		return 0.0;
	else if (mHabWidth[ht] == -1)	// immune
		return 0.0;
	else if (mHabCenter[ht] >= 85)
		return 0.0;
	else {
		return (86 - mHabCenter[ht]) / 100.0;
	}
}

long Race::HabFactor(const Planet * p) const	// gives how habitable this planet is
{
	long planetValuePoints = 0,redValue = 0,ideality = 10000;
	long value, poorPlanetMod, fromIdeal;

	HabType ht;
	for (ht = 0; ht < Rules::MaxHabType; ++ht) {
		if (HabCenter(ht) < 0)
			planetValuePoints += 10000;
		else {
			fromIdeal = abs(HabCenter(ht) - p->GetHabValue(ht));

			if (fromIdeal > HabWidth(ht))
				redValue += min(fromIdeal - HabWidth(ht), 15L);
			else {
				/* green planet */
				value = 100 - fromIdeal * 100 / HabWidth(ht);
				planetValuePoints += value * value;

				poorPlanetMod = fromIdeal * 2 - HabWidth(ht);
				if (poorPlanetMod > 0)
				{
					ideality *= HabWidth(ht) * 2 - poorPlanetMod;
					ideality /= HabWidth(ht) * 2;
				}
			}
		}
	}

	if (redValue != 0)
		return -redValue;

	planetValuePoints = long(sqrt((double)planetValuePoints / Rules::MaxHabType) + 0.9);
	planetValuePoints = planetValuePoints * ideality / 10000;

	return planetValuePoints;
}

long Race::HabFactor(const TestPlanet * p) const	// gives how habitable this test planet is
{
	long planetValuePoints = 0,redValue = 0,ideality = 10000;
	long value, poorPlanetMod, fromIdeal;

	HabType ht;
	for (ht = 0; ht < Rules::MaxHabType; ++ht) {
		if (HabCenter(ht) < 0)
			planetValuePoints += 10000;
		else {
			fromIdeal = abs(HabCenter(ht) - p->GetHabValue(ht));

			if (fromIdeal > HabWidth(ht))
				redValue += min(fromIdeal - HabWidth(ht), 15L);
			else {
				/* green planet */
				value = 100 - fromIdeal * 100 / HabWidth(ht);
				planetValuePoints += value * value;

				poorPlanetMod = fromIdeal * 2 - HabWidth(ht);
				if (poorPlanetMod > 0)
				{
					ideality *= HabWidth(ht) * 2 - poorPlanetMod;
					ideality /= HabWidth(ht) * 2;
				}
			}
		}
	}

	if (redValue != 0)
		return -redValue;

	planetValuePoints = long(sqrt((double)planetValuePoints / Rules::MaxHabType) + 0.9);
	planetValuePoints = planetValuePoints * ideality / 10000;

	return planetValuePoints;
}

long Race::HabCenter(HabType ht) const
{
	if (ht >= 0 && ht < Rules::MaxHabType)
		return mHabCenter[ht];
	else
		return 0;
}

long Race::HabWidth(HabType ht) const
{
	if (ht >= 0 && ht < Rules::MaxHabType)
		return mHabWidth[ht];
	else
		return 0;
}

double Race::TechCostFactor(TechType tt) const
{
	if (tt >= 0 && tt < Rules::MaxTechType)
		return mTechCostFactor[tt];
	else
		return 0;
}

long Race::PacketTerraform(CargoType ct) const
{
	long Result = Rules::GetArrayValue("PacketTerraformMinerals", ct);
	if (Result < 0 && mPacketMap.size() > ct)
		Result = mPacketMap[ct];

	return Result;
}

long Race::GetAdvantagePoints() const
{
	int points = Rules::GetConstant("StartingPoints", 1650);
	int hab;

	long operPoints, costPoints, prodPoints;
	int i, j, k;

	hab = GetHabPoints() / 2000;

	long grRateFactor, grRate;
	grRateFactor = long(mGrowthRate * 100.0 + 0.5);	// use raw growth rate, otherwise HEs pay for GR at 2x
	grRate = grRateFactor;
	if (grRateFactor <= 5)
		points += (6 - grRateFactor) * 4200;
	else if (grRateFactor <= 13) {
		switch(grRateFactor){
			case 6: points += 3600; break;
			case 7: points += 2250; break;
			case 8: points +=  600; break;
			case 9: points +=  225; break;
		}
		grRateFactor = grRateFactor * 2 - 5;
	}
	else if (grRateFactor < 20)
		grRateFactor = (grRateFactor - 6) * 3;
	else
		grRateFactor = 45;

	points -= (hab * grRateFactor) / 24;

	int immunities = 0;
	for (j = 0; j < Rules::MaxHabType; ++j) {
		if (HabCenter(j) == -1)
			immunities++;
		else
			points += abs(HabCenter(j) - 50) * 4;	// bonus points for off center habs
	}

	if (immunities > 1)	// if more then one immunity
		points -= 150;

	operPoints = FactoriesRun();
	prodPoints = FactoryRate();

	if (operPoints > 10 || prodPoints > 10)
	{
		operPoints -= 9;
		if (operPoints < 1)
			operPoints = 1;
		prodPoints -= 9;
		if (prodPoints < 1)
			prodPoints = 1;

		prodPoints *= FactoryProductionCost(); // HE penalty, 2 for all PRTs execpt 3 for HE

		// additional penalty for two- and three-immune
		if (immunities >= 2)
			points -= ((prodPoints * operPoints) * grRate) / 2;
		else
			points -= ((prodPoints * operPoints) * grRate) / 9;
	}

	j = PopEfficiency()/100;
	if (j > 25) j = 25;
	if (j <= 7) points -= 2400;
	else if (j == 8) points -=1260;
	else if (j == 9) points -= 600;
	else if (j > 10) points += (j - 10) * 120;

	if (ARTechType() == -1) {
		//factories
		prodPoints = FactoryRate() - 10;
		costPoints = FactoryCost().GetResources() - 10;
		operPoints = FactoriesRun() - 10;
		long tmpPoints = 0;

		if (prodPoints < 0)
			tmpPoints -= prodPoints * 100;
		else
			tmpPoints -= prodPoints * 121;

		if (costPoints < 0)
			tmpPoints += costPoints * costPoints * -60;
		else
			tmpPoints += costPoints * 55;

		if (operPoints < 0)
			tmpPoints -= operPoints * 40;
		else
			tmpPoints -= operPoints * 35;

		long llfp = Rules::GetConstant("LimitLowFactoryPoints", 700);
		if (tmpPoints > llfp)
			tmpPoints = (tmpPoints - llfp) / 3 + llfp;

		if (operPoints > 14)
			tmpPoints -= 360;
		else if (operPoints > 11)
			tmpPoints -= (operPoints - 7) * 45;
		else if (operPoints >= 7)
			tmpPoints -= (operPoints - 6) * 30;

		if (prodPoints >= 3)
			tmpPoints -= (prodPoints - 2) * 60;

		points += tmpPoints;

		if (FactoryCost()[2] == 3)
			points -= 175;

		// mines
		prodPoints = 10 - MineRate();
		costPoints = 3 - MineCost().GetResources();
		operPoints = 10 - MinesRun();
		tmpPoints = 0;

		if (prodPoints > 0)
			tmpPoints = prodPoints * 100;
		else
			tmpPoints = prodPoints * 169;

		if (costPoints > 0)
			tmpPoints -= 360;
		else
			tmpPoints += 80 - costPoints * 65;

		if (operPoints > 0)
			tmpPoints += operPoints * 40;
		else
			tmpPoints += operPoints * 35;

		points += tmpPoints;
	} else
		points += 210; // AR

	points -= PointCost();	// cost for PRT, all LRTs, and racial trait interactions

	// too many LRTs
	i = 0;
	k = 0;
	for(j = 0; j < GetLRTCount(); ++j)
	{
		if (GetLRT(j)->Name().empty())
			continue;

		if (GetLRT(j)->PointCost() < 0)
			i++;
		else
			k++;
	}

	if (i + k > 4)
		points -= (i + k) * (i + k - 4) * 10;
	if (i - k > 3)
		points -= (i - k - 3) * 60;
	if (k - i > 3)
		points -= (k - i - 3) * 40;

	// tech
	int techCosts = 0;
	for (j = 0; j < Rules::MaxTechType; ++j) {
		if (TechCostFactor(j) >= 1.75 - epsilon)
			--techCosts;
		else if (TechCostFactor(j) <= 0.5 + epsilon)
			++techCosts;
	}

	if (techCosts > 0)
	{
		points -= techCosts * techCosts * 130;
		if (techCosts >= 6)			// already paid 4680 so true cost is 3250
			points += 1430;
		else if (techCosts == 5)	// already paid 3250 so true cost is 2730
			points += 520;
		// cost for 4 is 2080
	} else if (techCosts < 0) {
		techCosts = -techCosts;
		points += techCosts * (techCosts + 9) * 15;
		if (techCosts >= 6)
			points += 30;
		if (techCosts > 4 && PopEfficiency() < 1000)
			points -= 190;
	}

	if (ARTechType() != -1) {
		if (TechCostFactor(ARTechType()) <= 0.5 + epsilon)
			points -= 100;
	}

	if (StartAt())
		points -= 180;

	if (mLeftoverPoints > points / 3) {
		Message * mess = TheGame->AddMessage("Warning: Invalid racial setting");
		mess->AddItem("Race", mSingularName);
		mess->AddLong("Leftover points claimed", mLeftoverPoints);
		mess->AddLong("Leftover points max", points/3);
	}

	return points;
}

long Race::GetHabPoints() const
{
	double advantagePoints;
	int tmpHab, TTCorrFactor, h, i;
	TestPlanet testPlanet;

	deque<long> testHabWidth;
	testHabWidth.insert(testHabWidth.begin(), Rules::MaxHabType, 50);
	deque<long> testHabStart;
	testHabStart.insert(testHabStart.begin(), Rules::MaxHabType, 25);
	deque<long> iterNum;
	iterNum.insert(iterNum.begin(), Rules::MaxHabType, 11);
	deque<long> TFDone;
	TFDone.insert(TFDone.begin(), Rules::MaxHabType, 0);
	advantagePoints = 0.0;

	for (h = 0; h < 3; ++h)
	{
		int DesireFactor;
		switch (h) {
		case 0:
			TTCorrFactor = TerraformFactor0();
			DesireFactor = 7;
			break;
		case 1:
			TTCorrFactor = TerraformFactor1();
			DesireFactor = 5;
			break;
		case 2:
		default:
			TTCorrFactor = TerraformFactor2();
			DesireFactor = 6;
			break;
		}

		for (i = 0; i < Rules::MaxHabType; ++i)
		{
			if (HabCenter(i) == -1)
			{
				testHabStart[i] = 50;
				testHabWidth[i] = 11;
				iterNum[i] = 1;
			}
			else
			{
				testHabStart[i] = HabCenter(i) - HabWidth(i) - TTCorrFactor;
				if (testHabStart[i] < 0)
					testHabStart[i] = 0;
				tmpHab = HabCenter(i) + HabWidth(i) + TTCorrFactor;
				if (tmpHab > 100)
					tmpHab = 100;
				testHabWidth[i] = tmpHab - testHabStart[i];
				iterNum[i] = 11;
			}
		}

		advantagePoints += HabPointsRecursion(DesireFactor, TTCorrFactor, 0, testHabWidth, testHabStart, iterNum, TFDone, &testPlanet);
	}

	return long(advantagePoints / 10.0 + 0.5);
}

double Race::HabPointsRecursion(int DesireFactor, int TTCorrFactor, int depth, deque<long> &testHabWidth, deque<long> &testHabStart, deque<long> &iterNum, deque<long> &TFDone, TestPlanet * testPlanet) const
{
	double sumOverHab;
	if (depth < Rules::MaxHabType) {
		sumOverHab = 0.0;
		int tmpHab;
		for (int i = 0; i < iterNum[depth]; ++i) {
        	if (i == 0 || iterNum[depth] <= 1)
	        	tmpHab = testHabStart[depth];
        	else
        		tmpHab = (testHabWidth[depth] * i) / (iterNum[depth] - 1) + testHabStart[depth];

        	if (TTCorrFactor != 0 && HabCenter(depth) != -1)
        	{
				int tmpHab2;
        		tmpHab2 = HabCenter(depth) - tmpHab;
				if (abs(tmpHab2) <= TTCorrFactor)
					tmpHab2 = 0;
				else if (tmpHab2 < 0)
					tmpHab2 += TTCorrFactor;
				else
					tmpHab2 -= TTCorrFactor;

				TFDone[depth] = tmpHab2;
				tmpHab = HabCenter(depth) - tmpHab2;
        	}
        	testPlanet->SetHabValue(depth, tmpHab);
			sumOverHab += HabPointsRecursion(DesireFactor, TTCorrFactor, depth+1, testHabWidth, testHabStart, iterNum, TFDone, testPlanet);
		}
		if (HabCenter(depth) != -1)
			sumOverHab = (sumOverHab * testHabWidth[depth]) / 100;
		else
			sumOverHab *= 11;
	} else {
		sumOverHab = HabFactor(testPlanet);

		long maxTerra = 0;
		for (int i = 0; i < Rules::MaxHabType; ++i)
			maxTerra += TFDone[i];

		if (Rules::GetConstant("PointsTerraformCostLimit"))
			maxTerra = long(maxTerra * ComponentCostFactor(CT_TERRAFORM));

		if (maxTerra > TTCorrFactor)
		{
			sumOverHab -= maxTerra - TTCorrFactor;
			if (sumOverHab < 0)
				sumOverHab = 0;
		}
		sumOverHab *= sumOverHab;
		sumOverHab *= DesireFactor;
	}

	return sumOverHab;
}

bool Race::ParseNode(const TiXmlNode * node, bool other, MessageSink& messageSink)
{
	const TiXmlNode * child1;
	const TiXmlNode * child2;
	long i;
	const char * ptr;

	for (child1 = node->FirstChild(); child1; child1 = child1->NextSibling()) {
		if (child1->Type() == TiXmlNode::COMMENT)
			continue;

		if (stricmp(child1->Value(), "Randomize") == 0) {
			// used elsewhere, ignore here
		} else if (stricmp(child1->Value(), "SingularName") == 0) {
			ptr = GetString(child1);
			if (ptr != NULL)
				mSingularName = ptr;
		} else if (stricmp(child1->Value(), "PluralName") == 0) {
			ptr = GetString(child1);
			if (ptr != NULL)
				mPluralName = ptr;
		} else if (stricmp(child1->Value(), "PrimaryRacialTrait") == 0) {
			mPRT = RacialTrait::ParsePRT(GetString(child1));
		} else if (stricmp(child1->Value(), "LesserRacialTrait") == 0) {
			const RacialTrait * rt;
			rt = RacialTrait::ParseLRT(GetString(child1));
			if (rt == NULL) {
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddItem("LesserRacialTrait", GetString(child1));
				return false;
			} else
				mLRTs.push_back(rt);
		} else if (stricmp(child1->Value(), "HabSettings") == 0) {
			for (child2 = child1->FirstChild("Hab"); child2; child2 = child2->NextSibling("Hab")) {
				const TiXmlElement * el;
				el = child2->ToElement();
				int i = Rules::HabID(el->Attribute("Name"));
				if (i < 0 || i >= Rules::MaxHabType) {
					Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
					mess->AddItem("Race", mSingularName);
					mess->AddItem("Hab type", el->Attribute("Name"));
					return false;
				}

				mHabCenter[i] = GetLong(child2->FirstChild("Center"));
				if (mHabCenter[i] != -1 && (mHabCenter[i] < 10 || mHabCenter[i] > 90)) {
					Message * mess = TheGame->AddMessage("Error: Invalid racial setting");
					mess->AddItem("Race", mSingularName);
					mess->AddLong("Hab center", mHabCenter[i]);
					return false;
				}
				mHabWidth[i] = GetLong(child2->FirstChild("Width"));
				if ((mHabCenter[i] == -1 && mHabWidth[i] != -1) ||
					(mHabCenter[i] != -1 && mHabWidth[i] == -1) ||
					(mHabWidth[i] > mHabCenter[i] || mHabWidth[i] > 100 - mHabCenter[i]))
				{
					Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
					mess->AddItem("Race", mSingularName);
					mess->AddLong("Hab width", mHabCenter[i]);
					return false;
				}
			}
		} else if (stricmp(child1->Value(), "GrowthRate") == 0) {
			mGrowthRate = GetDouble(child1);
			if (mGrowthRate < 0.009 || mGrowthRate > 0.201) {//magic number
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddFloat("GrowthRate", mGrowthRate);
				return false;
			}
		} else if (stricmp(child1->Value(), "PopEfficiency") == 0) {
			mPopEfficiency = GetLong(child1);
			if (mPopEfficiency < 700 || mPopEfficiency > 2500) {//magic number
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddLong("PopEfficiency", mPopEfficiency);
				return false;
			}
		} else if (stricmp(child1->Value(), "FactoryRate") == 0) {
			mFactoryRate = GetLong(child1);
			if (mFactoryRate < 5 || mFactoryRate > 15) {//magic number
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddLong("FactoryRate", mFactoryRate);
				return false;
			}
		} else if (stricmp(child1->Value(), "FactoryCost") == 0) {
			mFactoryCost.ReadCosts(child1);
			// magic number
			if (mFactoryCost.GetResources() < 5 || mFactoryCost.GetResources() > 25
				|| mFactoryCost[0] != 0
				|| mFactoryCost[1] != 0
				|| !(mFactoryCost[2] == 3 || mFactoryCost[2] == 4))
			{
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddLong("FactoryCost - Resources", mFactoryCost.GetResources());
				mess->AddLong("FactoryCost - Mineral", mFactoryCost[2]);
				return false;
			}
		} else if (stricmp(child1->Value(), "FactoriesRun") == 0) {
			mFactoriesRun = GetLong(child1);
			if (mFactoriesRun < 5 || mFactoriesRun > 25) {//magic number
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddLong("FactoriesRun", mFactoriesRun);
				return false;
			}
		} else if (stricmp(child1->Value(), "MineRate") == 0) {
			mMineRate = GetLong(child1);
			if (mMineRate < 5 || mMineRate > 25) {//magic number
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddLong("MineRate", mMineRate);
				return false;
			}
		} else if (stricmp(child1->Value(), "MineCost") == 0) {
			mMineCost.ReadCosts(child1);
			// magic number
			if (mMineCost.GetResources() < 2 || mMineCost.GetResources() > 15
				|| mMineCost[0] != 0
				|| mMineCost[1] != 0
				|| mMineCost[2] != 0)
			{
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddLong("Mine - Resources", mMineCost.GetResources());
				return false;
			}
		} else if (stricmp(child1->Value(), "MinesRun") == 0) {
			mMinesRun = GetLong(child1);
			if (mMinesRun < 5 || mMinesRun > 25) {//magic number
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddLong("MinesRun", mMinesRun);
				return false;
			}
		} else if (stricmp(child1->Value(), "TechFactor") == 0) {
			Rules::ParseArrayFloat(child1, mTechCostFactor, TECHS);
		} else if (stricmp(child1->Value(), "StartAt") == 0) {
			ptr = GetString(child1);
			if (ptr && stricmp(ptr, "true") == 0)
				mStartAt = true;
		} else if (stricmp(child1->Value(), "StartMinerals") == 0) {
			mLeftoverBuys = LOPB_MINERALS;
			mLeftoverPoints = GetLong(child1);
			if (mLeftoverPoints < 0 || mLeftoverPoints > 50) {//magic number
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddLong("StartMinerals", mLeftoverPoints);
				return false;
			}
		} else if (stricmp(child1->Value(), "StartConcentrations") == 0) {
			mLeftoverBuys = LOPB_CONCENTRATIONS;
			mLeftoverPoints = GetLong(child1);
			if (mLeftoverPoints < 0 || mLeftoverPoints > 50) {//magic number
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddLong("StartConcentrations", mLeftoverPoints);
				return false;
			}
		} else if (stricmp(child1->Value(), "StartMines") == 0) {
			mLeftoverBuys = LOPB_MINES;
			mLeftoverPoints = GetLong(child1);
			if (mLeftoverPoints < 0 || mLeftoverPoints > 50) {//magic number
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddLong("StartMines", mLeftoverPoints);
				return false;
			}
		} else if (stricmp(child1->Value(), "StartFactories") == 0) {
			mLeftoverBuys = LOPB_FACTORIES;
			mLeftoverPoints = GetLong(child1);
			if (mLeftoverPoints < 0 || mLeftoverPoints > 50) {//magic number
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddLong("StartFactories", mLeftoverPoints);
				return false;
			}
		} else if (stricmp(child1->Value(), "StartDefenses") == 0) {
			mLeftoverBuys = LOPB_DEFENSES;
			mLeftoverPoints = GetLong(child1);
			if (mLeftoverPoints < 0 || mLeftoverPoints > 50) {//magic number
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddLong("StartDefenses", mLeftoverPoints);
				return false;
			}
		} else if (stricmp(child1->Value(), "RaceEmblem") == 0) {
			mRaceEmblem = GetLong(child1);
			if (mRaceEmblem < 0 || mRaceEmblem > 32) { //magic number
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddLong("RaceEmblem", mRaceEmblem);
				return false;
			}
		} else if (!other && stricmp(child1->Value(), "Password") == 0) {
			ptr = GetString(child1);
			if (ptr != NULL)
				mPassword = ptr;
		} else if (	stricmp(child1->Value(), "MetaInfo") == 0 ||
					stricmp(child1->Value(), "Rules") == 0) {
			// skip it
		} else if (stricmp(child1->Value(), "PacketTerraformMinerals") == 0) {
			if (mPacketMap.size() > 0) {
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddItem("Duplicate section", "PacketTerraformMinerals");
				continue;
			}

			if (Rules::GetArrayValue("PacketTerraformMinerals", 0) >= 0) {
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddItem("Rules file doesn't allow section", "PacketTerraformMinerals");
				continue;
			}

			// we could also check to make sure this race could packet terraform,
			// but won't bother since it won't cause problems if they can't

			deque<long> array;
			if (Rules::ParsePacketMapping(child1, array))
				mPacketMap.insert(mPacketMap.begin(), array.begin(), array.end());
		} else if (stricmp(child1->Value(), "InitialSettings") == 0) {
			// skip it here
		} else {
			Message * mess = messageSink.AddMessage("Warning: Unknown section");
			mess->AddItem("Race", mSingularName);
			mess->AddItem("Section", child1->Value());
			continue;
		}
	}

	if (!other) {
		if (mPRT == NULL) {
			Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
			mess->AddItem("Race", mSingularName);
			mess->AddItem("Missing PRT", "");
			return false;
		}
		if (mGrowthRate <= 0.009) {
			Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
			mess->AddItem("Race", mSingularName);
			mess->AddFloat("GrowthRate", mGrowthRate);
			return false;
		}

		for (i = 0; i < Rules::MaxHabType; ++i) {
			if (mHabCenter[i] == 0 || mHabWidth[i] == 0) {
				Message * mess = messageSink.AddMessage("Error: Invalid racial setting");
				mess->AddItem("Race", mSingularName);
				mess->AddItem("missing Hab Setting", Rules::GetHabName(i));
				return false;
			}
		}
	}

	RacialTrait::AddInteractions(mLRTs, mPRT);

	return true;
}

void Race::WriteHabs(TiXmlNode * node) const
{
	TiXmlElement HS("HabSettings");
	for (int i = 0; i < Rules::MaxHabType; ++i) {
		TiXmlElement hab("Hab");
		hab.SetAttribute("Name", Rules::GetHabName(i));
		AddLong(&hab, "Center", mHabCenter[i]);
		AddLong(&hab, "Width", mHabWidth[i]);
		HS.InsertEndChild(hab);
	}
	node->InsertEndChild(HS);
}

void Race::WriteNode(TiXmlNode * node) const
{
	int i;

	AddString(node, "SingularName", mSingularName.c_str());
	AddString(node, "PluralName", mPluralName.c_str());
	AddString(node, "PrimaryRacialTrait", mPRT->Name().c_str());

	deque<const RacialTrait *>::const_iterator rti;
	for (rti = mLRTs.begin(); rti != mLRTs.end(); ++rti) {
		if (!(*rti)->Name().empty())
			AddString(node, "LesserRacialTrait", (*rti)->Name().c_str());
	}

	TiXmlElement HS("HabSettings");
	for (i = 0; i < Rules::MaxHabType; ++i) {
		TiXmlElement hab("Hab");
		hab.SetAttribute("Name", Rules::GetHabName(i));
		AddLong(&hab, "Center", mHabCenter[i]);
		AddLong(&hab, "Width", mHabWidth[i]);
		HS.InsertEndChild(hab);
	}
	node->InsertEndChild(HS);

	AddDouble(node, "GrowthRate", mGrowthRate);
	AddLong(node, "PopEfficiency", mPopEfficiency);
	AddLong(node, "FactoryRate", mFactoryRate);
	mFactoryCost.WriteCosts(node, "FactoryCost");
	AddLong(node, "FactoriesRun", mFactoriesRun);
	AddLong(node, "MineRate", mMineRate);
	mMineCost.WriteCosts(node, "MineCost");
	AddLong(node, "MinesRun", mMinesRun);
	node->LinkEndChild(Rules::WriteArrayFloat("TechFactor", mTechCostFactor, TECHS));

	if (mStartAt)
		AddString(node, "StartAt", "true");

	if (mLeftoverPoints > 0) {
		switch (mLeftoverBuys) {
		case LOPB_MINERALS:
			AddLong(node, "StartMinerals", mLeftoverPoints);
			break;
		case LOPB_CONCENTRATIONS:
			AddLong(node, "StartConcentrations", mLeftoverPoints);
			break;
		case LOPB_MINES:
			AddLong(node, "StartMines", mLeftoverPoints);
			break;
		case LOPB_FACTORIES:
			AddLong(node, "StartFactories", mLeftoverPoints);
			break;
		case LOPB_DEFENSES:
			AddLong(node, "StartDefenses", mLeftoverPoints);
			break;
		}
	}

	AddLong(node, "RaceEmblem", mRaceEmblem);
	AddString(node, "Password", mPassword.c_str());

	deque<double>::const_iterator li;
	li = max_element(CVEngineFailure.begin(), CVEngineFailure.end());
	if (*li > 0.0) {
		node->LinkEndChild(Rules::WriteArrayFloat("CVEngineFailure", "Warp", "Speed", CVEngineFailure));
	}

	if (mPacketMap.size() > 0) {
		TiXmlElement * ptm = new TiXmlElement("PacketTerraformMinerals");
		TiXmlElement * min;
		for (i = 0; i < mPacketMap.size(); ++i) {
			min = AddString(ptm, "Mineral", Rules::GetHabName(mPacketMap[i]).c_str());
			min->SetAttribute("Name", Rules::GetCargoName(i).c_str());
			ptm->LinkEndChild(min);
		}

		node->LinkEndChild(ptm);
	}
}

void Race::ResetDefaults()
{
	CVPointCost = -1;
	CCalcGroundAttackFactor = false;
	CCalcGroundDefenseFactor = false;
	CCalcGrowthRateFactor = false;
	CCalcPopulationFactor = false;
	CVCloakCargo = -1;
	CVMineSpeedBonus = -1;
	CCalcSpyTechBonus = false;
	CCalcBattleSpeedBonus = false;
	CCalcPermaformOdds = false;
	CVPermaformPopAdj = -1;
	CVCanSeeHabSettings = -1;
	CVTemporaryTerraform = -1;
	CVScanDesign = -1;
	CCalcRepairFactor = false;
	CCalcFreighterReproduction = false;
	CVMineFieldScanning = -1;
	CVCanRemoteDetonate = -1;
	CCalcMineDecayFactor = false;
	CVMoveAndLayMines = -1;

	CCalcPacketTerraformOdds = false;
	CVPacketTerraformMass = -1;
	CCalcPacketPermaformOdds = false;
	CVPacketPermaformMass = -1;
	CVPacketScanning = -1;
	CCalcPacketCostMinFactor = false;
	CVPacketSizeOneMin = -1;
	CVPacketSizeMixed = -1;
	CVPacketCostResources = -1;
	CCalcPacketDecayFactor = false;
	CCalcPacketCatchFactor = false;
	CVPacketDecayPenalty = -1;

	CVStartAtBonus = -1;
	CVGateCargo = -1;
	CCalcOvergateLossFactor = false;
	CVGateScanning = -1;
	CVARTechType = -2;
	CVARDivisor = 10;
	CCalcFuelFactor = false;
	CCalcMainResearch = false;
	CCalcOtherResearch = false;
	CVUltimateRecycle = -1;
	CCalcSpaceScanFactor = false;
	CCalcPenScanFactor = false;
	CCalcStartingPopFactor = false;
	CCalcZeroTechCost = false;
	CCalcMaxMiniturize = false;
	CCalcMiniturizeRate = false;
	CCalcShieldFactor = false;
	CCalcArmorFactor = false;
	CCalcShieldRegenRate = false;
	CCalcDefenseFactor = false;
	CVTerraformFactor0 = -1;
	CVTerraformFactor1 = -1;
	CVTerraformFactor2 = -1;
	CVFactoryProductionCost = -1;

	CVEngineFailure.clear();
	CVEngineFailure.insert(CVEngineFailure.begin(), Rules::GetConstant("MaxSpeed"), -1.0);
}

bool Race::HasSecondPlanet() const
{
	return mPRT->HasSecondPlanet();
}

long lInherentCloaking(long v, const RacialTrait * lrt, HullType hc)
{
	return v + (((lrt->InherentCloakHull() & hc) == hc) ? lrt->InherentCloakAmount() : 0);
}

long Race::InherentCloaking(HullType hc) const
{
	return accumulate(	mLRTs.begin(),
						mLRTs.end(),
						((mPRT->InherentCloakHull() & hc) == hc) ? mPRT->InherentCloakAmount() : 0,
						lInherentCloaking,
						hc);
}

static double lComponentCostFactor(double v, const RacialTrait * lrt, ComponentType ct)
{
	return v * (double(lrt->ComponentCostFactor(ct)));
}

double Race::ComponentCostFactor(ComponentType ct) const
{
	return accumulate(mLRTs.begin(),
						mLRTs.end(),
						double(mPRT->ComponentCostFactor(ct)),
						lComponentCostFactor,
						ct);
}

long lStartingTech(long v, const RacialTrait * lrt, TechType tt)
{
	return v + lrt->StartingTech(tt);
}

long Race::PRTStartTech(TechType tt) const
{
	return mPRT->StartingTech(tt);
}

long Race::LRTStartTech(TechType tt) const
{
	return accumulate(	mLRTs.begin(),
						mLRTs.end(),
						0,
						lStartingTech,
						tt);
}

double lEngineFailure(double v, const RacialTrait * lrt, long speed)
{
	return (1.0 - (1.0 - v) * (1.0 - lrt->EngineFailure(speed)));
}

double Race::EngineFailure(long speed) const
{
	if (CVEngineFailure[speed-1] >= 0.0) {
		double Result = accumulate(mLRTs.begin(),
									mLRTs.end(),
									mPRT->EngineFailure(speed),
									lEngineFailure,
									speed);
		const_cast<Race *>(this)->CVEngineFailure[speed-1] = Result;
	}
	return CVEngineFailure[speed-1];
}

long Race::BuiltinScan(const Player * player, HullType hc, bool PenScan, long pop /*= 0*/) const
{
	long Result = mPRT->BuiltinScan(player, hc, PenScan, pop);
	deque<const RacialTrait *>::const_iterator lrt;
	for (lrt = mLRTs.begin(); lrt != mLRTs.end(); ++lrt)
		Result = max(Result, (*lrt)->BuiltinScan(player, hc, PenScan, pop));

	return Result;
}


//Macros for the rest of the functions:
// define a local function that adds current total, and LRT capability
// make a member function that accumulates all RacialTraits using the local function, and initial value of the PRT capability

// Get the sum of the PRT and all LRTs
#define GET_RACE_SUM(Function)							\
	long l##Function(long v, const RacialTrait * lrt)	\
	{													\
		return v + lrt->Function();						\
	}													\
														\
	long Race::Function() const							\
	{													\
		if (CV##Function == -1)							\
			const_cast<Race *>(this)->CV##Function = std::accumulate(mLRTs.begin(), mLRTs.end(), mPRT->Function(), l##Function);	\
		return CV##Function;								\
	}
// Get the sum of the PRT and all LRTs
#define GET_RACE_SUMD(Function)							\
	double l##Function(long v, const RacialTrait * lrt)	\
	{													\
		return v + lrt->Function();						\
	}													\
														\
	double Race::Function() const							\
	{													\
		if (!CCalc##Function) {								\
			const_cast<Race *>(this)->CCalc##Function = true;\
			const_cast<Race *>(this)->CV##Function = std::accumulate(mLRTs.begin(), mLRTs.end(), mPRT->Function(), l##Function);	\
		}													\
		return CV##Function;								\
	}
// Get the product of all LRTs multiplied by each other
#define GET_RACE_PRODUCT(Function)							\
	static double l##Function(double v, const RacialTrait * lrt)	\
	{														\
		return v * double(lrt->Function());					\
	}														\
															\
	double Race::Function() const							\
	{														\
		if (!CCalc##Function) {								\
			const_cast<Race *>(this)->CCalc##Function = true;\
			const_cast<Race *>(this)->CV##Function = std::accumulate(mLRTs.begin(), mLRTs.end(), mPRT->Function(), l##Function);	\
		}													\
		return CV##Function;								\
	}
// Get the product of all LRTs multiplied by each other as a %
#define GET_RACE_PRODUCT_PER(Function)						\
	double l##Function(double v, const RacialTrait * lrt)	\
	{														\
		return (1.0 - (1.0 - v) * (1.0 - lrt->Function()));	\
	}														\
															\
	double Race::Function() const							\
	{														\
		if (!CCalc##Function) {								\
			const_cast<Race *>(this)->CCalc##Function = true;\
			const_cast<Race *>(this)->CV##Function = std::accumulate(mLRTs.begin(), mLRTs.end(), mPRT->Function(), l##Function);	\
		}													\
		return CV##Function;								\
	}
// Get the min value of all LRTs on the race
#define GET_RACE_MIN(Function)								\
	long l##Function(long v, const RacialTrait * lrt)		\
	{														\
		return min(v, lrt->Function());						\
	}														\
															\
	long Race::Function() const								\
	{														\
		return std::accumulate(mLRTs.begin(), mLRTs.end(), mPRT->Function(), l##Function);	\
	}
// Get the max value of all LRTs on the race
#define GET_RACE_MAX(Function)								\
	long l##Function(long v, const RacialTrait * lrt)		\
	{														\
		return max(v, lrt->Function());						\
	}														\
															\
	long Race::Function() const								\
	{														\
		return std::accumulate(mLRTs.begin(), mLRTs.end(), mPRT->Function(), l##Function);	\
	}
// Get the max value of all LRTs on the race
#define GET_RACE_MAXD(Function)								\
	double l##Function(double v, const RacialTrait * lrt)	\
	{														\
		return max(v, lrt->Function());						\
	}														\
															\
	double Race::Function() const							\
	{														\
		return std::accumulate(mLRTs.begin(), mLRTs.end(), mPRT->Function(), l##Function);	\
	}
// Get true if the race has the capability, false otherwise
#define GET_RACE_HAS(Function)								\
	bool Race::Function() const								\
	{														\
		if (CV##Function != -1)								\
			return CV##Function ? true : false;				\
		const_cast<Race *>(this)->CV##Function = 1;			\
		if (mPRT->Function())								\
			return true;									\
		for (deque<const RacialTrait *>::const_iterator i = mLRTs.begin(); i != mLRTs.end(); ++i)	\
			if ((*i)->Function())							\
				return true;								\
		const_cast<Race *>(this)->CV##Function = 0;			\
		return false;										\
	}

GET_RACE_SUM(PointCost)
GET_RACE_PRODUCT(GroundAttackFactor)
GET_RACE_PRODUCT(GroundDefenseFactor)
GET_RACE_PRODUCT(GrowthRateFactor)
GET_RACE_PRODUCT(PopulationFactor)
GET_RACE_HAS(CloakCargo)
GET_RACE_SUM(MineSpeedBonus)
GET_RACE_SUMD(SpyTechBonus)
GET_RACE_SUMD(BattleSpeedBonus)
GET_RACE_MAXD(PermaformOdds)
GET_RACE_MIN(PermaformPopAdj)
GET_RACE_HAS(CanSeeHabSettings)
GET_RACE_HAS(TemporaryTerraform)
GET_RACE_HAS(ScanDesign)
GET_RACE_PRODUCT(RepairFactor)
GET_RACE_PRODUCT_PER(FreighterReproduction)
GET_RACE_HAS(MineFieldScanning)
GET_RACE_HAS(CanRemoteDetonate)
GET_RACE_PRODUCT(MineDecayFactor)
GET_RACE_HAS(MoveAndLayMines)

GET_RACE_MAXD(PacketTerraformOdds)
GET_RACE_MIN(PacketTerraformMass)
GET_RACE_MAXD(PacketPermaformOdds)
GET_RACE_MIN(PacketPermaformMass)
GET_RACE_HAS(PacketScanning)
GET_RACE_PRODUCT_PER(PacketCostMinFactor)
GET_RACE_MIN(PacketSizeOneMin)
GET_RACE_MIN(PacketSizeMixed)
GET_RACE_MIN(PacketCostResources)
GET_RACE_PRODUCT(PacketDecayFactor)
GET_RACE_PRODUCT(PacketCatchFactor)
GET_RACE_SUM(PacketDecayPenalty)

GET_RACE_MAX(StartAtBonus)
GET_RACE_HAS(GateCargo)
GET_RACE_PRODUCT(OvergateLossFactor)
GET_RACE_HAS(GateScanning)
GET_RACE_MAX(ARTechType)
GET_RACE_MIN(ARDivisor)
GET_RACE_PRODUCT(FuelFactor)
GET_RACE_PRODUCT(MainResearch)
GET_RACE_PRODUCT(OtherResearch)
GET_RACE_HAS(UltimateRecycle)
GET_RACE_PRODUCT(SpaceScanFactor)
GET_RACE_PRODUCT(PenScanFactor)
GET_RACE_PRODUCT(StartingPopFactor)
GET_RACE_PRODUCT(ZeroTechCost)
GET_RACE_MAXD(MaxMiniturize)
GET_RACE_MAXD(MiniturizeRate)
GET_RACE_PRODUCT(ShieldFactor)
GET_RACE_PRODUCT(ArmorFactor)
GET_RACE_PRODUCT_PER(ShieldRegenRate)
GET_RACE_MAX(TerraformFactor0)
GET_RACE_MAX(TerraformFactor1)
GET_RACE_MAX(TerraformFactor2)
GET_RACE_MAX(FactoryProductionCost)

GET_RACE_PRODUCT(DefenseFactor)

}
