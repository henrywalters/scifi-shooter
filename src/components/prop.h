//
// Created by henry on 12/3/23.
//

#ifndef SCIFISHOOTER_PROP_H
#define SCIFISHOOTER_PROP_H

#include <hagame/core/entity.h>
#include <hagame/core/component.h>
#include <hagame/graphics/components/sprite.h>
#include "../common/propDef.h"
#include "item.h"

class Prop : public hg::Component {
public:

    struct Requirement {
        int state;
        std::string item;
    };

    std::shared_ptr<PropDef> def;
    //int state;
    hg::utils::uuid_t stateId;
    std::vector<Requirement> requirements;

    void toggle(hg::Entity* inventory);

    void addRequirement(std::string state, std::string item);

    void load(hg::utils::Config* config, std::string section) override;

    void save(hg::utils::Config* config, std::string section) override;

protected:

    void onUiUpdate() override;

    OBJECT_NAME(Prop)

};

HG_COMPONENT(Game, Prop)

#endif //SCIFISHOOTER_PROP_H
