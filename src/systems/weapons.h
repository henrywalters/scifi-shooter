//
// Created by henry on 9/24/23.
//

#ifndef SCIFISHOOTER_WEAPONS_H
#define SCIFISHOOTER_WEAPONS_H

#include <hagame/core/entity.h>
#include <hagame/core/system.h>
#include <hagame/core/scene.h>
#include <hagame/core/entityMap.h>
#include <hagame/graphics/tilemap.h>
#include "../gamestate.h"

class Weapons : public hg::System {
public:

    Weapons(GameState* state);

    void onFixedUpdate(double dt) override;

private:
    GameState* m_state;
};

#endif //SCIFISHOOTER_WEAPONS_H
