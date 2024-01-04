//
// Created by henry on 4/22/23.
//

#include "imgui.h"
#include "runtime.h"
#include "../common/constants.h"
#include "../components/projectile.h"

#include "../scifiGame.h"

#include <hagame/core/game.h>
#include <hagame/utils/profiler.h>

#include "../components/ai.h"

#include "../systems/renderer.h"
#include "../systems/player.h"
#include "../systems/enemies.h"
#include "../systems/actors.h"
#include "../systems/weapons.h"
#include "../systems/items.h"
#include "../systems/audio.h"

#include "../imgui_node.h"
#include "../systems/props.h"
#include "../components/light.h"


using namespace hg::utils;
using namespace hg::graphics;

void Runtime::loadLevel(std::string level) {

    for (const auto& entity : entities.groups.getEntities(ENEMY_GROUP)) {
        entities.remove(entity);
    }

    auto config = MultiConfig::Parse(level);

    auto items = getSystem<Items>();
    auto props = getSystem<Props>();

    std::cout << "ITEMS ID = " << Items::id << "\n";

    items->spawn(items->get("Wrench"), m_state->randomTilemapPos());
    auto lever = props->spawn(props->get("Lever"), m_state->randomTilemapPos());
    lever->getComponent<Prop>()->addRequirement("off", "Wrench");
}


void Runtime::onInit() {

    m_state->scene = this;

    addSystem<AudioSystem>();
    addSystem<Actors>(m_state.get());
    addSystem<Renderer>(m_window, m_state.get());
    addSystem<Enemies>(m_state.get());
    addSystem<Weapons>(m_state.get());
    addSystem<Items>()->load(MultiConfig::Parse(hg::ASSET_DIR + "items.hg"));
    addSystem<Props>()->load(MultiConfig::Parse(hg::ASSET_DIR + "props.hg"));
    addSystem<Player>(m_window, m_state.get());

    for (const auto& file : d_listFiles(hg::ASSET_DIR + "particles")) {
        auto parts = f_getParts(file);
        ParticleEmitterSettings settings;
        settings.load(Config::Parse(f_readLines(file)));
        m_state->particles.set(parts.name, settings);
        std::cout << "LOADING PARTICLE: " << parts.name << " (" << file << ")\n";
    }

    loadLevel(hg::ASSET_DIR + "levels/level_4.hgtmp");

    m_window->input.keyboardMouse.events.subscribe(hg::input::devices::KeyboardEvent::KeyPressed, [&](auto keyPress) {
        if (keyPress.key == '`') {
            m_console->toggle();
            m_state->paused = m_console->isOpen() || m_state->menuOpen;
        }

        if (!m_console->isOpen()) {
            return;
        }

        if (keyPress.key == hg::input::devices::KeyboardMouse::BACKSPACE) {
            m_console->backspace();
        }

        if (keyPress.key == hg::input::devices::KeyboardMouse::ENTER) {
            m_console->submit();
        }

        if (keyPress.key == hg::input::devices::KeyboardMouse::UP) {
            m_console->prevHistory();
        }

        if (keyPress.key == hg::input::devices::KeyboardMouse::DOWN) {
            m_console->nextHistory();
        }
    });

    m_window->input.keyboardMouse.events.subscribe(hg::input::devices::KeyboardEvent::TextInput, [&](auto keyPress) {
        if (m_console->status() == Console::Status::Open) {
            m_console->newChar(keyPress.key);
        }
    });

    m_console = std::make_unique<Console>(hg::getFont("8bit"));
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

    m_console->registerCommand("entities", [&](auto args) {
        entities.forEach([&](auto entity) {
            m_console->putLine(entity->name + " <" + std::to_string(entity->id()) + ">");
        });
        return 0;
    });

    m_console->registerCommand("help", [&](auto args) {
        for (const auto& [cmd, def] : m_console->m_commands) {
            m_console->putLine(cmd + " " + def.help());
        }
        return 0;
    });
}

void Runtime::onFixedUpdate(double dt) {

    if (getSystem<Enemies>()->size() == 0) {
        nextWave();
    }
}

void Runtime::onUpdate(double dt) {

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
    getSystem<Renderer>()->setWindowSize(size);
}

void Runtime::renderUI(double dt) {

    Profiler::Start("Runtime::renderUI");

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

    m_entityTree.render(this, entities.root.get());

    /*

    auto ss = ((Game*)game())->player()->getSource(m_source);
    if (ss) {
        auto settings = ss->settings();
        ImGui::DragFloat("Pitch", &settings.pitch, 0.01, 0.0, 2);
        ImGui::DragFloat("Gain", &settings.gain, 0.01, 0.0, 1);
        // settings.position[0] = m_window->input.keyboardMouse.mouse.position[0] * 1000;
        ((Game*)game())->player()->updateSource(m_source, settings);
    }

     */


    ImGui::End();

    //bool open;
    //ShowExampleAppCustomNodeGraph(&open);

    Profiler::End("Runtime::renderUI");
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
    int m = 0;

    for (int i = 0; i < y + m_state->wave * m; i++) {
        auto entity = enemies->spawn(EnemyType::Slime, m_state->randomTilemapPos().resize<3>());
        entity->getComponent<AI>()->target = player->player;
    }
}

Console *Runtime::console() {
    return m_console.get();
}

