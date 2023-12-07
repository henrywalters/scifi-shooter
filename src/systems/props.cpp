//
// Created by henry on 12/3/23.
//
#include <hagame/graphics/components/sprite.h>
#include "props.h"

using namespace hg::utils;

void Props::onSpawn(hg::Entity *entity) {
    auto prop = entity->getComponent<Prop>();
    auto sprite = entity->addComponent<hg::graphics::Sprite>(prop->def->size);
    prop->state = prop->def->defaultState;
    sprite->texture = prop->def->states[prop->state].texture;
}

void Props::load(MultiConfig config) {

    auto page = config.getPage("Props");

    for (const auto& name : page->sections()) {
        std::shared_ptr<PropDef> prop = std::make_unique<PropDef>();

        page->getArray<float, 2>(name, "size", prop->size.vector);
        std::vector<std::string> states = s_split(page->getRaw(name, "states"),',');

        auto findState = [states](std::string state)-> int {
            for (int i = 0; i < states.size(); i++) {
                if (states[i] == state) {
                    return i;
                }
            }
            return -1;
        };

        if (states.size() == 0) {
            throw std::runtime_error("Prop must have at least one state");
        }

        for (const auto& stateName : states) {
            PropDef::State state;
            state.name = stateName;
            state.texture = page->getRaw(name, stateName + "_texture");
            state.message = page->getRaw(name, stateName + "_message");
            state.collide = page->get<bool>(name, stateName + "_collide");
            state.nextState = findState(page->getRaw(name, stateName + "_next"));
            prop->states.push_back(state);
        }

        std::string defaultState = page->getRaw(name, "default_state");
        prop->defaultState = findState(defaultState);
        if (prop->defaultState == -1) {
            prop->defaultState = 0;
        }

        if (m_store.has(name)) {
            throw std::runtime_error("Prop already exists with this name: " + name);
        }

        m_store.set(name, prop);
    }
}

