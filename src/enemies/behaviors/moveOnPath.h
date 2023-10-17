//
// Created by henry on 10/10/23.
//

#ifndef SCIFISHOOTER_MOVEONPATH_H
#define SCIFISHOOTER_MOVEONPATH_H

#include <hagame/utils/behaviorTree.h>
#include <hagame/utils/pathfinding.h>

#include "../enemyState.h"

class MoveOnPath : public hg::utils::bt::Node<EnemyState> {

public:

    float minDistance = TILE_SIZE[0];

protected:

    void init(EnemyState* state, hg::utils::bt::data_context_t* ctx) override;
    hg::utils::bt::Status process(double dt, EnemyState* state, hg::utils::bt::data_context_t* ctx) override;

    OBJECT_NAME(MoveOnPath);

private:

    std::vector<hg::Vec2> m_path;
    int m_index;
    bool m_hasTarget;
    hg::Vec2 m_target;
    hg::Vec2 m_lastPos;

};


#endif //SCIFISHOOTER_MOVEONPATH_H
