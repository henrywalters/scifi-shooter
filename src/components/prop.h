//
// Created by henry on 12/3/23.
//

#ifndef SCIFISHOOTER_PROP_H
#define SCIFISHOOTER_PROP_H

#include <hagame/core/entity.h>
#include <hagame/core/component.h>
#include "../common/propDef.h"
#include "item.h"

class Prop : public hg::Component {
public:

    struct Requirement {
        int state;
        std::string item;
    };

    PropDef* def;
    int state;
    std::vector<Requirement> requirements;

    void toggle(hg::Entity* inventory) {

        bool meetsRequirements = true;

        std::vector<std::string> items;

        for (const auto& entity : inventory->children()) {
            auto item = ((hg::Entity*) entity)->getComponent<Item>();
            if (item) {
                items.push_back(item->def->tag);
            }
        }

        for (const auto& requirement : requirements) {
            if (requirement.state == def->states[state].nextState && std::find(items.begin(), items.end(), requirement.item) == items.end()) {
                std::cout << "MISSING " << requirement.item << "\n";
                meetsRequirements = false;
                break;
            }
        }

        if (!meetsRequirements) {
            return;
        }

        state = def->states[state].nextState;
        entity->getComponent<hg::graphics::Sprite>()->texture = def->states[state].texture;
    }

    void addRequirement(std::string state, std::string item) {
        for (int i = 0; i < def->states.size(); i++) {
            if (state == def->states[i].name) {
                requirements.push_back(Requirement{i, item});
            }
        }
    }

protected:

    OBJECT_NAME(Prop)

};

HG_COMPONENT(Game, Prop)

#endif //SCIFISHOOTER_PROP_H
