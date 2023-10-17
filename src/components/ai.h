//
// Created by henry on 6/5/23.
//

#ifndef SCIFISHOOTER_AI_H
#define SCIFISHOOTER_AI_H

#include <hagame/core/component.h>
#include <hagame/core/scene.h>
#include <hagame/graphics/tilemap.h>

#include "../enemies/enemy.h"

class AI : public hg::Component {
public:

    AI(EnemyType type, hg::graphics::Tilemap* tilemap, GameState* gameState);

    enum class State {
        Idle,
        Patrol,
        Pursue,
        Attack,
    };

    State state = State::Patrol;
    hg::Entity* target;
    hg::Vec2 direction = hg::Vec2::Zero();

    int pathfindingTicksPerFrame = 5;
    float attackWithin = 0.0f;

    void idle();

    void patrol();

    void pursue();

    void attack();

    void findPath(hg::Vec3 pos);

    bool canSee(hg::Entity* other) {

        std::array<hg::Vec2, 4> corners = {
                hg::Vec2(m_size[0] * -0.5, m_size[1] * -0.5),
                hg::Vec2(m_size[0] * 0.5, m_size[1] * -0.5),
                hg::Vec2(m_size[0] * 0.5, m_size[1] * 0.5),
                hg::Vec2(m_size[0] * -0.5, m_size[1] * 0.5),
        };

        for (int i = 0; i < 4; i++) {
            hg::Vec3 position = entity->transform.position + corners[i].resize<3>();
            float t;
            hg::math::Ray ray(position, other->transform.position - entity->transform.position);

            auto hit = m_tilemap->raycast(0, ray, t);

            if (hit.has_value()) {
                return false;
            }
        }

        return true;
    }

    std::unique_ptr<hg::utils::BehaviorTree<EnemyState>> m_behavior;

protected:

    void onUpdate(double dt) override;

private:

    bool isStuck();

    EnemyType m_type;

    EnemyState m_state;



    std::optional<std::vector<hg::Vec2i>> m_path;

    bool m_isPathfinding;
    bool m_hasTargetPos = false;
    hg::Vec2 m_targetPos;
    hg::Vec2 m_size;

    hg::graphics::Tilemap* m_tilemap;
    hg::utils::PathFinding m_pathfinding;

    double m_dt;

};

#endif //SCIFISHOOTER_AI_H
