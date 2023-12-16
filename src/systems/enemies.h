//
// Created by henry on 8/26/23.
//

#ifndef SCIFISHOOTER_ENEMIES_H
#define SCIFISHOOTER_ENEMIES_H

#include <hagame/core/entity.h>
#include <hagame/core/system.h>
#include <hagame/graphics/tilemap.h>

#include "../enemies/enemy.h"
#include "../common/gamestate.h"

class Enemies : public hg::System {
public:

    Enemies(GameState* state);

    HG_GET(size_t, size);

    void onUpdate(double dt) override;
    void onFixedUpdate(double dt) override;

    hg::Entity* spawn(EnemyType type, hg::Vec3 pos);

private:

    size_t m_size;

    GameState* m_state;
    hg::utils::PathFinding m_pathfinding;

    std::vector<hg::Entity*> m_enemies;

    void renderUI();
};

#endif //SCIFISHOOTER_ENEMIES_H
