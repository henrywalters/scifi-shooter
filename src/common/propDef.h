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
        hg::utils::uuid_t id;
        std::string name;
        std::string texture;
        bool collide;
        std::string message;
        std::optional<hg::utils::uuid_t> nextStateId = std::nullopt;
    };

    hg::utils::uuid_t defaultStateId;
    std::unordered_map<hg::utils::uuid_t, State> states;

};

#endif //SCIFISHOOTER_PROPDEF_H
