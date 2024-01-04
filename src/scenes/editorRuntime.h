//
// Created by henry on 12/17/23.
//

#ifndef SCIFISHOOTER_EDITORRUNTIME_H
#define SCIFISHOOTER_EDITORRUNTIME_H

#include <hagame/core/scene.h>
#include <hagame/graphics/window.h>
#include "../common/gamestate.h"
#include "../common/console.h"

class EditorRuntime : public hg::Scene {
public:

    friend class LevelEditor;

    EditorRuntime(hg::graphics::Window* window):
        m_window(window),
        m_state(std::make_unique<GameState>(TILE_SIZE)),
        hg::Scene()
    {}

    GameState* state() { return m_state.get(); }
    hg::graphics::Window* window() { return m_window; }

protected:

    void onInit() override;
    void onActivate() override;
    void onFixedUpdate(double dt) override;
    void onUpdate(double dt) override;

private:

    hg::graphics::Window* m_window;
    std::unique_ptr<Console> m_console;
    std::unique_ptr<GameState> m_state;

    void render(double dt);

};

#endif //SCIFISHOOTER_EDITORRUNTIME_H
