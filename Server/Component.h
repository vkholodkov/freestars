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
 **@file Component.h
 **@ingroup Server
 **@brief Ship components.
 **/

#if !defined(FreeStars_Component_h)
#define FreeStars_Component_h

#include <deque>
using namespace std;

#include "FSTypes.h"
#include "Rules.h"
#include "Cost.h"

class TiXmlNode;

namespace FreeStars {
class RacialTrait;

const long WT_BEAM		= 1;
const long WT_GATLING	= 2;
const long WT_SAPPER	= 3;
const long WT_TORP		= 4;
const long WT_MISSILE	= 5;

const long BT_NORMAL	= 1;
const long BT_SMART		= 2;
const long BT_TERRA		= 3;

const long MT_STD		= 1;
const long MT_SPEED		= 2;
const long MT_HEAVY		= 3;
const long MT_MAXIMUM	= MT_HEAVY;

// Remote terraform:
const long TERRA_DETERRA		= 0x0010;	// Can deterraform
const long TERRA_POSTERRA	= 0x0020;	// Can terraform for planet owner
const long TERRA_TERRABOTH	= (TERRA_DETERRA | TERRA_POSTERRA);	// can both de terra and terra
const long TERRA_TOINIT		= 0x0001;	// can deterraform to the initial value
const long TERRA_TOTECH		= 0x0002;	// can (de)terraform to the limits of the ship owners tech
const long TERRA_BOMB		= 0x0100;	// terraform during bombing
const long TERRA_REMOTE		= 0x0200;	// terraform during remote terraform phase
// OAs are therefore 0x23, bombs are 0x11

const long THIEF_NONE	= 0x00;
const long THIEF_SHIP	= 0x01;
const long THIEF_PLANET	= 0x02;
const long THIEF_BOTH	= THIEF_SHIP | THIEF_PLANET;

/**
 * Component.
 * A component is a piece of modular equipment
 * that is fit to a ship. Components modify
 * ship characteristics and capabilities.
 * \ingroup Server
 */
class Component {
public:
	Component();
	virtual ~Component();
	static void Cleanup();

//	long GetCost(CargoType ct) const			{ return mCost.GetCost(ct); }
	const Cost & GetCost() const		{ return mCost; }
	Cost GetCost(const Player * owner) const;

	long GetID() const					{ return ID; }
	ComponentType GetType() const		{ return Type; }
	bool IsType(ComponentType ct) const { return ct == Type;}
	const string GetName() const		{ return Name; }
	HullType GetHullType() const		{ return mHullType; }
	long GetTech(TechType tt) const		{ return Tech[tt]; }
	long GetMass() const				{ return Mass; }

