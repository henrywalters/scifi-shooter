//
// Created by henry on 4/12/23.
//

#include <hagame/graphics/windows.h>
#include <hagame/audio/source.h>

#include "scifiGame.h"
#include "scenes/runtime.h"
#include "scenes/loading.h"

#if USE_IMGUI
#include "imgui.h"
#include "imgui_demo.cpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#endif

using namespace hg::graphics;

void ScifiGame::onInit() {
#if !HEADLESS
    m_window = Windows::Create(GAME_NAME, m_size);

    Windows::Events.subscribe(WindowEvents::Close, [&](Window* window) {
        running(false);
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

    scenes()->add<Loading>("loading", m_window);
    scenes()->activate("loading");

}

void ScifiGame::onBeforeUpdate() {
#if !HEADLESS
    m_window->clear();
#endif

#if USE_IMGUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
#endif
}

void ScifiGame::onAfterUpdate() {
#if USE_IMGUI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

#if !HEADLESS
    m_window->render();
#endif
}

void ScifiGame::onDestroy() {
    // CLEAN UP
}

void ScifiGame::onUpdate(double dt) {

}