/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#ifndef _SPACE_OBJECT_SORTER_H_
#define _SPACE_OBJECT_SORTER_H_

#include <vector>

#include "FSServer.h"

namespace FreeStars {

typedef enum {
    SOT_PLANET              = 0x00000001,
    SOT_FLEET               = 0x00000002,
    SOT_MINEFIELD           = 0x00000004,

    SOT_OWN                 = 0x00010000,
    SOT_FRIEND              = 0x00020000,
    SOT_NEUTRAL             = 0x00040000,
    SOT_ENEMY               = 0x00080000,
    SOT_ALIEN               = 0x00100000,

    SOT_ALL_FRACTIONS       = SOT_OWN|SOT_FRIEND|SOT_NEUTRAL|SOT_ENEMY|SOT_ALIEN,
    SOT_ALL_TYPES           = SOT_PLANET|SOT_FLEET|SOT_MINEFIELD,

    SOT_ALL                 = SOT_ALL_FRACTIONS|SOT_ALL_TYPES
} SpaceObjectType;

class SpaceObjectSorter {
public:
    SpaceObjectSorter(const SpaceObject*, const Player *p = NULL, int sot = SOT_ALL);

    std::vector<const SpaceObject*> m_object_list;

private:
    static bool needObject(const SpaceObject*, const Player*, int);
};

};

#endif
