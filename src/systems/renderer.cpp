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
#include <hagame/graphics/components/tilemap.h>
#include <hagame/graphics/shaders/texture.h>
#include <hagame/graphics/shaders/particle.h>
#include <hagame/graphics/shaders/text.h>
#include <hagame/graphics/resolution.h>
#include <hagame/audio/components/source.h>
#include "../scenes/runtime.h"
#include "../components/item.h"
#include "../components/light.h"
#include "../components/startPoint.h"
#include "../components/prop.h"
#include "../scifiGame.h"
#include "../components/connection.h"
#include "../components/triggerConnection.h"
#include "../common/weapons.h"
#include "../components/requirementConnection.h"
#include "../components/endPoint.h"

using namespace hg;
using namespace hg::graphics;
using namespace hg::utils;
using namespace hg::math::collisions;
using namespace hg::math::components;

HG_SYSTEM(Graphics, Renderer)

Renderer::Renderer(Window* window, GameState* state, bool editorMode):
    m_editorMode(editorMode),
    m_state(state),
    m_displayQuad(window->size().cast<float>(), Vec2(0, 0), true),
    m_mesh(&m_displayQuad),
    m_laser(primitives::Line({Vec3::Zero(), Vec3::Zero()})),
    m_window(window),
    m_laserDisc(3, 10),
    m_light({}, Vec3::Zero(), 1000),
    m_lightMesh(&m_light),
    //m_quadMesh(&m_quad),
    m_lightTexture((AspectRatio{(float)GAME_SIZE[0], (float)GAME_SIZE[1]}).getViewport(Vec2(100.0, 100.0)).size.cast<int>()),
    m_startQuad(Vec2(1, 1)),
    m_startMesh(&m_startQuad),
    m_animQuad(Vec2(1, 1)),
    m_anim(&m_animQuad)
{
    m_displayQuad.centered(false);
    m_mesh.update(&m_displayQuad);

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
    //m_displayQuad.size(size.cast<float>());
    //m_mesh.update(&m_displayQuad);
    //m_renderPasses.resizeAll(size);
}

void Renderer::onInit() {
    m_camera.zoom = m_state->zoom;
    m_camera.size = GAME_SIZE.div(m_state->pixelsPerMeter).cast<float>();
    m_camera.centered = true;

    glViewport(0, 0, GAME_SIZE[0], GAME_SIZE[1]);
    m_renderPasses.create(RenderMode::Color, GAME_SIZE);
    m_renderPasses.create(RenderMode::Lighting, GAME_SIZE);
    m_renderPasses.create(RenderMode::Debug, GAME_SIZE);
    m_renderPasses.create(RenderMode::UI, GAME_SIZE);
    m_renderPasses.create(RenderMode::Combined, GAME_SIZE);

    auto colorShader = getShader("color");
    auto textShader = getShader(TEXT_SHADER.name);
    auto font = getFont("8bit");

    Debug::Initialize(colorShader, textShader, font);

    m_wave = TextBuffer(font, "Wave 0", Vec3(GAME_SIZE[0] / 2, GAME_SIZE[1] - 50, 0), TextHAlignment::Center);
    m_enemies = TextBuffer(font, "0 Enemies Remaining", Vec3(GAME_SIZE[0] / 2, GAME_SIZE[1] - 75, 0), TextHAlignment::Center);
    m_weapon = TextBuffer(font, "Shotgun", Vec3(GAME_SIZE[0] - 200, 50), Vec3(200, 200, 0), TextHAlignment::Right);
    m_ammo = TextBuffer(font, "10 / 12", Vec3(GAME_SIZE[0] - 200, 75), Vec3(200, 200, 0),TextHAlignment::Right);
    m_profiler = TextBuffer(font, "", Vec3(0, GAME_SIZE[1]), Vec3(GAME_SIZE[0], GAME_SIZE[1], 0), TextHAlignment::Left);
}

void Renderer::onBeforeUpdate() {

    // m_window->setMouseVisible(m_state->paused);


}

