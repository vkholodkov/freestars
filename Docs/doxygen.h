/*
 * Helper stuff for doxygen.
 */

//Template classes for doxygen to pick up.
namespace std{
	/**
	 * STL Deque container.
	 */
	template<class T> class deque {
		public:
			T element; /**< Element. */
	};
}

//Groups are defined here.
/** \defgroup Server */
/** \defgroup Client */

//Front page.
/*! \mainpage Freestars API Documentation
 *
 * \section intro_sec Introduction
 *
 * FreeStars is a project to create an easily modifiable clone
 * of the 4X turn-based strategy game Stars!.
 * With version 1, the goal is to clone Stars! as closely as possible.
 * For version 2, the goal is to make improvements in the balance and gameplay.
 *
 * \section install_sec Location
 *
 * This manual's current home is http://refdesk.rtk0.net/fsdocs/ .
 * It requires doxygen and graphviz to regenerate.
 * If you are viewing the docs online, the log from the current autobuild
 * can be found <a href="build.log">here</a>.
 *
 */

