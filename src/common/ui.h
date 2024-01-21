//
// Created by henry on 1/16/24.
//

#ifndef SCIFISHOOTER_UI_H
#define SCIFISHOOTER_UI_H

#include "imgui.h"
#include <hagame/core/scene.h>

// This file contains a set of helpful ImGUI utilities

namespace ui {

    // A generic Combo box for selecting values. If using non to_string able types, provide a valueLabels function
    template <typename T>
    T Select(
        std::string label,
        std::vector<T> values,
        T selected,
        T defaultValue,
        std::function<std::string(T)> valueLabels = [](auto a){ return std::to_string(a); }
    );

    // Select an entity from a scene - will return null or a valid pointer
    hg::Entity* SelectEntity(
        std::string label,
        hg::Scene* scene,
        hg::Entity* selected = nullptr,
        std::string selectMessage = "-- Select Entity --",
        std::string deselectMessage = "-- Deselect --"
    );
}


template <typename T>
T ui::Select(
        std::string label,
        std::vector<T> values,
        T selected,
        T defaultValue,
        std::function<std::string(T)> valueLabels
) {
    std::string preview = valueLabels(selected);
    T out = selected;
    if (ImGui::BeginCombo(label.c_str(), preview.c_str())) {
        ImGui::PushID(0);
        if (ImGui::Selectable(valueLabels(defaultValue).c_str())) {
            out = defaultValue;
        }
        ImGui::PopID();
        for (int i = 0; i < values.size(); i++) {
            ImGui::PushID(i + 1);
            if (ImGui::Selectable(valueLabels(values[i]).c_str(), selected == values[i])) {
                out = values[i];
            }
            ImGui::PopID();
        };
        ImGui::EndCombo();
    }
    return out;
}

#endif //SCIFISHOOTER_UI_H
