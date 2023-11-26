//
// Created by henry on 8/26/23.
//

#ifndef SCIFISHOOTER_ENEMY_H
#define SCIFISHOOTER_ENEMY_H

#include <string>
#include <hagame/math/aliases.h>
#include <hagame/utils/behaviorTree.h>

#include "behaviors/findPath.h"
#include "behaviors/randomLocation.h"
#include "behaviors/moveOnPath.h"
#include "behaviors/canSee.h"
#include "behaviors/targetLocation.h"
#include "behaviors/correctPath.h"
#include "behaviors/explode.h"


enum class EnemyType {
    Slime,
};

struct EnemyDef {
    EnemyType type;
    std::string texture;
    hg::Vec2 size;
    float speed;
    float health;
    std::shared_ptr<hg::utils::BehaviorTree<EnemyState>> behavior;
};

const EnemyDef Slime = EnemyDef{
        EnemyType::Slime,
        "slime",
        hg::Vec2(32, 32),
        100,
        200,
        std::make_shared<hg::utils::BehaviorTree<EnemyState>>()
};

const hg::utils::Store<EnemyType, EnemyDef> ENEMIES({
    std::make_tuple(EnemyType::Slime, Slime),
});

template <typename State>
void CreateBehavior(EnemyType type, hg::utils::BehaviorTree<State>* bt) {
    if (type == EnemyType::Slime) {
        auto sel = bt->template setRoot<hg::utils::bt::Selector<EnemyState>>();
        auto attackSeq = bt->template addChild<hg::utils::bt::Sequence<EnemyState>>(sel);
        auto patrolSeq = bt->template addChild<hg::utils::bt::Sequence<EnemyState>>(sel);
        bt->template addChild<RandomLocation>(patrolSeq);
        bt->template addChild<FindPath>(patrolSeq);
        bt->template addChild<MoveOnPath>(patrolSeq);

        bt->template addChild<TargetLocation>(attackSeq);
        bt->template addChild<FindPath>(attackSeq);
        bt->template addChild<MoveOnPath>(attackSeq);
        bt->template addChild<Explode>(attackSeq, 25, 1000, "slime_explosion");

        bt->template addService<CanSee>(std::vector<std::string>({"player"}))->ticksPerSecond = 16.0;
        bt->template addService<CorrectPath>()->ticksPerSecond = 16.0;
    }
}

#endif //SCIFISHOOTER_ENEMY_H
