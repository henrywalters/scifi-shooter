//
// Created by henry on 11/1/23.
//

#ifndef SCIFISHOOTER_ITEM_H
#define SCIFISHOOTER_ITEM_H

#include <hagame/core/component.h>
#include <hagame/core/entity.h>
#include "../common/itemDef.h"

class Item : public hg::Component {
public:

    ItemDef* def;

protected:

    OBJECT_NAME(Item);
};

HG_COMPONENT(Game, Item)

#endif //SCIFISHOOTER_ITEM_H
