//
// Created by henry on 12/5/23.
//
#include <hagame/graphics/windows.h>
#include "imgui.h"
#include "levelEditor.h"
#include "../systems/renderer.h"
#include "../levelEditor/tools/tilemapTool.h"
#include "../levelEditor/tools/shaderTool.h"
#include "../levelEditor/scriptExplorer.h"
#include "../levelEditor/events.h"
#include "../components/startPoint.h"
#include "../systems/audio.h"
#include "../systems/player.h"
#include "../levelEditor/tools/propTool.h"

using namespace hg;
using namespace hg::utils;
using namespace hg::graphics;

void LevelEditor::onInit() {

    m_runtime = addChild<EditorRuntime>(m_window);

    Windows::Events.subscribe(WindowEvents::Close, [&](WindowEvent e) {
        m_selectedEntity = nullptr;
    });

    Events()->subscribe(EventTypes::SelectEntity, [&](const auto& e) {
        m_selectedEntity = std::get<EntityEvent>(e.payload).entity;
    });

    Events()->subscribe(EventTypes::AddChild, [&](const auto& e) {
        m_runtime->entities.add(std::get<EntityEvent>(e.payload).entity);
    });

    Events()->subscribe(EventTypes::RemoveEntity, [&](const auto& e) {
        auto payload = std::get<EntityEvent>(e.payload);
        if (payload.entity == m_selectedEntity) {
            m_selectedEntity = nullptr;
        }
        m_runtime->entities.remove(payload.entity);
    });

    Events()->subscribe(EventTypes::AddChildTo, [&](const auto& e) {
        auto payload = std::get<EntityEvent>(e.payload);
        if (std::find(payload.entity->children().begin(), payload.entity->children().end(), payload.target) !=
            payload.entity->children().end()) {
            return; // Cant add a parent to its own child
        }
        payload.target->addChild(payload.entity);
    });

    Events()->subscribe(EventTypes::DuplicateEntity, [&](const auto& e) {
        auto payload = std::get<EntityEvent>(e.payload);
        auto entity = m_runtime->entities.add((hg::Entity*) payload.entity->parent());
        entity->transform = payload.entity->transform;
        for (const auto& component : payload.entity->components()) {
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

    m_tools.push_back(std::make_unique<TilemapTool>(m_runtime));
    m_tools.push_back(std::make_unique<ShaderTool>(m_runtime));
    m_tools.push_back(std::make_unique<PropTool>(m_runtime));

    for (const auto& tool : m_tools) {
        tool->init();
    }
}

void LevelEditor::onUpdate(double dt) {

    if (m_runtime->active()) {
        m_elapsedTime += dt;
    }

    m_window->clear();

    renderUI(game()->dt());

    if (!m_runtime->hasSystem<Renderer>()) {
        return;
    }

    auto renderer = m_runtime->getSystem<Renderer>();

    m_window->setVSync(m_runtime->m_state->params.vsync);

    m_rawMousePos = m_window->input.keyboardMouse.mouse.position;
    m_rawMousePos -= m_renderRect.pos;
    m_rawMousePos = m_rawMousePos.div(m_renderRect.size);
    m_rawMousePos[1] = 1.0 - m_rawMousePos[1];
    renderer->setRawMousePos(m_rawMousePos);
    m_mousePos = renderer->getMousePos();

    if (m_rawMousePos.x() >= 0 && m_rawMousePos.x() <= 1 && m_rawMousePos.y() >= 0 && m_rawMousePos.y() <= 1.0) {
        renderer->m_camera.transform.position += m_window->input.keyboardMouse.lAxis.resize<3>() * m_panSpeed * dt;
        m_runtime->state()->zoom = std::clamp<float>(renderer->m_camera.zoom + (float) m_window->input.keyboardMouse.mouse.wheel * m_zoomSpeed, 0.0001, 3);
        for (const auto& tool : m_tools) {
            tool->update(m_mousePos, dt);
        }
    }

    for (const auto& tool : m_tools) {
        renderer->addOverlay(RenderMode::Debug, [&tool, dt]() {
            tool->render(dt);
        });
    }
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

            if (!m_saveFile.empty() && ImGui::MenuItem("Save")) {
                saveToDisc();
            }

            if (ImGui::MenuItem("Load")) {
                loadFromDisc();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools")) {
            for (const auto& tool : m_tools) {
                if (ImGui::MenuItem(tool->getName().c_str())) {
                    tool->open();
                }
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

    m_entityTree.render(m_runtime, m_runtime->entities.root.get());

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
    reset();
    Events()->emit(EventTypes::NewLevel, Event{m_runtime});
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
    Events()->emit(EventTypes::SaveLevel, Event{m_runtime});
}

void LevelEditor::loadFromDisc() {
    m_browser.loadFile([&](auto path) {
        m_saveFile = path;
        auto config = utils::MultiConfig::Parse(m_saveFile);
        newScene();
        m_runtime->load(config);
        Events()->emit(EventTypes::LoadLevel, Event{m_runtime});
    }, {LEVEL_EXT});
}

void LevelEditor::onAfterUpdate() {

}

void LevelEditor::renderRenderWindow(double dt) {
    ImGui::Begin("Render");

    ImVec2 btnSize(24, 24);

    float width = ImGui::GetContentRegionAvail().x;
    float padding = width / 2.0 - btnSize.x - 8;

    ImGui::SetCursorPosX(padding);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);

    if (m_runtime->active()) {
        if (ImGui::ImageButton("Pause", (void*)getTexture("ui/pause")->id, btnSize)) {
            pause();
        }
    } else {
        if (ImGui::ImageButton("Play", (void*)getTexture("ui/play")->id, btnSize)) {
            play();
        }
    }

    ImGui::SameLine();

    if (ImGui::ImageButton("Reset", (void*)getTexture("ui/reset")->id, btnSize)) {
        reset();
    }

    ImGui::PopStyleVar(2);

    ImGui::SameLine();

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << m_elapsedTime;
    ImGui::Text(("Running for: " + ss.str() + "s").c_str());

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

        if (m_runtime->hasSystem<Renderer>()) {
            ImGui::Image((void *) m_runtime->getSystem<Renderer>()->getRender()->id, imageSize, ImVec2(0, 1), ImVec2(1, 0));
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void LevelEditor::renderSettingsWindow(double dt) {
    if (!m_runtime->hasSystem<Renderer>()) {
        return;
    }

    ImGui::Begin("Settings");

    auto renderer = m_runtime->getSystem<Renderer>();

    ImGui::SeparatorText("Stats");
    ImGui::Text(("FPS: " + std::to_string(1.0 / dt)).c_str());
    ImGui::Text(("Raw Mouse: " + m_rawMousePos.toString()).c_str());
    ImGui::Text(("Mouse: " + m_mousePos.toString()).c_str());
    ImGui::SeparatorText("Settings");
    ImGui::Checkbox("Debug Render", &m_runtime->m_state->params.debugRender);
    ImGui::Checkbox("Render Lighting", &m_runtime->m_state->useLighting);
    ImGui::Checkbox("VSync", &m_runtime->m_state->params.vsync);
    ImGui::DragFloat3("Camera Pos", renderer->m_camera.transform.position.vector);
    ImGui::DragFloat("Camera Zoom", &m_runtime->state()->zoom, 0.0001, 3, 0.001);

    ImGui::Text(("Window: " + m_window->size().toString() + ", " + m_window->pos().toString()).c_str());

    if (m_playing) {
        ImGui::Text(("Player Position: " + m_runtime->getSystem<Player>()->player->position().toString()).c_str());
    }

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

        m_runtime->getSystem<Player>()->spawn(startPos);

        m_runtime->activate();
    } else {
        m_runtime->getSystem<AudioSystem>()->play();
    }

    m_runtime->active(true);
}

void LevelEditor::pause() {
    m_runtime->getSystem<AudioSystem>()->pause();
    m_runtime->active(false);
}

void LevelEditor::reset(bool force) {
    if (m_playing || force) {
        if (m_runtime->hasSystem<AudioSystem>()) {
            m_runtime->getSystem<AudioSystem>()->stop();
        }
        m_runtime->deactivate();
        m_elapsedTime = 0;
        m_runtime->active(false);
        m_playing = false;
        m_selectedEntity = nullptr;
        m_runtime->clear();
        m_runtime->load(m_runtimeData);
        Events()->emit(EventTypes::LoadLevel, Event{m_runtime});
    }
}