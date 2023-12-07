//
// Created by henry on 12/3/23.
//

#ifndef SCIFISHOOTER_PROPDEF_H
#define SCIFISHOOTER_PROPDEF_H

#include <hagame/math/aliases.h>
#include <string>
#include <vector>
#include "worldObjectDef.h"

class PropDef : public WorldObjectDef {
public:

    struct State {
        std::string name;
        std::string texture;
        bool collide;
        std::string message;
        int nextState;
    };

    int defaultState;
    std::vector<State> states;
    hg::Vec2 size;
    std::string tag;
};

#endif //SCIFISHOOTER_PROPDEF_H
