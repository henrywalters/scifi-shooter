//
// Created by henry on 9/24/23.
//

#ifndef SCIFISHOOTER_ACTORS_H
#define SCIFISHOOTER_ACTORS_H

#include <hagame/core/entity.h>
#include <hagame/core/system.h>
#include <hagame/core/scene.h>
#include <hagame/graphics/tilemap.h>

#include <hagame/core/entityMap.h>

#include "../common/gamestate.h"

class Actors : public hg::System {
public:

    Actors(GameState* state);

    void onBeforeUpdate() override;
    void onFixedUpdate(double dt) override;
    void onAfterUpdate() override;

private:

    GameState* m_state;
};

#endif //SCIFISHOOTER_ACTORS_H
