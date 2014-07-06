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
 **@file Wormhole.h
 **@ingroup Server
 **@brief Wormholes.
 **/

#if !defined(FreeStars_Wormhole_h)
#define FreeStars_Wormhole_h

#include <deque>
using namespace std;

namespace FreeStars {
class Location;

/**
 * Wormholes.
 * A wormhole is a natural jump gate.
 * @ingroup Server
 */
class Wormhole : public SpaceObject
{

/**
 * Wormhole stability.
 * This is a _probability_ of the wormhole jumping next year.
 */
enum WHStability
{
	WORM_IMMOBILE		= 0,	/**< Will never shift. */
	WORM_VERYSTABLE		= 1,	/**< Very stable. */
	WORM_STABLE			= 2,	/**< Stable. */
	WORM_AVERAGE		= 3,	/**< Average. */
	WORM_UNSTABLE		= 4,	/**< Unstable. */
	WORM_VERYUNSTABLE	= 5,	/**< Very unstable. */
	WORM_WILLJUMP		= 100	/**< Will jump next year. */
};

public:
	Wormhole(Game*); ///< Constructor.
//	Wormhole(const Location & end)
//		: SpaceObject(), mStability(WORM_VERYSTABLE), mMaxStability(WORM_VERYSTABLE), mMinStability(WORM_VERYUNSTABLE)
//		{  Init(); SetLocation(end); mID = TheGalaxy->GetWormholeID(); }
	Wormhole(const Wormhole & hole); ///< Copy constructor.
	void Init(); ///< Initialization routine.

	virtual long GetCloak(const Player * p, bool pen) const	{
		return (pen || mHadSeen[p->GetID()-1]) ? 0 : Rules::GetConstant("WormholeCloak", 75); }
	virtual double GetMaxTachyon() const	{ return 1.0; }
	virtual long GetScanPenetrating() const			{ return -1; }
	virtual long GetScanSpace() const		{ return -1; }
	/// @note Wormholes should never be 'in orbit' and it has no effect if they are, so just claim we're never in orbit.
	virtual Planet * InOrbit()				{ return NULL; }
	virtual const Planet * InOrbit() const	{ return NULL; }

	/// Name of XML element associated with class.
	static const char* ELEMENT_NAME()	{return "Wormhole";}
	bool ParseNode(const TiXmlNode * node);
	virtual TiXmlNode * WriteNode(TiXmlNode * node, const Player * viewer) const;
	/// Get stability. (See WHStability.)
	long GetStability() const				{ return mStability; }
	/// Get the other end of this wormhole.
	const Wormhole * GetAttached() const;
	/// Set the other end of this wormhole.
	void SetAttached(const Wormhole * wh)	{ mAttached = wh; mAttachedID = wh ? wh->GetID() : 0; }
	/// ???. Updates the Traversed and HadSeen flags.
	void Enter(long id);
	/// ???. Updates the HadSeen flag.
	void Exit(long id) const;
	void AdjustTraverse();	/**< Fixup mTraversed. */
	void Jiggle();			/**< Move the wormhole or change its stability. */
	void Shift();			/**< Move the wormhole. */

private:
	const Wormhole * mAttached;	/**< The other end of the wormhole. */
	long mAttachedID;			/**< The ID of the other end. */
	
	/**
	 * Stability.
	 * Current stability of the wormhole.
	 * This affects the chance the wormhole will move next year.
	 * See #WHStability.
	 */
	long mStability;
	/**
	 * Maximum stability.
	 * The most stable this wormhole can get.
	 * (WORM_VERYSTABLE is default.)
	 * See #WHStability.
	 */
	long mMaxStability;
	/**
	 * Minimum stability.
	 * The most unstable this wormhole can get.
	 * (WORM_VERYUNSTABLE is default.)
	 * See #WHStability.
	 */
	long mMinStability;
	
	deque<bool> mHadSeen;	/**< Have players seen wormhole or not. */
	deque<bool> mTraversed;	/**< Have players traversed wormhole or not. */
};
}
#endif // FreeStars_Wormhole_h
