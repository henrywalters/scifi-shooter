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

    for (const auto& entity : entities.groups.getEntities(ENEMY_GROUP)) {
        entities.remove(entity);
    }

    auto config = MultiConfig::Parse(level);

    m_state->tilemap->load(config);
    m_state->tilemap->zIndex(1);

}


void Runtime::onInit() {

    m_state->scene = this;

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

    m_window->input.keyboardMouse.events.subscribe(input::devices::KeyboardEvent::KeyPressed, [&](auto keyPress) {
        if (keyPress.key == '`') {
            m_console->toggle();
            m_state->paused = m_console->isOpen() || m_state->menuOpen;
        }

        if (!m_console->isOpen()) {
            return;
        }

        if (keyPress.key == input::devices::KeyboardMouse::BACKSPACE) {
            m_console->backspace();
        }

        if (keyPress.key == input::devices::KeyboardMouse::ENTER) {
            m_console->submit();
        }

        if (keyPress.key == input::devices::KeyboardMouse::UP) {
            m_console->prevHistory();
        }

        if (keyPress.key == input::devices::KeyboardMouse::DOWN) {
            m_console->nextHistory();
        }
    });

    m_window->input.keyboardMouse.events.subscribe(input::devices::KeyboardEvent::TextInput, [&](auto keyPress) {
        if (m_console->status() == Console::Status::Open) {
            m_console->newChar(keyPress.key);
        }
    });

    m_console = std::make_unique<Console>(getFont("8bit"));
    m_console->registerCommand("test", [&](auto args) {
        for (int i = 0; i < std::get<int>(args[0].value); i++) {
            m_console->putLine(std::to_string(i));
        }
        return 0;
    }, {Console::ArgType::Int});

    m_console->registerCommand("spawn", [&](auto args) {

        EnemyType type;

        auto name = std::get<std::string>(args[0].value);
        auto count = std::get<int>(args[1].value);

        if (name == "slime") {
            type = EnemyType::Slime;
        } else {
            m_console->putLine("Invalid enemy type: " + name);
            return 1;
        }

        for (int i = 0; i < count; i++) {
            auto entity = getSystem<Enemies>()->spawn(type, m_state->randomTilemapPos().resize<3>());
            entity->getComponent<AI>()->target = getSystem<Player>()->player;
        }

        return 0;
    }, {Console::ArgType::String, Console::ArgType::Int});

    m_console->registerCommand("load_level", [&](auto args) {
        try {
            loadLevel(hg::ASSET_DIR + "levels/" + std::get<std::string>(args[0].value));
            return 0;
        } catch (std::exception& e) {
            m_console->putLine("Error: " + std::string(e.what()));
            return 1;
        }

    }, {Console::ArgType::String});

    m_console->registerCommand("clear_history", [&](auto args) {
        hg::utils::f_write(m_console->HISTORY_FILE, "");
        return 0;
    });
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

    if (m_window->input.keyboardMouse.keyboard.escPressed) {
        m_state->menuOpen = !m_state->menuOpen;
        m_state->paused = m_console->isOpen() || m_state->menuOpen;
    }

    m_console->update(dt);
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
    ImGui::Checkbox("Paused?", &m_state->paused);

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

    int y = 0;
    int m = 1;

    for (int i = 0; i < y + m_state->wave * m; i++) {
        auto entity = enemies->spawn(EnemyType::Slime, m_state->randomTilemapPos().resize<3>());
        entity->getComponent<AI>()->target = player->player;
    }
}

Console *Runtime::console() {
    return m_console.get();
}

