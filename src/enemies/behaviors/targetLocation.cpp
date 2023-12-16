//
// Created by henry on 10/22/23.
//
#include "targetLocation.h"
#include "tags.h"

using namespace hg;
using namespace hg::utils;
using namespace hg::utils::bt;

Status TargetLocation::process(double dt, EnemyState *state, data_context_t *ctx) {
    if (ctx->find((uuid_t)BTags::Target) == ctx->end()) {
        return Status::Failure;
    }

    SetData(ctx, (uuid_t)BTags::PathEnd, ((hg::Entity*)GetData<void*>(ctx, (uuid_t)BTags::Target))->transform.position.resize<2>());

    return Status::Success;
}
