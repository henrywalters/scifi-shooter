//
// Created by henry on 10/9/23.
//
#include "randomLocation.h"
#include "tags.h"
#include <hagame/graphics/debug.h>

using namespace hg::utils::bt;
using namespace hg::utils;
using namespace hg::graphics;

Status RandomLocation::process(double dt, EnemyState *state, data_context_t *ctx) {

    auto pos = state->game->randomTilemapPos();
    SetData(ctx, (uuid_t)BTags::PathEnd, pos);
    return Status::Success;
}