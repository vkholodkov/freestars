/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <set>
#include <iostream>

#include "space_object_sorter.h"

namespace FreeStars {

struct space_object_traits {
    space_object_traits(const SpaceObject *o)
        : planet(dynamic_cast<const Planet*>(o) != NULL ? 1 : 0)
        , fleet(dynamic_cast<const Fleet*>(o) != NULL ? 1 : 0)
    {
    }

    unsigned planet;
    unsigned fleet;
};

struct space_object_sorter : binary_function <const SpaceObject*,const SpaceObject*,bool> {
    bool operator() (const SpaceObject* x, const SpaceObject* y) const {
        space_object_traits sotx(x);
        space_object_traits soty(y);

        /*
         * Planets come first
         */
        if(sotx.planet) {
            return true;
        }

        if(soty.planet) {
            return false;
        }

        /*
         * Fleets compare with each other by ID
         */
        if(sotx.fleet && soty.fleet) {
            return x->GetID() < y->GetID();
        }

        return sotx.fleet;
    }
};

SpaceObjectSorter::SpaceObjectSorter(const SpaceObject *o, const Player *p, int sot)
{
    std::set<const SpaceObject*> objects;
    std::vector<const SpaceObject*> stack;

    objects.insert(o);
    stack.push_back(o);

    while(!stack.empty()) {
        const SpaceObject *current = stack.back();
        stack.pop_back();

        const std::deque<SpaceObject *> *also_here = o->GetAlsoHere();

        if(also_here != NULL) {
            for(std::deque<SpaceObject *>::const_iterator i = also_here->begin() ; i != also_here->end() ; i++) {
                if(!objects.count(*i)) {
                    objects.insert(*i);
                    stack.push_back(*i);
                }
            }
        }
    }

    m_object_list.reserve(objects.size());

    for(std::set<const SpaceObject*>::const_iterator i = objects.begin() ; i != objects.end() ; i++) {
        if(needObject(*i, p, sot)) {
            m_object_list.push_back(*i);
        }
    }

    std::sort(m_object_list.begin(), m_object_list.end(), space_object_sorter());
}

bool SpaceObjectSorter::needObject(const SpaceObject *o, const Player *p, int sot)
{
    space_object_traits sotr(o);

    if(sot & SOT_ALL_FRACTIONS == SOT_ALL_FRACTIONS) {
        if(sotr.planet && (sot & SOT_PLANET)) {
            return true;
        }
        else if(sotr.fleet && (sot & SOT_FLEET)) {
            return true;
        }
    }
    else {
        if(sotr.planet && (sot & SOT_PLANET)) {
            long relation = o->GetOwner()->GetRelations(p);

            if((sot & SOT_OWN) && (relation == PR_SELF)) {
                return true;
            }

            if((sot & SOT_FRIEND) && (relation == PR_FRIEND)) {
                return true;
            }

            if((sot & SOT_NEUTRAL) && (relation == PR_NEUTRAL)) {
                return true;
            }

            if((sot & SOT_ENEMY) && (relation == PR_ENEMY)) {
                return true;
            }
        }
        else if(sotr.fleet && (sot & SOT_FLEET)) {
            long relation = o->GetOwner()->GetRelations(p);

            if((sot & SOT_OWN) && (relation == PR_SELF)) {
                return true;
            }

            if((sot & SOT_FRIEND) && (relation == PR_FRIEND)) {
                return true;
            }

            if((sot & SOT_NEUTRAL) && (relation == PR_NEUTRAL)) {
                return true;
            }

            if((sot & SOT_ENEMY) && (relation == PR_ENEMY)) {
                return true;
            }
        }
    }

    return false;
}

};
