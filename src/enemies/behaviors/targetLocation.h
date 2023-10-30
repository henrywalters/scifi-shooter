//
// Created by henry on 10/22/23.
//

#ifndef SCIFISHOOTER_TARGETLOCATION_H
#define SCIFISHOOTER_TARGETLOCATION_H

#include <hagame/utils/behaviorTree.h>
#include <hagame/utils/pathfinding.h>

#include "../enemyState.h"

class TargetLocation : public hg::utils::bt::Node<EnemyState> {
protected:

    hg::utils::bt::Status process(double dt, EnemyState* state, hg::utils::bt::data_context_t* ctx) override;

    OBJECT_NAME(TargetLocation);
};

#endif //SCIFISHOOTER_TARGETLOCATION_H
