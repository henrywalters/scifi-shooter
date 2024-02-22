//
// Created by henry on 2/5/24.
//
#include "item.h"
#include "../systems/items.h"

void Item::onUiUpdate() {
    std::vector<hg::utils::uuid_t> itemIds;
    auto store = entity->scene->getSystem<Items>()->store();
    store.forEach([&itemIds](auto id, auto item) {
        itemIds.push_back(item->id());
    });

    if (itemIds.size() == 0) {
        return;
    }

    auto itemId = def ? def->id() : itemIds[0];

    ui::Select<hg::utils::uuid_t>(
        "Item",
        itemIds,
        itemId,
        itemIds[0],
        [&](hg::utils::uuid_t id) { return store.get(id)->tag; }
    );

    def = entity->scene->getSystem<Items>()->get(itemId);
}

void Item::load(hg::utils::Config *config, std::string section) {
    auto id = config->get<hg::utils::uuid_t>(section, "itemId");
    def = entity->scene->getSystem<Items>()->get(id);
}

void Item::save(hg::utils::Config *config, std::string section) {
    if (!def) { return ;}
    config->set(section, "itemId", def->id());
}