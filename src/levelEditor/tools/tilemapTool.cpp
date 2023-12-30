//
// Created by henry on 12/8/23.
//
#include <hagame/core/assets.h>
#include <hagame/graphics/components/tilemap.h>
#include "tilemapTool.h"
#include "imgui.h"
#include "../../thirdparty/imgui/misc/cpp/imgui_stdlib.h"
#include "../../scenes/editorRuntime.h"
#include "../../systems/renderer.h"
#include "../events.h"

using namespace hg;
using namespace hg::graphics;

void TilemapTool::onInit() {
    Events()->subscribe(EventTypes::SelectEntity, [&](const auto& e) {
        EntityEvent payload = std::get<EntityEvent>(e.payload);
        if (payload.entity->hasComponent<components::Tilemap>()) {
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
        auto tilemap = m_selectedEntity->getComponent<components::Tilemap>();
        m_mouseIndex = m_mousePos.div(tilemap->tileSize).floor().cast<int>() + hg::Vec2i(1, 1);
        ImGui::Text(("Mouse Pos: " + m_mouseIndex.toString()).c_str());
        if (ImGui::BeginCombo("Tile Type", "Color")) {
            for (int i = 0; i < TILE_MODE_NAMES.size(); i++) {
                const bool isSelected = (int) m_type == i;
                if (ImGui::Selectable(TILE_MODE_NAMES[i].c_str(), isSelected)) {
                    m_type = (components::Tilemap::TileType) i;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::DragFloat2("Tile Size", tilemap->tileSize.vector, 0.1, 0.0, 10.0);
        ImGui::Checkbox("Render Grid", &m_renderGrid);
        ImGui::ColorPicker4("Tile Color", m_color.vector);

        if (m_type == components::Tilemap::TileType::Texture) {
            auto texture = getTexture(m_texture);
            ImGui::InputText("Texture", &m_texture);
            ImGui::Image((void*) texture->id, ImVec2(64.0, 64.0));
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("png")) {
                    auto path = std::string((char*) payload->Data);
                    m_texture = hg::utils::s_replace(path, hg::ASSET_DIR + "textures/", "");
                }
                ImGui::EndDragDropTarget();
            }
        }


    } else {
        ImGui::Text("Select an Entity with a Tilemap Component to begin.");
    }

    ImGui::End();
}

void TilemapTool::onUpdate(double dt) {
    if (!m_selectedEntity) {
        return;
    }
    EditorRuntime* runtime = (EditorRuntime*) m_scene;
    auto tilemap = m_selectedEntity->getComponent<components::Tilemap>();

    if (tilemap->tileSize != m_grid.cellSize()) {
        m_grid.cellSize(tilemap->tileSize);
        m_mesh.update(&m_grid);
        m_quad.size(tilemap->tileSize);
        m_quadMesh.update(&m_quad);
    }

    if (runtime->window()->input.keyboardMouse.mouse.left) {
        if (!tilemap->tiles.has(m_mouseIndex)) {
            components::Tilemap::Tile tile;
            tile.color = m_color;
            tile.type = m_type;
            tile.index = m_mouseIndex;
            if (m_type == hg::graphics::components::Tilemap::TileType::Texture) {
                tile.texture = m_texture;
            }
            tilemap->tiles.set(m_mouseIndex, tile);
        }
    }

    if (runtime->window()->input.keyboardMouse.mouse.right) {
        if (tilemap->tiles.has(m_mouseIndex)) {
            tilemap->tiles.remove(m_mouseIndex);
        }
    }
}

void TilemapTool::renderOverlay() {

    if (!m_selectedEntity) {
        return;
    }
    auto tilemap = m_selectedEntity->getComponent<hg::graphics::components::Tilemap>();
    EditorRuntime* runtime = (EditorRuntime*) m_scene;

    auto renderer = m_scene->getSystem<Renderer>();
    auto shader = getShader("color");
    shader->use();
    shader->setMat4("projection", renderer->m_camera.projection());
    shader->setMat4("view", renderer->m_camera.view());
    shader->setVec4("color", Color(100, 100, 100, 100));
    shader->setMat4("model", Mat4::Translation(m_grid.cellSize().prod(m_grid.cells().cast<float>()).resize<3>() * -0.5));

    if (m_renderGrid) {
        m_grid.thickness(m_gridThickness * runtime->state()->zoom / runtime->state()->pixelsPerMeter);
        m_mesh.update(&m_grid);
        m_mesh.render();
    }

    if (m_type == hg::graphics::components::Tilemap::TileType::Texture) {
        shader = getShader("texture");
        shader->use();
        shader->setMat4("projection", renderer->m_camera.projection());
        shader->setMat4("view", renderer->m_camera.view());
        getTexture(m_texture)->bind();
    }
    shader->setMat4("model", Mat4::Translation((m_grid.cellSize().prod(m_mouseIndex.cast<float>()) - m_grid.cellSize() * 0.5).resize<3>()));
    shader->setVec4("color", m_color);
    m_quadMesh.render();
}
