//
// Created by henry on 11/1/23.
//

#ifndef SCIFISHOOTER_ITEM_H
#define SCIFISHOOTER_ITEM_H

#include <hagame/core/component.h>
#include <hagame/core/entity.h>
#include "../common/itemDef.h"
#include "../common/ui.h"

class Item : public hg::Component {
public:

    ItemDef* def;

    void load(hg::utils::Config* config, std::string section) override;

    void save(hg::utils::Config* config, std::string section) override;

protected:

    OBJECT_NAME(Item);

    void onUiUpdate() override;

};

HG_COMPONENT(Game, Item)

#endif //SCIFISHOOTER_ITEM_H
