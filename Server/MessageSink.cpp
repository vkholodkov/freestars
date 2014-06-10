
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
*/

#include <memory>

#include "FSServer.h"

#include "MessageSink.h"

#if defined(_DEBUG) && defined(DEBUG_NEW)
#define new DEBUG_NEW
#endif

/**
 * @file MessageSink.cpp
 * @ingroup Server
 */
namespace FreeStars {

NullMessageSink NullMessageSink::theNullMessageSink;

NullMessageSink::NullMessageSink()
    : mMessages()
{
}

NullMessageSink::~NullMessageSink()
{
    deleteAllMessages();
}

Message *NullMessageSink::AddMessage(const std::string &type)
{
    deleteAllMessages();

	std::auto_ptr<Message> mess(new Message(type));
	mMessages.push_back(mess.get());
	return mess.release();
}

void NullMessageSink::deleteAllMessages()
{
    for(std::list<Message*>::const_iterator i = mMessages.begin() ; i != mMessages.end() ; i++) {
        delete *i;
    }
}

};
