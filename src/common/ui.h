//
// Created by henry on 1/16/24.
//

#ifndef SCIFISHOOTER_UI_H
#define SCIFISHOOTER_UI_H

#include "imgui.h"
#include <hagame/utils/enum.h>
#include <hagame/core/scene.h>

// This file contains a set of helpful ImGUI utilities

namespace ui {

    // A generic Combo box for selecting values. If using non to_string able types, provide a valueLabels function
    template <typename T>
    bool Select(
        std::string label,
        std::vector<T> values,
        T& selected,
        T defaultValue,
        std::function<std::string(T)> valueLabels = [](auto a){ return std::to_string(a); }
    );

    bool SelectEnum(
        std::string label,
        std::vector<hg::utils::EnumElement> enums,
        hg::utils::enum_t& selected
    );

    template <typename T>
    T SelectNullable(
        std::string label,
        std::vector<T> values,
        T selected,
        std::string nullMessage,
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

    bool SelectAsset(
        std::string extension,
        std::string directory,
        std::string label,
        std::string& asset,
        std::function<std::string(hg::utils::FileParts)> formatter = [](auto parts) {return parts.absolutePath(); },
        std::string selectMessage = "-- Select Asset --",
        std::string deselectMessage = "-- Deselect --"
    );

    bool SelectTexture(
        std::string label,
        std::string& asset
    );

    bool SelectParticle(
        std::string label,
        std::string& asset
    );

    bool SelectWav(
        std::string label,
        std::string& asset
    );

    bool SelectSpriteSheet(
        std::string label,
        std::string& asset
    );
}


template <typename T>
bool ui::Select(
        std::string label,
        std::vector<T> values,
        T& selected,
        T defaultValue,
        std::function<std::string(T)> valueLabels
) {
    bool dirty = false;
    std::string preview = valueLabels(selected);
    if (ImGui::BeginCombo(label.c_str(), preview.c_str())) {
        ImGui::PushID(0);
        if (ImGui::Selectable(valueLabels(defaultValue).c_str())) {
            selected = defaultValue;
            dirty = false;
        }
        ImGui::PopID();
        for (int i = 0; i < values.size(); i++) {
            ImGui::PushID(i + 1);
            if (ImGui::Selectable(valueLabels(values[i]).c_str(), selected == values[i])) {
                selected = values[i];
                dirty = true;
            }
            ImGui::PopID();
        };
        ImGui::EndCombo();
    }
    return dirty;
}

template<typename T>
T ui::SelectNullable(
    std::string label,
    std::vector<T> values,
    T selected,
    std::string nullMessage,
    std::function<std::string(T)> valueLabels)
{
    std::string preview = selected ? valueLabels(selected) : nullMessage;
    T out = selected;
    if (ImGui::BeginCombo(label.c_str(), preview.c_str())) {
        ImGui::PushID(0);
        if (ImGui::Selectable(nullMessage.c_str())) {
            out = nullptr;
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
