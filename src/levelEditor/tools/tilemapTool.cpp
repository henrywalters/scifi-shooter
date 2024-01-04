//
// Created by henry on 12/8/23.
//
#include <hagame/core/assets.h>
#include <hagame/graphics/components/tilemap.h>
#include <hagame/math/functions.h>
#include "tilemapTool.h"
#include "imgui.h"
#include "../../thirdparty/imgui/misc/cpp/imgui_stdlib.h"
#include "../../scenes/editorRuntime.h"
#include "../../systems/renderer.h"
#include "../events.h"

using namespace hg;
using namespace hg::graphics;

void TilemapTool::onInit() {

    m_modes.push_back(std::make_unique<PointMode>());
    m_modes.push_back(std::make_unique<LineMode>());
    m_modes.push_back(std::make_unique<FillMode>());

    Events()->subscribe(EventTypes::SelectEntity, [&](const auto& e) {
        EntityEvent payload = std::get<EntityEvent>(e.payload);
        if (payload.entity->hasComponent<components::Tilemap>()) {
            m_selectedEntity = payload.entity;
            open();
        } else {
            m_selectedEntity = nullptr;
        }
    });

    Events()->subscribe(EventTypes::RemoveEntity, [&](const auto& e) {
        EntityEvent payload = std::get<EntityEvent>(e.payload);
        if (payload.entity == m_selectedEntity) {
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

    Events()->subscribe(EventTypes::SelectAsset, [&](const auto& e) {
        AssetEvent payload = std::get<AssetEvent>(e.payload);
        if (payload.extension == "png") {
            m_texture = hg::utils::s_replace(payload.path, hg::ASSET_DIR + "textures/", "") + payload.name;
            m_type = components::TileType::Texture;
        }
    });

    Events()->subscribe(EventTypes::NewLevel, [&](const auto& e) {
        m_selectedEntity = nullptr;
    });

    Events()->subscribe(EventTypes::LoadLevel, [&](const auto& e) {
        hg::Scene* scene = std::get<hg::Scene*>(e.payload);
        if (m_selectedEntity && !scene->entities.exists(m_selectedEntity->id())) {
            m_selectedEntity = nullptr;
        }
    });
}

void TilemapTool::renderUI(double dt) {

    EditorRuntime* runtime = (EditorRuntime*) m_scene;

    ImGui::Begin("Tilemap Editor", &m_open);

    if (m_selectedEntity) {
        auto tilemap = m_selectedEntity->getComponent<components::Tilemap>();
        m_mouseIndex = tilemap->getIndex(m_mousePos);
        ImGui::Text(("Mouse Pos: " + m_mouseIndex.toString()).c_str());

        if (m_transactions.size() > 0) {
            if (ImGui::Button("Undo") || (runtime->window()->input.keyboardMouse.keyboard.lCtrl && runtime->window()->input.keyboardMouse.keyboard.lettersPressed['z' - 'a'])) {
                rollback();
            }
        }

        if (ImGui::BeginCombo("Tiling Mode", m_modes[m_mode]->getName().c_str())) {
            for (int i = 0; i < m_modes.size(); i++) {
                const bool isSelected = (int) m_mode == i;
                if (ImGui::Selectable(m_modes[i]->getName().c_str(), isSelected)) {
                    m_mode = i;
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Tile Type", components::TILE_TYPE_NAMES[(int) m_type].c_str())) {
            for (int i = 0; i < components::TILE_TYPE_NAMES.size(); i++) {
                const bool isSelected = (int) m_type == i;
                if (ImGui::Selectable(components::TILE_TYPE_NAMES[i].c_str(), isSelected)) {
                    m_type = (components::TileType) i;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::DragFloat2("Tile Size", tilemap->tileSize.vector, 0.1, 0.0, 10.0);
        ImGui::Checkbox("Render Grid", &m_renderGrid);
        ImGui::ColorPicker4("Tile Color", m_color.vector);

        if (m_type == components::TileType::Texture) {
            auto texture = getTexture(m_texture);
            ImGui::InputText("Texture", &m_texture);
            ImGui::Image((void*) texture->id, ImVec2(64.0, 64.0));
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("png")) {
                    auto path = std::string((char*) payload->Data);
                    auto fileParts = hg::utils::f_getParts(path);
                    m_texture = hg::utils::s_replace(fileParts.path, hg::ASSET_DIR + "textures/", "") + fileParts.name;
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
        m_modes[m_mode]->left(this, tilemap);
    }

    if (runtime->window()->input.keyboardMouse.mouse.leftPressed) {
        m_modes[m_mode]->leftPress(this, tilemap);
    }

    if (runtime->window()->input.keyboardMouse.mouse.right) {
        m_modes[m_mode]->right(this, tilemap);
    }

    if (runtime->window()->input.keyboardMouse.mouse.rightPressed) {
        m_modes[m_mode]->rightPress(this, tilemap);
    }

    tilemap->bake();
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

    m_modes[m_mode]->render(this);
}

void TilemapTool::processTransaction(TilemapTool::Transaction transaction) {

    if (!m_selectedEntity) {
        return;
    }

    auto tilemap = m_selectedEntity->getComponent<components::Tilemap>();
    for (const auto& tile : transaction.tiles) {
        if (transaction.type == Transaction::Type::Addition) {
            tilemap->tiles.set(tile.index, tile);
        } else {
            tilemap->tiles.remove(tile.index);
        }
    }

    m_transactions.push_back(transaction);
}

void TilemapTool::rollback(int steps) {
    if (!m_selectedEntity ) {
        return;
    }

    auto tilemap = m_selectedEntity->getComponent<components::Tilemap>();

    for (int i = 0; i < steps; i++) {
        auto transaction = m_transactions[m_transactions.size() - 1];
        for (const auto& tile : transaction.tiles) {
            if (transaction.type == Transaction::Type::Addition) {
                tilemap->tiles.remove(tile.index);
            } else {
                tilemap->tiles.set(tile.index, tile);
            }
        }
        m_transactions.pop_back();
    }
}

hg::graphics::components::Tilemap *TilemapTool::tilemap() {
    if (!m_selectedEntity) {
        return nullptr;
    }

    return m_selectedEntity->getComponent<components::Tilemap>();
}

void PointMode::left(TilemapTool *tool, hg::graphics::components::Tilemap *tilemap) {
    if (!tilemap->tiles.has(tool->m_mouseIndex)) {
        TilemapTool::Transaction transaction;
        transaction.type = TilemapTool::Transaction::Type::Addition;
        components::Tile tile;
        tile.color = tool->m_color;
        tile.type = tool->m_type;
        tile.index = tool->m_mouseIndex;
        if (tool->m_type == hg::graphics::components::TileType::Texture) {
            tile.texture = tool->m_texture;
        }
        transaction.tiles = {tile};
        tool->processTransaction(transaction);
    }
}

void PointMode::right(TilemapTool *tool, hg::graphics::components::Tilemap *tilemap) {
    if (tilemap->tiles.has(tool->m_mouseIndex)) {
        TilemapTool::Transaction transaction;
        transaction.type = TilemapTool::Transaction::Type::Deletion;
        transaction.tiles = { tilemap->tiles.get(tool->m_mouseIndex).value };
        tool->processTransaction(transaction);
    }
}

void PointMode::render(TilemapTool *tool) {
    auto renderer = tool->m_scene->getSystem<Renderer>();
    auto shader = getShader("color");
    if (tool->m_type == hg::graphics::components::TileType::Texture) {
        shader = getShader("texture");
        shader->use();
        shader->setMat4("projection", renderer->m_camera.projection());
        shader->setMat4("view", renderer->m_camera.view());
        getTexture(tool->m_texture)->bind();
    }
    shader->setMat4("model", Mat4::Translation((tool->tilemap()->getPos(tool->m_mouseIndex) + tool->m_grid.cellSize() * 0.5).resize<3>()));
    shader->setVec4("color", tool->m_color);
    tool->m_quadMesh.render();
}

void LineMode::leftPress(TilemapTool *tool, hg::graphics::components::Tilemap *tilemap) {
    if (!m_lineStart.has_value()) {
        m_lineStart = tool->m_mouseIndex;
    } else {
        auto tiles = hg::bresenham(m_lineStart.value(), tool->m_mouseIndex);
        TilemapTool::Transaction transaction;
        transaction.type = TilemapTool::Transaction::Type::Addition;
        for (const auto& tileIndex : tiles) {
            if (!tilemap->tiles.has(tileIndex)) {
                components::Tile tile;
                tile.color = tool->m_color;
                tile.type = tool->m_type;
                tile.index = tileIndex;
                if (tool->m_type == hg::graphics::components::TileType::Texture) {
                    tile.texture = tool->m_texture;
                }
                transaction.tiles.push_back(tile);
            }
        }
        tool->processTransaction(transaction);
        m_lineStart = tool->m_mouseIndex;
    }
}

void LineMode::rightPress(TilemapTool *tool, hg::graphics::components::Tilemap *tilemap) {
    m_lineStart = std::nullopt;
}

void LineMode::render(TilemapTool *tool) {
    if (m_lineStart.has_value()) {
        auto tiles = hg::bresenham(m_lineStart.value(), tool->m_mouseIndex);
        auto renderer = tool->m_scene->getSystem<Renderer>();
        auto shader = getShader("color");
        if (tool->m_type == hg::graphics::components::TileType::Texture) {
            shader = getShader("texture");
            shader->use();
            shader->setMat4("projection", renderer->m_camera.projection());
            shader->setMat4("view", renderer->m_camera.view());
            getTexture(tool->m_texture)->bind();
        }
        shader->setVec4("color", tool->m_color);
        for (auto& tile : tiles) {
            shader->setMat4("model", Mat4::Translation((tool->tilemap()->getPos(tile) + tool->m_grid.cellSize() * 0.5).resize<3>()));
            tool->m_quadMesh.render();
        }
    }
}

void FillMode::leftPress(TilemapTool *tool, hg::graphics::components::Tilemap *tilemap) {
    TilemapTool::Transaction transaction;
    transaction.type = TilemapTool::Transaction::Type::Addition;

    auto getNeighbors = [&](hg::Vec2i index) -> std::vector<hg::Vec2i> {
        std::vector<hg::Vec2i> neighbors;
        std::array<hg::Vec2i, 4> offsets = {
            hg::Vec2i(0, 1),
            hg::Vec2i(1, 0),
            hg::Vec2i(0, -1),
            hg::Vec2i(-1, 0),
        };
        for (int i = 0; i < 4; i++) {
            hg::Vec2i neighborIndex = index + offsets[i];
            if (!tilemap->tiles.has(neighborIndex) &&
                neighborIndex[0] >= tilemap->tiles.min()[0] &&
                neighborIndex[0] < tilemap->tiles.max()[0] &&
                neighborIndex[1] >= tilemap->tiles.min()[1] &&
                neighborIndex[1] < tilemap->tiles.max()[1]
            ) {
                neighbors.push_back(neighborIndex);
            }
        }
        return neighbors;
    };

    std::vector<hg::Vec2i> indicesToFill;
    auto neighbors = getNeighbors(tool->m_mouseIndex);

    while (neighbors.size() > 0) {

        auto neighbor = neighbors[neighbors.size() - 1];
        neighbors.pop_back();

        if (std::find(indicesToFill.begin(), indicesToFill.end(), neighbor) == indicesToFill.end()) {
            indicesToFill.push_back(neighbor);
            auto toAdd = getNeighbors(neighbor);
            neighbors.insert(neighbors.end(), toAdd.begin(), toAdd.end());
        }
    }

    if (indicesToFill.size() == 0) {
        return;
    }

    for (const auto& index : indicesToFill) {
        components::Tile tile;
        tile.index = index;
        tile.color = tool->m_color;
        tile.texture = tool->m_texture;
        tile.type = tool->m_type;
        transaction.tiles.push_back(tile);
    }
    tool->processTransaction(transaction);
}
