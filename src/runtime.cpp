//
// Created by henry on 4/22/23.
//

#include "imgui.h"
#include "runtime.h"
#include "constants.h"
#include "components/projectile.h"

#include <hagame/core/game.h>
#include <hagame/utils/profiler.h>

#include "components/ai.h"

#include "systems/renderer.h"
#include "systems/player.h"
#include "systems/enemies.h"
#include "systems/actors.h"
#include "systems/weapons.h"
#include "imgui_node.h"

using namespace hg;
using namespace hg::utils;
using namespace hg::graphics;

void Runtime::loadLevel(std::string level) {

    m_state->tilemap->clear();
    auto config = MultiConfig::Parse(level);

    m_state->tilemap->load(config);
    m_state->tilemap->zIndex(1);

}


void Runtime::onInit() {

    addSystem<Actors>(m_state.get());
    addSystem<Renderer>(m_window, m_state.get());
    addSystem<Player>(m_window, m_state.get());
    addSystem<Enemies>(m_state.get());
    addSystem<Weapons>(m_state.get());

    for (const auto& file : d_listFiles(ASSET_DIR + "particles")) {
        auto parts = f_getParts(file);
        ParticleEmitterSettings settings;
        settings.load(Config::Parse(f_readLines(file)));
        m_state->particles.set(parts.name, settings);
        std::cout << "LOADING PARTICLE: " << parts.name << " (" << file << ")\n";
    }

    loadLevel(hg::ASSET_DIR + "levels/level_1.hgtmp");
}

void Runtime::onFixedUpdate(double dt) {
    if (getSystem<Enemies>()->size() == 0) {
        nextWave();
    }
}

void Runtime::onUpdate(double dt) {

    Debug::DrawCircle(0, 0, 100, Color::blue());

    renderUI(dt);
    render(dt);
    Profiler::Render();
}

void Runtime::setSize(hg::Vec2i size) {
    // m_camera.size = size;
}

void Runtime::renderUI(double dt) {

    Profiler::Start();

    ImGui::Begin("Runtime Settings");
    ImGui::Text(("FPS: " + std::to_string(1.0f / dt)).c_str());
    ImGui::Text(("Fixed Tick Rate: " + std::to_string(1.0f / game()->fixedDt())).c_str());
    ImGui::SliderInt("Ticks per Second", &game()->fixedTicksPerSecond, 1, 120);
    ImGui::DragFloat("Zoom", &m_state->zoom, 0.1f, 0.1f, 3.0f);

    getSystem<Player>()->ui();

    ImGui::Separator();
    ImGui::Text("Runtime Parameters");
    ImGui::Checkbox("Debug Render", &m_state->params.debugRender);
    ImGui::Checkbox("Player Invincible", &m_state->params.invincible);
    ImGui::Checkbox("VSync", &m_state->params.vsync);

    ImGui::End();

    //bool open;
    //ShowExampleAppCustomNodeGraph(&open);

    Profiler::End();
}

void Runtime::render(double dt) {

}

GameState* Runtime::state() {
    return m_state.get();
}

void Runtime::nextWave() {
    m_state->wave++;
    auto enemies = getSystem<Enemies>();
    auto player = getSystem<Player>();

    int y = 1;
    int m = 1;

    for (int i = 0; i < y + m_state->wave * m; i++) {
        auto entity = enemies->spawn(EnemyType::Slime, m_state->randomTilemapPos().resize<3>());
        entity->getComponent<AI>()->target = player->player;
    }
}

