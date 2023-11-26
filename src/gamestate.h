//
// Created by henry on 9/25/23.
//

#ifndef SCIFISHOOTER_GAMESTATE_H
#define SCIFISHOOTER_GAMESTATE_H

#include <hagame/core/entityMap.h>
#include <hagame/graphics/tilemap.h>
#include <hagame/graphics/particleEmitter.h>
#include <hagame/core/scene.h>
#include "runtimeParams.h"
#include "constants.h"
#include "items.h"

struct GameState {

    bool paused = false;
    bool menuOpen = false;
    int wave = 0;
    float zoom = 1.5f;
    hg::Scene* scene;
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

    // Perform a raycast that takes into consideration the tilemap and the Entity map. If an entity is hit, it will be returned.
    std::optional<hg::Entity*> raycast(hg::math::Ray ray, hg::Vec2& pos, std::vector<hg::Entity*> ignore = {});

    // Check if an entity is visible from a position, or occluded by wall.
    bool canSee(const hg::Vec2& pos, hg::Entity* entity);

    // Return the set of entities which are within the radius of the position, and not occluded by a wall. Useful for explosions and such
    std::vector<hg::Entity*> inVisibleRadius(hg::Vec2 pos, float radius, std::vector<hg::Entity*> ignore = {});

};

#endif //SCIFISHOOTER_GAMESTATE_H
