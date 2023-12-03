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
#include "../components/light.h"

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
        m_laserDisc(0.2, 10),
        m_light({}, Vec3::Zero(), 1000),
        m_lightMesh(&m_light),
        m_lightTexture((AspectRatio{(float)GAME_SIZE[0], (float)GAME_SIZE[1]}).getViewport(Vec2(100.0, 100.0)).size.cast<int>())
{
    m_mesh.update(&m_quad);
    m_laser.thickness(0.1);
    m_laserMesh = std::make_unique<MeshInstance>(&m_laser);
    m_laserDiscMesh = std::make_unique<MeshInstance>(&m_laserDisc);

    for (int i = 0; i < 4; i++) {
        m_crossHairQuads[i] = primitives::Quad();
        m_crossHairQuads[i].centered(true);
        m_crossHairMeshes[i] = std::make_unique<MeshInstance>(&m_crossHairQuads[i]);
    }
}

void Renderer::setWindowSize(hg::Vec2i size) {
    //m_camera.windowSize = size.cast<float>();
    //hg::Rect viewport = m_camera.getViewport();
    //m_quad.offset(viewport.pos);
    //m_quad.size(viewport.size);
    //m_renderPasses.resize(RenderMode::Color, viewport.size.cast<int>());
    //m_quad.size(size.cast<float>());
    //m_mesh.update(&m_quad);

    auto viewport = m_state->aspectRatio.getViewport(size.cast<float>());
    m_quad.offset(viewport.pos);
    m_quad.size(viewport.size);
    m_mesh.update(&m_quad);
    //m_renderPasses.resize(RenderMode::Color, viewport.size.cast<int>());
    m_wave.pos(Vec3(m_window->size().x() / 2, m_window->size().y() - 50, 0));
    m_enemies.pos(Vec3(m_window->size().x() / 2, m_window->size().y() - 75, 0));
    m_weapon.pos(Vec3(m_window->size().x() - 200, 50));
    m_ammo.pos(Vec3(m_window->size().x() - 200, 75));

}

void Renderer::onInit() {

    //m_camera.windowSize = Vec2(GAME_SIZE[0], GAME_SIZE[1]);

    m_camera.zoom = m_state->zoom;

    m_camera.size = Vec2(m_state->aspectRatio.ratio(), 1) * m_state->metersPerViewport;
    m_camera.centered = true;

    // m_renderPasses.create(RenderMode::Display, m_window->size());

    m_quad.centered(false);
    auto viewport = m_state->aspectRatio.getViewport(m_window->size().cast<float>());
    m_quad.offset(viewport.pos);
    m_quad.size(viewport.size);
    m_mesh.update(&m_quad);

    m_renderPasses.create(RenderMode::Color, HD);
    m_renderPasses.create(RenderMode::Lighting, HD);
    m_renderPasses.create(RenderMode::Debug, HD);
    m_renderPasses.create(RenderMode::UI, HD);

    m_aspectRatio = (float) HD[0] / HD[1];

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
    m_camera.size = Vec2(m_state->aspectRatio.ratio(), 1) * m_state->metersPerViewport;
    //m_camera.metersPerViewport = m_state->metersPerViewport;

    m_window->color(m_state->tilemap->background);

    glViewport(0, 0, HD[0], HD[1]);

    m_renderPasses.clear(RenderMode::Color, Color(0.5f, 0.5f, 0.5f));
    m_renderPasses.clear(RenderMode::Lighting, Color::black());
    m_renderPasses.clear(RenderMode::Debug, Color::black());
    m_renderPasses.clear(RenderMode::UI, Color::black());

    m_renderPasses.bind(RenderMode::Color);

    Debug::ENABLED = m_state->params.debugRender;

    auto shader = getShader(TEXT_SHADER.name);
    shader->use();

    //Rect viewport = m_camera.getViewport();
    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("view", Mat4::Identity());

    shader = getShader("color");
    shader->use();
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());

}

void Renderer::onUpdate(double dt) {

    m_window->setVSync(m_state->params.vsync);

    Profiler::Start();

    Profiler::Start("Color Pass");
    colorPass(dt);
    Profiler::End("Color Pass");

    Profiler::Start("Light Pass");
    lightPass(dt);
    Profiler::End("Light Pass");

    Profiler::Start("Debug Pass");
    debugPass(dt);
    Profiler::End("Debug Pass");

    Profiler::Start("UI Pass");
    uiPass(dt);
    Profiler::End("UI Pass");

    Profiler::Start("Combined Pass");
    combinedPass(dt);
    Profiler::End("Combined Pass");

    Profiler::End();
}

void Renderer::setCameraPosition(Vec3 pos) {
    m_camera.transform.position = pos;
}

