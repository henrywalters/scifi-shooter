//
// Created by henry on 9/25/23.
//

#ifndef SCIFISHOOTER_GAMESTATE_H
#define SCIFISHOOTER_GAMESTATE_H

#include <hagame/core/entityMap.h>
#include <hagame/graphics/tilemap.h>
#include <hagame/graphics/particleEmitter.h>
#include "runtimeParams.h"
#include "constants.h"

struct GameState {
    int wave = 0;
    float zoom = 1.5f;
    hg::utils::Random random;
    RuntimeParameters params;
    hg::EntityMap2D entityMap;
    std::unique_ptr<hg::graphics::Tilemap> tilemap;
    hg::utils::Store<std::string, hg::graphics::ParticleEmitterSettings> particles;

    GameState(hg::Vec2 tileSize):
            entityMap(tileSize),
            tilemap(std::make_unique<hg::graphics::Tilemap>(tileSize)),
            random(RANDOM_SEED)
    {}

    // Generate a random tilemap position, that is unoccupied by a tile
    hg::Vec2 randomTilemapPos();
};

#endif //SCIFISHOOTER_GAMESTATE_H
