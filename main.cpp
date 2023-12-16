#include <iostream>

#include <hagame/core/hg.h>
#include <hagame/graphics/resolution.h>
#include "src/scifiGame.h"

#include <hagame/structures/binaryHeap.h>
#include <hagame/utils/timer.h>
#include <hagame/utils/behaviorTree.h>
#include <set>

// The main function simply initializes the game and starts running it

// The main entrypoint of the game is found in src/game.h
// For global configurables, checkout src/constants.h

#if HEADLESS
Game game(GAME_NAME);
#else
ScifiGame game(GAME_NAME, GAME_SIZE);
#endif

void emscripten_tick() {
    game.tick();
}

int main() {
#ifdef __EMSCRIPTEN__
    game.initialize();
    emscripten_set_main_loop(emscripten_tick, 0, false);
    // clickVenture.destroy();
#else
    hg::HG::Run(&game);
#endif
    return 0;
}
