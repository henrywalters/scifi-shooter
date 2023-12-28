//
// Created by henry on 12/7/23.
//

#ifndef SCIFISHOOTER_TILEMAPTOOL_H
#define SCIFISHOOTER_TILEMAPTOOL_H

#include <hagame/core/scene.h>
#include <hagame/graphics/primitives/quad.h>
#include <hagame/graphics/mesh.h>
#include "tool.h"

class TilemapTool : public Tool {
public:

    TilemapTool(hg::Scene* scene):
        m_quad(hg::Vec2(4, 4)),
        m_mesh(&m_quad),
        Tool(scene)
    {}

    std::string getButtonLabel() override { return "Add Tilemap"; }
    std::string getName() override { return "Tilemap Tool"; }

protected:

    void onInit() override;
    void renderUI(double dt) override;
    void onUpdate(double dt) override;
    void renderOverlay() override;

private:

    hg::graphics::primitives::Quad m_quad;
    hg::graphics::MeshInstance m_mesh;

    float m_gridThickness = 0.1f;
    hg::Vec2i m_gridCells = hg::Vec2i(4, 4);

};

#endif //SCIFISHOOTER_TILEMAPTOOL_H
