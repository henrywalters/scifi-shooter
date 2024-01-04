//
// Created by henry on 10/22/23.
//
#include "correctPath.h"
#include "tags.h"

using namespace hg;
using namespace hg::utils;
using namespace hg::utils::bt;

Status CorrectPath::process(double dt, EnemyState *state, hg::utils::bt::data_context_t *ctx) {

    if (!HasData(ctx, (uuid_t)BTags::Target) || !HasData(ctx, (uuid_t)BTags::PathEnd)) {
        return Status::Failure;
    }

    auto target = (hg::Entity*)GetData<void*>(ctx, (uuid_t)BTags::Target);
    auto pathEnd = GetData<Vec2>(ctx,(uuid_t)BTags::PathEnd);

    //if (state->game->tilemap->getIndex(target->transform.position.resize<2>()) != state->game->tilemap->getIndex(pathEnd)) {
        SetData(ctx, (uuid_t)BTags::PathEnd, target->transform.position.resize<2>());
    //}

    return Status::Success;
}
