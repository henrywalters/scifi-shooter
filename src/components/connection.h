//
// Created by henry on 12/5/23.
//

#ifndef SCIFISHOOTER_CONNECTION_H
#define SCIFISHOOTER_CONNECTION_H

#include "imgui.h"
#include <hagame/core/component.h>
#include <hagame/core/entity.h>
#include <hagame/core/scene.h>
#include "../common/ui.h"

// The connection component is a way to link one or more components. For example, say you want to connect a levers signal to a door.
class Connection : public hg::Component {
public:

    hg::Entity* connectedTo;

    void load(hg::utils::Config* config, std::string section) override {
        loadConnectedTo(config, section);
    }

    void save(hg::utils::Config* config, std::string section) override {
        saveConnectedTo(config, section);
    }

protected:

    void loadConnectedTo(hg::utils::Config* config, std::string section) {
        if (config->has(section, "connected_to")) {
            connectedTo = entity->scene->entities.get(config->get<hg::utils::uuid_t>(section, "connected_to"));
        } else {
            connectedTo = nullptr;
        }
    }

    void saveConnectedTo(hg::utils::Config* config, std::string section) {
        if (connectedTo) {
            config->set(section, "connected_to", connectedTo->id());
        }
    }

    void selectConnectedTo() {
        connectedTo = ui::SelectEntity("Connected To", entity->scene, connectedTo);
    }

    void onUiUpdate() override {
        selectConnectedTo();
    }

    OBJECT_NAME(Connection)

};

HG_COMPONENT(Game, Connection)

#endif //SCIFISHOOTER_CONNECTION_H
