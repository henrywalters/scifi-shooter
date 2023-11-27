//
// Created by henry on 4/12/23.
//

#ifndef HGAMETEMPLATE_CONSTANTS_H
#define HGAMETEMPLATE_CONSTANTS_H

#define GAME_NAME "SCIFI SHOOTER"
#define GAME_SIZE hg::graphics::HD

#define HEADLESS 0 // If true, no window will be opened
#define USE_IMGUI 1 // If true, IMGUI will be enabled and taken care of in game.cpp

#define RANDOM_SEED 420

const hg::Vec2 TILE_SIZE = hg::Vec2(1, 1);

#define ENEMY_GROUP "enemies"
#define PLAYER_GROUP  "player"

#endif //HGAMETEMPLATE_CONSTANTS_H
