//
// Created by henry on 12/5/23.
//
#include <hagame/graphics/windows.h>
#include <hagame/graphics/monitors.h>
#include "imgui.h"
#include "levelEditor.h"
#include "../systems/renderer.h"
#include "../levelEditor/tools/tilemapTool.h"
#include "../levelEditor/scriptExplorer.h"
#include "../components/startPoint.h"
#include "../systems/player.h"
#include "../systems/audio.h"

using namespace hg;
using namespace hg::utils;
using namespace hg::graphics;

void LevelEditor::onInit() {

    m_runtime = addChild<EditorRuntime>(m_window);

    m_entityTree.events.subscribe(EntityTree::EventTypes::SelectEntity, [&](auto e) {
        m_selectedEntity = e.entity;
    });

    m_entityTree.events.subscribe(EntityTree::EventTypes::AddChild, [&](auto e) {
        m_runtime->entities.add(e.entity);
    });

    m_entityTree.events.subscribe(EntityTree::EventTypes::RemoveEntity, [&](auto e) {
        if (e.entity == m_selectedEntity) {
            m_selectedEntity = nullptr;
        }
        m_runtime->entities.remove(e.entity);
    });

    m_entityTree.events.subscribe(EntityTree::EventTypes::AddChildTo, [&](auto e) {
        if (std::find(e.entity->children().begin(), e.entity->children().end(), e.target) !=
            e.entity->children().end()) {
            return; // Cant add a parent to its own child
        }
        e.target->addChild(e.entity);
    });

    m_entityTree.events.subscribe(EntityTree::EventTypes::DuplicateEntity, [&](EntityTree::Event e) {
        auto entity = m_runtime->entities.add((hg::Entity*) e.entity->parent());
        entity->transform = e.entity->transform;
        for (const auto& component : e.entity->components()) {
            auto newComponent = ComponentFactory::Attach(entity, component->className());
            for (const ComponentFactory::ComponentField& field : ComponentFactory::GetFields(component->className())) {
                field.setter(newComponent, field.getter(component));
            }
        }
    });

    m_scripts = CppLibraryManager::Register(
            "/home/henry/development/games/scifi-shooter/cmake-build-debug/libscripts.so");

    auto script_paths = hg::utils::d_listFiles(ASSET_DIR + "scripts");

    for (const auto path: script_paths) {
        m_scripts->registerScript(path);
    }

    m_runtime->m_state->params.debugRender = true;
    m_runtime->m_state->useLighting = false;

    m_runtime->events.subscribe([&](SceneEvent e) {
        if (e == SceneEvent::Initialized) {
            pause();
        }
    });

}

void LevelEditor::onUpdate(double dt) {

    m_window->clear();

    renderUI(game()->dt());

    auto renderer = m_runtime->getSystem<Renderer>();

    m_window->setVSync(m_runtime->m_state->params.vsync);

    m_rawMousePos = m_window->input.keyboardMouse.mouse.position;
    m_rawMousePos -= m_renderRect.pos;
    m_rawMousePos = m_rawMousePos.div(m_renderRect.size);
    m_rawMousePos[1] = 1.0 - m_rawMousePos[1];
    renderer->setRawMousePos(m_rawMousePos);
    m_mousePos = renderer->getMousePos();

}

void LevelEditor::renderUI(double dt) {

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
                newScene();
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

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoMove |
            ImGuiConfigFlags_DockingEnable |
            ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeightWithSpacing()));
    ImGui::SetNextWindowSize(ImVec2(m_window->size()[0], m_window->size()[1] - ImGui::GetFrameHeightWithSpacing()));

    ImGui::Begin("Level Editor", nullptr, flags);

    if (ImGui::BeginTable("columns", 3, ImGuiTableFlags_Resizable)) {
        ImGui::TableNextColumn();
        ImGui::BeginChild("Left");
        ImGui::DockSpace(ImGui::GetID("Left"));
        ImGui::EndChild();

        ImGui::TableNextColumn();

        if (ImGui::BeginTable("rows", 1, ImGuiTableFlags_Resizable)) {
            ImGui::TableNextRow();
            ImGui::BeginChild("Middle_Top");
            ImGui::DockSpace(ImGui::GetID("Middle_Top"));
            ImGui::EndChild();
            ImGui::TableNextRow();
            ImGui::BeginChild("Middle_Bottom");
            ImGui::DockSpace(ImGui::GetID("Middle_Bottom"));
            ImGui::EndChild();
            ImGui::EndTable();
        }

        ImGui::TableNextColumn();
        ImGui::BeginChild("Right");
        ImGui::DockSpace(ImGui::GetID("Right"));
        ImGui::EndChild();

        ImGui::EndTable();
    }

    ImGui::End();

    renderEntityWindow(dt);
    renderScriptWindow(dt);
    renderSelectedEntityWindow(dt);
    renderSettingsWindow(dt);
    renderAssetWindow(dt);
    renderRenderWindow(dt);

    m_browser.render();
}

