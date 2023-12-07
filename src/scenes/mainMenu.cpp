//
// Created by henry on 12/6/23.
//
#include <hagame/graphics/shaders/text.h>
#include "mainMenu.h"
#include "runtime.h"

using namespace hg;
using namespace hg::graphics;
using namespace hg::utils;
using namespace hg::ui;

MainMenu::MainMenu(hg::graphics::Window *window):
        m_window(window),
        m_state(std::make_unique<GameState>(TILE_SIZE)),
        hg::Scene(),
        m_quad(window->size().cast<float>(), Vec2(0, 0), true),
        m_mesh(&m_quad)
{
    m_quad.centered(false);
    m_mesh.update(&m_quad);
}

void MainMenu::onInit() {

    game()->scenes()->add<Runtime>("runtime", m_window);

    m_renderPasses.create(RenderMode::Color, GAME_SIZE);

    auto font = getFont("8bit");

    m_versionBuffer = TextBuffer(font, "", Vec3(0, m_window->size().y() - 16, 0), TextHAlignment::Left);
    m_versionBuffer.text(BUILD_TAG);

    m_buttons.push_back(
        MenuButton {
    TextButton(Vec2(GAME_SIZE[0] / 2, GAME_SIZE[1] / 2 - 100), font, "Start Game"),
    "runtime"
        }
    );

    m_buttons.push_back(
            MenuButton {
                    TextButton(Vec2(GAME_SIZE[0] / 2, GAME_SIZE[1] / 2), font, "Level Editor"),
                    "level_editor"
            }
    );

    m_buttons.push_back(
            MenuButton {
                    TextButton(Vec2(GAME_SIZE[0] / 2, GAME_SIZE[1] / 2 + 100), font, "Settings"),
                    "settings"
            }
    );

    for (auto& button : m_buttons) {
        button.button.events.subscribe([&](ButtonEvents event) {

            std::cout << button.button.getText() << "\n";
            if (event == ButtonEvents::MouseEnter) {
                m_isHovering = true;
                m_hovering = button.scene;
                std::cout << "ENTER\n";
            } else if (event == ButtonEvents::MouseLeave) {
                m_isHovering = false;
                m_hovering = "";
                std::cout << "EXIT\n";
            } else if (event == ButtonEvents::MouseClick) {
                m_state->gotoScene(game(), button.scene);
            }
        });
    }
}

void MainMenu::onUpdate(double dt) {

    for (auto& button : m_buttons) {
        button.button.update(&m_window->input.keyboardMouse, dt);
    }

    render(dt);
}

void MainMenu::render(double dt) {
    m_window->color(m_state->tilemap->background);
    m_renderPasses.bind(RenderMode::Color);
    m_renderPasses.clear(RenderMode::Color, m_state->tilemap->background);

    auto shader = getShader(TEXT_BUFFER_SHADER.name);

    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("model", Mat4::Identity());

    for (auto& button : m_buttons) {
        shader->setVec4("textColor", m_isHovering && m_hovering == button.scene ? Color::red() : Color::white());
        button.button.render(dt);
    }

    m_renderPasses.render(RenderMode::Color, 1);

    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("model", Mat4::Identity());

    m_renderPasses.get(RenderMode::Color)->texture->bind();
    m_mesh.render();
}

