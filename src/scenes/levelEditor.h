//
// Created by henry on 12/5/23.
//

#ifndef SCIFISHOOTER_LEVELEDITOR_H
#define SCIFISHOOTER_LEVELEDITOR_H

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
#include <hagame/audio/player.h>

#include "../levelEditor/entityViewer.h"

#include "../components/actor.h"

#include "../common/constants.h"
#include "../common/gamestate.h"
#include "../common/console.h"

#include "../levelEditor/tools/tool.h"

class LevelEditor : public hg::Scene {
public:

    LevelEditor(hg::graphics::Window* window):
        m_window(window),
        m_state(std::make_unique<GameState>(TILE_SIZE)),
        hg::Scene()
    {}

    void selectTool(Tool* tool);

protected:

    void onInit() override;
    void onUpdate(double dt) override;

private:

    EntityViewer m_entityViewer;
    hg::graphics::Window* m_window;
    std::unique_ptr<Console> m_console;
    std::unique_ptr<GameState> m_state;

    hg::Vec2 m_rawMousePos;
    hg::Vec2 m_mousePos;
    hg::Vec2 m_snapSize = hg::Vec2(20);

    std::vector<std::unique_ptr<Tool>> m_tools;
    Tool* m_tool = nullptr;

    void renderUI(double dt);

};

#endif //SCIFISHOOTER_LEVELEDITOR_H
