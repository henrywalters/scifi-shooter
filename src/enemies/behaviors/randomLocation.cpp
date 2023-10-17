//
// Created by henry on 10/9/23.
//
#include "randomLocation.h"
#include <hagame/graphics/debug.h>

using namespace hg::utils::bt;
using namespace hg::graphics;

Status RandomLocation::process(double dt, EnemyState *state, data_context_t *ctx) {
    auto pos = state->game->randomTilemapPos();
    Debug::FillCircle(pos.x(), pos.y(), 50, Color::blue(), 10);
    SetData(ctx, "path_end", pos);
    return Status::Success;
}