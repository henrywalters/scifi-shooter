//
// Created by henry on 9/24/23.
//
#include <hagame/math/collisions.h>
#include <hagame/common/components/healthBar.h>
#include <hagame/graphics/components/particleEmitterComponent.h>
#include "weapons.h"
#include "../components/projectile.h"
#include "../components/actor.h"
#include "player.h"
#include "../components/explosive.h"

using namespace hg;
using namespace hg::graphics;

Weapons::Weapons(GameState* state):
    m_state(state)
{}

void Weapons::onFixedUpdate(double dt) {

    if (m_state->paused) {
        return;
    }

    scene->entities.forEach<Projectile>([&](Projectile* projectile, hg::Entity* entity) {

        auto vel = projectile->direction * projectile->speed;

        hg::Rect projectileRect(entity->transform.position.resize<2>() - projectile->size * 0.5, projectile->size);

        auto neighbors = m_state->entityMap.getNeighbors(entity->transform.position.resize<2>(), projectile->size);

        bool hitNeighbor = false;
        Entity* hitNeighborEntity;
        float minDistance;

        if (neighbors.size() > 0) {
            for (const auto& neighbor : neighbors) {

                if (neighbor == projectile->source) {
                    continue;
                }

                if (scene->entities.groups.inGroup(neighbor, "enemies") && scene->entities.groups.inGroup(projectile->source, "enemies")) {
                    continue;
                }

                auto actor = neighbor->getComponent<Actor>();

                if (!actor) {
                    continue;
                }

                auto actorRect = hg::Rect(neighbor->transform.position.resize<2>() - actor->size * 0.5, actor->size);
                float distance = (neighbor->transform.position - entity->transform.position).magnitude();
                if (hg::math::collisions::checkRectAgainstRect(projectileRect, actorRect) && (!hitNeighbor || distance < minDistance)) {
                    hitNeighbor = true;
                    minDistance = distance;
                    hitNeighborEntity = neighbor;
                }
            }

        }

        if (hitNeighbor) {
            auto hitEntity = scene->entities.add();
            hitEntity->transform.position = entity->transform.position;

            auto hitActor = hitNeighborEntity->getComponent<Actor>();

            float damage = hitNeighborEntity == scene->getSystem<Player>()->player && m_state->params.invincible ? 0 : projectile->damage * -1;

            hitActor->addHealth(damage);
            hitNeighborEntity->getComponent<HealthBar>()->health = hitActor->health;

            // hitNeighborEntity->getComponent<TopDownPlayerController>()->addVelocity(projectile->direction * 2000);

            if (hitActor->alive()) {
                if (entity->hasComponent<Explosive>()) {
                    entity->getComponent<Explosive>()->explode(scene, m_state);
                }
            }

            auto emitter = hitEntity->addComponent<ParticleEmitterComponent>(m_state->particles.get("blood_hit"));
            emitter->emitter()->fire();
            scene->entities.remove(entity);

            return;
        }



        auto hit = m_state->tilemap->isColliding(0, projectileRect, vel, dt);

        if (hit.has_value()) {

            if (entity->hasComponent<Explosive>()) {
                entity->getComponent<Explosive>()->explode(scene, m_state);
            }

            scene->entities.remove(entity);

            auto hitEntity = scene->entities.add();
            hitEntity->transform.position = hit.value().position;
            auto emitter = hitEntity->addComponent<ParticleEmitterComponent>(m_state->particles.get("wall_hit"));
            emitter->emitter()->fire();

            return;
        }

        projectile->update(dt);
    });
}
