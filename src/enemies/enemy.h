//
// Created by henry on 8/26/23.
//

#ifndef SCIFISHOOTER_ENEMY_H
#define SCIFISHOOTER_ENEMY_H

#include <string>
#include <hagame/math/aliases.h>
#include <hagame/utils/behaviorTree.h>

#include "behaviors/findPath.h"

enum class EnemyType {
    Slime,
};

struct EnemyDef {
    EnemyType type;
    std::string texture;
    hg::Vec2 size;
    float speed;
    std::shared_ptr<hg::utils::BehaviorTree<EnemyState>> behavior;
};

const EnemyDef Slime = EnemyDef{
        EnemyType::Slime,
        "slime",
        hg::Vec2(32, 32),
        100,
        std::make_shared<hg::utils::BehaviorTree<EnemyState>>()
};

const hg::utils::Store<EnemyType, EnemyDef> ENEMIES({
    std::make_tuple(EnemyType::Slime, Slime),
});

#endif //SCIFISHOOTER_ENEMY_H
