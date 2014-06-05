/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <string>

#include <QApplication>

#include "message_formatter.h"

namespace FreeStars {

MessageFormatter::MessageFormatter(const Player *_player)
    : player(_player)
{
}

MessageFormatter::~MessageFormatter()
{
}

void MessageFormatter::VisitMessage(const std::string &type) const {
    result = qApp->translate("FreeStarsMessages", type.c_str());
}

void MessageFormatter::VisitLocation(const std::string &desc, const Location *loc) const {
    const Planet *p = dynamic_cast<const Planet*>(loc);

    if(p != NULL) {
        result = result.replace("%{planet}", QString(p->GetName(player).c_str()));
        return;
    }

    const Fleet *f = dynamic_cast<const Fleet*>(loc);

    if(f != NULL) {
        result = result.replace("%{fleet}", QString(f->GetName(player).c_str()));
        return;
    }

    result = result.replace("${location}", QString("(%0, %1)").arg(loc->GetPosX()).arg(loc->GetPosY()));
}

void MessageFormatter::VisitPlayer(const std::string &desc, const Player *player) const {
}

void MessageFormatter::VisitNumber(const std::string &desc, long l) const {
    result = result.arg(l);
}

void MessageFormatter::VisitFloat(const std::string &desc, double d) const {
    result = result.arg(d);
}

void MessageFormatter::VisitString(const std::string &desc, const std::string &str) const {
    result = result.arg(QString(str.c_str()));
}

void MessageFormatter::VisitXMLNode(const std::string &desc, const TiXmlNode *node) const {
}

};
