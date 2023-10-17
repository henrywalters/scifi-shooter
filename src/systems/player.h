//
// Created by henry on 8/23/23.
//

#ifndef SCIFISHOOTER_PLAYER_H
#define SCIFISHOOTER_PLAYER_H

#include <hagame/core/entity.h>
#include <hagame/core/system.h>
#include <hagame/graphics/window.h>
#include "../gamestate.h"

class Player : public hg::System {
public:

    hg::Entity* player;

    hg::Vec2 m_mousePos;

    Player(hg::graphics::Window* window, GameState* state);

    void onInit() override;
    void onUpdate(double dt) override;
    void onFixedUpdate(double dt) override;

    void ui();

private:

    GameState* m_state;
    hg::graphics::Window* m_window;

};

#endif //SCIFISHOOTER_PLAYER_H
