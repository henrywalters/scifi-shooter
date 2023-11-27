//
// Created by henry on 8/23/23.
//
#include "renderer.h"
#include "../components/actor.h"
#include "player.h"
#include "enemies.h"
#include <iostream>
#include <hagame/math/components/rectCollider.h>
#include <hagame/math/components/circleCollider.h>
#include <hagame/graphics/components/spriteSheetAnimator.h>
#include <hagame/graphics/shaders/texture.h>
#include <hagame/graphics/shaders/particle.h>
#include <hagame/graphics/shaders/text.h>
#include <hagame/graphics/resolution.h>
#include "../runtime.h"
#include "../components/item.h"

using namespace hg;
using namespace hg::graphics;
using namespace hg::utils;
using namespace hg::math::collisions;
using namespace hg::math::components;

Renderer::Renderer(Window* window, GameState* state):
        m_state(state),
        m_quad(window->size().cast<float>(), Vec2(0, 0), true),
        m_mesh(&m_quad),
        m_laser(primitives::Line({Vec3::Zero(), Vec3::Zero()})),
        m_window(window),
        m_laserDisc(3, 10)
{
    m_quad.centered(false);
    m_mesh.update(&m_quad);

    m_laserMesh = std::make_unique<MeshInstance>(&m_laser);
    m_laserDiscMesh = std::make_unique<MeshInstance>(&m_laserDisc);

    for (int i = 0; i < 4; i++) {
        m_crossHairQuads[i] = primitives::Quad();
        m_crossHairQuads[i].centered(true);
        m_crossHairMeshes[i] = std::make_unique<MeshInstance>(&m_crossHairQuads[i]);
    }
}

void Renderer::setWindowSize(hg::Vec2i size) {
    m_camera.size = size.cast<float>();
    m_quad.size(size.cast<float>());
}

void Renderer::onInit() {
    m_camera.zoom = m_state->zoom;
    m_camera.size = GAME_SIZE.copy().cast<float>();
    m_camera.centered = true;
    m_renderPasses.create(RenderMode::Color, GAME_SIZE);

    m_aspectRatio = (float) GAME_SIZE[0] / GAME_SIZE[1];

    // m_window->setMouseVisible(false);

    auto colorShader = getShader("color");
    auto textShader = getShader(TEXT_SHADER.name);
    auto font = getFont("8bit");

    Debug::Initialize(colorShader, textShader, font);

    m_wave = TextBuffer(font, "Wave 0", Vec3(m_window->size().x() / 2, m_window->size().y() - 50, 0), TextHAlignment::Center);
    m_enemies = TextBuffer(font, "0 Enemies Remaining", Vec3(m_window->size().x() / 2, m_window->size().y() - 75, 0), TextHAlignment::Center);
    m_weapon = TextBuffer(font, "Shotgun", Vec3(m_window->size().x() - 200, 50), Vec3(200, 200, 0), TextHAlignment::Right);
    m_ammo = TextBuffer(font, "10 / 12", Vec3(m_window->size().x() - 200, 75), Vec3(200, 200, 0),TextHAlignment::Right);
}

void Renderer::onBeforeUpdate() {

    // m_window->setMouseVisible(m_state->paused);

    m_camera.zoom = m_state->zoom;

    m_window->color(m_state->tilemap->background);
    m_renderPasses.bind(RenderMode::Color);
    m_renderPasses.clear(RenderMode::Color, m_state->tilemap->background);

    Debug::ENABLED = m_state->params.debugRender;

    auto shader = getShader(TEXT_SHADER.name);
    shader->use();

    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("view", Mat4::Identity());

    shader = getShader("color");
    shader->use();

    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());

}