void Renderer::onRender(double dt) {

    Profiler::Start("Renderer::onRender");

    m_camera.zoom = m_state->zoom;

    m_window->color(Color::black());

    glViewport(0, 0, GAME_SIZE[0], GAME_SIZE[1]);
    m_renderPasses.clear(RenderMode::Color, Color::black());
    m_renderPasses.clear(RenderMode::Lighting, Color::black());
    m_renderPasses.clear(RenderMode::Debug, Color::black());
    m_renderPasses.clear(RenderMode::UI, Color::black());
    m_renderPasses.clear(RenderMode::Combined, Color::black());

    m_renderPasses.bind(RenderMode::Color);
    glViewport(0, 0, GAME_SIZE[0], GAME_SIZE[1]);

    Debug::ENABLED = m_state->params.debugRender;

    auto shader = getShader(TEXT_SHADER.name);
    shader->use();

    shader->setMat4("projection", Mat4::Orthographic(0, GAME_SIZE[0], 0, GAME_SIZE[1], -100, 100));
    shader->setMat4("view", Mat4::Identity());

    shader = getShader("color");
    shader->use();
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());


    m_window->setVSync(m_state->params.vsync);

    Profiler::Start("Renderer::colorPass");
    colorPass(dt);
    Profiler::End("Renderer::colorPass");
    Profiler::Start("Renderer::lightPass");
    lightPass(dt);
    Profiler::End("Renderer::lightPass");
    Profiler::Start("Renderer::debugPass");
    debugPass(dt);
    Profiler::End("Renderer::debugPass");
    Profiler::End("Renderer::onRender");

    uiPass(dt);
    combinedPass(dt);

}

void Renderer::setCameraPosition(Vec3 pos) {
    m_camera.transform.position = pos;
}

void Renderer::onAfterUpdate() {

}

void Renderer::onFixedUpdate(double dt) {
    Profiler::Start("Renderer::onFixedUpdate");
    scene->entities.forEach<ParticleEmitterComponent>([&](auto emitter, auto entity) {
        if (emitter->emitter()->finished()) {
            scene->entities.remove(entity);
            return;
        }

        emitter->update(dt);
    });
    Profiler::End("Renderer::onFixedUpdate");
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
    glViewport(0, 0, GAME_SIZE[0], GAME_SIZE[1]);

    int pIndex = 0;
    for (const auto& poly : m_state->levelRectangles) {
        Color color = m_state->randomColorsLUT[pIndex++ % m_state->randomColorsLUT.size()];
        //for (const auto& edge : poly) {
        //    Debug::DrawLine(edge.a[0], edge.a[1], edge.b[0], edge.b[1], color, 2.0 / 64.0);
        //}
        Debug::DrawRect(poly, color, 2. / 64.);
    }



    auto shader = getShader(TEXTURE_SHADER.name);
    shader->use();
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());

    m_batchRenderer.quads.clear();
    m_batchRenderer.sprites.clear();

    scene->entities.forEach<Quad>([&](auto quad, auto entity) {
        m_batchRenderer.quads.batch(entity, quad);
    });

    scene->entities.forEach<Sprite>([&](auto sprite, auto entity) {
        m_batchRenderer.sprites.batch(entity, sprite);
    });

    scene->entities.forEach<components::Tilemap>([&](auto tilemap, auto entity) {
        tilemap->tiles.forEach([&](hg::Vec2i index, components::Tile tile) {
            if (tile.type == components::TileType::Color) {
                m_batchRenderer.quads.batch(tilemap->tileSize, tilemap->tileSize * 0.5, tile.color, Mat4::Translation((tilemap->getPos(tile.index)).template resize<3>() + entity->position()));
            }

            if (tile.type == components::TileType::Texture) {
                m_batchRenderer.sprites.batch(tile.texture, tilemap->tileSize, tilemap->tileSize * 0.5, tile.color, Mat4::Translation((tilemap->getPos(tile.index)).template resize<3>() + entity->position()));
            }
        });
    });

    scene->entities.forEach<Prop>([&](Prop* prop, auto entity) {
        if (prop->def) {
            m_batchRenderer.sprites.batch(
                    prop->def->states[prop->stateId].texture,
                    prop->def->size,
                    prop->def->size * 0.5,
                    Color::white(),
                    entity->model()
            );
        }
    });

    scene->entities.forEach<Item>([&](Item* item, auto entity) {
        if (item->def) {
            m_batchRenderer.sprites.batch(
                    item->def->texture,
                    item->def->size,
                    item->def->size * 0.5,
                    Color::white(),
                    entity->model()
            );
        }
    });

    shader = getShader("batch_color");
    shader->use();
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());

    m_batchRenderer.quads.render();

    shader = getShader("batch_texture");
    shader->use();
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());

    m_batchRenderer.sprites.render();

    // Render any animated sprites above other stuff for automatic Z sorting
    shader = getShader(TEXTURE_SHADER.name);
    shader->use();
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());

    scene->entities.forEach<components::SpriteSheetAnimator>([&](auto animator, auto entity) {
        auto animation = (hg::graphics::SpriteSheet*) animator->player->get();
        if (animation) {
            animation->texture()->bind();
            shader->setMat4("model", entity->model());
            auto rect = animation->getRect();
            m_animQuad.texSize(rect.size);
            m_animQuad.texOffset(rect.pos);
            m_anim.update(&m_animQuad);
            m_anim.render();
        }
    });

    scene->entities.forEach<Actor>([&](Actor* actor, hg::Entity* entity) {
        Vec3 dir = actor->direction.resize<3>().normalized();
        Vec3 a = math::Quaternion<float>(actor->fov * -0.5, Vec3::Face()).rotatePoint(dir);
        Vec3 b = math::Quaternion<float>(actor->fov * 0.5, Vec3::Face()).rotatePoint(dir);
        Debug::DrawLine(math::LineSegment(entity->transform.position, entity->transform.position + dir * 100), Color::blue(), 2.0/ 64.0);
        Debug::DrawLine(math::LineSegment(entity->transform.position, entity->transform.position + a * 100), Color::blue(), 2.0/ 64.0);
        Debug::DrawLine(math::LineSegment(entity->transform.position, entity->transform.position + b * 100), Color::blue(), 2.0/ 64.0);
    });

    scene->entities.forEach<RectCollider>([&](RectCollider* coll, Entity* entity) {
        Debug::DrawRect(Rect(coll->pos + entity->position().resize<2>(), coll->size), Color::blue(), 2.0 / 64.0);
    });

    scene->entities.forEach<CircleCollider>([&](CircleCollider* coll, Entity* entity) {
        Vec2 pos = entity->transform.position.resize<2>();
        Debug::DrawCircle(pos[0] + coll->pos[0], pos[1] + coll->pos[1], coll->radius, Color::blue(), 2.0 / 64.0);
    });

    shader = getShader("color");
    shader->use();
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());

    shader = getShader(PARTICLE_SHADER.name);
    shader->use();
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());
    shader->setMat4("model", Mat4::Identity());

    scene->entities.forEach<ParticleEmitterComponent>([&](auto emitter, auto entity) {
        emitter->render(shader);
    });

    processOverlays(RenderMode::Color);

    m_renderPasses.render(RenderMode::Color, 1);
}

