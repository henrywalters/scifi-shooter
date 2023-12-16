//
// Created by henry on 12/5/23.
//
#include "imgui.h"
#include "levelEditor.h"
#include "../systems/renderer.h"
#include "../levelEditor/tools/tilemapTool.h"
#include "../levelEditor/scriptExplorer.h"

using namespace hg;
using namespace hg::graphics;

void LevelEditor::onInit() {

    m_entityTree.events.subscribe(EntityTree::EventTypes::SelectEntity, [&](auto e) {
        m_selectedEntity = e.entity;
    });

    m_entityTree.events.subscribe(EntityTree::EventTypes::AddChild, [&](auto e) {
        entities.add(e.entity);
    });

    m_entityTree.events.subscribe(EntityTree::EventTypes::RemoveEntity, [&](auto e) {
        if (e.entity == m_selectedEntity) {
            m_selectedEntity = nullptr;
        }
        entities.remove(e.entity);
    });

    m_entityTree.events.subscribe(EntityTree::EventTypes::AddChildTo, [&](auto e) {
        if (std::find(e.entity->children().begin(), e.entity->children().end(), e.target) != e.entity->children().end()) {
            return; // Cant add a parent to its own child
        }
        e.target->addChild(e.entity);
    });

    m_scripts = CppLibraryManager::Register("/home/henry/development/games/scifi-shooter/cmake-build-debug/libscripts.so");

    auto script_paths = hg::utils::d_listFiles(ASSET_DIR + "scripts");

    for (const auto path : script_paths) {
        m_scripts->registerScript(path);
    }

    m_state->params.debugRender = true;
    m_state->useLighting = false;

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

    for (int i = 0; i < 1; i++) {
        auto entity = entities.add();
        auto sprite = entity->addComponent<hg::graphics::Quad>();
        sprite->size = hg::Vec2(15.0);
        sprite->color = m_state->randomColor();
        entity->transform.position = hg::Vec3(m_state->random.real<float>(-100, 100), m_state->random.real<float>(-100, 100), 0);
    }
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

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
                reset();
            }

            if (ImGui::MenuItem("Save As")) {
                saveAs();
            }

            if (m_saveFile != "" && ImGui::MenuItem("Save")) {
                saveToDisc();
            }

            if (ImGui::MenuItem("Load")) {
                loadFromDisc();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Level Editor");

    ImGui::SeparatorText("Stats");
    ImGui::Text(("FPS: " + std::to_string(1.0 / dt)).c_str());
    ImGui::Text(("Raw Mouse: " + m_rawMousePos.toString()).c_str());
    ImGui::Text(("Mouse: " + m_mousePos.toString()).c_str());
    ImGui::SeparatorText("Settings");
    ImGui::Checkbox("Debug Render", &m_state->params.debugRender);
    ImGui::Checkbox("Render Lighting", &m_state->useLighting);
    ImGui::Checkbox("VSync", &m_state->params.vsync);
    ImGui::DragFloat2("Mouse Snap", m_snapSize.vector, 1, 1);

    ImGui::End();

    renderEntityWindow(dt);
    renderScriptWindow(dt);
    renderSelectedEntityWindow(dt);
    renderAssetWindow(dt);

    m_browser.render();
}

void LevelEditor::selectTool(Tool *tool) {
    m_tool = tool;
    m_tool->onInit();
}

void LevelEditor::renderEntityWindow(double dt) {

    ImGui::Begin("Entities");

    if (ImGui::Button("Add Entity")) {
        entities.add();
    }

    m_entityTree.render(this, entities.root.get());

    ImGui::End();
}

void LevelEditor::renderScriptWindow(double dt) {
    ImGui::Begin("Scripts");

    if (ImGui::Button("Add Script")) {
        ImGui::OpenPopup(SCRIPT_EXPLORER.c_str());
    }
    ImGui::SameLine();
    if (ImGui::Button("Recompile")) {
        int code = system("/home/henry/Downloads/CLion-2022.3.1/clion-2022.3.1/bin/cmake/linux/bin/cmake --build /home/henry/development/games/scifi-shooter/cmake-build-debug --target scripts -j 16");
        std::cout << "CODE = " << code << "\n";
        m_scripts->reload();
    }


    for (const auto& script : scripts()) {
        ImGui::Text(script->getDef().name.c_str());
        ImGui::SameLine();
        if (ImGui::Button("Remove")) {
            removeScript(script.get());
        }
    }

    auto newScript = scriptExplorer();

    if (newScript.has_value()) {
        auto script = m_scripts->get(newScript.value().name);
        auto cppScript = this->addScript<CppScript>(script);
        cppScript->init();
    }

    ImGui::End();
}

void LevelEditor::renderAssetWindow(double dt) {
    ImGui::Begin("Assets");

    m_assets.render();

    ImGui::End();
}

void LevelEditor::renderSelectedEntityWindow(double dt) {
    if (!m_selectedEntity) {
        return;
    }

    ImGui::Begin("Entity");

    entityViewer(m_selectedEntity);

    ImGui::End();
}

void LevelEditor::reset() {
    entities.clear();
    // m_activeScripts.clear();
    m_selectedEntity = nullptr;
}

void LevelEditor::saveAs() {
    m_browser.saveFile([&](auto path){
        m_saveFile = path;
        saveToDisc();
    }, {LEVEL_EXT});
}

void LevelEditor::saveToDisc() {
    auto config = save();
    utils::f_write(m_saveFile, config.toString());
}

void LevelEditor::loadFromDisc() {
    m_browser.loadFile([&](auto path) {
        m_saveFile = path;
        auto config = utils::MultiConfig::Parse(m_saveFile);
        reset();
        load(config);
    }, {LEVEL_EXT});
}

