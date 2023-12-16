//
// Created by henry on 6/14/23.
//
#include <hagame/common/components/topDownPlayerController.h>
#include <hagame/graphics/debug.h>
#include <hagame/graphics/components/spriteSheetAnimator.h>
#include <hagame/utils/profiler.h>
#include "ai.h"
#include "actor.h"
#include "../enemies/behaviors/randomLocation.h"
#include "../enemies/behaviors/moveOnPath.h"

using namespace hg;
using namespace graphics;
using namespace hg::utils;
using namespace hg::utils::bt;

AI::AI(EnemyType type, Tilemap *tilemap, GameState* gameState):
    m_type(type),
    m_tilemap(tilemap),
    m_pathfinding([&](utils::PathFinding::Node node) {
        return m_tilemap->findNeighbors(0, node);
    })
{

    m_state.game = gameState;

    m_behavior = std::make_unique<hg::utils::BehaviorTree<EnemyState>>();
    CreateBehavior(type, m_behavior.get());
}

void AI::idle() {
    direction = Vec2::Zero();
}

void AI::patrol() {
    direction = Vec2(1, 0);
}

void AI::pursue() {

    Actor* actor = entity->getComponent<Actor>();
    actor->attack = false;

    if (target != nullptr) {
        auto distance = (target->transform.position - entity->transform.position).magnitude();
        if (distance < attackWithin && canSee(target)) {
            state = State::Attack;
            return;
        }

        Vec2 pos = entity->transform.position.resize<2>();

        if (m_hasTargetPos) {

            direction = m_targetPos - pos;
            direction.normalize();
        }

        findPath(target->transform.position);

        if (m_path.has_value() && m_path.value().size() > 1) {

            m_hasTargetPos = true;

            bool foundTargetPos = false;

            for (int i = 0; i < m_path.value().size(); i++) {
                auto pathPos =  m_tilemap->getPos(m_path.value()[i]) + m_tilemap->tileSize() * 0.5;
                auto ray = math::Ray(entity->transform.position, pathPos.resize<3>() - entity->transform.position);
                float t;
                auto hit = m_tilemap->raycast(0, ray, t);
                //Debug::DrawRay(ray, Color::red(), 5);
                if (!hit.has_value()) {
                    m_targetPos = pathPos;
                    foundTargetPos = true;
                } else {
                    //Debug::DrawCircle(hit.value().position.x(), hit.value().position.y(), 10, Color::red());
                    break;
                }
            }

            if (!foundTargetPos) {
                m_targetPos = m_tilemap->getPos(m_path.value()[2]) + m_tilemap->tileSize() * 0.5;
            }
        }
    }
}

void AI::attack() {

    direction = Vec2::Zero();
    Vec3 delta = target->transform.position - entity->transform.position;
    float distance = delta.magnitude();

    if (target != nullptr) {
        if (!canSee(target) || distance > attackWithin) {
            state = State::Pursue;
            return;
        } else {
            Actor* actor = entity->getComponent<Actor>();
            actor->attack = true;
            actor->aimDirection = delta.resize<2>().normalized();
        }
    }
}

void AI::onUpdate(double dt) {

    hg::utils::Profiler::Start("AI::onUpdate");

    // m_size = entity->getComponentInChildren<hg::graphics::components::SpriteSheetAnimator>()->quad->size();

    m_dt = dt;

    m_state.entity = entity;

    m_behavior->tick(dt, &m_state);

    /*

    if (m_isPathfinding) {
        int ticks = 0;
        while (ticks++ < pathfindingTicksPerFrame) {
            m_pathfinding.tick();
            if (m_pathfinding.finished()) {
                break;
            }
        }

        if (m_pathfinding.finished()) {
            m_isPathfinding = false;

            if (m_pathfinding.m_foundPath) {
                m_path = m_pathfinding.constructPath(m_pathfinding.m_current);
            } else {
                m_path = std::nullopt;
            }
        }
    } else {
        switch (state) {
            case State::Idle:
                idle();
                break;
            case State::Patrol:
                patrol();
                break;
            case State::Pursue:
                pursue();
                break;
            case State::Attack:
                attack();
                break;
            default:
                throw std::runtime_error("Unhandled state!");
        }
    }

     */

    hg::utils::Profiler::End("AI::onUpdate");
}

bool AI::isStuck() {
    auto size = entity->getComponent<Actor>()->size + m_tilemap->tileSize();
    auto pos = entity->transform.position.resize<2>();
    auto rect = Rect(pos - size * 0.5, size);

    if (m_tilemap->isColliding(0, rect)) {
        return true;
    }

    return false;
}

void AI::findPath(hg::Vec3 pos) {
    if (m_isPathfinding) {
        return;
    }

    Vec2i start = m_tilemap->getIndex(entity->transform.position.resize<2>());
    Vec2i end = m_tilemap->getIndex(pos.resize<2>());

    m_pathfinding.start(start, end);

    m_isPathfinding = true;
}
