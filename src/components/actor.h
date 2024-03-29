//
// Created by henry on 5/26/23.
//

#ifndef SCIFISHOOTER_ACTOR_H
#define SCIFISHOOTER_ACTOR_H

#include <hagame/core/component.h>
#include <hagame/math/aliases.h>
#include <hagame/common/weapons.h>
#include <hagame/core/scene.h>

const hg::Vec2 DEFAULT_ACTOR_SIZE = hg::Vec2(64, 64);

class Actor : public hg::Component {
public:

    std::function<void()> onDeath = []() {};

    float maxHealth = 100.0f;
    float health = 100.0f;
    float fov = M_PI - M_PI / 8.0;
    hg::Vec2 size = DEFAULT_ACTOR_SIZE;
    hg::common::Weapons weapons;
    hg::Vec2 direction = hg::Vec2::Zero();
    hg::Vec2 aimDirection = hg::Vec2(1, 0);
    bool attack = false;

    void addHealth(float damage) {
        health = std::clamp<float>(health + damage, 0, maxHealth);
    }

    bool alive() const {
        return health > 0;
    }

protected:

    OBJECT_NAME(Actor)

};

HG_COMPONENT(Core, Actor)
HG_FIELD(Actor, float, maxHealth)
HG_FIELD(Actor, float, health)
HG_FIELD(Actor, float, fov)
HG_FIELD(Actor, hg::Vec2, size)

#endif //SCIFISHOOTER_ACTOR_H
