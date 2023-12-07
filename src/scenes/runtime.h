//
// Created by henry on 4/21/23.
//

#ifndef SCIFISCHOOTER_RUNTIME_H
#define SCIFISCHOOTER_RUNTIME_H

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

class Runtime : public hg::Scene {
public:

    friend class Player;

    Runtime(hg::graphics::Window* window):
        m_window(window),
        m_state(std::make_unique<GameState>(TILE_SIZE)),
        hg::Scene()
    {}

    void loadLevel(std::string level);
    void setSize(hg::Vec2i size);

    GameState* state();
    Console* console();

protected:

    void onInit() override;
    void onFixedUpdate(double dt) override;
    void onUpdate(double dt) override;

private:

    EntityViewer m_entityViewer;

    hg::audio::Player* m_player;

    hg::graphics::Window* m_window;
    std::unique_ptr<Console> m_console;
    std::unique_ptr<GameState> m_state;

    void renderUI(double dt);
    void render(double dt);

    void nextWave();

};

#endif //SCIFISCHOOTER_RUNTIME_H
