//
// Created by henry on 11/1/23.
//

#include <hagame/graphics/components/sprite.h>
#include "items.h"

void Items::load(hg::utils::MultiConfig config) {
    for (int i = 0; i < ITEM_TYPE_NAMES.size(); i++) {

        auto page = config.getPage(ITEM_TYPE_NAMES[i]);

        for (const auto& name : page->sections()) {
            std::shared_ptr<ItemDef> item;

            switch ((ItemType) i) {
                case ItemType::Weapon:
                    item = std::make_shared<WeaponItemDef>();
                    break;
                case ItemType::Ammo:
                    item = std::make_shared<AmmoItemDef>();
                    break;
                case ItemType::Health:
                    item = std::make_shared<HealthItemDef>();
                    break;
                case ItemType::PropRequirement:
                    item = std::make_shared<PropRequirementDef>();
                    break;
                default:
                    throw std::runtime_error("Unsupported item type in load function: " + ITEM_TYPE_NAMES[i]);
            }

            item->type = (ItemType) i;
            item->tag = name;
            page->getArray<float, 2>(name, "size", item->size.vector);
            item->description = page->getRaw(name, "description");
            item->texture = page->getRaw(name, "texture");
            item->loadItem(*page);

            if (m_store.has(name)) {
                throw std::runtime_error("Item already exists with this name: " + name);
            }

            m_store.set(name, item);
        }
    }
}

void Items::onSpawn(hg::Entity *entity) {
    auto item = entity->getComponent<Item>();
    auto sprite = entity->addComponent<hg::graphics::Sprite>();
    sprite->size = item->def->size.cast<float>();
    sprite->texture = item->def->texture;
    entity->name = item->def->tag;
}



