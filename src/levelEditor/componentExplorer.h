//
// Created by henry on 12/8/23.
//

#ifndef SCIFISHOOTER_COMPONENTEXPLORER_H
#define SCIFISHOOTER_COMPONENTEXPLORER_H

#include <hagame/core/component.h>
#include <optional>
#include "imgui.h"

const std::string COMPONENT_EXPLORER = "component_explorer";

std::optional<std::string> componentExplorer();

bool editComponentField(hg::Component* component, hg::ComponentFactory::ComponentField field);

#endif //SCIFISHOOTER_COMPONENTEXPLORER_H
