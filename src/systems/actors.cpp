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

void Actors::onUpdate(double dt) {
    m_state->entityMap.clear();

    scene->entities.forEach<Actor>([&](Actor* actor, hg::Entity* entity) {

        m_state->entityMap.insert(entity->transform.position.resize<2>(), actor->size, entity);

        auto controller = entity->getComponent<TopDownPlayerController>();
        controller->updateVelocity(dt, actor->direction.resize<3>());
        auto vel = controller->velocity();

        Rect rect(entity->transform.position.resize<2>() - actor->size * 0.5, actor->size);

        vel = m_state->tilemap->resolveCollisions(0, rect, vel, dt);
/*
        for (const auto& neighbor : m_state->entityMap.getNeighbors(entity->transform.position.resize<2>(), actor->size)) {

            if (!entity) {
                return;
            }

            if (neighbor == entity) {
                continue;
            }

            float t;
            Actor* neighborActor = neighbor->getComponent<Actor>();

            if (!neighborActor) {
                return;
            }

            Rect neighborRect(neighbor->transform.position.resize<2>() - neighborActor->size * 0.5, neighborActor->size);
            auto hit = hg::math::collisions::checkRayAgainstRect(
                    hg::math::Ray(rect.getCenter().resize<3>(), vel * dt),
                    neighborRect,
                    t
            );

            //if (hit.has_value() && t < 1.0f) {
            //    vel = Vec3::Zero();
            //}
        }
*/
        controller->velocity(vel);

        if (hg::nearEqual<float>(vel.magnitude(), 0, 1)) {
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
