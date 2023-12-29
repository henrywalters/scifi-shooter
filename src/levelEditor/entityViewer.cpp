//
// Created by henry on 12/12/23.
//
#include "entityViewer.h"
#include "imgui.h"
#include "componentExplorer.h"
#include "../../thirdparty/imgui/misc/cpp/imgui_stdlib.h"
#include "events.h"

void entityViewer(hg::Entity* entity) {
    ImGui::InputText("Name", &entity->name);

    if (entity->name.c_str() == "") {
        entity->name = "Entity<" + std::to_string(entity->id()) + ">";
    }

    ImGui::DragFloat3("Position", entity->transform.position.vector);
    ImGui::DragFloat3("Scale", entity->transform.scale.vector);

    hg::Vec3 axis = entity->transform.rotation.rotatePoint(hg::Vec3::Right());
    float degrees = std::atan2(axis[1], axis[0]) * hg::math::RAD2DEG;
    ImGui::DragFloat("Rotation", &degrees);

    entity->transform.rotation = hg::math::Quaternion<float>(degrees * hg::math::DEG2RAD, hg::Vec3::Face());

    if (ImGui::Button("Add Component")) {
        ImGui::OpenPopup(COMPONENT_EXPLORER.c_str());
    }

    auto addComponent = componentExplorer();
    if (addComponent.has_value()) {
        auto component = hg::ComponentFactory::Attach(entity, addComponent.value());
        Events()->emit(EventTypes::AddComponent, Event{ComponentEvent{entity, component->className(), ""}});
    }

    int index = 0;

    if (entity->components().size() > 0) {
        ImGui::SeparatorText("Components");
        for (const auto& component : entity->components()) {
            ImGui::Separator();
            ImGui::Text(component->operator std::string().c_str());
            ImGui::SameLine();
            ImGui::PushID(index++);
            if (ImGui::Button("remove")) {
                hg::ComponentFactory::Get(component->className()).remove(entity);
                Events()->emit(EventTypes::RemoveComponent, Event{ComponentEvent{entity, component->className(), ""}});
            }
            ImGui::PopID();
            for (const auto& field : hg::ComponentFactory::GetFields(component->className())) {
                ImGui::PushID(index++);

                if (editComponentField(component, field)) {
                    Events()->emit(EventTypes::UpdateComponent, Event{ComponentEvent{entity, component->className(), field.field}});
                }

                if (field.field == "texture") {
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("png")) {
                            auto path = (char*) payload->Data;
                            field.setter(component, std::string(path));
                        }
                        ImGui::EndDragDropTarget();
                    }
                }

                ImGui::PopID();
            }

            component->uiUpdate();
        }
    }
}