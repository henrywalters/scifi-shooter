//
// Created by henry on 9/24/23.
//
#include <hagame/common/components/topDownPlayerController.h>
#include <hagame/math/collisions.h>
#include <hagame/math/ray.h>
#include <hagame/utils/profiler.h>
#include <hagame/graphics/debug.h>
#include "actors.h"
#include "../components/actor.h"

using namespace hg;
using namespace hg::graphics;
using namespace hg::utils;

Actors::Actors(GameState* state):
    m_state(state)
{}

void Actors::onBeforeUpdate() {

    if (m_state->paused) {
        return;
    }

    Profiler::Start("Actors::onBeforeUpdate");

    m_state->entityMap.clear();
    scene->entities.forEach<Actor>([&](Actor *actor, hg::Entity *entity) {
        m_state->entityMap.insert(entity->transform.position.resize<2>(), actor->size, entity);
    });

    Profiler::End("Actors::onBeforeUpdate");
}

void Actors::onFixedUpdate(double dt) {

    if (m_state->paused) {
        return;
    }

    Profiler::Start("Actors::onFixedUpdate");

    scene->entities.forEach<Actor>([&](Actor* actor, hg::Entity* entity) {

        auto controller = entity->getComponent<TopDownPlayerController>();
        controller->updateVelocity(dt, actor->direction.resize<3>());
        auto vel = controller->velocity();

        Rect rect(entity->transform.position.resize<2>() - actor->size * 0.5, actor->size);

        // vel = m_state->tilemap->resolveCollisions(0, rect, vel, dt);

        if (vel.magnitude() > 0) {
            if (entity->hasComponent<math::components::CircleCollider>()) {
                auto collider = entity->getComponent<math::components::CircleCollider>();
                for (int i = 0; i < 3; i++) {
                    Vec3 dirVel;

                    switch (i) {
                        case 0:
                            dirVel[0] = vel[0];
                            break;
                        case 1:
                            dirVel[1] = vel[1];
                            break;
                        case 2:
                            dirVel[0] = vel[0];
                            dirVel[1] = vel[1];
                            break;
                    }

                    if (dirVel.magnitude() == 0) {
                        continue;
                    }
                    auto origin = entity->position() + dirVel.normalized() * collider->radius;
                    auto ray = hg::math::Ray(origin, dirVel * dt);
                    float t;
                    auto hit = m_state->raycastGeometry(ray, t);
                    if (hit.has_value() && t < 1.0f) {
                        std::cout << "HIT WALL\n";
                        vel += hit.value().normal.prod(dirVel.abs()) * (1 - t);
                    }
                }
            }
        }

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

    Profiler::End("Actors::onFixedUpdate");
}

void Actors::onAfterUpdate() {

    if (m_state->paused) {
        return;
    }

    Profiler::Start("Actors::onAfterUpdate");

    scene->entities.forEach<Actor>([&](Actor* actor, Entity* entity) {
        if (!actor->alive()) {
            actor->onDeath();
            scene->entities.remove(entity);
        }
    });

    Profiler::End("Actors::onAfterUpdate");
}