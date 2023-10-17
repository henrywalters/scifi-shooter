//
// Created by henry on 10/9/23.
//
#include "findPath.h"

using namespace hg;
using namespace hg::utils;
using namespace hg::utils::bt;

void FindPath::init(EnemyState *state, data_context_t *ctx) {
    auto start = state->entity->transform.position.resize<2>();
    auto end = GetData<Vec2>(ctx, "path_end");
    m_pathfinding.start(
    state->game->tilemap->getIndex(start),
    state->game->tilemap->getIndex(end)
    );
}

Status FindPath::process(double dt, EnemyState *state, data_context_t *ctx) {

    m_pathfinding.m_neighborFn = [&](utils::PathFinding::Node node) {
        return state->game->tilemap->findNeighbors(0, node);
    };

    for (int i = 0; i < ticksPerFrame; i++) {
        m_pathfinding.tick();
        if (m_pathfinding.finished()) {
            break;
        }
    }

    if (m_pathfinding.finished()) {

        if (!m_pathfinding.m_foundPath) {
            return Status::Failure;
        }

        std::vector<Vec2i> path = m_pathfinding.constructPath(m_pathfinding.m_current);
        SetData(ctx, "path", path);

        return Status::Success;
    }

    return Status::Running;
}
