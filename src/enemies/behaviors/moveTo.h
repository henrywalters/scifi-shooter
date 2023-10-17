//
// Created by henry on 10/9/23.
//

#ifndef SCIFISHOOTER_MOVETO_H
#define SCIFISHOOTER_MOVETO_H

#include <hagame/utils/behaviorTree.h>
#include <hagame/utils/pathfinding.h>

#include "../enemyState.h"

class MoveTo : public hg::utils::bt::Node<EnemyState> {
protected:

    void init(EnemyState* state, hg::utils::bt::data_context_t* ctx) override;
    hg::utils::bt::Status process(double dt, EnemyState* state, hg::utils::bt::data_context_t* ctx) override;

private:

    hg::utils::PathFinding m_pathfinding;

};

#endif //SCIFISHOOTER_MOVETO_H
