//
// Created by henry on 4/12/23.
//

#include <hagame/graphics/windows.h>
#include <hagame/core/assets.h>
#include <hagame/graphics/shaders/text.h>
#include <hagame/audio/device.h>
#include <hagame/audio/stream.h>
#include <hagame/audio/source.h>
#include <hagame/audio/player.h>

#include "game.h"
#include "scenes/runtime.h"
#include "scenes/loading.h"

#if USE_IMGUI
#include "imgui.h"
#include "imgui_demo.cpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#endif

using namespace hg::graphics;

// Function to generate a sine wave
void generateSineWave(ALsizei numSamples, ALsizei channels, ALsizei bitDepth, ALuint sampleRate, ALvoid* data) {
    for (ALsizei i = 0; i < numSamples; ++i) {
        // Generate a sine wave for the left channel
        float leftValue = std::sin(2.0f * M_PI * 218.0f * static_cast<float>(i) / sampleRate);

        // Generate a sine wave for the right channel (shifted in phase)
        float rightValue = std::sin(2.0f * M_PI * 440.0f * static_cast<float>(i) / sampleRate + M_PI / 2.0f);

        // Scale the values to fit in the specified bit depth
        int16_t scaledLeftValue = static_cast<int16_t>(leftValue * static_cast<float>(INT16_MAX));
        int16_t scaledRightValue = static_cast<int16_t>(rightValue * static_cast<float>(INT16_MAX));

        // Set the values in the stereo buffer
        reinterpret_cast<int16_t*>(data)[2 * i] = scaledLeftValue;
        reinterpret_cast<int16_t*>(data)[2 * i + 1] = scaledRightValue;
    }
}

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

    scenes()->add<Loading>("loading", m_window);
    scenes()->activate("loading");

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