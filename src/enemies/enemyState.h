//
// Created by henry on 10/9/23.
//

#ifndef SCIFISHOOTER_ENEMYSTATE_H
#define SCIFISHOOTER_ENEMYSTATE_H

#include "../common/gamestate.h"

struct EnemyState {
    GameState* game;
    hg::Entity* entity;

    float distanceToEntity(hg::Vec2 vec) {
        return (vec.resize<3>() - entity->transform.position).magnitude();
    }
};

#endif //SCIFISHOOTER_ENEMYSTATE_H
