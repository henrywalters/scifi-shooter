//
// Created by henry on 12/5/23.
//
#include "levelEditor.h"
#include "../systems/renderer.h"
#include "../levelEditor/tools/tilemapTool.h"

using namespace hg;
using namespace hg::graphics;

void LevelEditor::onInit() {

    m_state->params.debugRender = true;

    addSystem<Renderer>(m_window, m_state.get(), true);

    m_tools.push_back(std::make_unique<TilemapTool>(this));

    m_window->input.keyboardMouse.events.subscribe(hg::input::devices::KeyboardEvent::KeyPressed, [&](auto keyPress) {
        if (keyPress.key == '`') {
            m_console->toggle();
            m_state->paused = m_console->isOpen() || m_state->menuOpen;
        }

        if (!m_console->isOpen()) {
            return;
        }

        if (keyPress.key == hg::input::devices::KeyboardMouse::BACKSPACE) {
            m_console->backspace();
        }

        if (keyPress.key == hg::input::devices::KeyboardMouse::ENTER) {
            m_console->submit();
        }

        if (keyPress.key == hg::input::devices::KeyboardMouse::UP) {
            m_console->prevHistory();
        }

        if (keyPress.key == hg::input::devices::KeyboardMouse::DOWN) {
            m_console->nextHistory();
        }
    });

    m_window->input.keyboardMouse.events.subscribe(hg::input::devices::KeyboardEvent::TextInput, [&](auto keyPress) {
        if (m_console->status() == Console::Status::Open) {
            m_console->newChar(keyPress.key);
        }
    });

    m_console = std::make_unique<Console>(hg::getFont("8bit"));
}

void LevelEditor::onUpdate(double dt) {
    ImGui::ShowDemoWindow();

    auto renderer = getSystem<Renderer>();

    m_window->setVSync(m_state->params.vsync);

    m_rawMousePos = m_window->input.keyboardMouse.mouse.position;
    m_rawMousePos[1] = m_window->size()[1] - m_rawMousePos[1];
    m_mousePos = renderer->getMousePos(m_rawMousePos);
    m_mousePos = m_mousePos.div(m_snapSize).floor().prod(m_snapSize);

    Debug::DrawRect(m_mousePos.x(), m_mousePos.y(), 10, 10, Color::blue());

    if (m_tool) {
        m_tool->onUpdate(dt);
    }

    renderUI(dt);
}

void LevelEditor::renderUI(double dt) {
    ImGui::Begin("Level Editor");

    ImGui::SeparatorText("Stats");
    ImGui::Text(("FPS: " + std::to_string(1.0 / dt)).c_str());
    ImGui::Text(("Raw Mouse: " + m_rawMousePos.toString()).c_str());
    ImGui::Text(("Mouse: " + m_mousePos.toString()).c_str());
    ImGui::SeparatorText("Settings");
    ImGui::Checkbox("Debug Render", &m_state->params.debugRender);
    ImGui::Checkbox("VSync", &m_state->params.vsync);
    ImGui::DragFloat2("Mouse Snap", m_snapSize.vector, 1, 1);

    ImGui::SeparatorText("Tools");
    for (const auto& tool : m_tools) {
        if (ImGui::Button(tool->getButtonLabel().c_str())) {
            m_tool = tool.get();
            m_tool->onInit();
        }
    }

    if (m_tool) {
        ImGui::SeparatorText(m_tool->getName().c_str());
        m_tool->renderUI(dt);
    }

    ImGui::SeparatorText("Entities");
    if (ImGui::Button("Add Entity")) {
        entities.add();
    }
    m_entityViewer.render(entities.root.get());

    ImGui::End();
}

void LevelEditor::selectTool(Tool *tool) {
    m_tool = tool;
    m_tool->onInit();
}
