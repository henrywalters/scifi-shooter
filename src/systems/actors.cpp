//
// Created by henry on 9/24/23.
//
#include <hagame/common/components/topDownPlayerController.h>
#include <hagame/math/collisions.h>
#include <hagame/math/ray.h>
#include "actors.h"
#include "../components/actor.h"

using namespace hg;
using namespace hg::graphics;

Actors::Actors(GameState* state):
    m_state(state)
{}

void Actors::onBeforeUpdate() {

    if (m_state->paused) {
        return;
    }

    m_state->entityMap.clear();
    scene->entities.forEach<Actor>([&](Actor *actor, hg::Entity *entity) {
        m_state->entityMap.insert(entity->transform.position.resize<2>(), actor->size, entity);
    });
}

void Actors::onFixedUpdate(double dt) {

    if (m_state->paused) {
        return;
    }

    scene->entities.forEach<Actor>([&](Actor* actor, hg::Entity* entity) {

        auto controller = entity->getComponent<TopDownPlayerController>();
        controller->updateVelocity(dt, actor->direction.resize<3>());
        auto vel = controller->velocity();

        Rect rect(entity->transform.position.resize<2>() - actor->size * 0.5, actor->size);

        // vel = m_state->tilemap->resolveCollisions(0, rect, vel, dt);

        controller->velocity(vel);

        if (hg::nearEqual<float>(vel.magnitude(), 0, 0.001)) {
            controller->velocity(hg::Vec3::Zero());
        }

        entity->transform.position += controller->velocity() * dt;

        auto weapon = actor->weapons.getWeapon();

        if (weapon) {
            weapon->update(
                    entity->transform.position,
                    actor->aimDirection.resize<3>().normalized(),
                    dt,
                    actor->attack
            );
        }
    });
}

void Actors::onAfterUpdate() {

    if (m_state->paused) {
        return;
    }

    scene->entities.forEach<Actor>([&](Actor* actor, Entity* entity) {
        if (!actor->alive()) {
            actor->onDeath();
            scene->entities.remove(entity);
        }
    });
}