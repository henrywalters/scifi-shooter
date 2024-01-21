//
// Created by henry on 1/14/24.
//
#include "prop.h"
#include "../systems/props.h"
#include "../components/triggerConnection.h"
#include "../components/requirementConnection.h"

using namespace hg;
using namespace hg::utils;

void Prop::toggle(hg::Entity* inventory) {

    bool meetsRequirements = true;

    std::vector<std::string> items;

    for (const auto& entity : inventory->children()) {
        auto item = ((hg::Entity*) entity)->getComponent<Item>();
        if (item) {
            items.push_back(item->def->tag);
        }
    }

    entity->scene->entities.forEach<TriggerConnection>([&](TriggerConnection* connection, auto other) {
        if (connection->connectedTo && connection->connectedTo == entity) {
            std::cout << "Satisfied by " << other->name << "\n";
        }
    });

    for (const auto& requirement : requirements) {
        if (def->states[stateId].nextStateId.has_value()) {
            meetsRequirements = false;
            for (const auto& [id, other] : def->states) {
                if (id == def->states[stateId].nextStateId.value()) {
                    meetsRequirements = true;
                    break;
                }
            }
        }
    }

    if (!meetsRequirements) {
        std::cout << "DOESNT MEET REQUIREMENTS\n";
        return;
    }

    if (def->states[stateId].nextStateId.has_value()) {
        stateId = def->states[stateId].nextStateId.value();
    }

    auto connection = entity->getComponent<TriggerConnection>();
    if (
        connection &&
        connection->connectedTo &&
        connection->connectedTo->hasComponent<Prop>()
    )
    {
        connection->connectedTo->getComponent<Prop>()->toggle(inventory);
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
        config->set(section, "prop_id", def->id);
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
            if (ImGui::Selectable(prop->tag.c_str(), def && def->id == prop->id)) {
                def = prop;
            }
        });
        ImGui::EndCombo();
    }
}
