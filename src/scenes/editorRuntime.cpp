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

    for (const auto& file : d_listFiles(hg::ASSET_DIR + "particles")) {
        auto parts = f_getParts(file);
        hg::graphics::ParticleEmitterSettings settings;
        settings.load(Config::Parse(f_readLines(file)));
        m_state->particles.set(parts.name, settings);
    }

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
    /*
    math::LineSegment line(Vec3(-10, -10, 0), Vec3(10, 10, 0));
    Circle circle(getSystem<Renderer>()->getMousePos(), 1);
    Rect rect(getSystem<Renderer>()->getMousePos(), Vec2(2, 2));

    auto displayHit = [](std::optional<math::collisions::Hit> hit) {
        if (hit.has_value()) {
            graphics::Debug::DrawLine(hit.value().position.resize<2>(),
                                      (hit.value().position + hit.value().normal * hit.value().depth).resize<2>(),
                                      graphics::Color::red(), 2. / 64.);
        }
    };

    auto hit = math::collisions::checkCircleAgainstLine(circle, line);

    graphics::Debug::DrawCircle(circle.center.x(), circle.center.y(), circle.radius,
                                hit.has_value() ? graphics::Color::red() : graphics::Color::blue(), 2. / 64.0);

    graphics::Debug::DrawRect(rect, graphics::Color::blue(), 2. / 64.);

    graphics::Debug::DrawLine(line.a.resize<2>(), line.b.resize<2>(), graphics::Color::green(), 2. / 64.0);
    displayHit(hit);

    Circle circleTest(Vec2(0, 0), 2);

    Rect rectTest(Vec2(0, 5), Vec2(3, 5));

    graphics::Debug::DrawCircle(circleTest.center.x(), circleTest.center.y(), circleTest.radius,graphics::Color::blue(), 2. / 64.0);
    graphics::Debug::DrawRect(rectTest, graphics::Color::blue(), 2. / 64. );

    hit = math::collisions::checkCircleAgainstCircle(circle, circleTest);
    displayHit(hit);

    hit = math::collisions::checkCircleAgainstRect(circle, rectTest);
    displayHit(hit);

    hit = math::collisions::checkRectAgainstRect(rect, rectTest);
    displayHit(hit);
    */
}

void EditorRuntime::onActivate() {
    m_state->levelGeometry.clear();
}
