/*
 * Copyright (C) 2026 Valery Kholodkov
 */

#ifndef _MESSAGE_LOCATION_EXTRACTOR_H_
#define _MESSAGE_LOCATION_EXTRACTOR_H_

#include <functional>
#include <QString>

#include "FSServer.h"

namespace FreeStars {

class MessageLocationExtractor : public MessageVisitor {
public:
    MessageLocationExtractor(const Player*, std::function<void (const Location*)>);
    virtual ~MessageLocationExtractor();

    virtual void VisitMessage(const string &) const;
    virtual void VisitLocation(const string&, const Location*) const;
    virtual void VisitPlayer(const string&, const Player*) const;
    virtual void VisitNumber(const string&, long) const;
    virtual void VisitFloat(const string&, double) const;
    virtual void VisitString(const string&, const string&) const;
    virtual void VisitXMLNode(const string&, const TiXmlNode*) const;

    bool IsResearch() const { return mIsResearch; }

private:
    const Player *player;
    std::function<void (const Location*)> f;
    mutable bool mIsResearch;
};

};

#endif