void Renderer::onUpdate(double dt) {
    Profiler::Start();

    m_window->setVSync(m_state->params.vsync);

    Runtime* runtime = (Runtime*) scene;

    auto player = scene->getSystem<Player>();
    auto weapon = player->player->getComponent<Actor>()->weapons.getWeapon();

    if (weapon) {
        if (m_weapon.text() != weapon->settings.name) {
            m_weapon.text(weapon->settings.name);
        }

        std::string ammoText = weapon->settings.infinite ? "Inf" : std::to_string(weapon->ammoInClip()) + " / " + std::to_string(weapon->ammo());
        if (m_ammo.text() != ammoText) {
            m_ammo.text(ammoText);
        }
    }

    std::string enemyText = std::to_string(scene->getSystem<Enemies>()->size()) + " Enemies Remain";
    if (enemyText != m_enemies.text()) {
        m_enemies.text(enemyText);
    }

    std::string waveText = "Wave " + std::to_string(m_state->wave);
    if (m_wave.text() != waveText) {
        m_wave.text(waveText);
    }


    auto shader = getShader(TEXTURE_SHADER.name);
    shader->use();
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());

    scene->entities.forEach<Sprite>([&](auto sprite, auto entity) {
        shader->setMat4("model", entity->model());
        getTexture(sprite->texture)->bind();
        sprite->mesh()->render();
    });

    scene->entities.forEach<components::SpriteSheetAnimator>([&](auto sprites, auto entity) {
        sprites->update(dt);
        shader->setMat4("model", entity->model());
        SpriteSheet* sheet = (SpriteSheet*) sprites->player->get();
        if (!sheet) {
            return;
        }
        auto rect = sheet->getRect();
        sprites->quad->texOffset(rect.pos);
        sprites->quad->texSize(rect.size);
        sprites->mesh()->update(sprites->quad.get());
        sheet->texture()->bind();
        sprites->mesh()->render();
    });

    scene->entities.forEach<Actor>([&](Actor* actor, hg::Entity* entity) {
        Vec3 dir = actor->direction.resize<3>().normalized();
        Vec3 a = math::Quaternion<float>(actor->fov * -0.5, Vec3::Face()).rotatePoint(dir);
        Vec3 b = math::Quaternion<float>(actor->fov * 0.5, Vec3::Face()).rotatePoint(dir);
        Debug::DrawLine(math::LineSegment(entity->transform.position, entity->transform.position + dir * 100), Color::blue());
        Debug::DrawLine(math::LineSegment(entity->transform.position, entity->transform.position + a * 100), Color::blue());
        Debug::DrawLine(math::LineSegment(entity->transform.position, entity->transform.position + b * 100), Color::blue());
    });

    scene->entities.forEach<RectCollider>([&](RectCollider* coll, Entity* entity) {
        Debug::DrawRect(Rect(coll->rect.pos + entity->position().resize<2>(), coll->rect.size), Color::blue());
    });

    scene->entities.forEach<CircleCollider>([&](CircleCollider* coll, Entity* entity) {
        Vec2 pos = entity->transform.position.resize<2>();
        Debug::DrawCircle(pos[0], pos[1], coll->circle.radius, Color::blue());
    });

    shader = getShader("color");
    shader->use();

    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());

    m_state->tilemap->render(TileMode::Color, shader);

    scene->entities.forEach<HealthBar>([&](HealthBar* health, hg::Entity* entity) {
        health->render(getShader("color"));
    });

    shader->setMat4("model", Mat4::Identity());
    shader->setVec4("color", Color::red());
    m_laserMesh->render();
    m_laserDiscMesh->render();

    for (int i = 0; i < 4; i++) {
        m_crossHairMeshes[i]->render();
    }

    shader = getShader(PARTICLE_SHADER.name);
    shader->use();
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());
    shader->setMat4("model", Mat4::Identity());

    scene->entities.forEach<ParticleEmitterComponent>([&](auto emitter, auto entity) {
        emitter->render(shader);
    });

    shader = getShader(TEXT_BUFFER_SHADER.name);
    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("model", Mat4::Identity());
    shader->setVec4("textColor", Color::white());

    m_wave.render();
    m_weapon.render();
    m_ammo.render();
    m_enemies.render();

    shader = getShader("color");
    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("model", Mat4::Identity());

    runtime->console()->render();

    m_renderPasses.render(RenderMode::Color, 1);

    shader = getShader(TEXTURE_SHADER.name);
    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("model", Mat4::Identity());

    m_renderPasses.get(RenderMode::Color)->texture->bind();
    m_mesh.render();

    Profiler::End();
}

void Renderer::setCameraPosition(Vec3 pos) {
    m_camera.transform.position = pos;
}

hg::Vec2 Renderer::getMousePos(hg::Vec2 rawMousePos) {
    return m_camera.getGamePos(rawMousePos);
}

void Renderer::onAfterUpdate() {
    Debug::Render();
}

void Renderer::onFixedUpdate(double dt) {
    Profiler::Start();
    scene->entities.forEach<ParticleEmitterComponent>([&](auto emitter, auto entity) {
        if (emitter->emitter()->finished()) {
            scene->entities.remove(entity);
            return;
        }

        emitter->update(dt);
    });
    Profiler::End();
}

void Renderer::setLaserPointer(hg::Vec3 start, hg::Vec3 end) {
    m_laser.clearPoints();
    m_laser.addPoint(start);
    m_laser.addPoint(end);
    m_laserMesh->update(&m_laser);

    m_laserDisc.offset(end);
    m_laserDiscMesh->update(&m_laserDisc);
}

void Renderer::setCrossHair(hg::Vec2 pos, float innerRadius, float outerRadius) {

    float height = outerRadius - innerRadius;
    float mid = innerRadius + height * 0.5;

    for (int i = 0; i < 4; i++) {

        Vec2 size = i % 2 == 0 ?
            Vec2(CROSSHAIR_WIDTH, height) :
            Vec2(height, CROSSHAIR_WIDTH);

        Vec2 offset = i % 2 == 0 ?
            Vec2(0, mid * (-1 * (i != 0) + (i == 0))) :
            Vec2(mid * (-1 * (i != 1) + (i == 1)), 0);

        m_crossHairQuads[i].setSizeAndOffset(size, pos + offset);
    }

    for (int i = 0; i < 4; i++) {
        m_crossHairMeshes[i]->update(&m_crossHairQuads[i]);
    }
}


