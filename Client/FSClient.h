/*
Copyright 2005 Elliott Kleinrock

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

#if !defined(FSClient_h)
#define FSClient_h

// wx stuff
#include "wx/wxprec.h"

// wxDockIt stuff
#include "wx/LayoutManager.h"

// Server
#include "FSServer.h"

#include <vector>

// Client
#include "MainWindow.h"
#include "CGame.h"
#include "CGalaxy.h"

//pragma TBD(m) to get compile time messages 
//helper hacks
#define HACK1(x) #x
#define HACK2(x) HACK1(##x)
// macro body
#define TBD(m) message (__FILE__"(" HACK2(__LINE__)") : TODO: " #m)


#endif // FSClient_h
