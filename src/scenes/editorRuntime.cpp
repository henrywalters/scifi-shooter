//
// Created by henry on 12/17/23.
//
#include "editorRuntime.h"

#include "../systems/renderer.h"
#include "../systems/player.h"
#include "../systems/enemies.h"
#include "../systems/actors.h"
#include "../systems/weapons.h"
#include "../systems/items.h"
#include "../systems/audio.h"
#include "../systems/props.h"

using namespace hg;
using namespace hg::utils;

void EditorRuntime::onInit() {
    m_state->scene = this;

    addSystem<AudioSystem>();
    addSystem<Actors>(m_state.get());
    addSystem<Renderer>(m_window, m_state.get());
    //addSystem<Enemies>(m_state.get());
    addSystem<Weapons>(m_state.get());
    addSystem<Items>()->load(MultiConfig::Parse(hg::ASSET_DIR + "items.hg"));
    addSystem<Props>()->load(MultiConfig::Parse(hg::ASSET_DIR + "props.hg"));
    addSystem<Player>(m_window, m_state.get());

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
}

void EditorRuntime::onFixedUpdate(double dt) {

}

void EditorRuntime::onUpdate(double dt) {

}

void EditorRuntime::onActivate() {
    m_state->levelGeometry.clear();
}
