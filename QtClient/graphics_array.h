
#ifndef _GRAPHICS_ARRAY_H_
#define _GRAPHICS_ARRAY_H_

#include <QIcon>

#include <string>

#include "FSServer.h"

namespace FreeStars {

class GraphicsArray {
public:
    bool ParseNode(const TiXmlNode*);
    const QIcon *GetComponentIcon(const std::string&) const;
    const QIcon *GetRaceAvatar(const std::string&) const;
    
private:
    std::map<std::string, QIcon> graphics_map;
};

};

#endif
