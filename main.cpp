#include <iostream>

#include <hagame/core/hg.h>
#include <hagame/graphics/resolution.h>
#include <hagame/graphics/windows.h>
#include "src/scifiGame.h"

// The main function simply initializes the game and starts running it

// The main entrypoint of the game is found in src/game.h
// For global configurables, checkout src/constants.h

#ifdef __EMSCRIPTEN__

ScifiGame game(GAME_NAME, GAME_SIZE);

void emscripten_tick() {
    game.tick();
}

#endif


int main() {
#ifdef __EMSCRIPTEN__
    game.initialize();
    emscripten_set_main_loop(emscripten_tick, 0, false);
    // clickVenture.destroy();
#else
#if HEADLESS
    Game game(GAME_NAME);
#else
    ScifiGame game(GAME_NAME, GAME_SIZE);
#endif
    hg::HG::Run(&game);
#endif
    return 0;
}
