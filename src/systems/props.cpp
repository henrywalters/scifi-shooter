//
// Created by henry on 12/3/23.
//
#include <hagame/graphics/components/sprite.h>
#include "props.h"

using namespace hg::utils;

void Props::onSpawn(hg::Entity *entity) {
    auto prop = entity->getComponent<Prop>();
    auto sprite = entity->addComponent<hg::graphics::Sprite>();
    sprite->size = prop->def->size;
    prop->stateId = prop->def->defaultStateId;
    sprite->texture = prop->def->states[prop->stateId].texture;
}

void Props::load(MultiConfig config) {

    auto page = config.getPage("Props");

    for (const auto& id : page->sections()) {
        std::shared_ptr<PropDef> prop = std::make_unique<PropDef>();

        prop->setId(std::stol(id));
        prop->tag = page->getRaw(id, "tag");
        page->getArray<float, 2>(id, "size", prop->size.vector);
        std::vector<std::string> states = s_split(page->getRaw(id, "states"),',');
        prop->defaultStateId = page->get<hg::utils::uuid_t>(id, "default_state");

        if (states.size() == 0) {
            throw std::runtime_error("Prop must have at least one state");
        }

        for (const auto& stateId : states) {
            PropDef::State state;
            state.id = std::stol(stateId);
            state.texture = page->getRaw(id, stateId + "_texture");
            state.message = page->getRaw(id, stateId + "_message");
            state.collide = page->get<bool>(id, stateId + "_collide");
            state.triggerOnly = page->get<bool>(id, stateId + "_triggerOnly");
            state.name = page->getRaw(id, stateId + "_name");
            if (page->has(id, stateId + "_next")) {
                state.nextStateId = page->get<hg::utils::uuid_t>(id, stateId + "_next");
            }
            prop->states.insert(std::make_pair(state.id, state));
        }

        if (m_store.has(prop->id())) {
            throw std::runtime_error("Prop already exists with this id: " + id);
        }

        m_store.set(prop->id(), prop);
    }
}

void Props::save(MultiConfig &config) {
    auto page = config.addPage("Props");
    m_store.forEach([&](auto id, auto prop) {
        auto idStr = std::to_string(id);
        page->addSection(idStr);
        page->setRaw(idStr, "tag", prop->tag);
        page->setArray<float, 2>(idStr, "size", prop->size.vector);
        page->set(idStr, "default_state", prop->defaultStateId);

        std::vector<std::string> stateIds;

        for (const auto& [id, state]: prop->states) {
            auto stateId = std::to_string(state.id);
            stateIds.push_back(stateId);
            page->setRaw(idStr, stateId + "_name", state.name);
            page->setRaw(idStr, stateId + "_texture", state.texture);
            page->set(idStr, stateId + "_collide", state.collide);
            page->set(idStr, stateId + "_triggerOnly", state.triggerOnly);
            page->setRaw(idStr, stateId + "_message", state.message);
            if (state.nextStateId.has_value()) {
                page->set(idStr, stateId + "_next", state.nextStateId.value());
            }
        }

        page->setRaw(idStr, "states", s_join(stateIds, ","));
    });
}

