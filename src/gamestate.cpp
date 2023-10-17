//
// Created by henry on 9/25/23.
//
#include "gamestate.h"

hg::Vec2 GameState::randomTilemapPos() {
    bool isValid = false;
    hg::Vec2i idx;
    while (!isValid) {
        idx = hg::Vec2i(random.integer(tilemap->min().x(), tilemap->max().x()), random.integer(tilemap->min().y(), tilemap->max().y()));
        if (!tilemap->getLayer(0)->has(idx)) {
            isValid = true;
        }
    }
    return tilemap->getPos(idx);
}