void Renderer::lightPass(double dt) {

    m_state->levelGeometry.clear();
    m_state->levelRectangles.clear();

    scene->entities.forEach<hg::graphics::components::Tilemap>([&](auto tilemap, auto entity) {

        if (!tilemap->collide) {
            return;
        }

        if (tilemap->dynamic || !tilemap->isBaked()) {
            tilemap->bake();
        }

        auto geometry = tilemap->geometry();
        auto rectangles = tilemap->rectangles();

        m_state->levelGeometry.insert(m_state->levelGeometry.end(), geometry.begin(), geometry.end());
        m_state->levelRectangles.insert(m_state->levelRectangles.end(), rectangles.begin(), rectangles.end());
    });

    m_renderPasses.bind(RenderMode::Lighting);
    glViewport(0, 0, GAME_SIZE[0], GAME_SIZE[1]);

    auto shader = getShader("light");
    shader->use();
    shader->setMat4("view", m_camera.view());
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("model", Mat4::Identity());

    scene->entities.forEach<LightComponent>([&](LightComponent* light, Entity* entity) {
        shader->setVec2("origin", entity->position().resize<2>());
        shader->setVec4("color", light->color);
        shader->setFloat("attenuation", light->attenuation);
        // shader->setMat4("model", Mat4::Scale(1.1));
        if (light->dynamic || light->mesh.size() == 0) {
            light->computeMesh(m_state->levelGeometry);
        }

        light->mesh.render();
    });

    processOverlays(RenderMode::Lighting);

    m_renderPasses.render(RenderMode::Lighting, 1);
}

void Renderer::debugPass(double dt) {

    scene->entities.forEach<TriggerConnection>([&](auto conn, auto entity) {
        if (conn->connectedTo) {
            Debug::DrawLine(math::LineSegment(entity->position(), conn->connectedTo->position()), Color::green(), 2.0 / 64.0);
        }
    });

    scene->entities.forEach<RequirementConnection>([&](auto conn, auto entity) {
        if (conn->connectedTo) {
            Debug::DrawLine(math::LineSegment(entity->position(), conn->connectedTo->position()), Color::blue(), 2.0 / 64.0);
        }
    });

    m_renderPasses.bind(RenderMode::Debug);
    glViewport(0, 0, GAME_SIZE[0], GAME_SIZE[1]);

    auto shader = getShader("color");
    shader->use();
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());
    Debug::Render();

    shader = getShader("color");
    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, GAME_SIZE[0], 0, GAME_SIZE[1], -100, 100));
    shader->setMat4("model", Mat4::Identity());

    shader = getShader(TEXTURE_SHADER.name);
    shader->use();
    shader->setMat4("view",  m_camera.view());
    shader->setMat4("projection", m_camera.projection());
    getTexture("ui/flag")->bind();

    scene->entities.forEach<StartPoint>([&](auto start, auto entity) {
        shader->setMat4("model", entity->model());
        m_startMesh.render();
    });

    getTexture("ui/end")->bind();
    scene->entities.forEach<EndPoint>([&](auto start, auto entity) {
        shader->setMat4("model", entity->model());
        m_startMesh.render();
    });

    getTexture("ui/audio")->bind();
    //scene->entities.forEach<hg::audio::AudioSource>([&](auto source, hg::Entity* entity) {
    //    shader->setMat4("model", entity->model());
    //    m_startMesh.render();
    //});

    processOverlays(RenderMode::Debug);

    m_renderPasses.render(RenderMode::Debug, 1);
}

