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

    float maxHealth = 100.0f;
    float health = 100.0f;
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

    virtual void die(hg::Scene* scene) {
        scene->entities.remove(entity);
    }
};

#endif //SCIFISHOOTER_ACTOR_H