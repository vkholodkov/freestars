
#include "graphics_array.h"

namespace FreeStars {

bool GraphicsArray::ParseNode(const TiXmlNode *node)
{
    const TiXmlNode *child1, *child2;

    for (child1 = node->FirstChild("ComponentPicture"); child1 != NULL; child1 = child1->NextSibling("ComponentPicture")) {
        std::string name, path;

        for(child2 = child1->FirstChild(); child2; child2 = child2->NextSibling()) {
            if (child2->Type() != TiXmlNode::ELEMENT)
                continue;

            if (stricmp(child2->Value(), "Component") == 0) {
                name.assign(GetString(child2));
            } else if (stricmp(child2->Value(), "FileName") == 0) {
                path.assign(GetString(child2));
            }
        }

        if(!name.empty() && !path.empty()) {
            graphics_map.insert(std::make_pair(name, QIcon(path.c_str())));
        }
    }

    return true;
}

const QIcon *GraphicsArray::GetComponentIcon(const std::string &name) const {
    std::map<std::string, QIcon>::const_iterator i = graphics_map.find(name);
    return (i != graphics_map.end() ? &i->second : NULL);
}

const QIcon *GraphicsArray::GetRaceAvatar(const std::string &name) const {
    return NULL;
}

};
