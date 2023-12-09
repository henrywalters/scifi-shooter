//
// Created by henry on 12/5/23.
//
#include "entityViewer.h"

void EntityViewer::render(hg::Entity *root) {
    renderTree(root);
    if (m_selected) {
        renderEntity(m_selected);
    }
}

void EntityViewer::renderEntity(hg::Entity *entity) {
    ImGui::SeparatorText(entity->name.c_str());
    ImGui::InputText("Name", entity->name.data(), 500);

    if (entity->name.c_str() == "") {
        entity->name = "Entity<" + std::to_string(entity->id()) + ">";
    }

    ImGui::DragFloat3("Position", entity->transform.position.vector);
    ImGui::DragFloat4("Rotation", entity->transform.rotation.vector);
    ImGui::DragFloat3("Scale", entity->transform.scale.vector);

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
            for (const auto& field : hg::ComponentFactory::GetFields(component->className())) {
                if (editComponentField(component, field)) {
                    std::cout << "Field: " << field.field << " updated\n";
                }
            }
        }
    }
}

void EntityViewer::renderTree(hg::Entity* entity) {
    ImGui::ShowDemoWindow();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;

    if (entity->children().size() > 0) {
        flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    } else {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (entity == m_selected) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    bool open = ImGui::TreeNodeEx(entity->name.c_str(), flags);

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
        m_selected = entity;
    }

    if (open) {

        for (const auto& child : entity->children()) {
            renderTree((hg::Entity*)child);
        }
        ImGui::TreePop();
    }
}


