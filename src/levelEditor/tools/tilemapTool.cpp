//
// Created by henry on 12/8/23.
//
#include <hagame/core/assets.h>
#include "tilemapTool.h"
#include "imgui.h"
#include "../../scenes/editorRuntime.h"
#include "../../systems/renderer.h"

using namespace hg;
using namespace hg::graphics;

void TilemapTool::onInit() {

}

void TilemapTool::renderUI(double dt) {
    ImGui::Begin("Tilemap Editor", &m_open);

    EditorRuntime* runtime = (EditorRuntime*) m_scene;
    GameState* state = runtime->state();
    hg::graphics::Tilemap* tilemap = state->tilemap.get();
    hg::Vec2 tileSize = tilemap->tileSize();
    ImGui::DragFloat2("Tile Size", tileSize.vector, 0.1, 0);

    if (tileSize != tilemap->tileSize()) {
        tilemap->tileSize(tileSize);
    }

    ImGui::End();
}

void TilemapTool::onUpdate(double dt) {
    Tool::onUpdate(dt);
}

void TilemapTool::renderOverlay() {
    EditorRuntime* runtime = (EditorRuntime*) m_scene;

    auto renderer = m_scene->getSystem<Renderer>();
    auto shader = getShader("grid");
    shader->use();
    shader->setMat4("projection", renderer->m_camera.projection());
    shader->setMat4("view", renderer->m_camera.view());
    shader->setVec4("color", Color(255, 165, 0));
    shader->setMat4("model", Mat4::Identity());
    auto size = GAME_SIZE;
    shader->setVec2("cells", m_gridCells.cast<float>());
    shader->setFloat("thickness", m_gridThickness);
    m_mesh.render();
}
