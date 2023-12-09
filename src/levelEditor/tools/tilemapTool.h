//
// Created by henry on 12/7/23.
//

#ifndef SCIFISHOOTER_TILEMAPTOOL_H
#define SCIFISHOOTER_TILEMAPTOOL_H

#include <hagame/core/scene.h>
#include "tool.h"
#include "../../components/tilemap.h"

class TilemapTool : public Tool {
public:

    TilemapTool(hg::Scene* scene):
        Tool(scene)
    {}

    std::string getButtonLabel() override { return "Add Tilemap"; }
    std::string getName() override { return "Tilemap Tool"; }

    void onInit() override;
    void renderUI(double dt) override;
    void onUpdate(double dt) override;

};

#endif //SCIFISHOOTER_TILEMAPTOOL_H
