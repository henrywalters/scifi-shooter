//
// Created by henry on 8/23/23.
//

#ifndef SCIFISHOOTER_UTILS_H
#define SCIFISHOOTER_UTILS_H

#include <hagame/core/entity.h>
#include <hagame/core/scene.h>

hg::Entity* AddActor(hg::Scene* scene, hg::Vec3 pos, std::string texture, hg::Vec2 size, float maxSpeed, float maxHealth = 100.0f, float health = 100.0f);

#endif //SCIFISHOOTER_UTILS_H
