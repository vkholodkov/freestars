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
 **@file FSServer.h
 **@ingroup Server
 **@brief Compatibility header.
 **/

// Compatability stuff for Linux
#ifndef WIN32
#define stricmp strcasecmp
#define strnicmp strncasecmp
	// for the gnu build system
	#ifdef HAVE_CONFIG_H
	#include <config.h>
	#endif
#endif 

#ifdef WIN32
// stuff for windows
#pragma warning(disable : 4786)		// identifier was truncated to '255' characters in the debug information
#pragma warning(disable : 4514)		// unreferenced inline function has been removed
#pragma warning(disable : 4710)		// function '' not inlined
#pragma warning(disable : 4512)		// assignment operator could not be generated

// memory leak debugging
#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#endif // WIN32


#ifdef WIN32 // stuff for windows
#pragma warning(push, 3)
#endif // WIN32

// STL stuff
// two leaks in here, presumably cleaned up on termination
#include <iostream>
#include <deque>
#include <string>
#include <algorithm>
#include <numeric>
using namespace std;

// Standard C++ stuff
#include "math.h"
#include "assert.h"

// using tinyxml
#if !defined TIXML_USE_STL
#define TIXML_USE_STL
#endif
#include "../tinyxml/tinyxml.h"

#ifdef WIN32 // stuff for windows
#pragma warning(pop)
#endif // WIN32
#include "TinyXmlPlus.h"

extern "C" {
	//  Mersenne Twister for random numbers
	#include "../MersenneTwister/mt19937ar.h"
}
// _MIN is defined for MS because they have a prior defination of min that causes problems
#if defined(_MIN)
// redefine min to use _MIN
#undef min
#define min _MIN
#undef max
#define max _MAX
#endif

#ifdef WIN32 // stuff for windows
#pragma warning (disable : 4018)	// signed/unsigned mismatch
#endif

#ifndef _FreeStars_FSServer_h_
#define _FreeStars_FSServer_h_
namespace FreeStars {
inline long Random(long lmax)	{ return genrand_int32() % lmax; }
inline long Random(long lmin, long lmax)	{ return genrand_int32() % (lmax - lmin) + lmin; }
inline bool Randodd(double odds)	{ return genrand_real2() < odds; }
}
#endif

// FreeStars Stuff
#include "FSTypes.h"

#include "Rules.h"
#include "BattlePlan.h"
#include "Ship.h"
#include "RacialTrait.h"//#include "Component.h"
#include "Slot.h"
#include "WayOrder.h"
#include "Race.h"
#include "Player.h"
#include "Location.h"
#include "SpaceObject.h"
#include "CargoHolder.h"
#include "Fleet.h"
#include "Invasion.h"
#include "ProdOrder.h"
#include "Planet.h"
#include "Salvage.h"
#include "Galaxy.h"
#include "Game.h"
#include "Message.h"
#include "MineField.h"
#include "Wormhole.h"

