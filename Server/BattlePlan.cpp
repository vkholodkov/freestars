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

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

namespace FreeStars {
BattlePlan::BattlePlan(bool defined)
{
	mDefined = defined;
	SetDefault();
}

BattlePlan::~BattlePlan()
{
}

void BattlePlan::SetDefault()
{
	// defaults to a reasonable battle plan
	mPrimary = HC_ARMED;
	mSecondary = HC_ALL;
	mTactic = BPT_MAXRATIO;
	mEnemy = BPE_ENEMY;
	mDumpCargo = false;
}

/*
	<BattlePlan>
		<BattlePlanNum>1</BattlePlanNum>
		<BattlePlanName>Run like hell, but don't forget to shoot</BattlePlanName>
		<PrimaryTarget>NoneDisengage</PrimaryTarget> <!-- NoneDisengage, Any, Starbase, Armed, BombersFreighters, Unarmed, FuelXports, Freighters -->
		<SecondaryTarget>Any</SecondaryTarget> <!-- NoneDisengage, Any, Starbase, Armed, BombersFreighters, Unarmed, FuelXports, Freighters -->
		<Tactic>DisengageIfChallenged</Tactic> <!-- Disengage, DisengageIfChallenged, MinimizeDamageSelf, MaximizeNetDamage, MaximizeDamageRatio, MaximizeDamage -->
		<AttackWho>Enemies</AttackWho> <!-- Nobody, Enemies, EnemiesNeutrals, Everyone, 1, 2, 3, etc. (for player numbers)-->
		<DumpCargo>True</DumpCargo>
	</BattlePlan>
*/
bool BattlePlan::ParseNode(const TiXmlNode * node, Player * owner)
{
	const char * ptr;

	int num;
	node->ToElement()->Attribute("IDNumber", &num);
	if (num < 0 || num >= Rules::GetConstant("MaxBattlePlans")) {
		Message * mess = owner->AddMessage("Error: Invalid battle plan number");
		mess->AddLong("Battle Plan #", num);
		return false;
	}

	ptr = GetString(node->FirstChild("Name"));
	if (!ptr) {
		Message * mess = owner->AddMessage("Error: Battle Plan has no name");
		mess->AddLong("Battle Plan #", num);
		return false;
	}

	mName = ptr;

	ptr = GetString(node->FirstChild("PrimaryTarget"));
	if (ptr == NULL) {
		Message * mess = owner->AddMessage("Error: Battle Plan has no Primary target");
		mess->AddLong("Battle Plan #", num);
		return false;
	}

	mPrimary = GetTarget(ptr);
	if (mPrimary == HC_UNKNOWN) {
		Message * mess = owner->AddMessage("Error: Battle Plan has invalid Primary target");
		mess->AddLong("Battle Plan #", num);
		mess->AddItem("Battle Plan target", ptr);
		return false;
	}

	ptr = GetString(node->FirstChild("SecondaryTarget"));
	if (ptr == NULL) {
		Message * mess = owner->AddMessage("Error: Battle Plan has no Secondary target");
		mess->AddLong("Battle Plan #", num);
		return false;
	}

	mSecondary = GetTarget(ptr);
	if (mSecondary == HC_NONE) {
		Message * mess = owner->AddMessage("Error: Battle Plan has invalid Secondary target");
		mess->AddLong("Battle Plan #", num);
		mess->AddItem("Battle Plan target", ptr);
		return false;
	}

	ptr = GetString(node->FirstChild("Tactic"));
	if (ptr == NULL) {
		Message * mess = owner->AddMessage("Error: Battle Plan has no tactic");
		mess->AddLong("Battle Plan #", num);
		return false;
	}

	if (stricmp(ptr, "Disengage") == 0)
		mTactic = BPT_DISENGAGE;
	else if (stricmp(ptr, "DisengageIfChallenged") == 0)
		mTactic = BPT_DISIFHIT;
	else if (stricmp(ptr, "MinimizeDamageSelf") == 0)
		mTactic = BPT_MINDAM;
	else if (stricmp(ptr, "MaximizeNetDamage") == 0)
		mTactic = BPT_MAXNET;
	else if (stricmp(ptr, "MaximizeDamageRatio") == 0)
		mTactic = BPT_MAXRATIO;
	else if (stricmp(ptr, "MaximizeDamage") == 0)
		mTactic = BPT_MAXDAM;
	else {
		Message * mess = owner->AddMessage("Error: Battle Plan has invalid tactic");
		mess->AddLong("Battle Plan #", num);
		mess->AddItem("Battle Plan tactic", ptr);
		return false;
	}

	ptr = GetString(node->FirstChild("AttackWho"));
	if (ptr == NULL) {
		Message * mess = owner->AddMessage("Error: Battle Plan has no enemy");
		mess->AddLong("Battle Plan #", num);
		return false;
	}

	if (stricmp(ptr, "Nobody") == 0)
		mEnemy = BPE_NONE;
	else if (stricmp(ptr, "Enemies") == 0)
		mEnemy = BPE_ENEMY;
	else if (stricmp(ptr, "EnemiesNeutrals") == 0)
		mEnemy = BPE_ENEMYNEUTRAL;
	else if (stricmp(ptr, "Everyone") == 0)
		mEnemy = BPE_ALL;
	else {
		unsigned long num = atol(ptr);
		if (num > TheGame->NumberPlayers()) {
			Message * mess = owner->AddMessage("Error: Battle Plan has invalid enemy");
			mess->AddLong("Battle Plan #", num);
			mess->AddItem("Battle Plan enemy", ptr);
			return false;
		}
	}

	ptr = GetString(node->FirstChild("DumpCargo"));
	if (ptr && stricmp(ptr, "true") == 0)
		mDumpCargo = true;

	mDefined = true;
	return true;
}

void BattlePlan::WriteNode(TiXmlNode * node) const
{
	AddString(node, "Name", mName.c_str());
	AddString(node, "PrimaryTarget", WriteTarget(mPrimary));
	AddString(node, "SecondaryTarget", WriteTarget(mSecondary));
	if (mTactic == BPT_DISENGAGE)
		AddString(node, "Tactic", "Disengage");
	else if (mTactic == BPT_DISIFHIT)
		AddString(node, "Tactic", "DisengageIfChallenged");
	else if (mTactic == BPT_MINDAM)
		AddString(node, "Tactic", "MinimizeDamageSelf");
	else if (mTactic == BPT_MAXNET)
		AddString(node, "Tactic", "MaximizeNetDamage");
	else if (mTactic == BPT_MAXRATIO)
		AddString(node, "Tactic", "MaximizeDamageRatio");
	else if (mTactic == BPT_MAXDAM)
		AddString(node, "Tactic", "MaximizeDamage");
	else {
		TheGame->AddMessage("Error: Battle Plan has invalid Primary target");
		AddString(node, "Tactic", "");
	}

	if (mEnemy == BPE_NONE)
		AddString(node, "AttackWho", "Nobody");
	else if (mEnemy == BPE_ENEMY)
		AddString(node, "AttackWho", "Enemies");
	else if (mEnemy == BPE_ENEMYNEUTRAL)
		AddString(node, "AttackWho", "EnemiesNeutrals");
	else if (mEnemy == BPE_ALL)
		AddString(node, "AttackWho", "Everyone");
	else
		AddLong(node, "AttackWho", mEnemy);

	if (mDumpCargo)
		AddString(node, "DumpCargo", "true");
}

void BattlePlan::WriteNodeBattle(TiXmlNode * node) const
{
	AddString(node, "PrimaryTarget", WriteTarget(mPrimary));
	AddString(node, "SecondaryTarget", WriteTarget(mSecondary));
	if (mTactic == BPT_DISENGAGE)
		AddString(node, "Tactic", "Disengage");
	else if (mTactic == BPT_DISIFHIT)
		AddString(node, "Tactic", "DisengageIfChallenged");
	else if (mTactic == BPT_MINDAM)
		AddString(node, "Tactic", "MinimizeDamageSelf");
	else if (mTactic == BPT_MAXNET)
		AddString(node, "Tactic", "MaximizeNetDamage");
	else if (mTactic == BPT_MAXRATIO)
		AddString(node, "Tactic", "MaximizeDamageRatio");
	else if (mTactic == BPT_MAXDAM)
		AddString(node, "Tactic", "MaximizeDamage");
	else
		AddString(node, "Tactic", "Unknown");
}

HullType BattlePlan::GetTarget(const char * ptr)
{
	if (stricmp(ptr, "NoneDisengage") == 0)
		return HC_NONE;
	else if (stricmp(ptr, "Any") == 0)
		return HC_ALL;
	else if (stricmp(ptr, "Starbase") == 0)
		return HC_BASE;
	else if (stricmp(ptr, "Armed") == 0)
		return HC_ARMED;
	else if (stricmp(ptr, "BombersFreighters") == 0)
		return HC_FREIGHTER | HC_BOMBER;
	else if (stricmp(ptr, "Unarmed") == 0)
		return HC_UNARMED;
	else if (stricmp(ptr, "FuelXports") == 0)
		return HC_FUEL;
	else if (stricmp(ptr, "Freighters") == 0)
		return HC_FREIGHTER;
	else
		return HC_UNKNOWN;
}

const char * BattlePlan::WriteTarget(HullType target)
{
	if (target == HC_NONE)
		return "NoneDisengage";
	else if (target == HC_ALL)
		return "Any";
	else if (target == HC_BASE)
		return "Starbase";
	else if (target == HC_ARMED)
		return "Armed";
	else if (target == (HC_FREIGHTER | HC_BOMBER))
		return "BombersFreighters";
	else if (target == HC_UNARMED)
		return "Unarmed";
	else if (target == HC_FUEL)
		return "FuelXports";
	else if (target == HC_FREIGHTER)
		return "Freighters";
	else {
		TheGame->AddMessage("Error: Battle Plan has invalid target");
		return "";
	}
}

// does 'attacker' want to fight 'defender'
bool BattlePlan::WillFight(const Player * attacker, const Player * defender) const
{
	if (attacker == defender)
		return false;
	if (GetEnemy() == BPE_NONE)
		return false;
	else if (GetEnemy() == BPE_ALL)
		return true;
	else if (GetEnemy() == BPE_ENEMY)
		return attacker->GetRelations(defender) <= PR_ENEMY;
	else if (GetEnemy() == BPE_ENEMYNEUTRAL)
		return attacker->GetRelations(defender) <= PR_NEUTRAL;
	else
		return GetEnemy() == defender->GetID();
}

}
