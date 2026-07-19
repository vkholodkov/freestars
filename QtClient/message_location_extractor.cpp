/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <string>

#include <QApplication>

#include "message_location_extractor.h"

namespace FreeStars {

MessageLocationExtractor::MessageLocationExtractor(const Player *_player, std::function<void (const Location*)> _f)
    : player(_player)
    , f(_f)
{
}

MessageLocationExtractor::~MessageLocationExtractor()
{
}

void MessageLocationExtractor::VisitMessage(const std::string &type) const {
}

void MessageLocationExtractor::VisitLocation(const std::string &desc, const Location *loc) const {
    f(loc);
}

void MessageLocationExtractor::VisitPlayer(const std::string &desc, const Player *player) const {
}

void MessageLocationExtractor::VisitNumber(const std::string &desc, long l) const {
}

void MessageLocationExtractor::VisitFloat(const std::string &desc, double d) const {
}

void MessageLocationExtractor::VisitString(const std::string &desc, const std::string &str) const {
}

void MessageLocationExtractor::VisitXMLNode(const std::string &desc, const TiXmlNode *node) const {
}

};
