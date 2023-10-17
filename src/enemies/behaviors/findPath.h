//
// Created by henry on 10/9/23.
//

#ifndef SCIFISHOOTER_FINDPATH_H
#define SCIFISHOOTER_FINDPATH_H

#include <hagame/utils/behaviorTree.h>
#include <hagame/utils/pathfinding.h>

#include "../enemyState.h"

class FindPath : public hg::utils::bt::Node<EnemyState> {
public:

    int ticksPerFrame = 25;

protected:

    void init(EnemyState* state, hg::utils::bt::data_context_t* ctx) override;
    hg::utils::bt::Status process(double dt, EnemyState* state, hg::utils::bt::data_context_t* ctx) override;

    OBJECT_NAME(FindPath);

private:

    hg::utils::PathFinding m_pathfinding;

};

#endif //SCIFISHOOTER_FINDPATH_H
