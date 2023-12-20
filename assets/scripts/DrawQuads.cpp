//
// Created by henry on 12/17/23.
//
#ifndef SCIFISHOOTER_DRAW_QUADS_CPP
#define SCIFISHOOTER_DRAW_QUADS_CPP

#include <hagame/core/scene.h>
#include "../../src/systems/renderer.h"

using namespace hg;

extern "C" {

    void DrawQuads_init(Scene* scene) {

    }

    void DrawQuads_update(Scene* scene, double dt) {
        auto renderer = scene->getSystem<Renderer>("Renderer");
        auto mousePos = renderer->getMousePos();

        auto entity = scene->entities.add();
        entity->transform.position = mousePos.resize<3>();

        auto quad = entity->addComponent<hg::graphics::Quad>();
        quad->size = hg::Vec2(5, 5);
        quad->color = hg::graphics::Color::blue();
    }

    void DrawQuads_close(Scene* scene) {

    }
}

#endif