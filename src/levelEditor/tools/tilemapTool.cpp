//
// Created by henry on 12/8/23.
//
#include <hagame/core/assets.h>
#include <hagame/graphics/components/tilemap.h>
#include "tilemapTool.h"
#include "imgui.h"
#include "../../scenes/editorRuntime.h"
#include "../../systems/renderer.h"
#include "../events.h"

using namespace hg;
using namespace hg::graphics;

void TilemapTool::onInit() {
    Events()->subscribe(EventTypes::SelectEntity, [&](const auto& e) {
        EntityEvent payload = std::get<EntityEvent>(e.payload);
        if (payload.entity->hasComponent<hg::graphics::components::Tilemap>()) {
            m_selectedEntity = payload.entity;
            open();
        } else {
            m_selectedEntity = nullptr;
        }
    });

    Events()->subscribe(EventTypes::AddComponent, [&](const auto& e) {
        ComponentEvent payload = std::get<ComponentEvent>(e.payload);
        if (payload.component == "Tilemap") {
            m_selectedEntity = payload.entity;
            open();
        }
    });

    Events()->subscribe(EventTypes::RemoveComponent, [&](const auto& e) {
        ComponentEvent payload = std::get<ComponentEvent>(e.payload);
        if (payload.component == "Tilemap") {
            m_selectedEntity = nullptr;
        }
    });
}

void TilemapTool::renderUI(double dt) {

    EditorRuntime* runtime = (EditorRuntime*) m_scene;

    ImGui::Begin("Tilemap Editor", &m_open);

    if (m_selectedEntity) {
        auto tilemap = m_selectedEntity->getComponent<hg::graphics::components::Tilemap>();
        m_mouseIndex = m_mousePos.div(tilemap->tileSize).floor().cast<int>();
        ImGui::Text(("Mouse Pos: " + m_mouseIndex.toString()).c_str());
        ImGui::DragFloat2("Tile Size", tilemap->tileSize.vector, 0.1, 0.0, 10.0);
        ImGui::ColorPicker4("Tile Color", m_color.vector);
    } else {
        ImGui::Text("Select an Entity with a Tilemap Component to begin.");
    }

    ImGui::End();
}

void TilemapTool::onUpdate(double dt) {
    if (!m_selectedEntity) {
        return;
    }

    auto tilemap = m_selectedEntity->getComponent<hg::graphics::components::Tilemap>();

    if (tilemap->tileSize != m_grid.cellSize()) {
        m_grid.cellSize(tilemap->tileSize);
        m_mesh.update(&m_grid);
        m_quad.size(tilemap->tileSize);
        m_quadMesh.update(&m_quad);
    }
}

void TilemapTool::renderOverlay() {

    if (!m_selectedEntity) {
        return;
    }
    auto tilemap = m_selectedEntity->getComponent<hg::graphics::components::Tilemap>();
    EditorRuntime* runtime = (EditorRuntime*) m_scene;

    m_grid.thickness(m_gridThickness * runtime->state()->zoom / runtime->state()->pixelsPerMeter);
    m_mesh.update(&m_grid);

    auto renderer = m_scene->getSystem<Renderer>();
    auto shader = getShader("color");
    shader->use();
    shader->setMat4("projection", renderer->m_camera.projection());
    shader->setMat4("view", renderer->m_camera.view());
    shader->setVec4("color", Color(100, 100, 100, 100));
    shader->setMat4("model", Mat4::Translation(m_grid.cellSize().prod(m_grid.cells().cast<float>()).resize<3>() * -0.5));
    m_mesh.render();

    shader->setMat4("model", Mat4::Translation((m_grid.cellSize().prod(m_mouseIndex.cast<float>()) + m_grid.cellSize() * 0.5).resize<3>()));
    shader->setVec4("color", m_color);
    m_quadMesh.render();
}
