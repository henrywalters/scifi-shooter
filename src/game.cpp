//
// Created by henry on 4/12/23.
//

#include <hagame/graphics/windows.h>
#include <hagame/core/assets.h>
#include <hagame/graphics/shaders/text.h>

#include "game.h"
#include "runtime.h"


#if USE_IMGUI
#include "imgui.h"
#include "imgui_demo.cpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#endif

using namespace hg::graphics;

void Game::onInit() {
#if !HEADLESS
    m_window = Windows::Create(GAME_NAME, m_size);

    Windows::Events.subscribe(WindowEvents::Close, [&](Window* window) {
        running(false);
    });

    Windows::Events.subscribe(WindowEvents::Resize, [&](Window* window) {
        scenes()->get<Runtime>("runtime")->setSize(window->size());
    });
#endif

#if USE_IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForOpenGL(m_window->window(), true);
    ImGui_ImplOpenGL3_Init("#version 300 es");
#endif

    std::vector<std::string> shaders = {
        "color",
        "sprite",
        // "text",
        "display",
        "particle",
    };

    for (const auto& shader : shaders) {
        std::string vertSrc = hg::ASSET_DIR + "shaders/" + shader + ".vert";
        std::string fragSrc = hg::ASSET_DIR + "shaders/" + shader + ".frag";
        hg::loadShader(shader, vertSrc, fragSrc);
    }

    hg::loadShader(hg::graphics::TEXT_SHADER);
    hg::loadShader(hg::graphics::TEXT_BUFFER_SHADER);

    for (const auto& file : hg::utils::d_listFiles(hg::ASSET_DIR + "textures")) {
        auto parts = hg::utils::f_getParts(file);
        hg::loadTexture(parts.name, "textures/" + parts.fullName);
    }

    auto defaultFont = hg::loadFont("8bit", hg::ASSET_DIR + "fonts/8bit.ttf");
    defaultFont->fontSize(16);

    scenes()->add<Runtime>("runtime", m_window);
    scenes()->activate("runtime");

}

void Game::onBeforeUpdate() {
#if !HEADLESS
    m_window->clear();
#endif

#if USE_IMGUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
#endif
}

void Game::onAfterUpdate() {
#if USE_IMGUI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

#if !HEADLESS
    m_window->render();
#endif
}

void Game::onDestroy() {
    // CLEAN UP
}

void Game::onUpdate(double dt) {

}