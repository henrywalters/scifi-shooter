//
// Created by henry on 12/7/23.
//

#ifndef SCIFISHOOTER_TILEMAPTOOL_H
#define SCIFISHOOTER_TILEMAPTOOL_H

#include <hagame/core/scene.h>
#include <hagame/graphics/primitives/grid.h>
#include <hagame/graphics/mesh.h>
#include <hagame/graphics/components/quad.h>
#include <hagame/graphics/components/tilemap.h>
#include <hagame/utils/spatialMap.h>
#include "tool.h"
#include "../../common/constants.h"

class TilemapTool : public Tool {
public:

    TilemapTool(hg::Scene* scene):
        m_gridThickness(2.0),
        m_grid(hg::Vec2(0, 0), hg::Vec2i(0, 0), 0),
        m_mesh(&m_grid),
        m_quad(),
        m_quadMesh(&m_quad),
        Tool(scene)
    {
        m_grid.thickness(m_gridThickness / 64.0);
        m_grid.cells(hg::Vec2i(100, 100));
        m_grid.cellSize(hg::Vec2(0.0));
        m_mesh.update(&m_grid);

        m_quad.size(hg::Vec2::Zero());
        m_quadMesh.update(&m_quad);
    }

    std::string getButtonLabel() override { return "Add Tilemap"; }
    std::string getName() override { return "Tilemap Tool"; }

protected:

    void onInit() override;
    void renderUI(double dt) override;
    void onUpdate(double dt) override;
    void renderOverlay() override;

private:

    hg::graphics::primitives::Grid m_grid;
    hg::graphics::MeshInstance m_mesh;

    hg::graphics::primitives::Quad m_quad;
    hg::graphics::MeshInstance m_quadMesh;

    bool m_renderGrid = true;
    float m_gridThickness;
    std::string m_texture;

    hg::graphics::Color m_color = hg::graphics::Color::white();
    hg::graphics::components::Tilemap::TileType m_type;

    hg::Vec2i m_mouseIndex;

    hg::Entity* m_selectedEntity = nullptr;
};

#endif //SCIFISHOOTER_TILEMAPTOOL_H