    long GetArmor() const				{ return Armor; }
    long GetShield() const				{ return Shield; }
    long GetWeaponType() const			{ return WeaponType; }
    long GetPower() const				{ return Power; }
    long GetRange() const				{ return Range; }
    long GetInitiative() const		   	{ return Initiative; }
    double GetAccuracy() const			{ return Accuracy; }
    long GetBombType() const			{ return BombType; }
    double GetKillPercentage() const	{ return KillPercentage; }
	double GetNormalKillPercentage() const	{ return BombType==BT_NORMAL ? KillPercentage : 0;} 
	double GetSmartKillPercentage() const	{ return BombType==BT_SMART ? KillPercentage : 0;}
    long GetKillMin() const				{ return KillMin; }
    long GetKillInstallation() const	{ return KillInstallation; }
    long GetCloaking() const			{ return Cloaking; }
    double GetComputerPower() const		{ return ComputerPower; }
    double GetJamming() const			{ return Jamming; }
    long GetInitiativeAdjustment() const	{ return InitiativeAdjustment; }
    double GetCapacitors() const		{ return Capacitors; }
    double GetDeflection() const		{ return Deflection; }
	double GetTachyon() const			{ return Tachyon; }
    bool CanColonize() const			{ return Colonize; }
    long GetCargoCapacity() const		{ return CargoCapacity; }
    long GetFuelCapacity() const	    { return FuelCapacity; }
    long GetFuelGeneration() const		{ return FuelGeneration; }
	double GetRepairRate() const		{ return RepairRate; }
    double GetSpeedBonus() const		{ return SpeedBonus; }
    long GetMineType() const			{ return MineType; }
    long GetMineAmount() const			{ return MineAmount; }
    long GetMines() const				{ return Mines; }
    long GetRemoteTerraform() const		{ return RemoteTerraform; }
    long GetGateMass() const			{ return GateMass; }
    long GetGateRange() const			{ return GateRange; }
    long GetDriverSpeed() const			{ return DriverSpeed; }
    long GetScanSpace() const			{ return ScanSpace; }
    long GetScanPenetrating() const		{ return ScanPenetrating; }
    bool CanStealShip() const			{ return StealShip; }
    bool CanStealPlanet() const			{ return StealPlanet; }
    long GetTerraType() const			{ return TerraType; }
    long GetTerraLimit() const			{ return TerraLimit; }
	double GetDefensePower() const		{ return DefensePower; }
	long GetSweeping(long RangeBonus) const;
	long GetTerraPower() const			{ return TerraPower; }
	long GetARMaxPop() const			{ return ARMaxPop; }
	long GetDockBuildCapacity() const	{ return DockBuildCapacity; }
	bool CanRefuel() const				{ return Refuel; }
	double GetDampener() const			{ return Dampener; }
    double GetFuelUsage(unsigned long speed) const	{ if (FuelUsage.size() < speed || speed < 1 || speed > (unsigned long)Rules::GetConstant("MaxSpeed")) return 0; else return FuelUsage[speed-1]; }
	long GetRadiation() const			{ return Radiation; }
	long GetSafeSpeed() const			{ return SafeSpeed; }
	long GetMaxSpeed() const			{ return MaxSpeed; }
	long GetBattleSpeed() const			{ return BattleSpeed; }
	long GetFreeSpeed() const			{ return FreeSpeed; }
	bool CanJumpGate() const			{ return JumpGate; }
	bool CanNormalBomb() const {return BombType == BT_NORMAL;}
	bool CanSmartBomb() const {return BombType == BT_SMART;}
	bool CanTerraBomb() const {return BombType == BT_TERRA;}

	static bool LoadComponents(const TiXmlNode * comps, deque<Component *> &CompList);
	static HullType ParseHullType(const char * ptr);
	static ComponentType ParseCompType(const char * ptr);
	static long GetScore(const Component * check, long ValueType);
	static const Cost * DefenseCost()	{ return mDefenseCost; }
	static const Cost * ScannerCost()	{ return mScannerCost; }
	static const Component * T0Defense();
	static const Component * T0PlanetScanner();
	static long ParseSubType(const char * ptr, bool insert);
	long GetValueType() const		{ return SubTypeIndex.size() == 0 ? -1 : SubTypeIndex[0]; }

	bool IsBuildable(const Player * player) const;	// can this player build this component (right LRT and tech levels)
	
	/*
	 * Would the player be able to build this component
	 * if advances specified research field by specified
	 * number of levels.
	 */
	bool WouldBeBuildable(const Player*, long, long) const;

	static const Component * Upgrade(const Player * player);

private:
	void init();
	bool CheckPRTLRT(const Player*) const;

	// standard stuff for every component
	long ID;
	static long MaxID;
	string Name;
	ComponentType Type;	///< Base type, can also have other capabilities
	Cost mCost;			///< Costs
	deque<long> Tech;	///< 0 for tech 0 required, -1 for tech not needed. normal rules make no distinction. But a scout hull should be construction 0, -1 everything else
	long Mass;
	bool MTGift;

	// specific stuff, most will be defaults
	// armor and shield
	long Armor;		///< armor DP
	long Shield;	///< shield DP

	// weapons
	long WeaponType;	///< type of weapon
	long Power;		///< damage power
	long Range;		///< weapon range
	long Initiative;	///< weapon initiative
	double Accuracy;	///< base accuracy

