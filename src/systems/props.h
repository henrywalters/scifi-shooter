//
// Created by henry on 12/3/23.
//

#ifndef SCIFISHOOTER_PROPS_H
#define SCIFISHOOTER_PROPS_H

#include <hagame/core/system.h>
#include <hagame/utils/config.h>
#include <hagame/utils/store.h>

#include "../common/propDef.h"
#include "../components/prop.h"
#include "../common/gamestate.h"
#include "../components/connection.h"

#include "worldObjects.h"

class Props : public WorldObjects<hg::utils::uuid_t, PropDef, Prop> {
public:

    void load(hg::utils::MultiConfig config) override;
    void save(hg::utils::MultiConfig& config) override;

protected:

    void onSpawn(hg::Entity* entity) override;

};


#endif //SCIFISHOOTER_PROPS_H
