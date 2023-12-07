//
// Created by henry on 11/1/23.
//

#ifndef SCIFISHOOTER_ITEM_H
#define SCIFISHOOTER_ITEM_H

#include <hagame/core/component.h>
#include "../common/itemDef.h"

class Item : public hg::Component {
public:

    ItemDef* def;
    hg::Entity* owner;

protected:

    OBJECT_NAME(Item);
};

#endif //SCIFISHOOTER_ITEM_H
