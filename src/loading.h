//
// Created by henry on 11/9/23.
//

#ifndef SCIFISHOOTER_LOADING_H
#define SCIFISHOOTER_LOADING_H

#include <future>

#include <hagame/core/scene.h>
#include <hagame/graphics/tilemap.h>
#include <hagame/graphics/camera.h>
#include <hagame/graphics/resolution.h>
#include <hagame/graphics/renderPasses.h>
#include <hagame/graphics/window.h>
#include <hagame/core/assets.h>
#include <hagame/graphics/debug.h>
#include <hagame/graphics/particleEmitter.h>
#include <hagame/utils/pathfinding.h>
#include <hagame/core/entityMap.h>
#include <hagame/graphics/textBuffer.h>
#include "components/actor.h"

#include "constants.h"
#include "gamestate.h"
#include "console.h"
#include "systems/renderer.h"

class Loading : public hg::Scene {
public:

    Loading(hg::graphics::Window* window);

protected:

    void onInit() override;
    void onUpdate(double dt) override;

private:

    int m_shaderIdx = 0;
    int m_spriteSheetIdx = 0;
    int m_textureIdx = 0;
    int m_soundIdx = 0;

    std::string m_message;

    hg::graphics::RenderPasses<RenderMode> m_renderPasses;
    hg::graphics::primitives::Quad m_quad;
    hg::graphics::MeshInstance m_mesh;

    hg::graphics::Text m_text;
    hg::graphics::TextBuffer m_messageBuffer;

    std::shared_ptr<GameState> m_state;

    std::vector<hg::graphics::ShaderSource> m_shaders;
    std::vector<std::tuple<std::string, std::string>> m_spriteSheets;
    std::vector<std::tuple<std::string, std::string>> m_textures;
    std::vector<std::tuple<std::string, std::string>> m_sounds;

    hg::graphics::Window* m_window;

    void render(double dt);

};


#endif //SCIFISHOOTER_LOADING_H
