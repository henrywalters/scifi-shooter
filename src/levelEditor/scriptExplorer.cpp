//
// Created by henry on 12/10/23.
//
#include "scriptExplorer.h"
#include "imgui.h"

std::optional<hg::ScriptDef> scriptExplorer() {
    std::optional<hg::ScriptDef> out = std::nullopt;

    if (ImGui::BeginPopup(SCRIPT_EXPLORER.c_str())) {

        for (const auto& lib : hg::CppLibraryManager::All()) {
            auto fileParts = hg::utils::f_getParts(lib->libPath());
            ImGui::SeparatorText(fileParts.fullName.c_str());
            for (const auto& script : lib->scriptDefs()) {
                if (ImGui::Selectable(script.name.c_str())) {
                    out = script;
                }
            }
        }

        ImGui::EndPopup();
    }

    return out;
}