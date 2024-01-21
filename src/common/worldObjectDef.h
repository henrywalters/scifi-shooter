//
// Created by henry on 12/3/23.
//

#ifndef SCIFISHOOTER_WORLDOBJECTDEF_H
#define SCIFISHOOTER_WORLDOBJECTDEF_H

#include <hagame/math/aliases.h>

struct WorldObjectDef {
    hg::utils::uuid_t id;
    hg::Vec2 size;
    std::string tag;
};

template <typename T>
concept IsWorldObjectDef = std::is_base_of<WorldObjectDef, T>::value;

#endif //SCIFISHOOTER_WORLDOBJECTDEF_H
