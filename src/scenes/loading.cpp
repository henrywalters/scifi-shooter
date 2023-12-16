//
// Created by henry on 11/9/23.
//
#include "loading.h"
#include "../systems/renderer.h"
#include "runtime.h"
#include "mainMenu.h"
#include "../scifiGame.h"
#include <hagame/core/game.h>
#include <hagame/graphics/shaders/text.h>
#include <hagame/graphics/shaders/color.h>
#include <hagame/graphics/shaders/texture.h>
#include <hagame/graphics/shaders/particle.h>

using namespace hg;
using namespace hg::graphics;
using namespace hg::utils;

Loading::Loading(Window *window):
        m_window(window),
        m_state(std::make_unique<GameState>(TILE_SIZE)),
        hg::Scene(),
        m_quad(window->size().cast<float>(), Vec2(0, 0), true),
        m_mesh(&m_quad),
        m_logoQuad(Vec2(GAME_SIZE[0] / 2, GAME_SIZE[0] / 8), Vec2(GAME_SIZE[0] / 2, GAME_SIZE[1] / 2)),
        m_logo(&m_logoQuad)
{
    m_quad.centered(false);
    m_mesh.update(&m_quad);

    m_logoQuad.centered();
    m_logo.update(&m_logoQuad);
}


void Loading::onInit() {

    m_renderPasses.create(RenderMode::Color, GAME_SIZE);

    loadShader(COLOR_SHADER);
    loadShader(TEXTURE_SHADER);
    loadShader(TEXT_SHADER);
    loadShader(TEXT_BUFFER_SHADER);
    loadShader(PARTICLE_SHADER);
    loadShader("light", "shaders/light.vert", "shaders/light.frag");
    loadShader("combined", "shaders/combined.vert", "shaders/combined.frag");
    loadShader("batch_color", "shaders/batch_color.vert", "shaders/batch_color.frag");
    loadShader("batch_texture", "shaders/batch_texture.vert", "shaders/batch_texture.frag");

    setMissingTexture("textures/missing.png");
    loadTexture("logo", "textures/hg_studio.png");

    auto defaultFont = hg::loadFont("8bit", hg::ASSET_DIR + "fonts/8bit.ttf");
    defaultFont->fontSize(16);

    m_messageBuffer = TextBuffer(defaultFont.get(), "", Vec3(0, 0, 0), TextHAlignment::Left);
    m_versionBuffer = TextBuffer(defaultFont.get(), "", Vec3(0, m_window->size().y() - 16, 0), TextHAlignment::Left);
    m_versionBuffer.text(BUILD_TAG);

    for (const auto& file : d_listFiles(ASSET_DIR + "audio", true)) {
        auto parts = f_getParts(file);
        auto name = s_replace(parts.path, ASSET_DIR + "audio/", "") + parts.name;
        m_sounds.push_back(std::make_tuple(name, file));
    }

    for (const auto& file : d_listFiles(ASSET_DIR + "textures", true)) {
        auto parts = f_getParts(file);
        auto name = s_replace(parts.path, ASSET_DIR + "textures/", "") + parts.name;
        if (parts.extension == "png") {
            m_textures.push_back(std::make_tuple(name, file));
        } else if (parts.extension == "hgss") {
            m_spriteSheets.push_back(std::make_tuple(name, file));
        }
    }

    ((ScifiGame*) game())->console = std::make_unique<Console>(getFont("8bit"));
    Console* console = ((ScifiGame*) game())->console.get();

    m_window->input.keyboardMouse.events.subscribe(hg::input::devices::KeyboardEvent::KeyPressed, [&](auto keyPress) {

        Console* console = ((ScifiGame*) game())->console.get();

        if (keyPress.key == '`') {
            console->toggle();
            m_state->paused = console->isOpen() || m_state->menuOpen;
        }

        if (!console->isOpen()) {
            return;
        }

        if (keyPress.key == hg::input::devices::KeyboardMouse::BACKSPACE) {
            console->backspace();
        }

        if (keyPress.key == hg::input::devices::KeyboardMouse::ENTER) {
            console->submit();
        }

        if (keyPress.key == hg::input::devices::KeyboardMouse::UP) {
            console->prevHistory();
        }

        if (keyPress.key == hg::input::devices::KeyboardMouse::DOWN) {
            console->nextHistory();
        }
    });

    m_window->input.keyboardMouse.events.subscribe(hg::input::devices::KeyboardEvent::TextInput, [&](auto keyPress) {
        Console* console = ((ScifiGame*) game())->console.get();
        if (console->status() == Console::Status::Open) {
            console->newChar(keyPress.key);
        }
    });
}

void Loading::onUpdate(double dt) {

    m_window->color(m_state->tilemap->background);
    m_renderPasses.bind(RenderMode::Color);
    m_renderPasses.clear(RenderMode::Color, Color::black());

    if (m_spriteSheetIdx < m_spriteSheets.size()) {
        auto &[name, path] = m_spriteSheets[m_spriteSheetIdx++];
        m_message = "Loading sprite sheet: " + name + " from " + path;
        hg::loadSpriteSheet(name, path);
    } else if (m_textureIdx < m_textures.size()) {
        auto &[name, path] = m_textures[m_textureIdx++];
        m_message = "Loading texture: " + name + " from " + path;
        hg::loadTexture(name, path);
    } else if (m_soundIdx < m_sounds.size()) {
        auto &[name, path] = m_sounds[m_soundIdx++];
        m_message = "Loading sound: " + name + " from " + path;
        hg::loadAudioStream(name, path);
    } else {
        m_message = "Initializing";
        //game()->scenes()->add<Runtime>("runtime", m_window);
        //m_state->gotoScene(game(), "runtime");
        game()->scenes()->add<MainMenu>("main_menu", m_window);
        m_state->gotoScene(game(), "main_menu");
    }

    auto shader = getShader(TEXT_BUFFER_SHADER.name);

    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("model", Mat4::Identity());
    shader->setVec4("textColor", Color::white());

    m_messageBuffer.text(m_message);
    m_messageBuffer.render();

    m_versionBuffer.render();

    shader = getShader(TEXTURE_SHADER.name);
    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("model", Mat4::Identity());
    shader->setVec4("textColor", Color::white());

    getTexture("logo")->bind();
    m_logo.render();

    m_renderPasses.render(RenderMode::Color, 1);

    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("model", Mat4::Identity());

    m_renderPasses.get(RenderMode::Color)->texture->bind();
    m_mesh.render();
}

void Loading::render(double dt) {

}
