//
// Created by henry on 12/12/23.
//
#include "entityViewer.h"
#include "imgui.h"
#include "componentExplorer.h"
#include "../../thirdparty/imgui/misc/cpp/imgui_stdlib.h"

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
        hg::ComponentFactory::Attach(entity, addComponent.value());
    }

    if (entity->components().size() > 0) {
        ImGui::SeparatorText("Components");
        for (const auto& component : entity->components()) {
            ImGui::Text(component->operator std::string().c_str());
            int index = 0;
            for (const auto& field : hg::ComponentFactory::GetFields(component->className())) {
                ImGui::PushID(index++);
                if (editComponentField(component, field)) {
                    std::cout << "Field: " << field.field << " updated\n";
                }
                ImGui::PopID();
            }
        }
    }
}