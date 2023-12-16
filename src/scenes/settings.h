//
// Created by henry on 12/6/23.
//

#ifndef SCIFISHOOTER_SETTINGS_H
#define SCIFISHOOTER_SETTINGS_H

#include <future>

#include <hagame/core/scene.h>
#include <hagame/graphics/tilemap.h>
#include <hagame/graphics/camera.h>
#include <hagame/graphics/resolution.h>
#include <hagame/graphics/renderPasses.h>
#include <hagame/graphics/window.h>
#include <hagame/core/assets.h>
#include <hagame/graphics/debug.h>
#include <hagame/graphics/particleEmitter.h>
#include <hagame/utils/pathfinding.h>
#include <hagame/core/entityMap.h>
#include <hagame/graphics/textBuffer.h>
#include <hagame/ui/elements/textButton.h>

#include "../components/actor.h"

#include "../common/constants.h"
#include "../common/gamestate.h"
#include "../common/console.h"
#include "../systems/renderer.h"

class Settings : public hg::Scene {
public:

    Settings(hg::graphics::Window* window);

protected:

    void onInit() override;
    void onUpdate(double dt) override;

private:

    struct MenuButton {
        hg::ui::TextButton button;
        std::string scene;
    };

    hg::graphics::RenderPasses<RenderMode> m_renderPasses;
    hg::graphics::primitives::Quad m_quad;
    hg::graphics::MeshInstance m_mesh;

    std::vector<MenuButton> m_buttons;

    hg::graphics::TextBuffer m_versionBuffer;

    std::shared_ptr<GameState> m_state;

    hg::graphics::Window* m_window;

    std::string m_hovering;
    bool m_isHovering;

    void render(double dt);

};

#endif //SCIFISHOOTER_MAINMENU_H
