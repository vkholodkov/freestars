/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _MESSAGE_COMPILER_H_
#define _MESSAGE_COMPILER_H_

#include <QString>

#include "FSServer.h"

namespace FreeStars {

class MessageFormatter : public MessageVisitor {
public:
    MessageFormatter(const Player*);
    virtual ~MessageFormatter();

    virtual void VisitMessage(const string &) const;
    virtual void VisitLocation(const string&, const Location*) const;
    virtual void VisitPlayer(const string&, const Player*) const;
    virtual void VisitNumber(const string&, long) const;
    virtual void VisitFloat(const string&, double) const;
    virtual void VisitString(const string&, const string&) const;
    virtual void VisitXMLNode(const string&, const TiXmlNode*) const;

    const QString &toString() const { return result; };

public:
    mutable QString result;
    const Player *player;
};

};

#endif
