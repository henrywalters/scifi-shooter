//
// Created by henry on 8/26/23.
//

#include "imgui.h"

#include "enemies.h"

#include "../utils.h"
#include "../components/ai.h"
#include "player.h"
#include "../constants.h"
#include "../components/actor.h"

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

    scene->addToGroup(ENEMY_GROUP, entity);

    entity->addComponent<AI>(type, m_state->tilemap.get(), m_state);

    auto coll = entity->addComponent<hg::math::components::CircleCollider>();
    coll->circle.radius = def.size.magnitude() * 0.5;

    m_enemies.push_back(entity);

    return entity;
}

void Enemies::onFixedUpdate(double dt) {
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
    renderUI();
}

void Enemies::renderUI() {
    ImGui::Begin("Enemies");
    ImGui::Checkbox("Ignore Player?", &m_state->params.ignorePlayer);
    for (const auto& entity : scene->entities.groups.getEntities(ENEMY_GROUP)) {
        ImGui::Text(entity->name.c_str());
        auto ai = entity->getComponent<AI>();
        if (ai) {
            if (ai->m_behavior->getCurrent()) {
                auto behavior = ai->m_behavior->getCurrent()->operator std::string();
                ImGui::Text(behavior.c_str());
            }
        }

    }
    ImGui::End();
}