hg::Vec2 Renderer::getMousePos(hg::Vec2 rawMousePos) {
    Rect viewport = m_state->aspectRatio.getViewport(m_window->size().cast<float>());
    Vec2 relMousePos = (rawMousePos - viewport.pos) * (m_state->metersPerViewport / viewport.size[1]);
    return m_camera.getGamePos(relMousePos);
}

void Renderer::onAfterUpdate() {

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

void Renderer::colorPass(double dt) {

    m_renderPasses.bind(RenderMode::Color);

    int pIndex = 0;
    for (const auto& poly : m_state->levelGeometry) {
        Color color = m_state->randomColorsLUT[pIndex++ % m_state->randomColorsLUT.size()];
        for (const auto& edge : poly) {
            Debug::DrawLine(edge.a[0], edge.a[1], edge.b[0], edge.b[1], color, 0.01);
        }
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
        Debug::DrawLine(math::LineSegment(entity->transform.position, entity->transform.position + dir * 100), Color::blue(), 0.01);
        Debug::DrawLine(math::LineSegment(entity->transform.position, entity->transform.position + a * 100), Color::blue(), 0.01);
        Debug::DrawLine(math::LineSegment(entity->transform.position, entity->transform.position + b * 100), Color::blue(), 0.01);
    });

    scene->entities.forEach<RectCollider>([&](RectCollider* coll, Entity* entity) {
        Debug::DrawRect(Rect(coll->rect.pos + entity->position().resize<2>(), coll->rect.size), Color::blue(), 0.01);
    });

    scene->entities.forEach<CircleCollider>([&](CircleCollider* coll, Entity* entity) {
        Vec2 pos = entity->transform.position.resize<2>();
        Debug::DrawCircle(pos[0], pos[1], coll->circle.radius, Color::blue(), 0.01);
    });

    shader = getShader("color");
    shader->use();
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());

    m_state->tilemap->render(TileMode::Color, shader);

    scene->entities.forEach<HealthBar>([&](HealthBar* health, hg::Entity* entity) {
        // health->render(getShader("color"));
    });

    shader = getShader(PARTICLE_SHADER.name);
    shader->use();
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());
    shader->setMat4("model", Mat4::Identity());

    scene->entities.forEach<ParticleEmitterComponent>([&](auto emitter, auto entity) {
        emitter->render(shader);
    });

    m_renderPasses.render(RenderMode::Color, 1);
}

void Renderer::lightPass(double dt) {
    m_renderPasses.bind(RenderMode::Lighting);

    auto shader = getShader("light");
    shader->use();
    shader->setMat4("view", m_camera.view());
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("model", Mat4::Identity());

    scene->entities.forEach<LightComponent>([&](LightComponent* light, Entity* entity) {
        shader->setVec2("origin", entity->transform.position.resize<2>());
        shader->setVec4("color", light->color);
        shader->setFloat("attenuation", light->attenuation);

        if (light->dynamic || light->triangles.vertices.size() == 0) {
            light->computeMesh(m_state->levelGeometry);
        }

        light->mesh.render();
    });

    m_renderPasses.render(RenderMode::Lighting, 1);
}

void Renderer::debugPass(double dt) {

    m_renderPasses.bind(RenderMode::Debug);

    auto shader = getShader("color");
    shader->use();
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());
    Debug::Render();

    shader = getShader("color");
    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("model", Mat4::Identity());

    Runtime* runtime = (Runtime*) scene;
    runtime->console()->render();

    m_renderPasses.render(RenderMode::Debug, 1);
}

void Renderer::uiPass(double dt) {

    m_renderPasses.bind(RenderMode::UI);

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

    auto shader = getShader("color");
    shader->use();
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());
    shader->setMat4("model", Mat4::Identity());
    shader->setVec4("color", Color::red());

    m_laserMesh->render();
    m_laserDiscMesh->render();

    for (int i = 0; i < 4; i++) {
        m_crossHairMeshes[i]->render();
    }

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

    m_renderPasses.render(RenderMode::UI, 1);
}

void Renderer::combinedPass(double dt) {

    glViewport(0, 0, m_window->size()[0], m_window->size()[1]);

    auto shader = getShader("combined");
    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, m_window->size().x(), 0, m_window->size().y(), -100, 100));
    shader->setMat4("model", Mat4::Identity());

    glActiveTexture(GL_TEXTURE0 + 0);
    m_renderPasses.get(RenderMode::Color)->texture->bind();

    glActiveTexture(GL_TEXTURE0 + 1);
    m_renderPasses.get(RenderMode::Lighting)->texture->bind();

    glActiveTexture(GL_TEXTURE0 + 2);
    m_renderPasses.get(RenderMode::Debug)->texture->bind();

    glActiveTexture(GL_TEXTURE0 + 3);
    m_renderPasses.get(RenderMode::UI)->texture->bind();

    m_mesh.update(&m_quad);
    m_mesh.render();

    glActiveTexture(GL_TEXTURE0 + 0);
}


