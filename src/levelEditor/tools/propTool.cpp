//
// Created by henry on 1/9/24.
//
#include <hagame/core/assets.h>
#include "propTool.h"
#include "imgui.h"
#include "../../thirdparty/imgui/misc/cpp/imgui_stdlib.h"

#define DIRTY(Code) if (Code) { m_dirty = true; }

void PropTool::onInit() {
    m_selected.events.subscribe([&](SelectedEntityEvents event) {
        if (event == SelectedEntityEvents::Selected) {
            if (m_selected.component()->def) {
                m_selectedProp = *m_selected.component()->def;
            } else {
                m_selectedProp = PropDef{};
            }
            open();
        } else {
            close();
        }
    });
}

void PropTool::renderUI(double dt) {

    if (!m_loadedProps) {
        m_loadedProps = true;
        m_scene->getSystem<Props>()->store().forEach([&](auto key, auto prop) {
            m_props.push_back(prop);
        });
    }

    ImGui::Begin("Prop Tool", &m_open);

    auto propComponent = m_selected.component();

    auto preview = m_selectedProp.tag;

    if (ImGui::BeginCombo("Prop Type", preview.c_str())) {
        if (ImGui::Selectable("-- New Prop --")) {
            m_selectedProp = PropDef{};
        }
        for (const auto& prop : m_props) {
            const bool isSelected = m_selectedProp.id() == prop->id();
            if (ImGui::Selectable(prop->tag.c_str(), isSelected)) {
                m_selectedProp = *prop;
            }
        }
        ImGui::EndCombo();
    }

    DIRTY(ImGui::InputText("Tag", &m_selectedProp.tag));
    DIRTY(ImGui::InputFloat2("Size", m_selectedProp.size.vector));

    if (m_selectedProp.states.size() > 0 && ImGui::BeginCombo("Default State", getState(m_selectedProp.defaultStateId).name.c_str())) {
        for (int i = 0; i < m_selectedProp.states.size(); i++) {
            ImGui::PushID(i);
            if (ImGui::Selectable(m_selectedProp.states[i].name.c_str(), m_selectedProp.states[i].id == m_selectedProp.defaultStateId)) {
                m_selectedProp.defaultStateId = m_selectedProp.states[i].id;
            }
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }

    ImGui::SeparatorText("States");
    int index = 0;
    for (auto& [id, state] : m_selectedProp.states) {
        ImGui::Separator();
        ImGui::PushID(index++);
        DIRTY(ImGui::InputText("Name", &state.name));
        DIRTY(ImGui::InputText("Texture", &state.texture));
        DIRTY(ImGui::Checkbox("Collide", &state.collide));
        DIRTY(ImGui::Checkbox("Trigger Only", &state.triggerOnly));
        DIRTY(ImGui::InputText("Message", &state.message));
        auto preview = state.nextStateId.has_value() ? getState(state.nextStateId.value()).name : "";
        if (ImGui::BeginCombo("Next State", preview.c_str())) {
            if (ImGui::Selectable("-- No State --")) {
                state.nextStateId = std::nullopt;
                m_dirty = true;
            }
            for (const auto& [id, other] : m_selectedProp.states) {
                bool selected = state.nextStateId.has_value() && id == state.nextStateId.value();
                if (ImGui::Selectable(other.name.c_str(), selected)) {
                    state.nextStateId = other.id;
                    m_dirty = true;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::PopID();
    }

    if (m_error.has_value()) {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
        ImGui::Text("%s",m_error.value().c_str());
        ImGui::PopStyleColor();
    }

    if (ImGui::Button("Add State")) {
        PropDef::State state;
        state.id = hg::utils::UUID::Generate();
        m_selectedProp.states.insert(std::make_pair(state.id, state));
        if (m_selectedProp.states.size() == 1) {
            m_selectedProp.defaultStateId = state.id;
        }
    }

    if (m_dirty) {
        if (ImGui::Button("Save")) {

            m_error = std::nullopt;

            bool newProp = true;
            for (const auto& prop : m_props) {
                if (prop->id() == m_selectedProp.id()) {
                    prop->tag = m_selectedProp.tag;
                    prop->size = m_selectedProp.size;
                    prop->states = m_selectedProp.states;
                    prop->defaultStateId = m_selectedProp.defaultStateId;
                    newProp = false;
                    break;
                }
            }

            if (m_selectedProp.states.size() == 0) {
                m_error = "Must have at least one state";
            } else {
                if (newProp) {
                    m_props.push_back(std::make_shared<PropDef>(m_selectedProp));
                }

                for (const auto& prop : m_props) {
                    m_scene->getSystem<Props>()->store().set(prop->id(), prop);
                }

                hg::utils::MultiConfig config;
                m_scene->getSystem<Props>()->save(config);

                hg::utils::f_write(hg::ASSET_DIR + "props.hg", config.toString());

                m_dirty = false;
            }
        }
    }

    ImGui::End();
}

PropDef::State PropTool::getState(hg::utils::uuid_t id) {
    if (m_selectedProp.states.find(id) == m_selectedProp.states.end()) {
        throw std::runtime_error("State does not exist!");
    }
    return m_selectedProp.states.at(id);
}


