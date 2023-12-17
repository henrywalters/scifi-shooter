//
// Created by henry on 9/25/23.
//

#ifndef SCIFISHOOTER_GAMESTATE_H
#define SCIFISHOOTER_GAMESTATE_H

#include <hagame/core/entityMap.h>
#include <hagame/graphics/tilemap.h>
#include <hagame/graphics/particleEmitter.h>
#include <hagame/core/scene.h>
#include <hagame/core/game.h>
#include <hagame/structures/graph.h>
#include "runtimeParams.h"
#include "constants.h"
#include "itemDef.h"

struct GameState {

    bool paused = false;
    bool menuOpen = false;
    int wave = 0;
    float zoom = 0.8f;
    float pixelsPerMeter = 64;
    hg::Scene* scene;
    hg::utils::Random random;
    RuntimeParameters params;
    hg::EntityMap2D entityMap;
    bool useLighting = true;
    std::vector<hg::math::Polygon> levelGeometry;
    std::unique_ptr<hg::graphics::Tilemap> tilemap;
    hg::utils::Store<std::string, hg::graphics::ParticleEmitterSettings> particles;
    std::vector<hg::graphics::Color> randomColorsLUT;

    std::string parentScene;

    GameState(hg::Vec2 tileSize):
            entityMap(tileSize),
            tilemap(std::make_unique<hg::graphics::Tilemap>(tileSize)),
            random(RANDOM_SEED)
    {
        for (int i = 0; i < 100; i++) {
            randomColorsLUT.push_back(randomColor());
        }
    }

    void gotoScene(hg::Game* game, std::string next);

    // Generate a random tilemap position, that is unoccupied by a tile
    hg::Vec2 randomTilemapPos();

    hg::graphics::Color randomColor();

    // Perform a raycast that takes into consideration the tilemap and the Entity map. If an entity is hit, it will be returned.
    std::optional<hg::Entity*> raycast(hg::math::Ray ray, hg::Vec2& pos, std::vector<hg::Entity*> ignore = {});

    // Check if an entity is visible from a position, or occluded by wall.
    bool canSee(const hg::Vec2& pos, hg::Entity* entity);

    // Return the set of entities which are within the radius of the position, and not occluded by a wall. Useful for explosions and such
    std::vector<hg::Entity*> inVisibleRadius(hg::Vec2 pos, float radius, std::vector<hg::Entity*> ignore = {});

};

#endif //SCIFISHOOTER_GAMESTATE_H
