//
// Created by henry on 5/8/23.
//

#ifndef SCIFISHOOTER_PROJECTILE_H
#define SCIFISHOOTER_PROJECTILE_H

#include <hagame/core/component.h>
#include <hagame/math/aliases.h>
#include <hagame/graphics/particleEmitter.h>
#include <hagame/core/assets.h>
#include <hagame/core/entity.h>

class Projectile : public hg::Component {
public:

    hg::Vec2 size = hg::Vec2(10, 10);
    std::string shader;
    hg::Vec3 direction;
    float speed;
    float damage;
    hg::Entity* source;

protected:

    void onUpdate(double dt) override {
        entity->transform.position += direction * speed * dt;
    }

};

#endif //SCIFISHOOTER_PROJECTILE_H
