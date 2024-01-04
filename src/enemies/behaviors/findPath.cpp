//
// Created by henry on 10/9/23.
//
#include "findPath.h"
#include "tags.h"

using namespace hg;
using namespace hg::utils;
using namespace hg::utils::bt;

void FindPath::init(EnemyState *state, data_context_t *ctx) {
    auto start = state->entity->transform.position.resize<2>();
    auto end = GetData<Vec2>(ctx, (uuid_t)BTags::PathEnd);
    m_pathfinding.start(
    state->game->getTileIndex(start),
    state->game->getTileIndex(end)
    );
}

Status FindPath::process(double dt, EnemyState *state, data_context_t *ctx) {

    m_pathfinding.m_neighborFn = [&](utils::PathFinding::Node node) {
        return state->game->findTraversableNeighbors(node.position);
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
        SetData(ctx, (uuid_t)BTags::Path, path);

        return Status::Success;
    }

    return Status::Running;
}
