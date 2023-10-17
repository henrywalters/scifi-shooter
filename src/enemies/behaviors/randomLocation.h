//
// Created by henry on 10/9/23.
//

#ifndef SCIFISHOOTER_RANDOMLOCATION_H
#define SCIFISHOOTER_RANDOMLOCATION_H

#include <hagame/utils/behaviorTree.h>
#include <hagame/utils/pathfinding.h>

#include "../enemyState.h"

class RandomLocation : public hg::utils::bt::Node<EnemyState> {
protected:

    hg::utils::bt::Status process(double dt, EnemyState* state, hg::utils::bt::data_context_t* ctx) override;

    OBJECT_NAME(RandomLocation);
};

#endif //SCIFISHOOTER_RANDOMLOCATION_H