void LevelEditor::renderEntityWindow(double dt) {

    ImGui::Begin("Entities");

    if (ImGui::Button("Add Entity")) {
        m_runtime->entities.add();
    }

    m_entityTree.render(this, m_runtime->entities.root.get());

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


    for (const auto& script : m_runtime->scripts()) {
        ImGui::Text(script->getDef().name.c_str());
        ImGui::SameLine();
        if (ImGui::Button("Remove")) {
            m_runtime->removeScript(script.get());
        }
    }

    auto newScript = scriptExplorer();

    if (newScript.has_value()) {
        auto script = m_scripts->get(newScript.value().name);
        auto cppScript = m_runtime->addScript<CppScript>(script);
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

void LevelEditor::newScene() {
    m_runtime->clear();
    m_runtimeData = m_runtime->save();
    m_selectedEntity = nullptr;
}

void LevelEditor::saveAs() {
    m_browser.saveFile([&](auto path){
        m_saveFile = path;
        saveToDisc();
    }, {LEVEL_EXT});
}

void LevelEditor::saveToDisc() {
    auto config = m_runtime->save();
    utils::f_write(m_saveFile, config.toString());
}

void LevelEditor::loadFromDisc() {
    m_browser.loadFile([&](auto path) {
        m_saveFile = path;
        auto config = utils::MultiConfig::Parse(m_saveFile);
        newScene();
        m_runtime->load(config);
    }, {LEVEL_EXT});
}

void LevelEditor::onAfterUpdate() {

}

void LevelEditor::renderRenderWindow(double dt) {
    ImGui::Begin("Render");

    if (m_runtime->active()) {
        if (ImGui::Button("Pause")) {
            pause();
        }
    } else {
        if (ImGui::Button("Play")) {
            play();
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset")) {
        reset();
    }

    ImGui::BeginChild("RendererOutput");
    {
        auto size = ImGui::GetWindowSize();
        float aspectRatio = (float) GAME_SIZE[1] / GAME_SIZE[0];

        ImVec2 imageSize;

        if (size[0] * aspectRatio > size[1]) {
            imageSize = ImVec2(size[1] / aspectRatio, size[1]);
        } else {
            imageSize = ImVec2(size[0], size[0] * aspectRatio);
        }

        m_renderRect.size = Vec2(imageSize.x, imageSize.y);
        m_renderRect.pos = Vec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);

        ImGui::Image((void *) m_runtime->getSystem<Renderer>()->getRender()->id, imageSize, ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::EndChild();

    ImGui::End();
}

void LevelEditor::renderSettingsWindow(double dt) {
    ImGui::Begin("Settings");

    ImGui::SeparatorText("Stats");
    ImGui::Text(("FPS: " + std::to_string(1.0 / dt)).c_str());
    ImGui::Text(("Raw Mouse: " + m_rawMousePos.toString()).c_str());
    ImGui::Text(("Mouse: " + m_mousePos.toString()).c_str());
    ImGui::SeparatorText("Settings");
    ImGui::Checkbox("Debug Render", &m_runtime->m_state->params.debugRender);
    ImGui::Checkbox("Render Lighting", &m_runtime->m_state->useLighting);
    ImGui::Checkbox("VSync", &m_runtime->m_state->params.vsync);
    ImGui::DragFloat2("Mouse Snap", m_snapSize.vector, 1, 1);

    ImGui::Text(("Window: " + m_window->size().toString() + ", " + m_window->pos().toString()).c_str());

    ImGui::End();
}

void LevelEditor::play() {
    if (!m_playing) {

        Vec2 startPos;

        int startPointCount = 0;

        m_runtime->entities.forEach<StartPoint>([&](auto start, Entity* entity) {
            startPos = entity->transform.position.resize<2>();
            startPointCount++;
        });

        if (startPointCount == 0) {
            std::cout << "WARNING: No Start Point. Add it as a component\n";
        } else if (startPointCount > 1) {
            std::cout << "WARNING: Multiple Start Points found. A random one will be chosen!\n";
        }

        m_playing = true;
        m_runtimeData = m_runtime->save();
    } else {
        m_runtime->getSystem<AudioSystem>()->play();
    }

    m_runtime->active(true);
}

void LevelEditor::pause() {
    m_runtime->getSystem<AudioSystem>()->pause();
    m_runtime->active(false);
}

void LevelEditor::reset() {
    if (m_playing) {
        m_runtime->getSystem<AudioSystem>()->stop();
        m_runtime->active(false);
        m_playing = false;
        m_selectedEntity = nullptr;
        m_runtime->clear();
        m_runtime->load(m_runtimeData);
    }
}