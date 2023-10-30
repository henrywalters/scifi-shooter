//
// Created by henry on 10/10/23.
//
#include "moveOnPath.h"
#include "tags.h"
#include "../../components/actor.h"

#include <hagame/graphics/debug.h>

using namespace hg;
using namespace hg::utils;
using namespace hg::utils::bt;
using namespace hg::graphics;

void MoveOnPath::init(EnemyState *state, data_context_t *ctx) {
    m_index = 0;
    m_path.clear();

    if (!HasData(ctx, (uuid_t)BTags::Path)) {
        return;
    }

    auto path = GetData<std::vector<Vec2i>>(ctx, (uuid_t)BTags::Path);
    for (int i = 1; i < path.size(); i++) {

        auto pos = state->game->tilemap->getPos(path[i]);
        auto pos0 = state->game->tilemap->getPos(path[i - 1]);

        m_path.push_back(pos);
        // graphics::Debug::DrawLine(math::LineSegment(pos0.resize<3>(), pos.resize<3>()), graphics::Color::red(), 1, 10);
    }
}

Status MoveOnPath::process(double dt, EnemyState *state, data_context_t *ctx) {
    // We want to find the closest point that isn't

    auto actor = state->entity->getComponent<Actor>();

    if (m_path.size() == 0) {
        return Status::Failure;
    }

    if ((m_path[m_path.size() - 1].resize<3>() - state->entity->position()).magnitude() < minDistance) {
        actor->direction = Vec2::Zero();
        return Status::Success;
    }

    if (m_hasTarget && state->distanceToEntity(m_target) < TILE_SIZE[0]) {
        m_hasTarget = false;
    }

    if (m_hasTarget && state->distanceToEntity(m_lastPos) == 0) {
        return Status::Failure;
    }

    for (int i = 0; i < m_path.size(); i++) {
        auto pathPos =  m_path[i] + state->game->tilemap->tileSize() * 0.5;
        auto ray = math::Ray(state->entity->transform.position, pathPos.resize<3>() - state->entity->transform.position);
        float t;
        auto hit = state->game->tilemap->raycast(0, ray, t);
        if (!hit.has_value()) {
            m_target = pathPos;
            m_hasTarget = true;
        }
    }

    m_lastPos = state->entity->transform.position.resize<2>();

    actor->direction = (m_target- state->entity->position().resize<2>()).normalized();

    return Status::Running;
}


