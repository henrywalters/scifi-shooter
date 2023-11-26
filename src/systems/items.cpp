//
// Created by henry on 11/1/23.
//

#include <hagame/graphics/components/sprite.h>
#include "items.h"
#include "../components/item.h"

Items::Items(GameState *state):
        m_state(state),
        m_itemMap(TILE_SIZE)
{

}

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
                default:
                    throw std::runtime_error("Unsupported item type in load function: " + ITEM_TYPE_NAMES[i]);
            }

            item->type = (ItemType) i;
            item->tag = name;
            page->getArray<float, 2>(name, "size", item->size.vector);
            item->description = page->getRaw(name, "description");
            item->texture = page->getRaw(name, "texture");
            item->loadItem(*page);

            if (m_items.find(name) != m_items.end()) {
                throw std::runtime_error("Item already exists with this name: " + name);
            }

            m_items.insert(std::make_pair(name, item));
        }
    }
}

hg::Entity *Items::spawn(ItemDef *def, hg::Vec2 pos) {
    auto entity = scene->entities.add();
    auto item = entity->addComponent<Item>();
    auto sprite = entity->addComponent<hg::graphics::Sprite>(def->size.cast<float>());
    sprite->texture = def->texture;
    item->item = def;
    entity->transform.position = pos.resize<3>();
    m_itemMap.insert(entity->transform.position.resize<2>(), def->size.cast<float>(), entity);
    return entity;
}

void Items::remove(hg::Entity *entity) {
    auto item = entity->getComponent<Item>();
    if (!item) {
        return;
    }
    scene->entities.remove(entity);
}

std::vector<hg::Entity *> Items::getItems(hg::Vec2 pos, float radius) {
    std::vector<hg::Entity*> items;
    for (const auto& entity : m_itemMap.getNeighbors(pos, hg::Vec2(radius * 2, radius * 2))) {
        if ((entity->transform.position.resize<2>() - pos).magnitude() <= radius) {
            items.push_back(entity);
        }
    }
    return items;
}

void Items::onBeforeUpdate() {
    m_itemMap.clear();
    scene->entities.forEach<Item>([&](Item* item, hg::Entity* entity) {
        m_itemMap.insert(entity->position().resize<2>(), item->item->size, entity);
    });
}



