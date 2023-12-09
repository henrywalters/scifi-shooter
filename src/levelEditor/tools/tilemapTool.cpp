//
// Created by henry on 12/8/23.
//
#include "tilemapTool.h"

void TilemapTool::onInit() {
    auto entity = m_scene->entities.add();
    auto tilemap = entity->addComponent<TilemapComponent>();
}

void TilemapTool::renderUI(double dt) {
    Tool::renderUI(dt);
}

void TilemapTool::onUpdate(double dt) {
    Tool::onUpdate(dt);
}
