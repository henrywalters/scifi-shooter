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
#include "components/actor.h"

#include "constants.h"
#include "gamestate.h"

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

protected:

    void onInit() override;
    void onFixedUpdate(double dt) override;
    void onUpdate(double dt) override;

private:

    hg::graphics::Window* m_window;

    std::unique_ptr<GameState> m_state;

    void renderUI(double dt);
    void render(double dt);

    void nextWave();

};

#endif //SCIFISCHOOTER_RUNTIME_H
