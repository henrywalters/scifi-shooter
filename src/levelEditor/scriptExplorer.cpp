//
// Created by henry on 12/10/23.
//
#include "scriptExplorer.h"
#include "imgui.h"

std::optional<hg::ScriptDef> scriptExplorer(hg::CppScriptManager* manager) {
    std::optional<hg::ScriptDef> out = std::nullopt;

    if (ImGui::BeginPopup(SCRIPT_EXPLORER.c_str())) {
        for (const auto& script : manager->scriptDefs()) {
            if (ImGui::Selectable(script.name.c_str())) {
                out = script;
            }
        }
        ImGui::EndPopup();
    }

    return out;
}