//
// Created by henry on 12/27/23.
//
#include "shaderTool.h"
#include <hagame/core/assets.h>
#include "imgui.h"
#include "../../thirdparty/imgui/misc/cpp/imgui_stdlib.h"
#include "../events.h"

void ShaderTool::renderOverlay() {
    Tool::renderOverlay();
}

void ShaderTool::onInit() {
    Events()->subscribe(EventTypes::SelectAsset, [&](const auto& e) {
        AssetEvent file = std::get<AssetEvent>(e.payload);
        if (file.extension == "glsl" || file.extension == "frag" || file.extension == "vert") {
            if (!m_open) {
                open();
            }
            loadShader(file.absolutePath());
        }
    });
}

void ShaderTool::renderUI(double dt) {
    ImGui::Begin("Shader Tool");

    if (m_shaders.size() == 0) {
        ImGui::Text("Drag / click on a Shader file (.glsl / .frag / .vert)");
        ImGui::Dummy(ImGui::GetContentRegionAvail());
        if (ImGui::BeginDragDropTarget()) {
            acceptDragDrop();
            ImGui::EndDragDropTarget();
        }
    } else {
        ImGuiTabBarFlags flags = ImGuiTabBarFlags_Reorderable;
        if (ImGui::BeginTabBar("Shaders", flags)) {
            for (const auto& shader : m_shaders) {
                bool open = true;
                std::string label = shader->file.name + "." + shader->file.extension;
                if (ImGui::BeginTabItem(label.c_str(), &open)) {
                    if (ImGui::Button("Save & Compile")) {
                        m_error = std::nullopt;
                        try {
                            hg::utils::f_write(shader->file.absolutePath(), shader->contents);
                            hg::recompileShader(shader->file.name);
                        } catch (std::exception& e) {
                            m_error = e.what();
                        }
                    }

                    ImGui::InputTextMultiline("Source", &shader->contents, ImGui::GetContentRegionAvail());

                    if (m_error.has_value()) {
                        ImGui::Text(("Error: " + m_error.value()).c_str());
                    }

                    if (ImGui::BeginDragDropTarget()) {
                        acceptDragDrop();
                        ImGui::EndDragDropTarget();
                    }

                    ImGui::EndTabItem();
                }
                if (!open) {
                    m_shaders.erase(std::remove(m_shaders.begin(), m_shaders.end(), shader), m_shaders.end());
                }
            }
            ImGui::EndTabBar();
        }

    }

    ImGui::End();
}

void ShaderTool::onUpdate(double dt) {
    Tool::onUpdate(dt);
}

void ShaderTool::acceptDragDrop() {
    std::vector<std::string> extensions = {"glsl", "vert", "frag"};

    for (const auto& ext : extensions) {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(ext.c_str());
        if (payload) {
            auto path = std::string((char*) payload->Data);
            loadShader(path);
        }
    }
}

void ShaderTool::loadShader(std::string path) {
    m_shaders.push_back(std::make_unique<Shader>());
    auto shader = m_shaders[m_shaders.size() - 1].get();
    shader->file = hg::utils::f_getParts(path);
    if (hg::utils::f_exists(shader->file.absolutePath())) {
        shader->contents = hg::utils::f_read(shader->file.absolutePath());
    }
}
