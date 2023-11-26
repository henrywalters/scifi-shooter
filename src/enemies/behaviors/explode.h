//
// Created by henry on 10/30/23.
//

#ifndef SCIFISHOOTER_EXPLODE_H
#define SCIFISHOOTER_EXPLODE_H

#include <hagame/utils/behaviorTree.h>

#include "../enemyState.h"

class Explode : public hg::utils::bt::Node<EnemyState> {
public:

    Explode(float damage, float radius, std::string particles);

    hg::utils::bt::Status process(double dt, EnemyState* state, hg::utils::bt::data_context_t* ctx) override;

private:

    std::string m_particles;
    float m_damage;
    float m_radius;

};

#endif //SCIFISHOOTER_EXPLODE_H