	// bombs
	long BombType;
	double KillPercentage;	///< Bomb kill percentage
	long KillMin;	///< Bomb min killed
	long KillInstallation;	///< Bomb damage to instalations

	// electrical
	long Cloaking;		///< cloaking units
	double ComputerPower;	///< affect on inaccuracy of your missiles 1 for no affect, .8 is +20%
	double Jamming;		///< affect on accuracy of incoming missiles 1 for no affect, .8 is 20% jamming
	long InitiativeAdjustment;	///< adjustment to initiative, and hull base inititive
	double Capacitors;	///< beam damage multiplier - 1.1 and 1.2 for capacitors
	double Deflection;	///< beam damage multiplier - .9 for deflectors
	double Tachyon;		///< Power of tachyon detectors
	double Dampener;	///< Energy Dampener (slows all ships in combat)

	// mechanical
	bool Colonize;	///< true if it allows colonization
	long CargoCapacity;	///< cargo capacity
	long FuelCapacity;	///< Fuel capacity
	long FuelGeneration;	///< Fuel generation ability
	double RepairRate;	///< Max Repair rate helps all ships in the same fleet
	double SpeedBonus;	///< speed bonus (.25 for Maneuver Jet)

	// engines
	deque<double> FuelUsage;	///< FuelUsage for warp 1 through warp 10
	long Radiation;	///< hab type of radiation emitted, killing pop if the race can't take a lot of it (85th tick or higher)
	long SafeSpeed;	///< normally 9, some engines allow warp 10
	long MaxSpeed;	///< For expantion for now 10 for all engines
	long BattleSpeed;	///< battle speed of the engine, normaly dependant on FuelUsage
	long FreeSpeed;	///< 1 for stds, free speed for rams

	// Mine layers
	long MineType;	///< 1 standards, 2 Heavies, 3 speed traps
	long MineAmount;	///< number laid per year

	// remote mining
	long Mines;		///< Number of remote mines
	long RemoteTerraform;	///< Remote terraform: 0x01 - deterra only, 0x02 improve only, 0x03 both deterra: 0x10 to initial, 0x20 terra tech (OAs are therefore 0x23, bombs are 0x11)
	long TerraPower;	///< power of the remote terraformer

	// Orbitals
	long GateMass;	///< -1 for infinite
	long GateRange;	///< -1 for infinite
	long DriverSpeed;	///< mass driver speed
	bool JumpGate;		///< can a ship with this part gate from anywhere

	// Scanners
	long ScanSpace;	///< deep space (red) scanning coverage
	long ScanPenetrating;	///< orbital (yellow) scanning coverage - every scanner can see a world it is in orbit of
	bool StealShip;
	bool StealPlanet;

	// Terraforming
	long TerraType;	///< -1 for any
	long TerraLimit;	///< max amount of terraforming at this tech
	double DefensePower;	///< Defensive power at this tech

	// Station attributes
	bool Refuel;	///< can this station refuel
	long DockBuildCapacity;		///< max ship size built here -1 for infinite
	long ARMaxPop;	///< Max pop for AR races, in cargo units (100 pop a unit)

	// Specials
	bool PlanetReset;	///< building this resets the planet

	HullType mHullType;	///< Allowed on what hull (or what hull this is)

	// restrictions:
	deque<const RacialTrait *> PRTNeeded;	///< list of PRTs allowed to use this item, if empty then no restriction
	deque<const RacialTrait *> PRTDenied;	///< list of PRTs denied use this item
	deque<const RacialTrait *> LRTNeeded;	///< list of LRTs that allow use of this item, if empty then no restriction
	deque<const RacialTrait *> LRTDenied;	///< list of LRTs that deny use of this item
	static deque<string> Subtypes;
	deque<long> SubTypeIndex;
	deque<long> SubTypeValue;

	static Cost * mDefenseCost;
	static Cost * mScannerCost;
};
}
#endif // !defined(FreeStars_Component_h)
