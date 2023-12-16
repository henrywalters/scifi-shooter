//
// Created by henry on 4/12/23.
//

#ifndef HGAMETEMPLATE_GAME_H
#define HGAMETEMPLATE_GAME_H

#include <hagame/core/game.h>
#include "common/constants.h"
#include "common/console.h"
#include <hagame/audio/player.h>

class ScifiGame : public hg::Game {
public:

    std::unique_ptr<Console> console;

#if HEADLESS
    Game(std::string name):
        hg::Game(name)
    {}
#else
    ScifiGame(std::string name, hg::Vec2i size):
        m_size(size),
        hg::Game(name)
    {}

    hg::graphics::Window* window() const { return m_window; }

#endif

protected:

    void onInit() override;
    void onBeforeUpdate() override;
    void onUpdate(double dt) override;
    void onAfterUpdate() override;
    void onDestroy() override;

private:

#if !HEADLESS
    hg::graphics::Window* m_window;
    hg::Vec2i m_size;
#endif

};

#endif //HGAMETEMPLATE_GAME_H
