//
// Created by henry on 8/23/23.
//

#ifndef SCIFISHOOTER_PLAYER_H
#define SCIFISHOOTER_PLAYER_H

#include <hagame/core/entity.h>
#include <hagame/core/system.h>
#include <hagame/graphics/window.h>
#include "weapons.h"
#include "../gamestate.h"

class Player : public hg::System {
public:

    const float PICKUP_DISTANCE = 200;
    const float PICKUP_SPEED = 500;

    hg::Entity* player;

    hg::Vec2 m_mousePos;

    Player(hg::graphics::Window* window, GameState* state);

    void spawn(hg::Vec2 pos);
    void onInit() override;
    void onUpdate(double dt) override;
    void onFixedUpdate(double dt) override;

    void ui();

    void pickUpWeapon(WeaponItemDef* weapon, int ammo = 0);

private:

    GameState* m_state;
    hg::graphics::Window* m_window;

    std::vector<hg::Entity*> m_pickingUp;

};

#endif //SCIFISHOOTER_PLAYER_H
