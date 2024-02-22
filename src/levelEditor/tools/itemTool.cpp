//
// Created by henry on 1/21/24.
//
#include <hagame/core/assets.h>
#include "itemTool.h"
#include "imgui.h"
#include "../../common/ui.h"
#include "../../thirdparty/imgui/misc/cpp/imgui_stdlib.h"
#include "../../systems/items.h"

#define DIRTY(Code) if (Code) { m_dirty = true; }

void ItemTool::onInit() {

}

void ItemTool::renderUI(double dt) {
    if (!m_initialized) {
        m_initialized = true;
        m_scene->getSystem<Items>()->store().forEach([&](auto key, auto item) {
            m_items.push_back(item);
        });
    }

    ImGui::Begin("Item Tool", &m_open);

    m_selectedItem = ui::SelectNullable<std::shared_ptr<ItemDef>>(
        "Edit Item",
        m_items,
        m_selectedItem,
        "-- New Item --",
        [](auto item) { return item->tag; }
    );

    auto itemTypes = ENUMS(ItemType);

    std::string preview = m_selectedItem ? itemTypes->at(m_selectedItem->type).label : "-- Select Type --";

    if (ImGui::BeginCombo("Item Type", preview.c_str())) {

        for (int i = 0; i < itemTypes->size(); i++) {
            ImGui::PushID(i);
            auto item = itemTypes->at(i);
            if (ImGui::Selectable(item.label.c_str(), m_selectedItem && m_selectedItem->type == item.key)) {
                m_selectedItem = GET_FACTORY(ItemType, ItemDef)->at(item.key)();
            }
            ImGui::PopID();
        }

        ImGui::EndCombo();
    }

    if (m_selectedItem) {

        ImGui::Separator();

        m_selectedItem->ui();

        if (m_selectedItem->dirty() && ImGui::Button("Save")) {

            m_error = std::nullopt;

            bool newItem = true;
            for (const auto& prop : m_items) {
                if (m_selectedItem && prop->id() == m_selectedItem->id()) {
                    prop->tag = m_selectedItem->tag;
                    prop->size = m_selectedItem->size;
                    newItem = false;
                    break;
                }
            }

            if (newItem) {
                m_items.push_back(m_selectedItem);
            }

            for (const auto& item : m_items) {
                m_scene->getSystem<Items>()->store().set(item->id(), item);
            }

            hg::utils::MultiConfig config;
            m_scene->getSystem<Items>()->save(config);

            hg::utils::f_write(hg::ASSET_DIR + "items.hg", config.toString());
        }
    }

    ImGui::End();
}
