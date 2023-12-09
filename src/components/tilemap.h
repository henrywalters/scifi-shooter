//
// Created by henry on 12/8/23.
//

#ifndef SCIFISHOOTER_TILEMAP_COMPONENT_H
#define SCIFISHOOTER_TILEMAP_COMPONENT_H

#include <hagame/core/component.h>
#include <hagame/graphics/tilemap.h>
#include "../common/constants.h"

class TilemapComponent : public hg::Component {
public:

    TilemapComponent():
        tilemap(TILE_SIZE)
    {}

    hg::graphics::Tilemap tilemap;

protected:

    OBJECT_NAME(TilemapComponent);

};

#endif //SCIFISHOOTER_TILEMAP_H
