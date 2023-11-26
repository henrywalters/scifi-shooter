//
// Created by henry on 11/1/23.
//

#ifndef SCIFISHOOTER_ITEM_SYSTEM_H
#define SCIFISHOOTER_ITEM_SYSTEM_H

#include <hagame/core/entity.h>
#include <hagame/core/system.h>
#include <hagame/graphics/window.h>
#include "../gamestate.h"
#include "../items.h"

class Items : public hg::System {
public:

    Items(GameState* state);

    void load(hg::utils::MultiConfig config);

    ItemDef* get(std::string name) {
        return m_items[name].get();
    }

    void onBeforeUpdate() override;

    std::vector<hg::Entity*> getItems(hg::Vec2 pos, float radius);

    hg::Entity* spawn(ItemDef* def, hg::Vec2 pos);
    void remove(hg::Entity* item);

private:

    GameState* m_state;

    hg::EntityMap2D m_itemMap;

    std::unordered_map<std::string, std::shared_ptr<ItemDef>> m_items;
};

#endif //SCIFISHOOTER_ITEM_SYSTEM_H
