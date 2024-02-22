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
#include "props.h"
#include "../components/endPoint.h"

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
                auto circle = collider->getCircle();
                // circle.radius += vel.magnitude() * dt;

                for (const auto& rect : m_state->levelRectangles) {
                    auto hit = math::collisions::checkCircleAgainstRect(circle, rect);
                    if (hit.has_value()) {
                        entity->transform.move(hit->normal * hit->depth * -1);
                    }
                }

                for (const auto& propEntity : scene->getSystem<Props>()->getWithinRadius(entity->position().resize<2>(), 5)) {
                    auto prop = propEntity->getComponent<Prop>();
                    if (prop->def->states[prop->stateId].collide) {
                        auto rect = propEntity->getComponent<math::components::RectCollider>()->getRect();
                        auto hit = math::collisions::checkCircleAgainstRect(circle, rect);
                        if (hit.has_value()) {
                            entity->transform.move(hit->normal * hit->depth * -1);
                        }
                    }
                }

                scene->entities.forEach<EndPoint>([&](auto end, auto entity) {
                    auto rect = Rect(entity->position().template resize<2>() - Vec2(0.5), Vec2(1.0));
                    auto hit = math::collisions::checkCircleAgainstRect(circle, rect);
                    if (hit.has_value()) {
                        std::cout << "LEVEL END\n";
                    }
                });
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