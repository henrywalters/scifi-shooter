//
// Created by henry on 8/23/23.
//

#ifndef SCIFISHOOTER_RENDERER_H
#define SCIFISHOOTER_RENDERER_H

#include <hagame/core/system.h>
#include <hagame/core/scene.h>
#include <hagame/core/assets.h>

#include <hagame/graphics/window.h>
#include <hagame/graphics/resolution.h>
#include <hagame/graphics/camera.h>
#include <hagame/graphics/renderPasses.h>
#include <hagame/graphics/primitives/quad.h>
#include <hagame/graphics/mesh.h>
#include <hagame/graphics/color.h>
#include <hagame/graphics/debug.h>
#include <hagame/graphics/components/sprite.h>
#include <hagame/graphics/components/particleEmitterComponent.h>
#include <hagame/graphics/tilemap.h>
#include <hagame/graphics/textBuffer.h>

#include <hagame/common/components/healthBar.h>

#include <hagame/utils/profiler.h>

#include "../constants.h"
#include "../gamestate.h"

enum class RenderMode {
    Color,
    Display,
};

class Renderer : public hg::System {
public:

    const float CONSOLE_MOVE_DUR = 1;
    const float CROSSHAIR_WIDTH = 0.1f;

    hg::graphics::OrthographicCamera m_camera;

    Renderer(hg::graphics::Window* window, GameState* state);

    void onInit() override;

    void onBeforeUpdate() override;
    void onUpdate(double dt) override;
    void onFixedUpdate(double dt) override;
    void onAfterUpdate() override;

    void setCameraPosition(hg::Vec3 pos);
    hg::Vec2 getMousePos(hg::Vec2 rawMousePos);

    void setLaserPointer(hg::Vec3 start, hg::Vec3 end);
    void setCrossHair(hg::Vec2 pos, float innerRadius, float outerRadius);

    void setWindowSize(hg::Vec2i size);

private:

    hg::graphics::Window* m_window;
    GameState* m_state;

    hg::graphics::RenderPasses<RenderMode> m_renderPasses;

    hg::graphics::primitives::Quad m_quad;
    hg::graphics::MeshInstance m_mesh;

    hg::graphics::Text m_text;

    hg::graphics::TextBuffer m_wave;
    hg::graphics::TextBuffer m_weapon;
    hg::graphics::TextBuffer m_ammo;
    hg::graphics::TextBuffer m_enemies;

    std::unique_ptr<hg::graphics::MeshInstance> m_laserMesh;
    hg::graphics::primitives::Line m_laser;
    hg::graphics::primitives::Disc m_laserDisc;
    std::unique_ptr<hg::graphics::MeshInstance> m_laserDiscMesh;

    std::array<hg::graphics::primitives::Quad, 4> m_crossHairQuads;
    std::array<std::unique_ptr<hg::graphics::MeshInstance>, 4> m_crossHairMeshes;
};

#endif //SCIFISHOOTER_RENDERER_H