void Renderer::uiPass(double dt) {

    m_renderPasses.bind(RenderMode::UI);
    glViewport(0, 0, GAME_SIZE[0], GAME_SIZE[1]);

    if (m_editorMode) {
        return;
    }

    if (scene->hasSystem<Player>()) {
        auto player = scene->getSystem<Player>()->player;

        if (player) { // TODO: get rid of this digusting if chain
            auto actor = player->getComponent<Actor>();
            if (actor) {
                auto weapon = (GameWeapon*) actor->weapons.getWeapon();

                if (weapon) {
                    if (m_weapon.text() != weapon->item->tag) {
                        m_weapon.text(weapon->item->tag);
                    }

                    std::string ammoText = weapon->settings.infinite ? "Inf" : std::to_string(weapon->ammoInClip()) + " / " +
                                                                               std::to_string(weapon->ammo());
                    if (m_ammo.text() != ammoText) {
                        m_ammo.text(ammoText);
                    }
                }
            }
        }
    }

    if (scene->hasSystem<Enemies>()) {
        std::string enemyText = std::to_string(scene->getSystem<Enemies>()->size()) + " Enemies Remain";
        if (enemyText != m_enemies.text()) {
            m_enemies.text(enemyText);
        }
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

    for (int i = 0; i < 4; i++) {
        m_crossHairMeshes[i]->render();
    }

    shader = getShader(TEXT_BUFFER_SHADER.name);
    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, GAME_SIZE[0], 0, GAME_SIZE[1], -100, 100));
    shader->setMat4("model", Mat4::Identity());
    shader->setVec4("textColor", Color::white());

    m_wave.render();
    m_weapon.render();
    m_ammo.render();

    shader->setVec4("textColor", Color::blue());

    if (m_state->params.profileRender) {
        std::string profilerText = "";

        double total = 0;

        for (const auto& [name, profile] : Profiler::Profiles()) {
            profilerText += profile.name + ": " + std::to_string(profile.duration() * 1000) + "ms\n";
            total += profile.duration() * 1000;
        }

        profilerText += "Total: " + std::to_string(total) + "ms";

        auto messageSize = getFont("8bit")->calcMessageSize(profilerText);
        m_profiler.pos(Vec3(0, messageSize[1], 0));

        m_profiler.text(profilerText);

        m_profiler.render();
    }

    processOverlays(RenderMode::UI);

    m_renderPasses.render(RenderMode::UI, 1);
}

void Renderer::combinedPass(double dt) {


    glDisable(GL_DEPTH_TEST);
    // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    m_renderPasses.bind(RenderMode::Combined);
    glViewport(0, 0, GAME_SIZE[0], GAME_SIZE[1]);

    auto shader = getShader("combined");
    shader->use();
    shader->setFloat("useLighting", m_state->useLighting ? 1.0 : 0.0);
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", Mat4::Orthographic(0, GAME_SIZE[0], 0, GAME_SIZE[1], -100, 100));
    shader->setMat4("model", Mat4::Identity());

    glActiveTexture(GL_TEXTURE0 + 0);
    m_renderPasses.get(RenderMode::Color)->texture->bind();

    glActiveTexture(GL_TEXTURE0 + 1);
    m_renderPasses.get(RenderMode::Lighting)->texture->bind();

    glActiveTexture(GL_TEXTURE0 + 2);
    m_renderPasses.get(RenderMode::Debug)->texture->bind();

    glActiveTexture(GL_TEXTURE0 + 3);
    m_renderPasses.get(RenderMode::UI)->texture->bind();

    m_mesh.render();

    glActiveTexture(GL_TEXTURE0 + 0);

    m_renderPasses.unbind(RenderMode::Combined);
    glViewport(0, 0, m_window->size()[0], m_window->size()[1]);
}

hg::graphics::RawTexture<GL_RGBA32F> *Renderer::getRender() {
    return m_renderPasses.get(RenderMode::Combined)->texture.get();
}

void Renderer::onUpdate(double dt) {
    scene->entities.forEach<components::SpriteSheetAnimator>([&](auto animator, auto entity) {
        animator->update(dt);
    });
}



