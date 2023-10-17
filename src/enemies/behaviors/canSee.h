//
// Created by henry on 10/9/23.
//

#ifndef SCIFISHOOTER_CANSEE_H
#define SCIFISHOOTER_CANSEE_H

#include <hagame/utils/behaviorTree.h>

#include "../enemyState.h"

class CanSee : public hg::utils::bt::Node<EnemyState> {
public:

    CanSee(std::vector<std::string> groups);

    hg::utils::bt::Status process(double dt, EnemyState* state, hg::utils::bt::data_context_t* ctx) override;

protected:



private:

    std::vector<std::string> m_groups;

};

#endif //SCIFISHOOTER_CANSEE_H
