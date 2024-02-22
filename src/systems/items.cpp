//
// Created by henry on 11/1/23.
//

#include <hagame/graphics/components/sprite.h>
#include "items.h"

void Items::load(hg::utils::MultiConfig config) {

    auto page = config.getPage("items");

    for (const auto& id : page->sections()) {
        std::shared_ptr<ItemDef> item;
        auto type = page->get<int>(id, "type");

        item = GET_FACTORY(ItemType, ItemDef)->at(type)();

        item->setId(std::stol(id));
        item->load(*page);

        if (m_store.has(item->id())) {
            throw std::runtime_error("Item already exists with this id: " + id);
        }

        m_store.set(item->id(), item);
    }

}

void Items::save(hg::utils::MultiConfig &config) {
    auto page = config.getPage("items");

    m_store.forEach([&](hg::utils::uuid_t id, std::shared_ptr<ItemDef> item) {
        item->save(*page);
    });
}

void Items::onSpawn(hg::Entity *entity) {
    auto item = entity->getComponent<Item>();
    auto sprite = entity->addComponent<hg::graphics::Sprite>();
    sprite->size = item->def->size.cast<float>();
    sprite->texture = item->def->texture;
    entity->name = item->def->tag;
}



