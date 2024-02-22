//
// Created by henry on 10/3/23.
//

#ifndef SCIFISHOOTER_EXPLOSIVE_H
#define SCIFISHOOTER_EXPLOSIVE_H

#include <hagame/core/component.h>
#include <hagame/core/scene.h>
#include <hagame/graphics/components/particleEmitterComponent.h>
#include <hagame/graphics/debug.h>
#include <hagame/common/components/healthBar.h>
#include "../common/gamestate.h"
#include "actor.h"

class Explosive : public hg::Component {
public:

    float blastRadius;
    float damage;
    std::string particles;

    void explode(hg::Scene* scene, GameState* state) {
        hg::graphics::Debug::DrawCircle(entity->transform.position.x(), entity->transform.position.y(), blastRadius, hg::graphics::Color::red(), 1, 10);
        auto neighbors = state->entityMap.getNeighbors(entity->position().resize<2>(), hg::Vec2(blastRadius, blastRadius));
        for (const auto& neighbor : neighbors) {
            if (neighbor->name == "Player" && state->params.invincible) {
                continue;
            }

            if (neighbor->hasComponent<Actor>()) {
                auto actor = neighbor->getComponent<Actor>();
                actor->addHealth(-damage);
                if (neighbor->hasComponent<hg::HealthBar>()) {
                    if (actor->alive()) {
                        neighbor->getComponent<hg::HealthBar>()->health = actor->health;
                    }
                }
            }
        }

        auto emitterEntity = scene->entities.add();
        emitterEntity->transform.position = entity->position();
        if (state->particles.has(particles)) {
            auto emitter = emitterEntity->addComponent<hg::graphics::ParticleEmitterComponent>(
                    state->particles.get(particles));
            emitter->emitter()->fire();
        } else {
            std::cout << "Missing explosive particles: " << particles << "\n";
        }
    }
};

#endif //SCIFISHOOTER_EXPLOSIVE_H
