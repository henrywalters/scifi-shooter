//
// Created by henry on 12/5/23.
//
#include "entityTree.h"
#include "imgui.h"
#include "events.h"

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

    if (m_selected && entity->id() == m_selected->id()) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    bool open = ImGui::TreeNodeEx(entity->name.c_str(), flags);

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
        m_selected = entity;
        Events()->emit(EventTypes::SelectEntity, Event{EntityEvent{entity, nullptr}});
    }
    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        ImGui::OpenPopup(std::to_string(entity->id()).c_str());
    }

    if (ImGui::BeginPopup(std::to_string(entity->id()).c_str())) {
        if (ImGui::Button("Add Child")) {
            Events()->emit(EventTypes::AddChild, Event{EntityEvent{entity, nullptr}});
        }

        if (ImGui::Button("Duplicate")) {
            Events()->emit(EventTypes::DuplicateEntity, Event{EntityEvent{entity, nullptr}});
        }

        if (!root && ImGui::Button("Delete")) {
            Events()->emit(EventTypes::RemoveEntity, Event{EntityEvent{entity, nullptr}});
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
            Events()->emit(EventTypes::AddChildTo, Event{EntityEvent{dragging, entity}});
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


