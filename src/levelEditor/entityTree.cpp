//
// Created by henry on 12/5/23.
//
#include "entityTree.h"
#include "imgui.h"

void EntityTree::render(hg::Scene* scene, hg::Entity *root) {
    renderTree(scene, root, true);
}

void EntityTree::renderTree(hg::Scene* scene, hg::Entity* entity, bool root) {
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
        events.emit(EventTypes::SelectEntity, Event{entity, nullptr});
    }
    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        ImGui::OpenPopup(std::to_string(entity->id()).c_str());
    }

    if (ImGui::BeginPopup(std::to_string(entity->id()).c_str())) {
        if (ImGui::Button("Add Child")) {
            events.emit(EventTypes::AddChild, {entity, nullptr});
        }

        if (!root && ImGui::Button("Delete")) {
            events.emit(EventTypes::RemoveEntity, {entity, nullptr});
        }

        ImGui::EndPopup();
    }

    if (!root && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        auto id = entity->id();
        ImGui::SetDragDropPayload("ENTITY", (void *) &id, sizeof(hg::utils::uuid_t));
        ImGui::Text(entity->name.c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY")) {
            hg::utils::uuid_t entityId = *(const hg::utils::uuid_t *) payload->Data;
            auto dragging = scene->entities.get(entityId);
            events.emit(EventTypes::AddChildTo, {dragging, entity});
        }
        ImGui::EndDragDropTarget();
    }

    if (open) {
        for (const auto& child : entity->children()) {
            renderTree(scene, (hg::Entity*)child, false);
        }
        ImGui::TreePop();
    }
}


