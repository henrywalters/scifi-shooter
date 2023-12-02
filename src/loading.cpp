//
// Created by henry on 11/9/23.
//
#include "loading.h"
#include "systems/renderer.h"
#include "runtime.h"
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
        m_mesh(&m_quad)
{
    m_quad.centered(false);
    m_mesh.update(&m_quad);
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

    auto defaultFont = hg::loadFont("8bit", hg::ASSET_DIR + "fonts/8bit.ttf");
    defaultFont->fontSize(16);

    m_messageBuffer = TextBuffer(defaultFont.get(), "", Vec3(m_window->size().x() / 2 - 100, m_window->size().y() / 2, 0), TextHAlignment::Left);

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

}

void Loading::onUpdate(double dt) {

    m_window->color(m_state->tilemap->background);
    m_renderPasses.bind(RenderMode::Color);
    m_renderPasses.clear(RenderMode::Color, m_state->tilemap->background);

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
        game()->scenes()->add<Runtime>("runtime", m_window);
        game()->scenes()->activate("runtime");
    }

    std::cout << m_message << "\n";

    float percent = (float) (m_shaderIdx + m_spriteSheetIdx + m_textureIdx) / (m_shaders.size() + m_spriteSheets.size() + m_textures.size());

    auto shader = getShader(TEXT_BUFFER_SHADER.name);

    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("model", Mat4::Identity());
    shader->setVec4("textColor", Color::white());

    m_messageBuffer.text(m_message);
    m_messageBuffer.render();

    m_renderPasses.render(RenderMode::Color, 1);

    shader = getShader(TEXTURE_SHADER.name);
    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("model", Mat4::Identity());

    m_renderPasses.get(RenderMode::Color)->texture->bind();
    m_mesh.render();
}

void Loading::render(double dt) {

}
