//
// Created by henry on 8/23/23.
//

#ifndef SCIFISHOOTER_RENDERER_H
#define SCIFISHOOTER_RENDERER_H

#include <deque>

#include <hagame/core/system.h>
#include <hagame/core/scene.h>
#include <hagame/core/assets.h>

#include <hagame/graphics/window.h>
#include <hagame/graphics/resolution.h>
#include <hagame/graphics/camera.h>
#include <hagame/graphics/renderPasses.h>
#include <hagame/graphics/primitives/quad.h>
#include <hagame/graphics/primitives/light.h>
#include <hagame/graphics/mesh.h>
#include <hagame/graphics/color.h>
#include <hagame/graphics/debug.h>
#include <hagame/graphics/components/sprite.h>
#include <hagame/graphics/components/particleEmitterComponent.h>
#include <hagame/graphics/tilemap.h>
#include <hagame/graphics/textBuffer.h>
#include <hagame/graphics/batchRenderer.h>

#include <hagame/common/components/healthBar.h>

#include <hagame/utils/profiler.h>

#include "../common/constants.h"
#include "../common/gamestate.h"

enum class RenderMode {
    Color,
    Lighting,
    Debug,
    UI,
    Combined,
};

class Renderer : public hg::System {
public:

    const float CONSOLE_MOVE_DUR = 1;
    const int CROSSHAIR_WIDTH = 4.0 / 64.0;

    hg::graphics::OrthographicCamera m_camera;

    Renderer(hg::graphics::Window* window, GameState* state, bool editorMode = false);

    void onInit() override;

    void onBeforeUpdate() override;
    void onRender(double dt) override;
    void onUpdate(double dt) override;
    void onFixedUpdate(double dt) override;
    void onAfterUpdate() override;

    void addOverlay(RenderMode mode, std::function<void()> overlay) {
        if (m_overlays.find(mode) == m_overlays.end()) {
            m_overlays.insert(std::make_pair(mode, std::deque<std::function<void()>>()));
        }
        m_overlays[mode].push_back(overlay);
    }

    void setCameraPosition(hg::Vec3 pos);

    // Set the mouse pos in screen coordinates in the range of [0, 1]
    void setRawMousePos(hg::Vec2 rawMousePos) {
        m_mousePos = m_camera.getGamePos(rawMousePos.prod(m_camera.size));
    }
    hg::Vec2 getMousePos() const {
        return m_mousePos;
    }

    void setLaserPointer(hg::Vec3 start, hg::Vec3 end);
    void setCrossHair(hg::Vec2 pos, float innerRadius, float outerRadius);

    void setWindowSize(hg::Vec2i size);

    hg::graphics::RawTexture<GL_RGBA32F>* getRender();

private:

    std::unordered_map<RenderMode, std::deque<std::function<void()>>> m_overlays;

    hg::Vec2 m_mousePos;

    bool m_editorMode;

    hg::graphics::RawTexture<GL_R16F, GL_FLOAT, GL_RED> m_lightTexture;

    hg::graphics::BatchRenderer m_batchRenderer;

    hg::graphics::Window* m_window;
    GameState* m_state;

    hg::graphics::RenderPasses<RenderMode> m_renderPasses;

    hg::graphics::primitives::Quad m_animQuad;
    hg::graphics::MeshInstance m_anim;

    hg::graphics::primitives::Quad m_displayQuad;
    hg::graphics::MeshInstance m_mesh;

    hg::graphics::primitives::Light m_light;
    hg::graphics::MeshInstance m_lightMesh;

    hg::graphics::Text m_text;

    hg::graphics::TextBuffer m_wave;
    hg::graphics::TextBuffer m_weapon;
    hg::graphics::TextBuffer m_ammo;
    hg::graphics::TextBuffer m_enemies;
    hg::graphics::TextBuffer m_profiler;

    std::unique_ptr<hg::graphics::MeshInstance> m_laserMesh;
    hg::graphics::primitives::Line m_laser;
    hg::graphics::primitives::Disc m_laserDisc;
    std::unique_ptr<hg::graphics::MeshInstance> m_laserDiscMesh;

    std::array<hg::graphics::primitives::Quad, 4> m_crossHairQuads;
    std::array<std::unique_ptr<hg::graphics::MeshInstance>, 4> m_crossHairMeshes;

    hg::graphics::primitives::Quad m_startQuad;
    hg::graphics::MeshInstance m_startMesh;

    void colorPass(double dt);
    void lightPass(double dt);
    void debugPass(double dt);
    void uiPass(double dt);
    void combinedPass(double dt);

    void processOverlays(RenderMode mode) {
        if (m_overlays.find(mode) != m_overlays.end()) {
            while (m_overlays[mode].size() > 0) {
                auto overlay = m_overlays[mode].front();
                overlay();
                m_overlays[mode].pop_front();
            }
        }
    }
};

#endif //SCIFISHOOTER_RENDERER_H
