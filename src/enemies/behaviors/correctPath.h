//
// Created by henry on 10/22/23.
//

#ifndef SCIFISHOOTER_CORRECTPATH_H
#define SCIFISHOOTER_CORRECTPATH_H

#include <hagame/utils/behaviorTree.h>

#include "../enemyState.h"

class CorrectPath : public hg::utils::bt::Service<EnemyState> {
public:

    hg::utils::bt::Status process(double dt, EnemyState* state, hg::utils::bt::data_context_t* ctx) override;

protected:

    OBJECT_NAME(CorrectPath);

};

#endif //SCIFISHOOTER_CORRECTPATH_H
