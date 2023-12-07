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
    ImGui::Text(entity->name.c_str());
    ImGui::DragFloat3("Position", entity->transform.position.vector);
    ImGui::DragFloat4("Rotation", entity->transform.rotation.vector);
    ImGui::DragFloat3("Scale", entity->transform.scale.vector);

    if (entity->components().size() > 0) {
        ImGui::Text("Components");
        for (const auto& component : entity->components()) {
            ImGui::Text(component->operator std::string().c_str());
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


