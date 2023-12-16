//
// Created by henry on 10/9/23.
//
#include <hagame/graphics/debug.h>
#include "canSee.h"
#include "../../components/actor.h"
#include "tags.h"

using namespace hg;
using namespace hg::utils;
using namespace hg::utils::bt;

CanSee::CanSee(std::vector<std::string> groups):
    m_groups(groups)
{}

Status CanSee::process(double dt, EnemyState *state, data_context_t *ctx) {

    if (!state->entity->hasComponent<Actor>()) {
        return Status::Failure;
    }

    Actor* actor = state->entity->getComponent<Actor>();

    std::vector<Entity*> entities;

    Vec3 dir = actor->direction.normalized().resize<3>();

    float t;

    for (int i = 0; i <= RAYCAST_CHECKS; i++) {
        float theta = actor->fov * -0.5 + actor->fov * ((float) i / RAYCAST_CHECKS);
        Vec3 raycastDir = Quat(theta, Vec3::Face()).rotatePoint(dir);
        math::Ray ray(state->entity->transform.position, raycastDir);
        Vec2 hitPos;
        auto hitEntity = state->game->raycast(ray, hitPos, {state->entity});
        if (
            hitEntity.has_value() &&
            std::find(entities.begin(), entities.end(), hitEntity.value()) == entities.end() &&
            state->game->scene->entities.groups.inOneOfGroups(hitEntity.value(), m_groups)
        ) {
            entities.push_back(hitEntity.value());
        }
    }

    if (entities.size() > 0) {
        m_tree->setCurrent(nullptr);
        std::sort(entities.begin(), entities.end(), [state](Entity* a, Entity* b) {
            return (a->transform.position - state->entity->transform.position).magnitudeSq() <
                    (b->transform.position - state->entity->transform.position).magnitudeSq();
        });
        SetData(ctx, (uuid_t)BTags::Target, (void*)entities[0]);
    } else {
        ctx->erase((uuid_t)BTags::Target);
    }

    SetData(ctx, (uuid_t)BTags::EntitiesInSight, (void*)entities.data());

    return Status::Success;
}