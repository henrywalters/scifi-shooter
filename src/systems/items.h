//
// Created by henry on 11/1/23.
//

#ifndef SCIFISHOOTER_ITEM_SYSTEM_H
#define SCIFISHOOTER_ITEM_SYSTEM_H

#include <hagame/core/entity.h>
#include <hagame/core/system.h>
#include <hagame/graphics/window.h>
#include "../common/gamestate.h"
#include "../common/itemDef.h"
#include "worldObjects.h"
#include "../components/item.h"

class Items : public WorldObjects<std::string, ItemDef, Item> {
public:

    void load(hg::utils::MultiConfig config) override;
    void save(hg::utils::MultiConfig& config) override;

protected:

    void onSpawn(hg::Entity* entity) override;

};

#endif //SCIFISHOOTER_ITEM_SYSTEM_H
