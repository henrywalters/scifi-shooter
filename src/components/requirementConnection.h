//
// Created by henry on 1/20/24.
//

#ifndef SCIFISHOOTER_REQUIREMENTCONNECTION_H
#define SCIFISHOOTER_REQUIREMENTCONNECTION_H

#include "connection.h"

class RequirementConnection : public Connection {
public:

    hg::utils::uuid_t stateId;

    void load(hg::utils::Config* config, std::string section) override {
        loadConnectedTo(config, section);
        stateId = config->get<hg::utils::uuid_t>(section, "state_id");
    }

    void save(hg::utils::Config* config, std::string section) override {
        saveConnectedTo(config, section);
        config->set(section, "state_id", stateId);
    }

protected:

    void onUiUpdate() override {
        selectConnectedTo();
        std::vector<hg::utils::uuid_t> stateIds;
        auto prop = connectedTo ? connectedTo->getComponent<Prop>() : nullptr;
        if (prop) {
            for (const auto &[id, state] : prop->def->states) {
                stateIds.push_back(id);
            }

            ui::Select<hg::utils::uuid_t>(
                "Required for",
                stateIds,
                stateId,
                stateIds[0],
                [prop] (auto id) { return prop->def->states[id].name; }
            );

        }
    }

    OBJECT_NAME(RequirementConnection)

};

HG_COMPONENT(Game, RequirementConnection)

#endif //SCIFISHOOTER_REQUIREMENTCONNECTION_H
