//
// Created by henry on 8/26/23.
//

#include <hagame/core/assets.h>

#include "imgui.h"
#include <variant>
#include <hagame/graphics/components/spriteSheetAnimator.h>
#include "enemies.h"

#include "../common/utils.h"
#include "../components/ai.h"
#include "player.h"
#include "../common/constants.h"
#include "../components/actor.h"
#include "../enemies/behaviors/tags.h"

using namespace hg;
using namespace hg::utils;
using namespace hg::utils::bt;

Enemies::Enemies(GameState* state):
    m_state(state),
    m_pathfinding([&](PathFinding::Node node) {
        std::vector<PathFinding::Node> nodes;

        for (const auto& position : m_state->tilemap->getLayer(0)->getNeighbors(node.position)) {

            if (m_state->tilemap->getLayer(0)->has(position)) {
                continue;
            }

            PathFinding::Node neighbor;
            neighbor.position = position;
            neighbor.cost = 0;

        }

        return nodes;
    })
{}

Entity* Enemies::spawn(EnemyType type, hg::Vec3 pos) {
    EnemyDef def = ENEMIES.get(type);

    hg::Entity* entity = AddActor(scene, pos + TILE_SIZE.resize<3>() / 2, def.texture, def.size, def.speed);
    auto actor = entity->getComponent<Actor>();
    auto animator = entity->getComponentInChildren<hg::graphics::components::SpriteSheetAnimator>();
    auto aPlayer = animator->player.get();
    aPlayer->addAnimation("slime", getSpriteSheet("slime"));
    aPlayer->trigger("slime");

    entity->getComponent<Actor>()->maxHealth = def.health;
    entity->getComponent<Actor>()->health = def.health;

    entity->name = def.texture;

    scene->addToGroup(ENEMY_GROUP, entity);

    entity->addComponent<AI>(type, m_state->tilemap.get(), m_state);

    auto coll = entity->addComponent<hg::math::components::CircleCollider>();
    coll->circle.radius = def.size.magnitude() * 0.5;

    m_enemies.push_back(entity);

    return entity;
}

void Enemies::onFixedUpdate(double dt) {

    if (m_state->paused) {
        return;
    }

    m_size = 0;
    scene->entities.forEach<AI>([&](AI* ai, hg::Entity* entity) {
        if (!entity) {
            return;
        }

        EnemyState state;
        state.entity = entity;
        state.game = m_state;

        ai->update(dt);
        m_size++;
    });
}

void Enemies::onUpdate(double dt) {
    // renderUI();
}

void Enemies::renderUI() {
    ImGui::Begin("Enemies");
    ImGui::Checkbox("Ignore Player?", &m_state->params.ignorePlayer);
    for (const auto& entity : scene->entities.groups.getEntities(ENEMY_GROUP)) {
        ImGui::Text(entity->name.c_str());
        auto ai = entity->getComponent<AI>();
        if (ai) {
            if (ai->m_behavior->getCurrent()) {
                ImGui::SameLine();
                auto behavior = ai->m_behavior->getCurrent()->operator std::string();
                ImGui::Text(("[" + behavior + "]").c_str());
            }

            for (auto &[key, value]: *ai->m_behavior->context()) {
                ImGui::Text(("\t" + BTagNames[key] + " = " + toString(value)).c_str());
            }
        }

    }
    ImGui::End();
}

