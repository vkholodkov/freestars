
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

/**
 **@file MessageSink.h
 **@ingroup Server
 **@brief An interface for collecting messages from subsystems
 **/

#if !defined(FreeStars_MessageSink_h)
#define FreeStars_MessageSink_h

#include <string>

#include "Message.h"

namespace FreeStars {

class MessageSink {
public:
    virtual Message *AddMessage(const std::string &type) = 0;
};

class NullMessageSink : public MessageSink {
public:
    NullMessageSink();
    ~NullMessageSink();

    virtual Message *AddMessage(const std::string &type);

    static MessageSink &Instance() { return theNullMessageSink; };

private:
    void deleteAllMessages();

private:
    static NullMessageSink theNullMessageSink;
    std::list<Message*> mMessages;
};

};

#endif //FreeStars_MessageSink_h
