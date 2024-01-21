//
// Created by henry on 1/16/24.
//
#include "ui.h"

using namespace hg;
using namespace std;

Entity * ui::SelectEntity(string label, Scene *scene, Entity* selected, string selectMessage, string deselectMessage) {
    std::string preview = selected ? selected->name : selectMessage;
    Entity* out = selected;
    if (ImGui::BeginCombo(label.c_str(), preview.c_str())) {
        if (selected && ImGui::Selectable(deselectMessage.c_str())) {
            out = nullptr;
        }
        scene->entities.forEach([&](auto other) {
            if (ImGui::Selectable(other->name.c_str(), selected && selected->id() == other->id())) {
                out = other;
            }
        });
        ImGui::EndCombo();
    }
    return out;
}
