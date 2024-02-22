//
// Created by henry on 1/14/24.
//
#include "prop.h"
#include "../systems/props.h"
#include "../components/triggerConnection.h"
#include "../components/requirementConnection.h"

using namespace hg;
using namespace hg::utils;

void Prop::toggle(hg::Entity* inventory, bool fromPlayer) {

    if (!def->states[stateId].nextStateId.has_value()) {
        return;
    }

    if (def->states[def->states[stateId].nextStateId.value()].triggerOnly && fromPlayer) {
        std::cout << "TRIGGER ONLY!";
        return;
    }

    bool meetsRequirements = true;

    std::vector<hg::Entity*> items;

    for (const auto& entity : inventory->children()) {
        auto item = ((hg::Entity*) entity)->getComponent<Item>();
        if (item) {
            items.push_back((hg::Entity*) entity);
        }
    }

    entity->scene->entities.forEach<RequirementConnection>([&](RequirementConnection* connection, auto other) {
        if (connection->connectedTo && connection->connectedTo == entity) {
            meetsRequirements = false;
            for (const auto& item : items) {
                if (item->id() == other->id()) {
                    meetsRequirements = true;
                }
            }
        }
    });

    if (!meetsRequirements) {
        std::cout << "DOESNT MEET REQUIREMENTS\n";
        return;
    }

    stateId = def->states[stateId].nextStateId.value();

    auto connection = entity->getComponent<TriggerConnection>();
    if (
        connection &&
        connection->connectedTo &&
        connection->connectedTo->hasComponent<Prop>()
    )
    {
        connection->connectedTo->getComponent<Prop>()->toggle(inventory, false);
    }
}

void Prop::addRequirement(std::string state, std::string item) {
    for (int i = 0; i < def->states.size(); i++) {
        if (state == def->states[i].name) {
            requirements.push_back(Requirement{i, item});
        }
    }
}

void Prop::load(Config* config, std::string section) {
    if (config->has(section, "prop_id")) {
        auto store = entity->scene->getSystem<Props>()->store();
        auto id = config->get<uuid_t>(section, "prop_id");
        if (store.has(id)) {
            def = store.get(id);
        }
        if (def) {
            stateId = def->defaultStateId;
        }

    }
}

void Prop::save(Config* config, std::string section) {
    if (def) {
        config->set(section, "prop_id", def->id());
    }
}

void Prop::onUiUpdate() {
    auto preview = def ? def->tag : "-- Select Prop --";
    if (ImGui::BeginCombo("Prop Type", preview.c_str())) {
        if (def && ImGui::Selectable("-- No Prop --")) {
            def.reset();
            def = nullptr;
        }
        entity->scene->getSystem<Props>()->store().forEach([&](auto id, auto prop) {
            if (ImGui::Selectable(prop->tag.c_str(), def && def->id() == prop->id())) {
                def = prop;
            }
        });
        ImGui::EndCombo();
    }
}
